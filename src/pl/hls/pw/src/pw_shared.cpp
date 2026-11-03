#include "pw_shared.hpp"
#include "hls_stream.h"
#include "xf_blas/helpers.hpp"
#include "xf_blas/gemm/runtimeSystolicArray.hpp"
using Word=xf::blas::WideType<float,4>;
using Stream=hls::stream<Word::t_TypeInt>;
static void feed_w(const float w[PW_MAX_C][PW_MAX_C],int oc,int kp,int np,Stream &a){
    for(int tile=0;tile<np/4;++tile) for(int k=0;k<kp;++k){
#pragma HLS PIPELINE II=1
        Word v;
        for(int m=0;m<4;++m){
#pragma HLS UNROLL
            v[m]=w[oc+m][k];
        }
        a.write(v);
    }
}
static void feed_x(const float x[PW_MAX_C][PW_TILE_N],int kp,int np,Stream &b){
    for(int tile=0;tile<np/4;++tile) for(int k=0;k<kp;++k){
#pragma HLS PIPELINE II=1
        Word v;
        for(int n=0;n<4;++n){
#pragma HLS UNROLL
            v[n]=x[k][tile*4+n];
        }
        b.write(v);
    }
}
static void collect(Stream &c,float y[4][PW_TILE_N],int np){
    for(int tile=0;tile<np/4;++tile) for(int m=0;m<4;++m){
#pragma HLS PIPELINE II=1
        Word v=c.read();
        for(int n=0;n<4;++n){
#pragma HLS UNROLL
            y[m][tile*4+n]=v[n];
        }
    }
}
static void compute(const float w[PW_MAX_C][PW_MAX_C],const float x[PW_MAX_C][PW_TILE_N],
                    float y[4][PW_TILE_N],int oc,int kp,int np){
#pragma HLS INLINE off
#pragma HLS DATAFLOW
    Stream a("weights"),b("input"),c("output");
#pragma HLS STREAM variable=a depth=32
#pragma HLS STREAM variable=b depth=32
#pragma HLS STREAM variable=c depth=32
    feed_w(w,oc,kp,np,a);
    feed_x(x,kp,np,b);
    xf::blas::RuntimeSystolicArray<float,4> array(np/4,kp);
    array.process(a,b,c);
    collect(c,y,np);
}
extern "C" int pw_shared(const float *weights,const float *input,float *output,int ci,int co,int length){
#pragma HLS INTERFACE m_axi port=weights offset=slave bundle=mem depth=9216 max_read_burst_length=64
#pragma HLS INTERFACE m_axi port=input offset=slave bundle=mem depth=196608 max_read_burst_length=64
#pragma HLS INTERFACE m_axi port=output offset=slave bundle=mem depth=196608 max_write_burst_length=64
#pragma HLS INTERFACE s_axilite port=weights bundle=control
#pragma HLS INTERFACE s_axilite port=input bundle=control
#pragma HLS INTERFACE s_axilite port=output bundle=control
#pragma HLS INTERFACE s_axilite port=ci bundle=control
#pragma HLS INTERFACE s_axilite port=co bundle=control
#pragma HLS INTERFACE s_axilite port=length bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
    if(ci<1||ci>PW_MAX_C||co<1||co>PW_MAX_C||length<1||length>PW_MAX_N) return -1;
    int kp=(ci+3)/4*4; if(kp<8)kp=8;
    int mp=(co+3)/4*4;
    float w[PW_MAX_C][PW_MAX_C],x[PW_MAX_C][PW_TILE_N],y[4][PW_TILE_N];
#pragma HLS ARRAY_PARTITION variable=w cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=x cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=y cyclic factor=4 dim=2
    // Cache the layer's weights once; pad only inside local memories.
    for(int m=0;m<mp;++m) for(int k=0;k<kp;++k){
#pragma HLS PIPELINE II=1
        w[m][k]=(m<co&&k<ci)?weights[m*ci+k]:0.0f;
    }
    // Cache each input tile once and reuse it for every output channel group.
    for(int pos=0;pos<length;pos+=PW_TILE_N){
        int count=length-pos; if(count>PW_TILE_N) count=PW_TILE_N;
        int np=(count+3)/4*4;
        for(int k=0;k<kp;++k) for(int n=0;n<np;++n){
#pragma HLS PIPELINE II=1
            x[k][n]=(k<ci&&n<count)?input[k*length+pos+n]:0.0f;
        }
        for(int oc=0;oc<mp;oc+=4){
            compute(w,x,y,oc,kp,np);
            for(int m=0;m<4;++m) for(int n=0;n<count;++n){
#pragma HLS PIPELINE II=1
                if(oc+m<co) output[(oc+m)*length+pos+n]=y[m][n];
            }
        }
    }
    return 0;
}
