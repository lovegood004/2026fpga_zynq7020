// Only integer indexing and table access are optimized. Complex-double
// multiply/add order is the same j=0..p-1 order as the reference implementation.
struct MixedStage {size_t n,m,step,radix_step;unsigned p;};
struct MixedPlan {
    MixedStage stages[24]{};unsigned used=0;
    Z w2[4],w3[9],w5[25],w7[49];
    bool prepare(size_t n,const Z*roots){
        const size_t root_n=n;unsigned seen=0;
        while(n>1){
            if(used>=24)return false;
            unsigned p=2;while(p<=31&&n%p)++p;
            if(p>31){stages[used++]={n,0,0,0,0};break;}
            stages[used++]={n,n/p,root_n/n,root_n/p,p};
            if((p==2||p==3||p==5||p==7)&&!(seen&(1u<<p))){
                Z*table=p==2?w2:p==3?w3:p==5?w5:w7;
                for(unsigned q=0;q<p;++q)for(unsigned j=0;j<p;++j)
                    table[q*p+j]=roots[((j*q)%p)*(root_n/p)];
                seen|=1u<<p;
            }
            n/=p;
        }
        return true;
    }
};
template<unsigned P,unsigned J> struct OrderedSum {
    static void add(Z &sum,const Z*values,const Z*weights){
        OrderedSum<P,J-1>::add(sum,values,weights);
        sum+=values[J-1]*weights[J-1];
    }
};
template<unsigned P> struct OrderedSum<P,0>{static void add(Z&,const Z*,const Z*){}};
template<unsigned P> bool planned_butterfly(Z*out,const Z*roots,const MixedStage&s,const Z*weights){
    Z values[P],result[P];
    for(size_t k=0;k<s.m;++k){
        const size_t offset=k*s.step;
        for(unsigned j=0;j<P;++j)values[j]=out[j*s.m+k]*roots[j*offset];
        for(unsigned q=0;q<P;++q){result[q]=0.;OrderedSum<P,P>::add(result[q],values,weights+q*P);}
        for(unsigned q=0;q<P;++q)out[q*s.m+k]=result[q];
        if(s.n>=256&&k%16==0&&!checkpoint())return false;
    }
    return true;
}
bool mixed_planned(Z*out,const Z*in,size_t stride,size_t n,const Z*roots,bool inverse,const MixedPlan&plan,unsigned level){
    if(n==1){out[0]=in[0];return true;}
    if(level>=plan.used)return false;
    const auto&s=plan.stages[level];
    if(!s.p)return bluestein(out,in,stride,n,inverse);
    for(unsigned j=0;j<s.p;++j)
        if(!mixed_planned(out+j*s.m,in+j*stride,stride*s.p,s.m,roots,inverse,plan,level+1))return false;
    switch(s.p){
        case 2:return planned_butterfly<2>(out,roots,s,plan.w2);
        case 3:return planned_butterfly<3>(out,roots,s,plan.w3);
        case 5:return planned_butterfly<5>(out,roots,s,plan.w5);
        case 7:return planned_butterfly<7>(out,roots,s,plan.w7);
    }
    // Other supported radices retain the original dynamic butterfly and order.
    Z values[31],result[31];
    for(size_t k=0;k<s.m;++k){
        for(unsigned j=0;j<s.p;++j)values[j]=out[j*s.m+k]*roots[j*k*s.step];
        for(unsigned q=0;q<s.p;++q){result[q]=0.;for(unsigned j=0;j<s.p;++j)result[q]+=values[j]*roots[((j*q)%s.p)*s.radix_step];}
        for(unsigned q=0;q<s.p;++q)out[q*s.m+k]=result[q];
        if(n>=256&&k%16==0&&!checkpoint())return false;
    }
    return true;
}
