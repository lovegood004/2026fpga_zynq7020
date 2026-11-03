// Generated from original main/main.cpp by prepare_ui.py.
void draw_status(const char *l1, const char *l2 = nullptr, const char *l3 = nullptr,
                 uint16_t accent = ST77XX_CYAN, bool clear = true) {
    if (clear) {
        g_tft.fillScreen(ST77XX_BLACK);
        g_tft.fillRect(0, 0, 320, 34, accent);
        g_tft.setTextWrap(false); g_tft.setTextSize(2);
        g_tft.setTextColor(ST77XX_BLACK, accent);
        g_tft.setCursor(10, 9); g_tft.print("Heart-Lung AI");
        draw_corner_logo();
        g_tft.fillRect(14, 56, 292, 130, ST77XX_BLACK);
    } else {
        // 非首次：清空文字区域但保留标题栏
        g_tft.fillRect(14, 56, 292, 140, ST77XX_BLACK);
    }
    g_tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    g_tft.setTextSize(2);
    if (l1) { g_tft.setCursor(14, 62); g_tft.print(l1); }
    if (l2) { g_tft.setCursor(14, 96); g_tft.print(l2); }
    if (l3) { g_tft.setCursor(14, 130); g_tft.print(l3); }
}

void draw_menu_icon(int idx, int x, int y, uint16_t color) {
    g_tft.setTextColor(color);
    switch (idx) {
        case 0: { // Bluetooth
            g_tft.drawLine(x+4, y+2, x+11, y+13, color);
            g_tft.drawLine(x+4, y+13, x+11, y+2, color);
            g_tft.drawLine(x+4, y+2, x+4, y+13, color);
            g_tft.drawLine(x+7, y+5, x+4, y+2, color);
            g_tft.drawLine(x+7, y+10, x+4, y+13, color);
            g_tft.drawLine(x+7, y+5, x+11, y+2, color);
            g_tft.drawLine(x+7, y+10, x+11, y+13, color);
            break;
        }
        case 2: { // SD card
            g_tft.drawRect(x+2, y+2, 12, 13, color);
            g_tft.drawLine(x+10, y+2, x+14, y+6, color);
            g_tft.drawLine(x+14, y+6, x+14, y+14, color);
            g_tft.drawLine(x+10, y+2, x+10, y+6, color);
            g_tft.drawLine(x+10, y+6, x+14, y+6, color);
            g_tft.drawLine(x+5, y+5, x+5, y+8, color);
            g_tft.drawLine(x+8, y+5, x+8, y+8, color);
            break;
        }
        case 1: // Lung waveform
        case 3: { // Waveform
            g_tft.drawLine(x+1, y+8, x+3, y+8, color);
            g_tft.drawLine(x+3, y+8, x+5, y+4, color);
            g_tft.drawLine(x+5, y+4, x+7, y+12, color);
            g_tft.drawLine(x+7, y+12, x+9, y+2, color);
            g_tft.drawLine(x+9, y+2, x+11, y+12, color);
            g_tft.drawLine(x+11, y+12, x+13, y+4, color);
            g_tft.drawLine(x+13, y+4, x+14, y+8, color);
            break;
        }
        case 4: { // Status gauge
            g_tft.drawCircle(x+7, y+8, 6, color);
            g_tft.drawLine(x+7, y+8, x+10, y+4, color);
            g_tft.fillCircle(x+7, y+8, 1, color);
            g_tft.drawPixel(x+7, y+1, color);
            break;
        }
    }
}

void draw_menu() {
    g_tft.fillScreen(ST77XX_BLACK);
    g_tft.fillRect(0, 0, 320, 34, ST77XX_CYAN);
    g_tft.fillRect(0, 34, 320, 3, 0x033F);
    g_tft.setTextWrap(false); g_tft.setTextSize(2);
    g_tft.setTextColor(ST77XX_BLACK, ST77XX_CYAN); g_tft.setCursor(10, 9); g_tft.print("Heart-Lung Menu");
    draw_corner_logo();
    static const uint16_t kAccent[] = {0xF800, 0xFFE0, 0x07E0, 0xFD20, 0x07FF};
    for (int i = 0; i < kMenuCount; ++i) {
        int y = 44 + i * 35;
        int barY = y - 3, barH = 29, cy = barY + barH / 2;
        if (i == g_menu_index) {
            g_tft.fillRect(6, barY, 308, barH, ST77XX_BLUE);
            g_tft.fillRect(6, barY, 4, barH, kAccent[i]);
            g_tft.setTextColor(ST77XX_WHITE, ST77XX_BLUE);
            draw_menu_icon(i, 14, cy - 8, ST77XX_WHITE);
        } else {
            g_tft.fillRect(6, barY, 4, barH, kAccent[i]);
            g_tft.setTextColor(kAccent[i], ST77XX_BLACK);
            draw_menu_icon(i, 14, cy - 8, kAccent[i]);
        }
        g_tft.setCursor(34, cy - 8);
        g_tft.printf("%s", kMenuItems[i]);
    }
    g_tft.setTextSize(1); g_tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    g_tft.setCursor(10, 228); g_tft.print("K1 next/hold prev  K2 OK/hold back");
}

