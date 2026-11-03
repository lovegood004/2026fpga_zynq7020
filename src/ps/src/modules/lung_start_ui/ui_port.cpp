#include "ui_port.hpp"
#include "keys.h"
#include "esp_timer.h"
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace {
constexpr View result_pages[]={View::Summary,View::Wave,View::Stft,View::Table,View::Scores,View::Cycle};
constexpr unsigned result_page_count=sizeof(result_pages)/sizeof(result_pages[0]);
bool valid_scores(const Result &r){
    if(!r.ready||r.failed||r.position.total<=0)return false;
    for(float s:r.position.mean)if(!std::isfinite(s)||s<0||s>1)return false;
    return true;
}
bool select_result(App &a){
    if(a.results[a.site].ready)return true;
    for(unsigned i=0;i<5;i++)if(a.results[i].ready){a.site=i;return true;}
    return false;
}
bool next_missing(App &a){
    for(unsigned n=1;n<=5;n++){unsigned i=(a.site+n)%5;
        if(!a.results[i].ready){a.site=i;return true;}}
    return false;
}
bool result_view(View v){
    return v==View::Summary||v==View::Wave||v==View::Stft||v==View::Table||
        v==View::Scores||v==View::Cycle||v==View::LungResult;
}
void prepare_measurement(App &a){
    if(!a.lung_mode)next_missing(a);
    a.view=View::Move;a.page=0;a.message="";
}
bool saved_pcm(const App &a,unsigned pos){
    return pos<5&&(a.sink->valid_mask&(1u<<pos))&&
        raw::valid(a.sink->meta[pos])&&a.sink->meta[pos].site==pos;
}
bool select_saved(App &a,unsigned first,bool reverse=false){
    for(unsigned n=0;n<5;n++){unsigned pos=(first+(reverse?5-n:n))%5;
        if(saved_pcm(a,pos)){a.site=pos;return true;}}
    return false;
}
void request_cycle(App &a,bool force=false){
    if(a.lung_mode||a.view!=View::Cycle||!a.cycle_marks_enabled)return;
    if(a.results[a.site].ready&&saved_pcm(a,a.site)&&(force||!a.cycles[a.site].attempted))a.feature_request=a.site+1;
}
uint64_t lung_clock(){return uint64_t(esp_timer_get_time());}
}
void App::tick(uint32_t t){now=t;sink->tick(t);
    if(capture_lung&&(sink->busy()||sink->pending_ready)){
        if(stats_session!=sink->wanted.session||capture_stats.samples>sink->points){
            stats_session=sink->wanted.session;capture_stats={0,0,32767,-32768};
        }
        const size_t end=std::min(size_t(sink->points),std::min(size_t(raw::Samples),capture_stats.samples+4096));
        const auto*pcm=sink->staging_data();
        while(capture_stats.samples<end){int v=pcm[capture_stats.samples++];capture_stats.sum+=v;
            capture_stats.minimum=std::min(capture_stats.minimum,v);capture_stats.maximum=std::max(capture_stats.maximum,v);}
    }
    if(sink->pending_ready&&!analysis_request&&!analysing){analysis_request=sink->pending_descriptor().site+1;view=View::Capture;}
    if(view==View::Capture&&!sink->busy()&&!analysis_request&&!analysing)view=View::Connect;
}
void App::key(unsigned which,unsigned ev,uint32_t t){
    now=t;if(!ev||which>5)return;
    if(analysing){
        if((capture_lung||analysis_cycle)&&((which==5&&(ev==KEY_SHORT||ev==KEY_LONG))||(which==1&&ev==KEY_LONG))){
            analysis_cancelled=1;analysis_to_menu=which==1;
        }
        return;
    }
    if(which>=2){ // PL silk KEY1..4. One long action, no repeat on release.
        if(which==4&&ev==KEY_LONG&&!lung_mode&&view==View::Cycle&&!sink->busy()&&
           !sink->pending_ready&&!analysis_request&&!feature_request){cycle_marks_enabled=true;request_cycle(*this,true);return;}
        if(ev==KEY_SHORT||ev==KEY_LONG){constexpr UiAction actions[]={UiAction::Previous,UiAction::Next,UiAction::Confirm,UiAction::Back};
            action(actions[which-2],t);}
        return;
    }
    // Long exit takes precedence over the busy-page short cancel action.
    if(which==1&&ev==KEY_LONG){
        if(sink->busy())sink->stop(t);
        if(sink->pending_ready){sink->discard_pending();analysis_request=0;}
        feature_request=0;analysis_request=0;reanalyse_saved=false;view=View::Menu;message="";return;
    }
    if(sink->busy()){
        if(which==1&&ev==KEY_SHORT&&sink->phase!=raw::Phase::Stopping){sink->stop(t);view=View::Connect;message="Capture cancelled";}return;
    }
    if(sink->pending_ready||analysis_request||feature_request)return;
    if((view==View::Wave||(!lung_mode&&view==View::Cycle))&&ev==KEY_SHORT){
        const unsigned count=std::min(11u,lung_mode?lung_wave.count:previews[site].wave_count),last=count?count-1:0;
        auto &window=lung_mode?lung_wave_window:wave_window;
        if(window>last)window=last;
        if(which==0){if(window)--window;}
        else if(window<last)++window;
        return;
    }
    if(view==View::Menu){
        if(which==0){menu=(menu+(ev==KEY_LONG?4:1))%5;return;}
        if(ev!=KEY_SHORT)return;
        switch(menu){case 0:lung_mode=false;site=0;view=View::Connect;break;
        case 1:lung_mode=true;site=0;view=View::Connect;break;
        case 2:view=View::NewExamConfirm;break;
        case 3:lung_mode=false;if(select_saved(*this,site)){view=View::AnalyzeSaved;page=0;message="";}else{view=View::Unavailable;message="No saved PCM / record first";}break;
        case 4:view=View::Status;break;
        default:view=View::Unavailable;message="Lung model not ported";break;}return;
    }
    if(view==View::AnalyzeSaved){
        if(which==0){select_saved(*this,(site+(ev==KEY_LONG?4:1))%5,ev==KEY_LONG);return;}
        if(ev==KEY_SHORT&&saved_pcm(*this,site)&&!sink->pending_ready){
            reanalyse_saved=true;capture_lung=false;analysis_request=site+1;message="";
        }return;
    }
    if(view==View::NewExamConfirm){
        if(which==0){view=View::Menu;return;}
        if(ev==KEY_SHORT&&sink->clear_committed()){
            for(unsigned i=0;i<5;i++){results[i]=Result{};previews[i]=Preview{};timings[i]=StageTiming{};}
            for(auto &c:cycles)c=Cycle{};cycle_marks_enabled=false;
            lung_ready=false;lung_result=feature_lung_result{};lung_preview=Preview{};lung_meta=raw::Descriptor{};
            lung_timing=StageTiming{};lung_wave.count=0;lung_wave_window=0;feature_request=0;lung_mode=false;
            ++examination;if(!examination)++examination;
            analysis_request=0;reanalyse_saved=false;seen_completed=sink->completed;site=0;page=0;
            view=View::Connect;message="New examination / no recording yet";
        }return;
    }
    if(view==View::Connect||view==View::Move){
        if(which==0){
            if(lung_mode){if(ev==KEY_LONG&&lung_ready)view=View::LungResult;return;}
            if(ev==KEY_LONG){if(select_result(*this)){view=View::Summary;page=0;message="";}
                else message="No analysed RAM session";}
            else site=(site+1)%5;
            return;
        }
        if(ev==KEY_SHORT){
            if(!session)session=sink->peer_session;uint32_t id=++session;if(!id)id=++session;
            if(sink->start(id,lung_mode?0:site,t)){capture_lung=lung_mode;view=View::Capture;message="Previous result kept until success";}
            else message="Peer not ready / raw v2 only";
        }return;
    }
    if(lung_mode&&(view==View::LungResult||view==View::Wave||view==View::Stft)){
        if(which==0){constexpr View pages[]={View::LungResult,View::Wave,View::Stft};
            unsigned i=0;while(i<3&&pages[i]!=view)++i;view=pages[(i+(ev==KEY_LONG?2:1))%3];}
        else if(ev==KEY_SHORT&&view==View::LungResult)view=View::Connect;
        return;
    }
    if(view==View::Summary||view==View::Wave||view==View::Stft||view==View::Table||view==View::Scores||view==View::Cycle){
        if(which==0){
            unsigned current=0;while(current<result_page_count&&result_pages[current]!=view)++current;
            page=(current+(ev==KEY_LONG?result_page_count-1:1))%result_page_count;view=result_pages[page];request_cycle(*this);
        }
        else if(ev==KEY_SHORT){if(view==View::Cycle){
                if(results[site].ready&&(sink->valid_mask&(1u<<site)))feature_request=site+1;
                else message="No committed heart recording";
            }else if(view==View::Wave||view==View::Stft||view==View::Scores){
                action(UiAction::NextSite,t);}
            else if(view==View::Summary){prepare_measurement(*this);}
            else{select_result(*this);page=1;view=View::Wave;}}
    }
}
void App::key_batch(const unsigned events[6],uint32_t t){
    // Cancel wins over every simultaneous navigation or confirmation event.
    if(events[5]){key(5,events[5],t);return;}
    if(events[1]==KEY_LONG){key(1,events[1],t);return;}
    unsigned count=0,which=0;
    for(unsigned i=0;i<6;i++)if(events[i]){++count;which=i;}
    if(count==1)key(which,events[which],t);
}
void App::action(UiAction input,uint32_t t){
    now=t;if(analysing)return;
    if(input==UiAction::Menu){key(1,KEY_LONG,t);return;}
    // Pending input belongs to an explicit analysis/commit transaction. Leave
    // it untouched until analysis or an explicit cancel; navigation cannot START.
    if(sink->pending_ready||analysis_request||feature_request){
        if(input==UiAction::Back){
            if(sink->pending_ready)sink->discard_pending();
            analysis_request=feature_request=0;reanalyse_saved=false;
            view=View::Connect;message="Capture cancelled";
        }return;
    }
    if(sink->busy()){
        if(input==UiAction::Back)key(1,KEY_SHORT,t);
        return;
    }
    if(input==UiAction::Measure){
        if(view==View::Summary||view==View::LungResult)prepare_measurement(*this);
        return;
    }
    if(input==UiAction::Back){
        message="";
        if(view==View::Connect||view==View::Move){
            if(lung_mode&&lung_ready)view=View::LungResult;
            else if(!lung_mode&&select_result(*this))view=View::Summary;
            else view=View::Menu;
        }else view=View::Menu;
        return;
    }
    if(input==UiAction::NextSite){
        if(!lung_mode&&result_view(view)){
            for(unsigned n=1;n<=5;n++){unsigned i=(site+n)%5;
                if(results[i].ready){site=i;break;}}
            if(previews[site].wave_count)wave_window=std::min(wave_window,previews[site].wave_count-1);
            request_cycle(*this);
        }return;
    }
    if(input==UiAction::Confirm){
        if(lung_mode&&view==View::Wave)return; // PL3 must not masquerade as PS2 sliding.
        if(!lung_mode&&(view==View::Wave||view==View::Cycle)){action(UiAction::NextSite,t);return;}
        // Share the PS confirmation action: browse a recorded site on detail
        // pages; only the result overview opens the next recording setup.
        key(1,KEY_SHORT,t);return;
    }
    const bool reverse=input==UiAction::Previous;
    if(view==View::NewExamConfirm)return; // movement never clears a group
    if(view==View::Connect||view==View::Move){
        if(!lung_mode)site=(site+(reverse?4:1))%5;
        return;
    }
    if(result_view(view)){
        if(lung_mode){constexpr View pages[]={View::LungResult,View::Wave,View::Stft};
            unsigned i=0;while(i<3&&pages[i]!=view)++i;
            view=pages[(i+(reverse?2:1))%3];
        }else{
            unsigned i=0;while(i<result_page_count&&result_pages[i]!=view)++i;
            page=(i+(reverse?result_page_count-1:1))%result_page_count;view=result_pages[page];request_cycle(*this);
        }return;
    }
    key(0,reverse?KEY_LONG:KEY_SHORT,t);
}
void App::analyse(Progress service){
    if(!analysis_request)return;unsigned pos=analysis_request-1;analysis_request=0;
    const bool saved=reanalyse_saved;reanalyse_saved=false;
    if(pos>=5||sink->busy())return;
    if(saved){
        if(!saved_pcm(*this,pos)||sink->pending_ready)return;
    }else if(!sink->pending_ready||sink->pending_descriptor().site!=pos)return;
    analysing=true;site=pos;analysis_cancelled=0;analysis_to_menu=false;analysis_stage=LUNG_PREVIEW;analysis_elapsed_ms=0;candidate_lung_profile=feature_lung_timing{};
    const auto d=saved?sink->meta[pos]:sink->pending_descriptor();
    const int16_t *pcm=saved?sink->pcm[pos]:sink->pending_data();
    // Recheck RAM data before rerunning the real models. Corrupt saved audio
    // must not replace the previous valid result or start a new capture.
    if(saved&&raw::crc(reinterpret_cast<const uint8_t*>(pcm),size_t(d.samples)*2)!=d.checksum){
        message="Saved PCM CRC failed; result kept";view=View::AnalyzeSaved;analysing=false;return;
    }
    // Metadata validated at transport boundary; the original source contract is explicit.
    candidate_result=Result{};candidate_result.source="Raw PCM / nominal rate";
    candidate_preview=Preview{};
    auto &timing=candidate_timing;timing=StageTiming{};timing.session=d.session;
    timing.stream_window=d.streaming;
    if(!saved&&sink->timing_complete&&sink->timing_upload){
        timing.record_wait_ms=sink->upload_ms-sink->started_ms;
        timing.upload_ms=sink->complete_ms-sink->upload_ms;
        timing.capture_to_crc_ms=sink->complete_ms-sink->started_ms;timing.valid=true;
        timing.first_data_ms=sink->first_data_ms-sink->started_ms;
        timing.pcm_to_crc_ms=sink->complete_ms-sink->last_data_ms;
    }
    if(service)service(0);
    auto began=esp_timer_get_time();
    bool preview=build_preview(pcm,d.samples,d.rate,d.den,candidate_preview,capture_lung?service:nullptr,!capture_lung,capture_lung?&analysis_cancelled:nullptr);
    timing.preview_us=uint32_t(esp_timer_get_time()-began);
    timing.wave_us=candidate_preview.wave_us;timing.stft_us=candidate_preview.stft_us;
    if(service)service(0);
    began=esp_timer_get_time();
    feature_lung_result lung_candidate{};
    bool success;
    if(capture_lung){candidate_lung_wave.count=0;
        const auto*stats=stats_session==d.session&&capture_stats.samples==d.samples?&capture_stats:nullptr;
        feature_lung_control control{&analysis_cancelled,stats,&candidate_lung_profile,lung_clock};
        WaveBuildContext wave{&candidate_lung_wave,service,&analysis_cancelled};
        if(preview&&!analysis_cancelled)lung_candidate=feature_lung_pcm16_controlled(pcm,d.samples,int(d.rate/d.den),service,build_lung_wave_serviced,&wave,&control);
        success=lung_candidate.valid!=0&&candidate_lung_wave.count!=0;feature_lung_release();}
    else success=run_analysis(pcm,d.samples,d.rate,d.den,candidate_result,service);
    timing.analysis_us=uint32_t(esp_timer_get_time()-began);timing.inference_ms=candidate_result.inference_ms;
    timing.preprocessing_us=candidate_result.preprocessing_us;timing.inference_us=candidate_result.inference_us;
    if(capture_lung){timing.preprocessing_us=candidate_lung_profile.preprocessing_us;timing.inference_us=candidate_lung_profile.inference_us;timing.inference_ms=candidate_lung_profile.inference_us/1000;}
    timing.mobile_ms=candidate_result.mobile_ms;timing.tcn_ms=candidate_result.tcn_ms;
    std::memcpy(timing.operator_us,candidate_result.operator_us,sizeof(timing.operator_us));
    candidate_result.crc=d.checksum;
    // service() can detect a transport fault while analysis is running. A failed
    // or superseded pending input must not publish a result under an old slot.
    if(service)service(0);
    success=success&&preview&&!analysis_cancelled&&(capture_lung||valid_scores(candidate_result));
    if(saved)success=success&&saved_pcm(*this,pos)&&sink->meta[pos].session==d.session&&sink->meta[pos].checksum==d.checksum;
    else success=success&&sink->pending_ready&&sink->pending_descriptor().session==d.session;
    if(success&&!saved)success=capture_lung?sink->commit_pending_external(lung_pcm,lung_meta):sink->commit_pending();
    if(success){
        if(capture_lung){lung_result=lung_candidate;lung_preview=candidate_preview;lung_wave=candidate_lung_wave;lung_wave_window=0;
            lung_timing=timing;lung_profile=candidate_lung_profile;lung_ready=true;view=View::LungResult;}
        else{results[pos]=candidate_result;previews[pos]=candidate_preview;timings[pos]=timing;if(!saved)cycles[pos]=Cycle{};view=View::Summary;}
        seen_completed=sink->completed;message="";page=0;wave_window=0;}
    else{if(!saved)sink->discard_pending();message=analysis_cancelled?"Analysis cancelled; previous result kept":"Analysis failed; previous result kept";
        view=analysis_cancelled&&analysis_to_menu?View::Menu:saved?View::AnalyzeSaved:View::Connect;}
    analysing=false;
}
void App::analyse_feature(Progress service){
    if(!feature_request||analysing||sink->busy()||sink->pending_ready)return;
    unsigned pos=feature_request-1;feature_request=0;
    if(pos>=5||!results[pos].ready||!(sink->valid_mask&(1u<<pos)))return;
    analysing=true;analysis_cycle=true;analysis_cancelled=0;analysis_to_menu=false;
    if(service)service(9);auto began=esp_timer_get_time();
    candidate_cycle=Cycle{};auto &c=candidate_cycle;const auto d=sink->meta[pos];
    c.result=feature_cycle_pcm16_serviced(sink->pcm[pos],d.samples,int(d.rate/d.den),c.states,sizeof(c.states),service,&analysis_cancelled);
    c.elapsed_ms=uint32_t((esp_timer_get_time()-began)/1000);c.attempted=true;
    if(service)service(9);
    if(!analysis_cancelled&&saved_pcm(*this,pos)&&sink->meta[pos].session==d.session&&sink->meta[pos].checksum==d.checksum){
        if(c.result.valid||!cycles[pos].attempted)cycles[pos]=c;
    }
    if(analysis_cancelled)cycle_marks_enabled=false;
    analysing=false;analysis_cycle=false;site=pos;view=analysis_cancelled&&analysis_to_menu?View::Menu:View::Cycle;
}

