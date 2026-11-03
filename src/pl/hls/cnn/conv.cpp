#include "conv.hpp"
#include <hls_stream.h>
template<int Lane> static float ordered_mac(float previous,float x,float w,bool valid){
#pragma HLS INLINE off
#pragma HLS PIPELINE II=1
    float product=x*w;
#pragma HLS BIND_OP variable=product op=fmul impl=fulldsp latency=4
    float sum=previous+product;
#pragma HLS BIND_OP variable=sum op=fadd impl=fabric latency=6
    return valid?sum:previous;
}
// Three tasks share bounded FIFOs; all eight FP32 recurrence lanes retain
// their exact c/ky/kx order and distance-16 accumulator schedule.
static void prefetch_weights(const float*weight,const float*bias,hls::stream<float>&weights,
                             hls::stream<float>&biases,int ci,int co,int ow){
    float kernel[1152];const bool narrow=ow<=32,medium=ow<=64;
#pragma HLS INLINE off
    for(int o=0;o<co;++o){
        biases.write(bias[o]);
        for(int k=0;k<ci*9;++k){
#pragma HLS PIPELINE II=1
            kernel[k]=weight[o*ci*9+k];
        }
        // Replay on-chip weights in exactly the consumer's pass/tile/c/ky/kx
        // order. A coefficient token feeds all 16 accumulator groups; it is
        // never copied into another full per-channel kernel buffer.
        for(int pass=0;pass<(narrow?1:medium?2:4);++pass)
            for(int first=0;first<(medium?1:ow);first+=128)
                for(int k=0;k<ci*9;++k){
#pragma HLS PIPELINE II=1
                    weights.write(kernel[k]);
                }
    }
}
static void compute_channels(const float rows[8][16][LUNG_CONV_ROW/16],const float extra[16][LUNG_CONV_ROW/16],hls::stream<float>&weights,
                             hls::stream<float>&biases,hls::stream<float>&results,
                             int ci,int co,int oh,int ow,int yy,int height,int width){
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=rows complete dim=1
#pragma HLS ARRAY_PARTITION variable=rows complete dim=2
#pragma HLS ARRAY_PARTITION variable=extra complete dim=1
    float acc[8][16];
#pragma HLS ARRAY_PARTITION variable=acc complete dim=1
#pragma HLS BIND_STORAGE variable=acc type=ram_2p impl=lutram latency=1
    const bool narrow=ow<=32,medium=ow<=64;
        channels: for(int o=0;o<co;++o){
            const float b=biases.read();
            // The same sixteen recurrence groups cover four rows x32 pixels,
            // two rows x64, or one row x128. Keep RAW distance sixteen.
            passes: for(int pass=0;pass<(narrow?1:medium?2:4);++pass){
                tiles: for(int first=0;first<(medium?1:ow);first+=128){
                    init_group: for(int i=0;i<16;++i){
#pragma HLS PIPELINE II=1
                        init_lane: for(int l=0;l<8;++l){
#pragma HLS UNROLL
                            acc[l][i]=0.0f;
                        }
                    }
                    float w=0.0f;
                    mac_c: for(unsigned short c=0;c<ci;++c)
                    mac_y: for(unsigned short ky=0;ky<3;++ky)
                    mac_x: for(unsigned short kx=0;kx<3;++kx)
                    mac: for(unsigned short g=0;g<16;++g){
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=acc inter RAW distance=16 true
                        const int rr=narrow?(g>>2):medium?pass*2+(g>>3):pass;
                        const int gx=narrow?(g&3):medium?(g&7):g;
                        const int start=(first+gx*8)*2+int(kx)-1;
                        const int base=int(c)*width+start;
                        const int sy=(yy+rr)*2+int(ky)-1;
                        const bool row_valid=yy+rr<oh&&sy>=0&&sy<height;
                        if(g==0)w=weights.read();
                        float bank_sample[16],samples[8];bool valid[8];
#pragma HLS ARRAY_PARTITION variable=bank_sample complete
#pragma HLS ARRAY_PARTITION variable=samples complete
#pragma HLS ARRAY_PARTITION variable=valid complete
                        banks: for(int bank=0;bank<16;++bank){
#pragma HLS UNROLL
                            const int delta=(bank-(base&15))&15;
                            const int sx=start+delta,position=first+gx*8+(delta>>1);
                            // Express the bank-word carry explicitly. Do not synthesize the
                            // truncated low-bit cancellation (base+delta)>>4.
                            const int word=(base>>4)+int(bank<(base&15));
                            const bool take=row_valid&&!(delta&1)&&position<ow&&sx>=0&&sx<width;
                            bank_sample[bank]=take?(2*rr+ky==8?extra[bank][word]:rows[2*rr+ky][bank][word]):0.0f;
                        }
                        lanes: for(int lane=0;lane<8;++lane){
#pragma HLS UNROLL
                            const int x=first+gx*8+lane,sx=x*2+int(kx)-1;
                            valid[lane]=row_valid&&x<ow&&sx>=0&&sx<width;
                            samples[lane]=bank_sample[(base+lane*2)&15];
                        }
                        acc[0][g]=ordered_mac<0>(acc[0][g],samples[0],w,valid[0]);
                        acc[1][g]=ordered_mac<1>(acc[1][g],samples[1],w,valid[1]);
                        acc[2][g]=ordered_mac<2>(acc[2][g],samples[2],w,valid[2]);
                        acc[3][g]=ordered_mac<3>(acc[3][g],samples[3],w,valid[3]);
                        acc[4][g]=ordered_mac<4>(acc[4][g],samples[4],w,valid[4]);
                        acc[5][g]=ordered_mac<5>(acc[5][g],samples[5],w,valid[5]);
                        acc[6][g]=ordered_mac<6>(acc[6][g],samples[6],w,valid[6]);
                        acc[7][g]=ordered_mac<7>(acc[7][g],samples[7],w,valid[7]);
                    }
                    store: for(int i=0;i<128;++i){
#pragma HLS PIPELINE II=1
                        const int rr=narrow?(i>>5):medium?pass*2+(i>>6):pass;
                        const int x=first+(narrow?(i&31):medium?(i&63):i);
                        if(yy+rr<oh&&x<ow)results.write(acc[i%8][i/8]+b);
                    }
                }
            }
        }
}
static void write_results(hls::stream<float>&results,float*output,int co,int oh,int ow,int yy){
#pragma HLS INLINE off
    const bool narrow=ow<=32,medium=ow<=64;
    for(int o=0;o<co;++o)for(int pass=0;pass<(narrow?1:medium?2:4);++pass)
        for(int first=0;first<(medium?1:ow);first+=128)for(int i=0;i<128;++i){
#pragma HLS PIPELINE II=1
            const int rr=narrow?(i>>5):medium?pass*2+(i>>6):pass;
            const int x=first+(narrow?(i&31):medium?(i&63):i);
            if(yy+rr<oh&&x<ow)output[(o*oh+yy+rr)*ow+x]=results.read();
        }
}
static void row_pipeline(const float rows[8][16][LUNG_CONV_ROW/16],const float extra[16][LUNG_CONV_ROW/16],const float*weight,
                         const float*bias,float*output,int ci,int co,int oh,int ow,
                         int yy,int height,int width){
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=rows complete dim=1
#pragma HLS ARRAY_PARTITION variable=rows complete dim=2
#pragma HLS ARRAY_PARTITION variable=extra complete dim=1
#pragma HLS DATAFLOW
    hls::stream<float>weights("weight_fifo"),biases("bias_fifo"),results("result_fifo");
#pragma HLS STREAM variable=weights depth=1024
#pragma HLS STREAM variable=biases depth=2
#pragma HLS STREAM variable=results depth=64
#pragma HLS BIND_STORAGE variable=results type=fifo impl=srl
    prefetch_weights(weight,bias,weights,biases,ci,co,ow);
    compute_channels(rows,extra,weights,biases,results,ci,co,oh,ow,yy,height,width);
    write_results(results,output,co,oh,ow,yy);
}
extern "C" int lung_conv_fp32(const float *input,const float *weight,const float *bias,
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
    // Four output rows share nine input rows and one weight load. Explicit
    // banks expose one read per bank rather than four ambiguous read ports.
    float rows[8][16][LUNG_CONV_ROW/16],extra[16][LUNG_CONV_ROW/16];
#pragma HLS ARRAY_PARTITION variable=rows complete dim=1
#pragma HLS ARRAY_PARTITION variable=rows complete dim=2
#pragma HLS ARRAY_PARTITION variable=extra complete dim=1
#pragma HLS BIND_STORAGE variable=rows type=ram_2p impl=bram latency=1
#pragma HLS BIND_STORAGE variable=extra type=ram_s2p impl=lutram latency=1
    row_pair: for(int yy=0;yy<oh;yy+=4){
        load_y: for(int ky=0;ky<9;++ky){
            const int sy=yy*2+ky-1;
            load_c: for(int c=0;c<ci;++c)load_x: for(int x=0;x<width;++x){
#pragma HLS PIPELINE II=1
                const int index=c*width+x;
                const float sample=(sy>=0&&sy<height)?input[(c*height+sy)*width+x]:0.0f;
                if(ky==8)extra[index&15][index>>4]=sample;
                else rows[ky][index&15][index>>4]=sample;
            }
        }
        row_pipeline(rows,extra,weight,bias,output,ci,co,oh,ow,yy,height,width);

    }
    return LUNG_CONV_OK;
}