void draw_wave_detail(int pos) {
    g_tft.fillRect(0,0,320,34,ST77XX_CYAN);
    g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_BLACK,ST77XX_CYAN);
    g_tft.setCursor(52,9);g_tft.printf("Wave %s",kSN[pos]);draw_corner_logo();
    const int x0=35,y0=48,w=270,h=130,mid=y0+h/2;
    g_tft.drawRect(x0,y0,w,h,0x632C);g_tft.drawFastHLine(x0,mid,w,0x3186);
    auto *p=g_signal_preview[pos];
    if(p&&p->valid){
        int32_t peak=1;
        for(int x=0;x<w;++x){
            int32_t a=abs((int)p->wave_min[x]),b=abs((int)p->wave_max[x]);
            if(a>peak)peak=a;if(b>peak)peak=b;
        }
        float sc=(h/2-4)/(float)peak;uint16_t em=1;
        for(int x=0;x<w;++x)if(p->envelope[x]>em)em=p->envelope[x];
        for(int x=0;x<w;++x){
            int ya=mid-(int)(p->wave_max[x]*sc),yb=mid-(int)(p->wave_min[x]*sc);
            ya=constrain(ya,y0+2,y0+h-2);yb=constrain(yb,y0+2,y0+h-2);
            if(yb>=ya)g_tft.drawFastVLine(x0+x,ya,yb-ya+1,ST77XX_CYAN);
            int ye=mid-(int)((h/2-5)*(float)p->envelope[x]/em);
            g_tft.drawPixel(x0+x,constrain(ye,y0+2,mid),ST77XX_YELLOW);
        }
    }else{
        g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_RED,ST77XX_BLACK);
        g_tft.setCursor(90,100);g_tft.print("No preview");
    }
    float pp=g_pos_total[pos]?100.0f*g_pos_abnormal[pos]/g_pos_total[pos]:0;
    g_tft.setTextSize(2);g_tft.setTextColor(pp>=kAbnormalThresholdPercent?ST77XX_RED:ST77XX_WHITE,ST77XX_BLACK);
    g_tft.setCursor(35,184);g_tft.printf("BPM %.0f  Abn %.0f%%",g_pos_stats[pos].heart_rate_bpm,pp);
    g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_CYAN,ST77XX_BLACK);
    g_tft.setCursor(35,207);g_tft.printf("%.2fs | cyan wave | yellow envelope",g_wave_seconds);
}

