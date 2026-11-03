#include "dot.hpp"
extern "C" int lung_linear_int8(const uint8_t*x,const int8_t*w,int32_t*y,
                                int rows,int ci,int co,int xzero,int wzero){
#pragma HLS INTERFACE m_axi port=x offset=slave bundle=mem depth=32768 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=w offset=slave bundle=mem depth=262144 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=y offset=slave bundle=mem depth=32768 max_write_burst_length=64 num_write_outstanding=4
#pragma HLS INTERFACE s_axilite port=x offset=0x20 bundle=control
#pragma HLS INTERFACE s_axilite port=w offset=0x30 bundle=control
#pragma HLS INTERFACE s_axilite port=y offset=0x40 bundle=control
#pragma HLS INTERFACE s_axilite port=rows offset=0x50 bundle=control
#pragma HLS INTERFACE s_axilite port=ci offset=0x58 bundle=control
#pragma HLS INTERFACE s_axilite port=co offset=0x60 bundle=control
#pragma HLS INTERFACE s_axilite port=xzero offset=0x68 bundle=control
#pragma HLS INTERFACE s_axilite port=wzero offset=0x70 bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
 if(rows<1||rows>256||ci<1||ci>512||co<1||co>512||rows*ci>int(LINEAR_INPUT)||
    rows*co>int(LINEAR_OUTPUT)||xzero<0||xzero>255||wzero< -128||wzero>127)return -1;
 // |sum| <= 512*255*255=33292800: INT32 never overflows.
 uint8_t input[LINEAR_INPUT];int8_t weight[8][512];int32_t acc[8];
#pragma HLS BIND_STORAGE variable=input type=ram_1p impl=bram latency=1
#pragma HLS ARRAY_PARTITION variable=weight complete dim=1
#pragma HLS BIND_STORAGE variable=weight type=ram_1p impl=bram latency=1
#pragma HLS ARRAY_PARTITION variable=acc complete
 load_input:for(int i=0;i<rows*ci;++i){
#pragma HLS PIPELINE II=1
  input[i]=x[i];
 }
 output_tile:for(int first=0;first<co;first+=8){
  load_lane:for(int lane=0;lane<8;++lane)load_weight:for(int k=0;k<ci;++k){
#pragma HLS PIPELINE II=1
   weight[lane][k]=first+lane<co?w[(first+lane)*ci+k]:int8_t(wzero);
  }
  output_row:for(int row=0;row<rows;++row){
   clear:for(int lane=0;lane<8;++lane){
#pragma HLS UNROLL
    acc[lane]=0;
   }
   dot_k:for(int k=0;k<ci;++k){
#pragma HLS PIPELINE II=1
    const int16_t a=int(input[row*ci+k])-xzero;
    mac:for(int lane=0;lane<8;++lane){
#pragma HLS UNROLL
     const int16_t b=int(weight[lane][k])-wzero;
     int32_t product=a*b;
#pragma HLS BIND_OP variable=product op=mul impl=dsp latency=2
     int32_t sum=acc[lane]+product;
#pragma HLS BIND_OP variable=sum op=add impl=fabric latency=1
     acc[lane]=sum;
    }
   }
   store:for(int lane=0;lane<8;++lane){
#pragma HLS PIPELINE II=1
    if(first+lane<co)y[row*co+first+lane]=acc[lane];
   }
  }
 }
 return LINEAR_OK;
}
