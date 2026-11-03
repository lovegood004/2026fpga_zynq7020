#include "lung_start_icon.inc"
// English UI using the original Adafruit bitmap font and senior-project palette.
namespace {
constexpr const char *site_names[]={"Aortic area","Pulmonary area","Erb's point","Tricuspid area","Mitral area"};
constexpr const char *site_short[]={"Ao","Pu","Er","Tr","Mi"};
constexpr const char *menu_names[]={"Heart Sound","Lung Sound","New examination","Analyze recording","System status"};
void ui_text(unsigned x,unsigned y,const char *s,uint16_t color=ST77XX_WHITE,unsigned size=2,uint16_t bg=ST77XX_BLACK){
    g_tft.setTextSize(size);g_tft.setTextColor(color,bg);g_tft.setCursor(x,y);g_tft.print(s);
}
void ui_title(const char *s,const App &a,uint16_t color=ST77XX_CYAN,bool counter=true){
    g_tft.fillRect(0,0,320,34,color);ui_text(10,9,s,ST77XX_BLACK,2,color);draw_corner_logo();
    if(counter&&a.view!=View::Menu){ui_text(234,14,"",ST77XX_BLACK,1,color);g_tft.printf("%u/5",a.site+1);}
}
void ui_footer(const App &a){
    g_tft.fillRect(0,220,320,20,ST77XX_BLACK);
    const char *hint;
#ifdef UI_PL_KEYS
    if(a.analysing&&(a.capture_lung||a.analysis_cycle))hint="PL4 Cancel   Hold PS2 Menu";
    else if(a.sink->busy())hint="PL4 Cancel   Hold PS2 Menu";
    else if(a.view==View::Connect||a.view==View::Move)hint="PL1/2 Site  PL3 Start  PL4 Back";
    else if(a.view==View::Summary||a.view==View::LungResult)hint="PL1/2 Page  PL3 Record  PL4 Menu";
    else if(a.view==View::Cycle)hint="PL3 Site / hold Label   PL4 Menu";
    else if(a.view==View::Table)hint="PL1/2 Page  PL3 Wave  PL4 Menu";
    else if(!a.lung_mode&&(a.view==View::Wave||a.view==View::Stft||a.view==View::Scores))hint="PL1/2 Page  PL3 Site  PL4 Menu";
    else if(result_view(a.view))hint="PL1/2 Page  PL4 Menu";
    else if(a.view==View::NewExamConfirm)hint="PL3 Clear   PL4 Keep";
    else if(a.view==View::AnalyzeSaved)hint="PL1/2 Site  PL3 Analyze  PL4 Back";
    else hint="PL1/2 Select  PL3 OK  PL4 Back";
#else
    if(a.sink->busy())hint="PS2 Cancel   Hold PS2 Menu";
    else if(a.view==View::Summary||a.view==View::LungResult)hint="PS1 Page  PS2 Record/hold Menu";
    else if(a.view==View::Cycle)hint="PS1 Page  PS2 Label/hold Menu";
    else if(a.view==View::Table)hint="PS1 Page  PS2 Wave/hold Menu";
    else if(result_view(a.view))hint="PS1 Page  PS2 Site/hold Menu";
    else hint="PS1 Select   PS2 OK/hold Menu";
#endif
    const bool browsing=result_view(a.view)&&!a.sink->busy();
    ui_text(6,browsing?221:227,hint,ST77XX_YELLOW,1);
    if(browsing){
        unsigned index=0,count=result_page_count;
        if(a.lung_mode){
            count=3;index=a.view==View::LungResult?0:a.view==View::Wave?1:2;
        }else while(index+1<count&&result_pages[index]!=a.view)++index;
        ui_text(6,231,"",ST77XX_YELLOW,1);g_tft.printf("%u/%u",index+1,count);
        if(a.view==View::Wave||a.view==View::Cycle)g_tft.print("  PS1 <  PS2 >  step 1s");
    }
}
const char *ui_message(const char *message){
    struct Message{const char *source,*text;};
    constexpr Message messages[]={
        {"Capture cancelled","Cancelled"},
        {"No saved PCM / record first","No recording. Record now"},
        {"No analysed RAM session","No results. Record first"},
        {"New examination / no recording yet","Ready to record"},
        {"Previous result kept until success","Ready to record"},
        {"Peer not ready / raw v2 only","Sensor not ready"},
        {"No committed heart recording","No heart recording"},
        {"Saved PCM CRC failed; result kept","Recording check failed"},
        {"Analysis cancelled; previous result kept","Cancelled"},
        {"Analysis failed; previous result kept","Analysis failed"}
    };
    for(const auto &m:messages)if(!std::strcmp(message,m.source))return m.text;
    return *message?"Unable to complete":"";
}
const char *class_hint_name(const Position &p){
    const char *hint=p.hint();
    if(!std::strcmp(hint,"AS"))return "Aortic stenosis";
    if(!std::strcmp(hint,"AR"))return "Aortic regurgitation";
    if(!std::strcmp(hint,"MR"))return "Mitral regurgitation";
    if(!std::strcmp(hint,"MS"))return "Mitral stenosis";
    if(!std::strcmp(hint,"N"))return "Normal";
    return !std::strcmp(hint,"?")?"Abnormal: type uncertain":"Result unavailable";
}
void draw_heart_guide(const App &a){
    for(int y=0;y<32;y++)for(int x=0;x<32;x++)if(kHeartIcon[y][x]==0xF061)
        g_tft.fillRect(112+x*3,50+y*3,3,3,0xF061);
    constexpr int xs[]={175,145,160,148,172},ys[]={72,72,88,108,115};
    for(unsigned i=0;i<5;i++){
        if(i==a.site)g_tft.fillCircle(xs[i],ys[i],5,ST77XX_YELLOW);
        else if(valid_scores(a.results[i]))g_tft.fillCircle(xs[i],ys[i],5,a.results[i].position.positive()?ST77XX_RED:ST77XX_GREEN);
        g_tft.drawCircle(xs[i],ys[i],5,ST77XX_WHITE);
    }
    ui_text(10,169,site_names[a.site],ST77XX_YELLOW);
}
void ui_plot_caption(const App &a){
    if(a.view==View::Cycle){
        // Keep the original cycle time ticks at y=180 visible.
        g_tft.fillRect(0,190,320,30,ST77XX_BLACK);
        constexpr const char *names[]={"S1","Systole","S2","Diastole"};
        constexpr unsigned xs[]={10,61,145,196};
        for(unsigned i=0;i<4;i++){g_tft.fillRect(xs[i],194,7,7,cycle_colors[i+1]);ui_text(xs[i]+10,194,names[i],ST77XX_WHITE,1);}
        const auto &c=a.cycles[a.site];
        if(a.analysing){ui_text(10,208,"",ST77XX_YELLOW,1);g_tft.printf("Marking %.1fs",a.analysis_elapsed_ms/1000.0);}
        else ui_text(10,208,c.attempted?(c.result.valid?"Cycle labels":"Labels unavailable"):"Hold PL3 to label",ST77XX_YELLOW,1);
    }else{
        g_tft.fillRect(0,179,320,41,ST77XX_BLACK);
        if(a.view==View::Wave){
            ui_text(35,180,"",ST77XX_WHITE,1);g_tft.printf("%.1fs",g_wave_first);
            ui_text(157,180,"",ST77XX_WHITE,1);g_tft.printf("%.1fs",g_wave_first+g_wave_seconds*.5f);
            ui_text(275,180,"",ST77XX_WHITE,1);g_tft.printf("%.1fs",g_wave_first+g_wave_seconds);
            ui_text(10,195,a.lung_mode?"Lung sound":"Heart sound",ST77XX_WHITE);
            ui_text(10,212,a.lung_mode?"Processed 4kHz | 5s window":"Processed 1kHz | 5s window",ST77XX_WHITE,1);
        }else{
            ui_text(10,184,"Spectrogram",ST77XX_WHITE);
            ui_text(10,207,"",ST77XX_WHITE,1);g_tft.printf("FFT256  %.1f - %.1f Hz  60dB",g_plot_low_hz,g_plot_high_hz);
        }
    }
}
#ifdef UI_PL_ACQ
void draw_capture_preview(const App &a){
    const auto &p=*a.live_preview;
    ui_text(10,58,a.lung_mode?"Lung sound":site_names[a.site],ST77XX_YELLOW);
    constexpr int left=35,top=98,height=54,mid=top+height/2;
    g_tft.drawRect(left-1,top-1,252,height+2,0x3186);
    g_tft.drawFastHLine(left,mid,250,0x2104);
    int scale=256;
    for(unsigned i=0;i<p.columns;i++)scale=std::max(scale,std::max(std::abs(int(p.low[i])),std::abs(int(p.high[i]))));
    for(unsigned i=0;i<p.columns;i++){
        const int high=mid-int(p.high[i])*(height/2-1)/scale;
        const int low=mid-int(p.low[i])*(height/2-1)/scale;
        g_tft.drawFastVLine(left+i,high,std::max(1,low-high+1),ST77XX_CYAN);
    }
    ui_text(35,158,"",ST77XX_WHITE,1);g_tft.printf("%.1fs",p.first_sample/11025.0);
    ui_text(146,158,"",ST77XX_WHITE,1);g_tft.printf("%.1fs",p.first_sample/11025.0+5.0);
    ui_text(263,158,"",ST77XX_WHITE,1);g_tft.printf("%.1fs",p.first_sample/11025.0+10.0);
    ui_text(10,177,"",p.clips?ST77XX_YELLOW:ST77XX_WHITE,1);
    g_tft.printf("Peak %.1f%%  RMS %.1f%%  Clip %.2f%%",p.peak*100.0/32768.0,
        p.samples?std::sqrt(double(p.square_sum)/p.samples)*100.0/32768.0:0.0,
        p.samples?p.clips*100.0/p.samples:0.0);
    ui_text(10,188,"10 s window / auto scale",ST77XX_WHITE,1);
}
#endif
void paint_english(const App &a){
    const auto view=a.view;
    if(view==View::Menu){
        ui_title("Heart-Lung Menu",a);g_tft.fillRect(0,34,320,3,0x033F);
        constexpr uint16_t accents[]={ST77XX_RED,ST77XX_YELLOW,ST77XX_GREEN,0xFD20,ST77XX_CYAN};
        for(unsigned i=0;i<5;i++){
            const int y=41+i*35;const bool selected=i==a.menu;
            const uint16_t bg=selected?ST77XX_BLUE:ST77XX_BLACK,fg=selected?ST77XX_WHITE:accents[i];
            g_tft.fillRect(6,y,308,29,bg);g_tft.fillRect(6,y,4,29,accents[i]);draw_menu_icon(i,14,y+6,fg);
            ui_text(34,y+6,menu_names[i],fg,2,bg);
        }
    }else if(view==View::Connect||view==View::Move||view==View::Capture){
        const bool busy=a.sink->busy(),online=a.sink->online(a.now),ready=online&&a.sink->ready;
        ui_title(a.analysing?"Analysing":busy?"Recording":a.lung_mode?"Lung Sound":"Heart Sound",a);
        ui_text(10,38,online?(ready?"Sensor connected":"Sensor not ready"):"Sensor disconnected",ready?ST77XX_GREEN:ST77XX_YELLOW,1);
#ifdef UI_PL_ACQ
        const bool live=busy&&a.sink->phase!=raw::Phase::Stopping&&a.live_preview&&a.live_preview->available&&
            a.live_preview->session==a.sink->wanted.session;
        if(live)draw_capture_preview(a);
        else
#endif
        if(!a.lung_mode)draw_heart_guide(a);else {
            if(!busy&&!a.analysing&&(view==View::Connect||view==View::Move))draw_lung_start_icon();
            ui_text(10,169,"Lung sound",ST77XX_YELLOW);
        }
        if(a.analysing){
            if(a.capture_lung){
                constexpr const char*stages[]={"Preparing preview","Reading statistics","Resampling","Spectrogram","Processed waveform","CNN","Mixer","BiGRU","Classifier"};
                ui_text(10,186,stages[std::min(a.analysis_stage,8u)],ST77XX_WHITE,1);
                ui_text(10,198,"",ST77XX_WHITE);g_tft.printf("Analysing %.1fs",a.analysis_elapsed_ms/1000.0);
            }else ui_text(10,198,"Analysing...",ST77XX_WHITE);
        }
        else if(busy){
            const unsigned count=a.sink->phase==raw::Phase::Recording?a.sink->recorded:a.sink->points;
            const float progress=std::min(count,unsigned(raw::Samples))/float(raw::Samples);
            if(a.sink->phase==raw::Phase::Stopping)ui_text(10,198,"Stopping...",ST77XX_YELLOW);
            else{ui_text(10,198,"",ST77XX_WHITE);g_tft.printf("REC %.1f%%",100.0*progress);}
            g_tft.drawRect(206,201,104,10,0x3186);g_tft.fillRect(207,202,int(102*progress),8,ST77XX_CYAN);
        }else if(a.sink->error!=raw::Error::None)ui_text(10,198,"Record failed. Retry",ST77XX_YELLOW);
        else if(*a.message)ui_text(10,198,ui_message(a.message),ST77XX_WHITE);
        else ui_text(10,198,"Place sensor. Press OK",ST77XX_WHITE);
    }else if(view==View::Summary){
        unsigned ready=0;Session summary;
        for(unsigned i=0;i<5;i++)if(valid_scores(a.results[i])){++ready;summary.positions[i]=a.results[i].position;}
        ui_title("Heart Results",a,ready?(summary.positive()?ST77XX_RED:ST77XX_GREEN):ST77XX_CYAN);
        ui_text(10,43,"",ST77XX_WHITE);g_tft.printf("%u/5  Avg %.0f BPM",ready,summary.average_bpm());
        for(unsigned i=0;i<5;i++){
            const unsigned y=69+i*22;const auto &r=a.results[i];
            ui_text(10,y,site_short[i],i==a.site?ST77XX_CYAN:ST77XX_WHITE);
            ui_text(69,y,"",ST77XX_WHITE);
            if(valid_scores(r))g_tft.printf("%3.0f BPM  %s",r.position.bpm,r.position.hint());else g_tft.print(r.ready?"Unavailable":"Not recorded");
        }
        ui_text(10,184,ready?(summary.positive()?"Screen: Abnormal":"Screen: Normal"):"No results",ST77XX_WHITE);
        ui_text(10,207,"",ST77XX_WHITE,1);g_tft.printf("Segments %d   Abnormal %d  (%.1f%%)",summary.total(),summary.abnormal(),summary.total()?100.0*summary.abnormal()/summary.total():0.0);
    }else if(view==View::Wave||view==View::Stft||view==View::Cycle){
        if(view==View::Wave)draw_wave_detail(int(a.site));else if(view==View::Stft)draw_stft_detail(int(a.site));else draw_cycle_detail(a.site,a.cycles[a.site],a.analysing);
        ui_title(view==View::Cycle?"Heart Cycle":view==View::Wave?(a.lung_mode?"Lung Wave":"Heart Wave"):"Spectrogram",a,view==View::Stft?ST77XX_GREEN:ST77XX_CYAN);
        const bool preview=(a.lung_mode?a.lung_preview:a.previews[a.site]).valid;
        if(!preview){g_tft.fillRect(75,91,150,30,ST77XX_BLACK);ui_text(94,100,"No waveform",ST77XX_YELLOW);}
        ui_plot_caption(a);
    }else if(view==View::Table){
        ui_title("5 Positions",a);
        ui_text(10,43,"Site",ST77XX_CYAN,1);ui_text(69,43,"BPM",ST77XX_CYAN,1);ui_text(137,43,"Abn %",ST77XX_CYAN,1);ui_text(230,43,"Type",ST77XX_CYAN,1);
        for(unsigned i=0;i<5;i++){
            const unsigned y=62+i*30;const auto &r=a.results[i];const bool valid=valid_scores(r);
            const uint16_t bg=(i&1)?0x1082:ST77XX_BLACK;g_tft.fillRect(4,y-4,312,27,bg);
            ui_text(10,y,site_short[i],i==a.site?ST77XX_CYAN:ST77XX_WHITE,2,bg);
            ui_text(69,y,"",ST77XX_WHITE,2,bg);if(valid)g_tft.printf("%.0f",r.position.bpm);else g_tft.print("--");
            ui_text(137,y,"",ST77XX_WHITE,2,bg);if(valid)g_tft.printf("%.0f%%",r.position.percent());else g_tft.print("--");
            ui_text(230,y,valid?r.position.hint():"--",ST77XX_WHITE,2,bg);
            if(valid)g_tft.fillCircle(302,y+6,3,r.position.positive()?ST77XX_RED:ST77XX_GREEN);
        }
    }else if(view==View::Scores){
        ui_title("Class Scores",a);const auto &r=a.results[a.site];const bool valid=valid_scores(r);
        ui_text(10,42,site_names[a.site],ST77XX_CYAN);
        ui_text(10,64,valid?class_hint_name(r.position):"No valid result",ST77XX_WHITE);
        constexpr const char *names[]={"AS","AR","MR","MS","Normal"};
        for(unsigned i=0;i<5;i++){
            const unsigned y=93+i*24;ui_text(10,y,names[i]);ui_text(107,y,"",ST77XX_WHITE);
            if(valid)g_tft.printf("%.3f",r.position.mean[i]);else g_tft.print(" ---");
            g_tft.fillRect(183,y+2,125,11,0x2104);
            if(valid)g_tft.fillRect(183,y+2,int(r.position.mean[i]*125+.5f),11,i==4?ST77XX_GREEN:ST77XX_CYAN);
        }
    }else if(view==View::LungResult){
        ui_title("Lung Results",a);const auto &r=a.lung_result;
        bool valid=a.lung_ready&&r.valid&&r.label>=0&&r.label<5&&r.segments>0;
        for(float v:r.mean_probability)valid=valid&&std::isfinite(v)&&v>=0&&v<=1;
        
        ui_text(10,44,valid?feature_lung_label(r.label):"No valid result",ST77XX_WHITE);
        for(unsigned i=0;i<5;i++){
            const unsigned y=74+i*27;ui_text(10,y,feature_lung_label(int(i)));ui_text(244,y,"",ST77XX_WHITE);
            if(valid)g_tft.printf("%.3f",r.mean_probability[i]);else g_tft.print(" ---");
            g_tft.fillRect(142,y+3,88,9,0x2104);
            if(valid)g_tft.fillRect(142,y+3,int(r.mean_probability[i]*88+.5f),9,int(i)==r.label?ST77XX_YELLOW:ST77XX_CYAN);
        }
    }else if(view==View::NewExamConfirm){
        ui_title("New Examination",a,ST77XX_CYAN,false);
        ui_text(10,60,"Clear all recordings?",ST77XX_YELLOW);
        ui_text(10,94,"Results will be cleared");ui_text(10,150,"OK: Clear   Back: Keep");
    }else if(view==View::AnalyzeSaved){
        ui_title("Analyze Recording",a);ui_text(10,49,site_names[a.site],ST77XX_CYAN);
        if(saved_pcm(a,a.site)){const auto &d=a.sink->meta[a.site];ui_text(10,80,"",ST77XX_WHITE);
            g_tft.printf("%.2fs  %lu Hz",double(d.samples)*d.den/d.rate,(unsigned long)(d.rate/d.den));}
        ui_text(10,166,a.analysing?"Analysing...":"Select audio. Press OK");ui_text(10,198,ui_message(a.message),ST77XX_YELLOW);
    }else if(view==View::Status){
        ui_title("System Status",a,ST77XX_CYAN,false);
        constexpr const char *labels[]={"Sensor","State","Heart","Lung","Duration"};
        for(unsigned i=0;i<5;i++)ui_text(14,53+i*31,labels[i]);
        const bool online=a.sink->online(a.now);
        ui_text(146,53,online?"Online":"Offline",online?ST77XX_GREEN:ST77XX_YELLOW);
        ui_text(146,84,online&&a.sink->ready?"Ready":"Not ready");
        unsigned saved=0;for(unsigned i=0;i<5;i++)if(a.sink->valid_mask&(1u<<i))++saved;
        ui_text(146,115,"",ST77XX_WHITE);g_tft.printf("%u/5",saved);
        ui_text(146,146,a.lung_ready?"Saved":"None");ui_text(146,177,"15 sec");
    }else{
        ui_title("Information",a,ST77XX_CYAN,false);ui_text(10,73,ui_message(a.message),ST77XX_YELLOW);ui_text(10,119,"Press Back to return");
    }
    ui_footer(a);
}
}
