#pragma once
#include <cstdint>
namespace gru_profile {
struct Direction {uint64_t input_us=0,recurrent_us=0,activation_us=0,service_us=0,total_us=0;uint32_t input_steps=0,recurrent_steps=0,activation_steps=0,polls=0;};
struct Report {uint32_t magic=0x47525531,version=1,sequence=0,optimized=0,tracing=0,valid=0,directions=0,sequence_end=0;Direction d[4];};
static_assert(sizeof(Report)==256,"GRU ABI");
void reset();void publish(bool valid);
extern Report state;
}
extern "C" {extern unsigned lung_gru_optimized,lung_gru_trace;extern volatile gru_profile::Report lung_gru_report;}