namespace {
constexpr uint16_t ST77XX_BLACK=0,ST77XX_WHITE=0xffff,ST77XX_CYAN=0x07ff,
    ST77XX_YELLOW=0xffe0,ST77XX_GREEN=0x07e0,ST77XX_RED=0xf800,ST77XX_BLUE=0x001f;
struct Canvas {
    uint16_t *pixels=nullptr;unsigned cx=0,cy=0,size=1;uint16_t fg=0xffff,bg=0;
    void drawPixel(int x,int y,uint16_t c){if(x>=0&&x<320&&y>=0&&y<240)pixels[y*320+x]=c;}
    void fillRect(int x,int y,int w,int h,uint16_t c){for(int j=std::max(0,y);j<std::min(240,y+h);j++)
        for(int i=std::max(0,x);i<std::min(320,x+w);i++)drawPixel(i,j,c);}
    void fillScreen(uint16_t c){std::fill(pixels,pixels+320*240,c);}
    void drawFastVLine(int x,int y,int n,uint16_t c){fillRect(x,y,1,n,c);}
    void drawFastHLine(int x,int y,int n,uint16_t c){fillRect(x,y,n,1,c);}
    void drawRect(int x,int y,int w,int h,uint16_t c){drawFastHLine(x,y,w,c);drawFastHLine(x,y+h-1,w,c);drawFastVLine(x,y,h,c);drawFastVLine(x+w-1,y,h,c);}
    void drawLine(int x,int y,int ex,int ey,uint16_t c){int dx=std::abs(ex-x),sx=x<ex?1:-1,dy=-std::abs(ey-y),sy=y<ey?1:-1,err=dx+dy;
        for(;;){drawPixel(x,y,c);if(x==ex&&y==ey)break;int e=2*err;if(e>=dy){err+=dy;x+=sx;}if(e<=dx){err+=dx;y+=sy;}}}
    void drawCircle(int x,int y,int r,uint16_t c){for(int i=0;i<64;i++){float a=i*6.2831853f/64;drawPixel(x+int(r*std::cos(a)),y+int(r*std::sin(a)),c);}}
    void fillCircle(int x,int y,int r,uint16_t c){for(int j=-r;j<=r;j++)for(int i=-r;i<=r;i++)if(i*i+j*j<=r*r)drawPixel(x+i,y+j,c);}
    void setTextWrap(bool){}void setTextSize(unsigned s){size=s;}
    void setTextColor(uint16_t f){fg=f;bg=f;}
    void setTextColor(uint16_t f,uint16_t b){fg=f;bg=b;}
    void setCursor(unsigned x,unsigned y){cx=x;cy=y;}
    void print(const char*);
    void printf(const char*fmt,...){char s[160];va_list a;va_start(a,fmt);std::vsnprintf(s,sizeof(s),fmt,a);va_end(a);print(s);}
} g_tft;
// Original Adafruit font, repacked as rows; original UI is English.
#include "ascii_font.inc"
void Canvas::print(const char*s){while(*s){
    unsigned ch=uint8_t(*s++);
    if(ch<32||ch>=127)ch='?';
    for(unsigned y=0;y<8;y++)for(unsigned x=0;x<6;x++){
        bool set=ascii_rows[ch-32][y]&(1u<<x);
        if(set||bg!=fg)fillRect(int(cx+x*size),int(cy+y*size),size,size,set?fg:bg);
    }
    cx+=6*size;
}}
template<class T> T constrain(T v,T a,T b){return std::max(a,std::min(b,v));}
constexpr int kMenuCount=5,kNumPositions=5;
constexpr float kAbnormalThresholdPercent=40;
constexpr const char *kMenuItems[]={"Heart Sound","Lung Sound","New examination","Analyze PCM","Status"};
constexpr const char *kSN[]={"1.Ao","2.Pu","3.Er","4.Tr","5.Mi"};
unsigned g_menu_index;
int g_pos_total[5],g_pos_abnormal[5];
const char *g_pos_hint[5];
const char *position_hint(int pos){return pos>=0&&pos<5?g_pos_hint[pos]:"N/A";}
struct Stats {float heart_rate_bpm=0;};Stats g_pos_stats[5];
struct OriginalPreview {int16_t wave_min[270]{},wave_max[270]{};uint16_t envelope[270]{};uint8_t stft[108][270]{};bool valid=false;};
OriginalPreview converted;OriginalPreview *g_signal_preview[5]{};
float g_plot_high_hz,g_plot_low_hz,g_plot_first_s,g_plot_last_s,g_wave_seconds,g_wave_first;
uint32_t g_wave_sample_first=0,g_wave_samples=0,g_wave_rate=0;
namespace chd {constexpr int kStftRows=108,kStftColumns=270;
uint16_t heat_color(uint8_t x){unsigned r,g,b;
    if(x<64){r=x;g=0;b=x*2;}else if(x<128){r=64+(x-64)*3;g=0;b=128-(x-64)*2;}
    else if(x<192){r=255;g=(x-128)*3;b=0;}else{r=255;g=192+x-192;b=(x-192)*3;}
    return uint16_t(((r&248)<<8)|((g&252)<<3)|(b>>3));}}
#include "logo_bitmap.h"
#include "heart_guide_bitmap.inc"
void draw_corner_logo(){
    for(int y=0;y<LOGO_CORNER_H;y++)for(int x=0;x<LOGO_CORNER_W;x++){
        const auto c=LOGO_CORNER_DATA[y*LOGO_CORNER_W+x];
        if(c)g_tft.drawPixel(320-LOGO_CORNER_W-6+x,1+y,c);
    }
}
#include "senior_draw.inc"
void label(unsigned y,const char*s,uint16_t color=ST77XX_WHITE){g_tft.setTextSize(2);g_tft.setTextColor(color);g_tft.setCursor(10,y);g_tft.print(s);}
void detail_header(const char *title){
    g_tft.fillRect(0,0,320,34,ST77XX_CYAN);
    g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_BLACK,ST77XX_CYAN);
    g_tft.setCursor(10,9);g_tft.print(title);draw_corner_logo();
    g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
}
constexpr uint16_t cycle_colors[]={0x632c,ST77XX_RED,ST77XX_YELLOW,ST77XX_CYAN,ST77XX_GREEN};
constexpr uint16_t cycle_shades[]={0x1082,0x2800,0x2120,0x0124,0x0120};
// Project window centres onto the original full-recording 20 ms labels.
unsigned cycle_state_at(const App::Cycle &c,float seconds,unsigned x){
    if(!c.attempted||!c.result.valid||c.result.frames<=0||
       c.result.frames>int(sizeof(c.states))||!std::isfinite(seconds)||seconds<=0)return 0;
    if(x>=WAVE_W)return 0;
    const double frame=g_wave_rate?double(((uint64_t(g_wave_sample_first)*2*WAVE_W+uint64_t(2*x+1)*g_wave_samples)*FEATURE_CYCLE_FRAME_HZ)/(uint64_t(2)*WAVE_W*g_wave_rate)):
        (g_wave_first+(double(x)+0.5)*seconds/WAVE_W)*FEATURE_CYCLE_FRAME_HZ;
    if(!std::isfinite(frame)||frame<0||frame>=c.result.frames)return 0;
    unsigned state=c.states[unsigned(frame)];return state<=4?state:0;
}
void draw_cycle_detail(unsigned site,const App::Cycle &c,bool analysing){
    // Reuse the original Wave renderer, including its min/max waveform,
    // envelope and amplitude scale, with the selected five-second time window.
    draw_wave_detail(int(site));
    char title[32];std::snprintf(title,sizeof(title),"Cycle %s",kSN[site]);detail_header(title);
    g_tft.fillRect(0,179,320,49,ST77XX_BLACK);
    const bool valid=c.attempted&&c.result.valid&&c.result.frames>0&&c.result.frames<=int(sizeof(c.states));
    auto *p=g_signal_preview[site];
    if(valid&&p&&p->valid){
        for(unsigned x=0;x<270;x++){
            unsigned state=cycle_state_at(c,g_wave_seconds,x);
            for(unsigned y=49;y<177;y++){
                auto &pixel=g_tft.pixels[y*320+35+x];
                if(pixel==ST77XX_BLACK)pixel=cycle_shades[state];
            }
            g_tft.drawFastVLine(35+x,49,3,cycle_colors[state]);
        }
    }
    g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
    g_tft.setCursor(35,180);g_tft.printf("%.1fs",g_wave_first);
    g_tft.setCursor(157,180);g_tft.printf("%.1fs",g_wave_first+g_wave_seconds*.5f);
    g_tft.setCursor(275,180);g_tft.printf("%.1fs",g_wave_first+g_wave_seconds);
    constexpr const char *names[]={"S1","Systole","S2","Diastole"};
    constexpr unsigned xs[]={35,83,161,209};
    for(unsigned i=0;i<4;i++){
        g_tft.fillRect(xs[i],193,7,7,cycle_colors[i+1]);
        g_tft.setCursor(xs[i]+10,193);g_tft.print(names[i]);
    }
    g_tft.setCursor(35,205);
    if(valid)g_tft.printf("S1:%d  S2:%d | predicted labels",c.result.s1_events,c.result.s2_events);
    else g_tft.print(c.attempted?"Annotation unavailable":"K2: mark cycles on this recording");
    g_tft.setCursor(35,217);g_tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
    if(analysing)g_tft.print("Marking cycles...");
    else if(valid&&c.result.transitions_invalid)g_tft.printf("Check %d discontinuous transitions",c.result.transitions_invalid);
    else if(valid)g_tft.print("Colour = phase, not disease severity");
    else if(c.attempted)g_tft.printf("%.43s",c.result.reason?c.result.reason:"No valid annotation");
    else g_tft.print("Saved audio | no new recording");
}
void draw_lung_result(const feature_lung_result &r,bool ready){
    detail_header("Lung result");
    bool valid=ready&&r.valid&&r.label>=0&&r.label<5&&r.segments>0;
    for(float v:r.mean_probability)valid=valid&&std::isfinite(v)&&v>=0&&v<=1;
    if(valid){
        g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
        g_tft.setCursor(14,47);g_tft.printf("%.24s",feature_lung_label(r.label));
        g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
        g_tft.setCursor(14,70);g_tft.print("Release Mixer | 4kHz input");
    }else{
        g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
        g_tft.setCursor(14,48);g_tft.print("No valid lung result");
    }
    for(unsigned i=0;i<5;i++){
        const unsigned y=92+i*22;const uint16_t color=valid&&int(i)==r.label?ST77XX_YELLOW:ST77XX_CYAN;
        g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
        g_tft.setCursor(14,y);g_tft.printf("%.17s",feature_lung_label(int(i)));
        g_tft.fillRect(124,y,130,9,0x2104);
        if(valid)g_tft.fillRect(124,y,int(r.mean_probability[i]*130+.5f),9,color);
        g_tft.setCursor(265,y);if(valid)g_tft.printf("%.3f",r.mean_probability[i]);else g_tft.print(" ---");
    }
    g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);g_tft.setCursor(14,203);
    g_tft.print("Bars: mean model scores | not a diagnosis");
    g_tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);g_tft.setCursor(14,217);
    g_tft.print("K1: waveform / STFT | K2: record page");
}
}
#include "ui_english.inc"
void paint_startup(uint16_t *fb){
    // Reuse the original competition splash and its exact bitmap.
    g_tft.pixels=fb;g_tft.fillScreen(ST77XX_BLACK);
    g_tft.fillRect(0,0,320,40,ST77XX_CYAN);
    g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_BLACK,ST77XX_CYAN);
    g_tft.setCursor(105,13);g_tft.print("CHD TinyML");
    for(int y=0;y<LOGO_BOOT_H;y++)for(int x=0;x<LOGO_BOOT_W;x++){
        const auto c=LOGO_BOOT_DATA[y*LOGO_BOOT_W+x];
        if(c)g_tft.drawPixel((320-LOGO_BOOT_W)/2+x,45+y,c);
    }
    g_tft.setTextColor(ST77XX_CYAN,ST77XX_BLACK);g_tft.setCursor(46,190);g_tft.print("Heart & Lung Sounds");
    g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);g_tft.setCursor(64,215);g_tft.print("Listen with Care");
}
void App::paint(uint16_t*fb)const{
    g_tft.pixels=fb;g_tft.fillScreen(ST77XX_BLACK);g_menu_index=menu;
    for(unsigned i=0;i<5;i++){g_pos_total[i]=results[i].ready?results[i].position.total:0;
        g_pos_abnormal[i]=results[i].position.abnormal;g_pos_stats[i].heart_rate_bpm=results[i].position.bpm;g_signal_preview[i]=nullptr;
        g_pos_hint[i]=valid_scores(results[i])?results[i].position.hint():"N/A";}
    const auto &p=lung_mode?lung_preview:previews[site];converted.valid=p.valid;
    if(p.valid){for(unsigned x=0;x<270;x++){unsigned src=x*(PLOT_W-1)/269;
        converted.wave_min[x]=p.low[src];converted.wave_max[x]=p.high[src];converted.envelope[x]=p.envelope[src];
        for(unsigned y=0;y<108;y++)converted.stft[y][x]=p.stft[y][src];}g_signal_preview[site]=&converted;}
    g_plot_high_hz=p.high_hz;g_plot_low_hz=p.low_hz;g_wave_seconds=p.seconds;g_wave_first=0;g_wave_rate=0;g_wave_sample_first=0;g_wave_samples=0;
    if(lung_mode&&view==View::Wave&&p.valid&&lung_wave.count){
        const auto &w=lung_wave.windows[std::min(lung_wave_window,std::min(11u,lung_wave.count)-1)];
        for(unsigned x=0;x<WAVE_W;x++){
            converted.wave_min[x]=w.low[x];converted.wave_max[x]=w.high[x];converted.envelope[x]=w.envelope[x];}
        g_wave_first=w.first;g_wave_seconds=w.seconds;g_wave_sample_first=w.first_sample;g_wave_samples=w.samples;g_wave_rate=w.sample_rate;
    }
    if(!lung_mode&&(view==View::Wave||view==View::Cycle)&&p.valid){
        const auto &w=p.wave[p.wave_count?std::min(wave_window,std::min(11u,p.wave_count)-1):0];
        if(w.seconds>0){for(unsigned x=0;x<WAVE_W;x++){
            converted.wave_min[x]=w.low[x];converted.wave_max[x]=w.high[x];converted.envelope[x]=w.envelope[x];}
            g_wave_first=w.first;g_wave_seconds=w.seconds;g_wave_sample_first=w.first_sample;g_wave_samples=w.samples;g_wave_rate=w.sample_rate;}
    }
    g_plot_first_s=.128f;g_plot_last_s=std::max(.128f,p.seconds-.128f);
    paint_english(*this);
}
