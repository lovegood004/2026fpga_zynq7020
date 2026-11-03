// Four independent output accumulators. Each keeps the reference's original
// k=0..N-1 order and adds its own bias only after completing the dot product.
template<unsigned N> void gru_dot4(const float*x,const float*w,const float*b,float*y){
 for(unsigned g=0;g<384;g+=4){
  const float*w0=w+g*N,*w1=w0+N,*w2=w1+N,*w3=w2+N;
  float a=0,c=0,d=0,e=0;
  for(unsigned k=0;k<N;++k){const float v=x[k];a+=v*w0[k];c+=v*w1[k];d+=v*w2[k];e+=v*w3[k];}
  y[g]=a+b[g];y[g+1]=c+b[g+1];y[g+2]=d+b[g+2];y[g+3]=e+b[g+3];
 }
}
