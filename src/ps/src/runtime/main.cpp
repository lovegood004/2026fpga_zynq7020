#include "lung_pl.hpp"
#include "linear_pl.hpp"
#define shared_arm_enable lung_runtime_enable
#include "runtime.cpp"
#undef shared_arm_enable
extern "C" void shared_arm_enable(int);
extern "C" void lung_runtime_enable(int enabled){shared_arm_enable(enabled);lung_pl_enable(true);linear_pl_enable(true);}
