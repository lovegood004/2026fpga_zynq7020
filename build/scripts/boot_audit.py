"""Independently check this project's unencrypted three-partition Zynq image."""
from pathlib import Path
import argparse, hashlib, json, struct, zipfile

def need(ok,message):
    if not ok:raise ValueError(message)

def words(data,offset,count):
    need(0<=offset<=len(data)-4*count,'Header outside image')
    return struct.unpack_from('<'+'I'*count,data,offset)

def bit_payload(data):
    n=int.from_bytes(data[:2],'big');p=2+n
    need(data[p:p+3]==b'\0\x01a','Unsupported bit header');p+=3
    for tag in [b'b',b'c',b'd',b'e']:
        n=int.from_bytes(data[p:p+2],'big');p+=2+n
        need(data[p:p+1]==tag,'Malformed bit fields');p+=1
    n=int.from_bytes(data[p:p+4],'big');p+=4
    need(n>0 and p+n==len(data) and n%4==0,'Malformed bit payload length')
    # Bootgen converts the .bit big-endian words to little-endian PCAP words.
    payload=data[p:]
    result=b''.join(payload[i:i+4][::-1] for i in range(0,n,4))
    # Zynq Bootgen appends PCAP NOOP words to a 32-byte boundary.
    return result+struct.pack('<I',0x20000000)*((-len(result)%32)//4)

def elf_payload(data,load,entry):
    need(data[:7]==b'\x7fELF\x01\x01\x01','Expected ELF32 little endian')
    h=struct.unpack_from('<HHIIIIIHHHHHH',data,16)
    need(h[1]==40 and h[3]==entry and h[8]==32,'ELF architecture/entry mismatch')
    segments=[];memory=[]
    for i in range(h[9]):
        kind,off,virt,phys,size,mem,flags,align=words(data,h[4]+i*h[8],8)
        if kind!=1:continue
        need(size<=mem and off+size<=len(data),'Invalid ELF load segment')
        memory.append((phys,mem))
        if size:segments.append((phys,data[off:off+size]))
    need(segments and min(p for p,_ in segments)==load,'ELF load address mismatch')
    need(any(p<=entry<p+n for p,n in memory),'Entry outside ELF memory')
    end=max(p+len(b) for p,b in segments)
    need(end-load<32*1024*1024,'Unbounded ELF payload')
    result=bytearray(end-load);occupied=[]
    for p,b in sorted(segments):
        need(all(p+len(b)<=a or p>=z for a,z in occupied),'Overlapping ELF segments')
        occupied.append((p,p+len(b)));result[p-load:p-load+len(b)]=b
    return bytes(result)

def audit(image,files):
    need(0<len(image)<32*1024*1024,'Image exceeds nominal board capacity')
    header=words(image,0x20,11)
    need(header[:2]==(0xaa995566,0x584c4e58) and sum(header)&0xffffffff==0xffffffff,'Boot header checksum/magic')
    need(header[2]==0,'Encrypted boot image unsupported')
    iht,pht=words(image,0x98,2);table=words(image,iht,5)
    need(table[0]==0x01020000 and table[1]==3 and table[2]*4==pht and table[4]==0,'Unexpected image table')
    ih=table[3]*4;images=[];seen=set()
    for i,name in enumerate(['fsbl.elf','system.bit','application.elf']):
        need(ih not in seen,'Image header cycle');seen.add(ih)
        h=words(image,ih,4)
        need(h[1]*4==pht+64*i and h[2]==0 and h[3]==1,'Unexpected partition mapping')
        # Bootgen stores four-character filename groups in reversed word order.
        raw=image[ih+16:ih+64]
        decoded=b''.join(raw[j:j+4][::-1] for j in range(0,len(raw),4)).split(b'\0')[0]
        need(decoded==name.encode(),'Unexpected partition name')
        images.append(ih);ih=h[0]*4
    need(ih==0,'Extra images')
    ranges=[];reports=[]
    for i,name in enumerate(['fsbl.elf','system.bit','application.elf']):
        h=words(image,pht+64*i,16)
        need(sum(h)&0xffffffff==0xffffffff,'Partition checksum')
        need(h[0]==h[1]==h[2] and h[1]>0,'Encrypted/padded partition unsupported')
        need(h[6]==(0x20 if i==1 else 0x10) and h[7]==1 and h[8]==h[10]==0 and h[9]*4==images[i],'Partition attributes')
        start=h[5]*4;size=h[1]*4;end=start+size
        need(start>=pht+3*64 and end<=len(image),'Partition bounds')
        need(all(end<=a or start>=b for a,b in ranges),'Overlapping partitions');ranges.append((start,end))
        actual=image[start:end]
        expected=bit_payload(files[name]) if i==1 else elf_payload(files[name],h[3],h[4])
        need(len(expected)<=size<len(expected)+4,'Payload length mismatch')
        need(actual[:len(expected)]==expected,'Payload differs from '+name)
        need(actual[len(expected):]==b'\xff'*(size-len(expected)),'Unexpected payload padding')
        if i==0:
            need((start,size,h[3],h[4],size)==tuple(header[4:9]),'FSBL boot/partition header mismatch')
        reports.append(dict(name=name,offset=start,bytes=size,load_address=hex(h[3]),entry=hex(h[4]),payload_sha256=hashlib.sha256(actual).hexdigest(),matches_input=True))
    return dict(passed=True,board_accessed=False,cold_boot_verified=False,bytes=len(image),boot_sha256=hashlib.sha256(image).hexdigest(),partitions=reports,scope='Header checksums, names/order, bounds, ELF PT_LOAD bytes/entry and converted bit payload; no physical capacity or boot claim')

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('folder',type=Path);ap.add_argument('--self-test',action='store_true');a=ap.parse_args()
    files={n:(a.folder/n).read_bytes() for n in ['fsbl.elf','system.bit','application.elf']}
    image=(a.folder/'BOOT.bin').read_bytes();r=audit(image,files)
    receipt=json.loads((a.folder/'receipt.json').read_text())
    for name,record in receipt['files'].items():need(hashlib.sha256((a.folder/name).read_bytes()).hexdigest()==record['sha256'],'Build receipt mismatch: '+name)
    with zipfile.ZipFile(a.folder/'hardware.xsa') as z:
        bits=[z.read(n) for n in z.namelist() if n.endswith('.bit')]
    need(bits==[files['system.bit']],'XSA/bit mismatch')
    if a.self_test:
        locations=[0x48,words(image,0x98,2)[1]+60]+[p['offset']+32 for p in r['partitions']]
        for offset in locations:
            damaged=bytearray(image);damaged[offset]^=1
            try:audit(damaged,files)
            except ValueError:pass
            else:raise ValueError('Corruption was accepted at '+hex(offset))
        r['real_image_corruption_checks_rejected']=len(locations)
    (a.folder/'payload_audit.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))

if __name__=='__main__':main()
