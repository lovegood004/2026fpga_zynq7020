#include "events.hpp"
namespace {void(*event_hook)(unsigned)=nullptr;}
extern "C" void lung_set_event_hook(void(*hook)(unsigned)){event_hook=hook;}
extern "C" void lung_phase_complete(unsigned phase){if(event_hook)event_hook(phase);}
