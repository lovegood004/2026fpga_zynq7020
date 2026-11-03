#include "conv.hpp"
template<int Lane> static float ordered_mac(float previous,float x,float w,bool valid){
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1
    float product=x*w;
#pragma HLS BIND_OP variable=product op=fmul impl=fulldsp latency=4
    float sum=previous+product;
#pragma HLS BIND_OP variable=sum op=fadd impl=fabric latency=6
    return valid?sum:previous;
}
extern "C" int lung_conv_reference(const float *input,const float *weight,const float *bias,
                              float *output,int ci,int co,int height,int width){
#pragma HLS INTERFACE m_axi port=input offset=slave bundle=mem depth=131072 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=weight offset=slave bundle=mem depth=32768 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=bias offset=slave bundle=mem depth=256
#pragma HLS INTERFACE m_axi port=output offset=slave bundle=mem depth=32768 max_write_burst_length=64 num_write_outstanding=4
#pragma HLS INTERFACE s_axilite port=input offset=0x20 bundle=control
#pragma HLS INTERFACE s_axilite port=weight offset=0x30 bundle=control
#pragma HLS INTERFACE s_axilite port=bias offset=0x40 bundle=control
#pragma HLS INTERFACE s_axilite port=output offset=0x50 bundle=control
#pragma HLS INTERFACE s_axilite port=ci offset=0x60 bundle=control
#pragma HLS INTERFACE s_axilite port=co offset=0x68 bundle=control
#pragma HLS INTERFACE s_axilite port=height offset=0x70 bundle=control
#pragma HLS INTERFACE s_axilite port=width offset=0x78 bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
    if(ci<1||ci>128||co<1||co>256||height<1||height>128||width<1||width>468||
       ci*width>LUNG_CONV_ROW||ci*height*width>LUNG_CONV_INPUT)return -1;
    const int oh=(height+1)/2,ow=(width+1)/2;
    if(co*oh*ow>LUNG_CONV_INPUT)return -1;
    // Two output rows share five input rows and one weight load. Explicit
    // banks expose one read per bank rather than four ambiguous read ports.
    float rows[5][8][LUNG_CONV_ROW/8],kernel[1152],acc[4][16];
#pragma HLS ARRAY_PARTITION variable=rows complete dim=1
#pragma HLS ARRAY_PARTITION variable=rows complete dim=2
#pragma HLS BIND_STORAGE variable=rows type=ram_2p impl=bram latency=1
#pragma HLS ARRAY_PARTITION variable=acc complete dim=1
#pragma HLS BIND_STORAGE variable=acc type=ram_2p impl=lutram latency=1
    const bool narrow=ow<=32;
    row_pair: for(int yy=0;yy<oh;yy+=2){
        load_y: for(int ky=0;ky<5;++ky){
            const int sy=yy*2+ky-1;
            load_c: for(int c=0;c<ci;++c)load_x: for(int x=0;x<width;++x){
#pragma HLS PIPELINE II=1
                const int index=c*width+x;
                rows[ky][index&7][index>>3]=(sy>=0&&sy<height)?input[(c*height+sy)*width+x]:0.0f;
            }
        }
        channels: for(int o=0;o<co;++o){
            load_weight: for(int k=0;k<ci*9;++k){
#pragma HLS PIPELINE II=1
                kernel[k]=weight[o*ci*9+k];
            }
            const float b=bias[o];
            // A narrow map uses 32 slots for each of two rows. There are still
            // 16 independent accumulator groups; never shorten RAW distance.
            passes: for(int pass=0;pass<(narrow?1:2);++pass){
                tiles: for(int first=0;first<(narrow?1:ow);first+=64){
                    init_l: for(int l=0;l<4;++l)init: for(int i=0;i<16;++i){
#pragma HLS UNROLL
                        acc[l][i]=0.0f;
                    }
                    mac_c: for(unsigned short c=0;c<ci;++c)
                    mac_y: for(unsigned short ky=0;ky<3;++ky)
                    mac_x: for(unsigned short kx=0;kx<3;++kx)
                    mac: for(unsigned short g=0;g<16;++g){
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=acc inter RAW distance=16 true
                        const int rr=narrow?(g>>3):pass;
                        const int gx=narrow?(g&7):g;
                        const int start=(first+gx*4)*2+int(kx)-1;
                        const int base=int(c)*width+start;
                        const int sy=(yy+rr)*2+int(ky)-1;
                        const bool row_valid=yy+rr<oh&&sy>=0&&sy<height;
                        const float w=kernel[c*9+ky*3+kx];
                        float bank_sample[8],samples[4];bool valid[4];
#pragma HLS ARRAY_PARTITION variable=bank_sample complete
#pragma HLS ARRAY_PARTITION variable=samples complete
#pragma HLS ARRAY_PARTITION variable=valid complete
                        banks: for(int bank=0;bank<8;++bank){
#pragma HLS UNROLL
                            const int delta=(bank-(base&7))&7;
                            const int sx=start+delta,position=first+gx*4+(delta>>1);
                            // Express the bank-word carry explicitly. Do not synthesize the
                            // truncated low-bit cancellation (base+delta)>>3.
                            const int word=(base>>3)+int(bank<(base&7));
                            const bool take=row_valid&&!(delta&1)&&position<ow&&sx>=0&&sx<width;
                            bank_sample[bank]=take?rows[2*rr+ky][bank][word]:0.0f;
                        }
                        lanes: for(int lane=0;lane<4;++lane){
#pragma HLS UNROLL
                            const int x=first+gx*4+lane,sx=x*2+int(kx)-1;
                            valid[lane]=row_valid&&x<ow&&sx>=0&&sx<width;
                            samples[lane]=bank_sample[(base+lane*2)&7];
                        }
                        acc[0][g]=ordered_mac<0>(acc[0][g],samples[0],w,valid[0]);
                        acc[1][g]=ordered_mac<1>(acc[1][g],samples[1],w,valid[1]);
                        acc[2][g]=ordered_mac<2>(acc[2][g],samples[2],w,valid[2]);
                        acc[3][g]=ordered_mac<3>(acc[3][g],samples[3],w,valid[3]);
                    }
                    store: for(int i=0;i<64;++i){
#pragma HLS PIPELINE II=1
                        const int rr=narrow?(i>>5):pass;
                        const int x=first+(narrow?(i&31):i);
                        if(yy+rr<oh&&x<ow)output[(o*oh+yy+rr)*ow+x]=acc[i%4][i/4]+b;
                    }
                }
            }
        }
    }
    return LUNG_CONV_OK;
}
