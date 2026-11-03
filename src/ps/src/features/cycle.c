#include "cycle.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
int feature_cycle_infer_chunk_serviced(const float *,int *,size_t *,void(*)(unsigned),const int *);
static int cycle_running=0;
static feature_cycle_result cycle_finish(feature_cycle_result r){cycle_running=0;return r;}
const char *feature_cycle_state_name(int s){
    static const char *names[]={"Unknown","S1","Systole","S2","Diastole"};
    return s>=0&&s<5?names[s]:names[0];
}
feature_cycle_result feature_cycle_pcm16(const int16_t *pcm,size_t count,int rate,
                                       uint8_t *states,size_t capacity) {
    return feature_cycle_pcm16_serviced(pcm,count,rate,states,capacity,NULL,NULL);
}
feature_cycle_result feature_cycle_pcm16_serviced(const int16_t*pcm,size_t count,int rate,uint8_t*states,size_t capacity,void(*service)(unsigned),const int*cancelled){
    feature_cycle_result r={0};r.reason="invalid input";
    if(cycle_running){r.reason="cycle model busy";return r;}
    cycle_running=1;
    if(capacity>3000){r.reason="output capacity";return cycle_finish(r);}
    if(states)memset(states,0,capacity);
    if(!pcm||!states||rate<1000||rate>192000||count==0||count>(size_t)rate*60)return cycle_finish(r);
    size_t n=(size_t)((double)count*1000.0/rate+0.5);
    size_t frames=n/20;
    if(frames<50){r.reason="less than one second";return cycle_finish(r);}
    if(frames>capacity){r.reason="output capacity";return cycle_finish(r);}
    float *audio=(float*)malloc(n*sizeof(float));
    if(!audio){r.reason="preprocess memory";return cycle_finish(r);}
    double sum=0,sum2=0;
    for(size_t i=0;i<n;i++) {
        if(i%1024==0){if(service)service(9);if(cancelled&&*cancelled){free(audio);r.reason="analysis cancelled";return cycle_finish(r);}}
        double pos=(double)i*(count-1)/(n>1?n-1:1);
        size_t j=(size_t)pos;if(j>=count)j=count-1;
        float a=pcm[j]/32768.0f,b=pcm[j+1<count?j+1:j]/32768.0f;
        audio[i]=(float)(a+(b-a)*(pos-j));sum+=audio[i];sum2+=(double)audio[i]*audio[i];
    }
    double mean=sum/n,var=sum2/n-mean*mean;
    if(var<1e-10){free(audio);r.reason="silence or constant: unknown";return cycle_finish(r);}
    double sd=sqrt(var)+1e-12;
    for(size_t i=0;i<n;i++)audio[i]=(float)((audio[i]-mean)/sd);
    /* The baremetal stack is small; this non-reentrant module owns its workspace. */
    static float segment[10000];static int predicted[500];
    for(size_t f0=0;f0<frames;f0+=500) {
        size_t start=f0*20,take=n-start;if(take>10000)take=10000;
        memcpy(segment,audio+start,take*sizeof(float));
        memset(segment+take,0,(10000-take)*sizeof(float));
        size_t peak=0;
        int ok=feature_cycle_infer_chunk_serviced(segment,predicted,&peak,service,cancelled);
        if(!ok) {
            r.arena_highwater=peak;memset(states,0,capacity);free(audio);r.reason=cancelled&&*cancelled?"analysis cancelled":"model scratch exhausted";return cycle_finish(r);
        }
        if(peak>r.arena_highwater)r.arena_highwater=peak;
        size_t copy=frames-f0;if(copy>500)copy=500;
        for(size_t j=0;j<copy;j++) {
            if(predicted[j]<1||predicted[j]>4){memset(states,0,capacity);free(audio);r.reason="invalid model state";return cycle_finish(r);}
            states[f0+j]=(uint8_t)predicted[j];
        }
    }
    free(audio);r.frames=(int)frames;r.duration_s=(float)frames/50;
    float first_s1=-1,last_s1=-1;
    for(size_t i=0;i<frames;i++) {
        int current=states[i],previous=i?states[i-1]:0;
        if(i&&current!=previous&&current!=previous%4+1)r.transitions_invalid++;
        if(current==1&&previous!=1){r.s1_events++;last_s1=(float)i/50;if(first_s1<0)first_s1=last_s1;}
        if(current==3&&previous!=3)r.s2_events++;
    }
    if(r.s1_events>=2&&last_s1>first_s1)r.bpm_from_s1=60*(r.s1_events-1)/(last_s1-first_s1);
    /* Independent ten-second windows may disagree at the boundary. Keep output
       but label this unverified; do not coerce states or change the original model. */
    r.valid=1;r.reason=r.transitions_invalid?"original model; boundary discontinuity":"original model; annotation unverified";
    return cycle_finish(r);
}
