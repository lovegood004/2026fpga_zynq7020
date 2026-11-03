import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

SCRIPT = Path(__file__).with_name('zynq_validate.py')
sys.dont_write_bytecode = True
spec = importlib.util.spec_from_file_location('zv', SCRIPT)
z = importlib.util.module_from_spec(spec)
spec.loader.exec_module(z)

class VerificationTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.r = Path(self.tmp.name)
    def tearDown(self):
        self.tmp.cleanup()
    def write(self, name, value):
        p = self.r/name
        p.parent.mkdir(parents=True,exist_ok=True)
        p.write_text(value,encoding='utf-8')
        return str(p)
    def cli(self,*args):
        p = subprocess.run([sys.executable,str(SCRIPT),*map(str,args)],capture_output=True,text=True,encoding='utf-8')
        return p.returncode,json.loads(p.stdout)
    def test_bytes_fail_and_empty(self):
        a=self.write('a','one'); b=self.write('b','two')
        self.assertEqual(self.cli('compare','--golden',a,'--actual',a)[0],0)
        self.assertEqual(self.cli('compare','--golden',a,'--actual',b)[0],1)
        self.assertEqual(self.cli('compare','--golden',self.write('empty',''),'--actual',b)[0],1)
    def test_hls_all_words_and_ids(self):
        def tx(i,words): return f'[[[runtime]]]\n[[transaction]] {i}\n{words}\n[[/transaction]]\n[[[/runtime]]]'
        a=self.write('a',tx(0,'0000 0001 ff'))
        for value,expected in [(tx(0,'0 1 FF'),0),(tx(0,'0 2 FF'),1),(tx(1,'0 1 FF'),1),(tx(0,'0 1'),1),(tx(0,'X 1 FF'),1),(tx(0,''),1),('',1),('PASS',1)]:
            b=self.write('b',value)
            self.assertEqual(self.cli('compare','--format','hls','--golden',a,'--actual',b)[0],expected)
    def test_numeric_tolerance_and_nonfinite(self):
        a=self.write('a','1, 0 -2')
        for value,extra,expected in [('1 0 -2',[],0),('1.0001 0 -2',[],1),('1.0001 0 -2',['--atol','0.001'],0),('NaN 0 -2',[],1),('1 0',[],1),('1 0 -2',['--atol','nan'],1)]:
            b=self.write('b',value)
            self.assertEqual(self.cli('compare','--format','numeric','--golden',a,'--actual',b,*extra)[0],expected)
    def reports(self):
        timing='WNS(ns) TNS(ns) TNS Failing Endpoints ...\n------- -------\n0.1 0 0 12 0.01 0 0 12 0.2 0 0 10\n'
        timing+='\n'.join(f'checking {c} (0)' for c in z.CHECKS)
        util='Design State : Routed\n'+'\n'.join(f'| {v} | 5 | 0 | 0 | 100 | 5.00 |' for v in z.LABELS.values())
        policy=json.dumps({'max_util_pct':dict.fromkeys(z.LABELS,80)})
        return timing,util,policy
    def test_qor_failure_modes(self):
        t,u,p=self.reports()
        for timing,util,policy,expected in [(t,u,p,0),(t.replace('0.1 0','-0.1 -1'),u,p,1),(t.replace('0.01 0','-0.01 -1'),u,p,1),(t.replace('0.2 0','-0.2 -1'),u,p,1),(t.replace('no_clock (0)','no_clock (1)'),u,p,1),(t.replace('checking no_clock (0)',''),u,p,1),(t,u.replace('5 | 0 | 0 | 100 | 5.00','90 | 0 | 0 | 100 | 90.00'),p,1),(t,u.replace('Routed','Synthesized'),p,1),(t,u.replace('5.00','6.00'),p,1)]:
            args=['qor','--timing',self.write('t',timing),'--utilization',self.write('u',util),'--policy',self.write('p',policy)]
            self.assertEqual(self.cli(*args)[0],expected)
    def test_manifest_tamper_and_escape(self):
        self.write('source','good')
        spec=self.write('spec.json',json.dumps({'files':['source'],'toolchain':{'python':sys.version}}))
        report=self.r/'manifest.json'
        self.assertEqual(self.cli('--report',report,'freeze','--root',self.r,'--spec',spec)[0],0)
        self.assertEqual(self.cli('verify','--root',self.r,'--manifest',report)[0],0)
        self.write('source','bad')
        self.assertEqual(self.cli('verify','--root',self.r,'--manifest',report)[0],1)
        with self.assertRaises(ValueError): z.inside(self.r,'../outside')
    def test_two_fresh_builds(self):
        source=self.r/'project';source.mkdir()
        (source/'build.py').write_text('from pathlib import Path\nPath("out.bin").write_bytes(b"deterministic")\n')
        spec=self.write('spec.json',json.dumps({'files':['build.py'],'toolchain':{'python':sys.version}}))
        manifest=self.r/'manifest.json'
        self.assertEqual(self.cli('--report',manifest,'freeze','--root',source,'--spec',spec)[0],0)
        rebuild=self.write('rebuild.json',json.dumps({'commands':[['{python}','{root}/build.py']],'outputs':['out.bin'],'timeout_seconds':20}))
        args=['reproduce','--root',source,'--manifest',manifest,'--spec',rebuild,'--work',self.r/'work']
        self.assertEqual(self.cli(*args)[0],2)
        self.assertFalse((self.r/'work').exists())
        self.assertEqual(self.cli(*args,'--execute')[0],0)
        self.assertFalse((source/'out.bin').exists())
    def test_nondeterministic_build_rejected(self):
        source=self.r/'project';source.mkdir()
        (source/'build.py').write_text('from pathlib import Path\nPath("out.bin").write_text(str(Path.cwd()))\n')
        spec=self.write('spec.json',json.dumps({'files':['build.py'],'toolchain':{'python':sys.version}}))
        manifest=self.r/'manifest.json'
        self.cli('--report',manifest,'freeze','--root',source,'--spec',spec)
        rebuild=self.write('rebuild.json',json.dumps({'commands':[['{python}','{root}/build.py']],'outputs':['out.bin'],'timeout_seconds':20}))
        code,result=self.cli('reproduce','--root',source,'--manifest',manifest,'--spec',rebuild,'--work',self.r/'work','--execute')
        self.assertEqual(code,1)
        self.assertEqual(result['changed_outputs'],['out.bin'])

if __name__=='__main__':
    unittest.main(verbosity=2)
