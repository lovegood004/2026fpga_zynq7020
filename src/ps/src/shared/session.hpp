#pragma once
#include "mobileheart_classifier.h"
#include <cmath>
#include <cstring>
// Pure calculation extracted from integration/main.cpp; no BLE/display/SD stubs.
// Preserve independent sigmoid scores; they are NOT softmax probabilities.
struct Position {
    int total=0,abnormal=0;
    float sum[5]={},mean[5]={};
    float bpm=0;
    bool add(const mobileheart::Prediction &p) {
        if(!p.valid)return false;
        for(int i=0;i<5;++i) if(!std::isfinite(p.ensemble[i])||p.ensemble[i]<0||p.ensemble[i]>1)return false;
        bool ab=false;
        for(int i=0;i<5;++i){sum[i]+=p.ensemble[i];if(i<4&&p.ensemble[i]>0.5f)ab=true;}
        ++total;if(ab)++abnormal;
        for(int i=0;i<5;++i)mean[i]=sum[i]/total;
        return true;
    }
    float percent() const {return total?100.0f*abnormal/total:0;}
    bool positive() const {return total&&percent()>=40.0f;}
    const char *hint() const {
        if(!total)return "N/A";
        float s=0;for(float v:mean)s+=v;
        if(!std::isfinite(s)||s<0.5f)return "N/A";
        if(!positive())return "N";
        int top=0,second=1;
        if(mean[second]>mean[top]){int tmp=top;top=second;second=tmp;}
        for(int i=2;i<4;++i){if(mean[i]>mean[top]){second=top;top=i;}else if(mean[i]>mean[second])second=i;}
        if(mean[top]<0.5f||mean[top]-mean[second]<0.08f)return "?";
        return mobileheart::label_short(top);
    }
};
struct Session {
    Position positions[5];
    bool positive()const {for(const auto&p:positions)if(p.positive())return true;return false;}
    int total()const{int n=0;for(const auto&p:positions)n+=p.total;return n;}
    int abnormal()const{int n=0;for(const auto&p:positions)n+=p.abnormal;return n;}
    int valid()const{int n=0;for(const auto&p:positions)if(p.total>0&&std::isfinite(p.bpm)&&p.bpm>0)++n;return n;}
    float average_bpm()const{float s=0;for(const auto&p:positions)if(p.total>0&&std::isfinite(p.bpm)&&p.bpm>0)s+=p.bpm;return valid()?s/valid():0;}
};
static inline bool session_selftest(){
    Position p; mobileheart::Prediction a{};a.valid=true;a.ensemble[0]=0.5f;a.ensemble[4]=0.8f;
    if(!p.add(a)||p.abnormal!=0)return false; // Strict > 0.5 for each disease.
    a.ensemble[0]=0.9f;
    if(!p.add(a)||!p.add(a))return false;
    a.ensemble[0]=0;
    if(!p.add(a)||!p.add(a)||p.total!=5||p.abnormal!=2||!p.positive())return false; // >=40%.
    Position empty;if(std::strcmp(empty.hint(),"N/A"))return false;
    Session s;s.positions[3]=p;s.positions[3].bpm=72;
    if(!s.positive()||s.valid()!=1||s.average_bpm()!=72)return false;
    a.valid=false;if(p.add(a)||p.total!=5)return false;
    return true;
}
