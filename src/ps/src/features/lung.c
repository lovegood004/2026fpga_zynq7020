#include "lung.h"
feature_lung_result feature_lung_pcm16(const int16_t *pcm,size_t count,int rate) {
    return feature_lung_pcm16_serviced(pcm,count,rate,0);
}
