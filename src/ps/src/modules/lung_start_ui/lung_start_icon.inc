// Native RGB565 drawing; no external image, allocation or capture-side work.
namespace {
void draw_lung_start_icon(){
    struct Point {int x,y;};
    constexpr Point left[]={{152,76},{146,66},{137,68},{128,78},{121,94},
        {116,114},{115,133},{120,145},{130,148},{144,142},{151,132}};
    constexpr Point right[]={{168,76},{174,66},{183,68},{192,78},{199,94},
        {204,114},{205,133},{200,145},{190,148},{178,143},{174,131},{169,119}};
    const auto lobe=[](const Point*p,unsigned n){
        for(int y=66;y<=148;++y){
            int first=320,last=-1;
            for(unsigned i=0,j=n-1;i<n;j=i++){
                const auto &a=p[j],&b=p[i];
                if((a.y<=y&&y<b.y)||(b.y<=y&&y<a.y)){
                    const int x=a.x+(y-a.y)*(b.x-a.x)/(b.y-a.y);
                    first=std::min(first,x);last=std::max(last,x);
                }
            }
            if(last>=first)g_tft.drawFastHLine(first,y,last-first+1,0xA98B);
        }
        for(unsigned i=0,j=n-1;i<n;j=i++)g_tft.drawLine(p[j].x,p[j].y,p[i].x,p[i].y,0xF38E);
    };
    lobe(left,sizeof(left)/sizeof(left[0]));lobe(right,sizeof(right)/sizeof(right[0]));
    g_tft.fillRect(156,53,9,30,0x3186);
    g_tft.drawFastVLine(156,53,28,ST77XX_CYAN);
    g_tft.drawFastVLine(164,53,28,ST77XX_CYAN);
    for(int y=55;y<80;y+=5)g_tft.drawFastHLine(157,y,7,ST77XX_CYAN);
    const auto branch=[](int x,int y,int xx,int yy){
        g_tft.drawLine(x,y,xx,yy,0xBFFF);g_tft.drawLine(x+1,y,xx+1,yy,0xBFFF);
    };
    branch(160,80,144,98);branch(160,80,176,98);
    branch(144,98,132,104);branch(144,98,139,120);
    branch(139,120,126,131);branch(139,120,139,137);
    branch(176,98,188,104);branch(176,98,183,120);
    branch(183,120,196,131);branch(183,120,183,137);
    g_tft.drawLine(120,120,139,128,0xF38E);
    g_tft.drawLine(182,119,201,110,0xF38E);
}
}
