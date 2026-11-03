// Architecture borrowing: explicit prepare/compute/commit stages and bounded
// reusable storage. No external FFT/denoising/UKF algorithm code is copied.
struct RootSlot {Buffer<Z> data;MixedPlan plan;bool ready=false;};
struct FrontCache {
 RootSlot roots[2];Buffer<Z> scratch;size_t capacity=0;Z twiddle[511];bool stft_ready=false,scratch_used=false;
 void clear(){for(auto&r:roots){r.data.reset();r.ready=false;r.plan.used=0;}scratch.reset();capacity=0;stft_ready=scratch_used=false;}
 // Reserve all persistent blocks before allocating per-recording audio/FFT
 // buffers. Interleaving retained roots with temporary spectra fragments the
 // fixed newlib heap and can prevent the later model Workspace allocation.
 bool reserve(){
  if(!roots[0].data){roots[0].data=allocate<Z>(165375);if(!roots[0].data)return false;}
  if(!roots[1].data){roots[1].data=allocate<Z>(60000);if(!roots[1].data)return false;}
  if(!scratch){scratch=allocate<Z>(165375);if(!scratch)return false;capacity=165375;}
  return true;
 }
 uint64_t bytes()const{return (roots[0].data?165375ull*16:0)+(roots[1].data?60000ull*16:0)+capacity*16ull;}
 unsigned ready()const{return (roots[0].ready?1u:0u)|(roots[1].ready?2u:0u)|(stft_ready?4u:0u)|(capacity?8u:0u);}
} front_cache;
bool front_active=false,front_cache_enabled=false;Service front_original_service=nullptr;
void front_service(unsigned stage){const auto at=front_profile::now();front_original_service(stage);front_profile::state.service_us+=front_profile::now()-at;}
struct FrontSession {
 Work &work;bool entered=false,valid=false;uint64_t started=0;
 FrontSession(Work&w,size_t count,unsigned rate):work(w){
  if(front_active)return;
  front_active=entered=true;front_cache_enabled=lung_front_optimized&&count==165375&&rate==11025;
  if(!front_cache_enabled)front_cache.clear();
  front_profile::begin(work.control?work.control->clock_us:nullptr);
  front_profile::state.samples=count;front_profile::state.rate=rate;started=front_profile::now();
  front_original_service=work.service;
  if(lung_front_trace&&work.service)work.service=front_service;
 }
 ~FrontSession(){if(!entered)return;front_profile::state.total_us=front_profile::now()-started;
  front_profile::publish(valid,front_cache.ready(),front_cache.bytes());front_profile::end();
  work.service=front_original_service;front_original_service=nullptr;front_active=front_cache_enabled=false;}
};
extern "C" void lung_front_cache_clear(){if(!front_active)front_cache.clear();}
extern "C" uint64_t lung_front_cache_bytes(){return front_cache.bytes();}
bool prepare_cache(){const auto at=front_profile::now();const bool okay=!front_cache_enabled||front_cache.reserve();
 front_profile::state.prepare_us=front_profile::now()-at;return okay;}
bool radix2(Z*a,size_t n,bool inverse){
 if(!front_cache_enabled||n!=512||inverse)return radix2_reference(a,n,inverse);
 if(!front_cache.stft_ready){
  for(size_t len=2;len<=512;len<<=1){const Z root=std::polar(1.,-2*pi/len);Z w=1.;
   for(size_t j=0;j<len/2;++j){front_cache.twiddle[len/2-1+j]=w;w*=root;}}
  front_cache.stft_ready=true;++front_profile::state.stft_plan_builds;
 }
 unsigned butterflies=0;
 for(size_t i=1,j=0;i<n;i++){size_t b=n>>1;for(;j&b;b>>=1)j^=b;j^=b;if(i<j)std::swap(a[i],a[j]);if(i%2048==0&&!checkpoint())return false;}
 for(size_t len=2;len<=n;len<<=1){const Z*twiddle=front_cache.twiddle+len/2-1;
  for(size_t i=0;i<n;i+=len)for(size_t j=0;j<len/2;++j){
   Z u=a[i+j],v=twiddle[j]*a[i+j+len/2];a[i+j]=u+v;a[i+j+len/2]=u-v;
   if(++butterflies==1024){butterflies=0;if(!checkpoint())return false;}}
  if(!checkpoint())return false;
 }
 return checkpoint();
}
bool cached_fft(Z*a,size_t n,bool inverse){
 auto&r=*front_profile::record;const auto allocate_at=front_profile::now();
 const unsigned slot=inverse?1u:0u;
 if((slot==0&&n!=165375)||(slot==1&&n!=60000))return fft_reference(a,n,inverse);
 auto &entry=front_cache.roots[slot];r.root_hit=entry.ready;r.scratch_reused=front_cache.scratch_used;front_cache.scratch_used=true;
 if(front_cache.capacity<n){auto next=allocate<Z>(n);if(!next)return false;front_cache.scratch=std::move(next);front_cache.capacity=n;}
 if(!entry.data){entry.data=allocate<Z>(n);if(!entry.data)return false;}
 r.alloc_us=front_profile::now()-allocate_at;const auto roots_at=front_profile::now();
 for(size_t i=0;i<n;++i){
  if(!entry.ready)entry.data[i]=std::polar(1.,(inverse?2:-2)*pi*i/n);
  // Preserve every original checkpoint even when reusing immutable roots.
  if(i%256==0&&!checkpoint())return false;
 }
 if(!entry.ready){entry.plan.used=0;if(!entry.plan.prepare(n,entry.data.get()))return false;entry.ready=true;++front_profile::state.root_builds;}
 r.roots_us=front_profile::now()-roots_at;const auto transform_at=front_profile::now();
 if(lung_fft_optimized){if(!mixed_planned(front_cache.scratch.get(),a,1,n,entry.data.get(),inverse,entry.plan,0))return false;}
 else if(!mixed(front_cache.scratch.get(),a,1,n,entry.data.get(),n,inverse))return false;
 r.transform_us=front_profile::now()-transform_at;const auto copy_at=front_profile::now();
 for(size_t i=0;i<n;++i){a[i]=front_cache.scratch[i]/double(inverse?n:1);if(i%2048==0&&!checkpoint())return false;}
 r.copy_us=front_profile::now()-copy_at;return true;
}
bool fft(Z*a,size_t n,bool inverse){
 if(!front_active)return fft_reference(a,n,inverse);
 const unsigned index=front_profile::state.fft_calls++;if(index>=2)return false;
 auto&r=front_profile::state.fft[index];r.n=uint32_t(n);front_profile::record=&r;
 const auto at=front_profile::now();const bool okay=front_cache_enabled?cached_fft(a,n,inverse):fft_reference(a,n,inverse);
 r.total_us=front_profile::now()-at;r.error=!okay;front_profile::record=nullptr;return okay;
}