void draw_stft_detail(int pos) {
    g_tft.fillRect(0,0,320,34,ST77XX_GREEN);
    g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_BLACK,ST77XX_GREEN);
    g_tft.setCursor(52,9);g_tft.printf("STFT %s",kSN[pos]);draw_corner_logo();
    const int x0=36,y0=46;auto *p=g_signal_preview[pos];
    if(p&&p->valid){
        for(int y=0;y<chd::kStftRows;++y)
            for(int x=0;x<chd::kStftColumns;++x)
                g_tft.drawPixel(x0+x,y0+y,chd::heat_color(p->stft[y][x]));
        g_tft.drawRect(x0-1,y0-1,chd::kStftColumns+2,chd::kStftRows+2,ST77XX_WHITE);
    }
    g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
    g_tft.setCursor(5,y0);g_tft.printf("%.0f", g_plot_high_hz);g_tft.setCursor(5,y0+52);g_tft.printf("%.0f", (g_plot_high_hz+g_plot_low_hz)*0.5f);
    g_tft.setCursor(11,y0+101);g_tft.printf("%.0f", g_plot_low_hz);g_tft.setCursor(x0,162);g_tft.printf("%.2fs", g_plot_first_s);
    g_tft.setCursor(x0+126,162);g_tft.printf("%.2fs", (g_plot_first_s+g_plot_last_s)*0.5f);g_tft.setCursor(x0+232,162);g_tft.printf("%.2fs", g_plot_last_s);
    if(!p||!p->valid){g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_RED,ST77XX_BLACK);
        g_tft.setCursor(90,100);g_tft.print("No preview");}
    float pp=g_pos_total[pos]?100.0f*g_pos_abnormal[pos]/g_pos_total[pos]:0;
    g_tft.setTextSize(2);g_tft.setTextColor(pp>=kAbnormalThresholdPercent?ST77XX_RED:ST77XX_WHITE,ST77XX_BLACK);
    g_tft.setCursor(36,181);g_tft.printf("BPM %.0f  Abn %.0f%%",g_pos_stats[pos].heart_rate_bpm,pp);
    g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_YELLOW,ST77XX_BLACK);
    g_tft.setCursor(36,207);g_tft.printf("FFT256 | %.1f-%.1fHz | 60dB",g_plot_low_hz,g_plot_high_hz);
}

void draw_position_table(const float bpm[5]) {
    g_tft.fillRect(0,0,320,34,ST77XX_CYAN);g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_BLACK,ST77XX_CYAN);g_tft.setCursor(62,9);g_tft.print("5 Positions");draw_corner_logo();
    for(int i=0;i<5;++i){int y=39+i*37;float pp=g_pos_total[i]?100.0f*g_pos_abnormal[i]/g_pos_total[i]:0;bool ab=g_pos_total[i]&&pp>=kAbnormalThresholdPercent;uint16_t bg=(i&1)?0x1082:ST77XX_BLACK;g_tft.fillRect(4,y,312,34,bg);g_tft.setTextSize(2);g_tft.setTextColor(ab?ST77XX_RED:ST77XX_CYAN,bg);g_tft.setCursor(9,y+8);g_tft.printf("%-2s",kSN[i]+2);g_tft.setTextColor(ST77XX_WHITE,bg);g_tft.setCursor(48,y+8);g_tft.printf("%3.0f",bpm[i]);g_tft.setCursor(100,y+8);g_tft.printf("%3.0f%%",pp);g_tft.setTextSize(1);g_tft.setTextColor(ab?ST77XX_RED:ST77XX_GREEN,bg);g_tft.setCursor(174,y+4);g_tft.print(!g_pos_total[i]?"NOT READY":ab?"SCREEN +":"SCREEN -");g_tft.setTextColor(ST77XX_WHITE,bg);g_tft.setCursor(174,y+19);g_tft.printf("Seg %d  Hint %s",g_pos_total[i],position_hint(i));}
}

void draw_original_summary(const float bpm[5],int st,int sa,float pct,bool ab,float valid_average,unsigned completed,uint32_t g_total_analysis_ms){
float pct2=pct;bool ab2=ab;float pavg=valid_average;
                            uint16_t ac2=ab2?ST77XX_RED:ST77XX_GREEN;
                            g_tft.fillRect(0,0,320,34,ac2);g_tft.setTextSize(2);g_tft.setTextColor(ST77XX_BLACK,ac2);g_tft.setCursor(10,9);g_tft.print(completed==5?"Result / 5 positions":"Result / INCOMPLETE");
                            draw_corner_logo();
                            g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);g_tft.setCursor(14,56);
                            g_tft.printf("BPM:%.0f %.0f %.0f %.0f %.0f",bpm[0],bpm[1],bpm[2],bpm[3],bpm[4]);
                            g_tft.setCursor(14,90);g_tft.printf("Avg:%.0f  Seg:%d",pavg,st);
                            g_tft.setCursor(14,124);g_tft.printf("Abn:%d  %.1f%%  RAM only",sa,pct2);
                            g_tft.setCursor(14,155);
                            g_tft.printf("%lu ms",(unsigned long)g_total_analysis_ms);
                        
    // Restore the final dual-model integration's per-position Hint line.
    g_tft.setTextSize(1);g_tft.setTextColor(ST77XX_WHITE,ST77XX_BLACK);
    g_tft.setCursor(14,204);g_tft.print("Hint: ");
    for(int i=0;i<5;++i){g_tft.print(position_hint(i));if(i<4)g_tft.print(" ");}

}

