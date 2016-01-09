#include "../common.h"
#include "client_func.h"


static SDL_Surface *gHaikei;
static SDL_Surface *gSubTitle;
static SDL_Surface *gSetumei;
static SDL_Surface *gMachine1, *gMachine2, *gMachine3, *gMachine4, *gMachine5, 
    *gMachine6, *gMachine7, *gMachine8, *gMachine9, *gMachine10;
static SDL_Surface *gMark1, *gMark2, *gMark3, *gMark4;
static SDL_Rect src_rect, dst_rect;

static void MarkDraw(int n);


void PlSeLoad() {

    gHaikei = IMG_Load("sozai/main_resource/00_common/common_bg.png");

    gSubTitle = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_frtk_label.png");

    gSetumei = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_ex_label.png");

    gMachine1 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_fighter/slt_fighter1.png");
    gMachine2 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_fighter/slt_fighter2.png");
    gMachine3 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_fighter/slt_fighter3.png");
    gMachine4 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_fighter/slt_fighter4.png");
    gMachine5 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_fighter/slt_fighter5.png");
    gMachine6 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_tank/slt_tank1.png");
    gMachine7 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_tank/slt_tank2.png");
    gMachine8 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_tank/slt_tank3.png");
    gMachine9 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_tank/slt_tank4.png");
    gMachine10 = IMG_Load("sozai/main_resource/02_slt_fighter_tank/slt_tank/slt_tank5.png");

    gMark1 = IMG_Load("sozai/main_resource/00_common/common_frame1.png");
    gMark2 = IMG_Load("sozai/main_resource/00_common/common_frame2.png");
    gMark3 = IMG_Load("sozai/main_resource/00_common/common_frame3.png");
    gMark4 = IMG_Load("sozai/main_resource/00_common/common_frame4.png");

    int i;
    for(i = 0; i < MAX_CLIENTS; i++) {
        pla_sele[i].kndP = i;
        pla_sele[i].kPflag = 0;
    }
}

/************************************
void PlayerSelect(int num)
引数：num クライアント数
機能：
返値：
***********************************/
void PlayerSelect(int myid, int num) {

//背景
    src_rect = SrcRectInit(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    dst_rect = DstRectInit(0, 0);
    SDL_BlitSurface(gHaikei, &src_rect, window, &dst_rect);

//サブタイトル
    src_rect = SrcRectInit(0, 0, 1300, 100);
    dst_rect = DstRectInit(150, 50);
    SDL_BlitSurface(gSubTitle, &src_rect, window, &dst_rect);

//説明書
    src_rect = SrcRectInit(0, 0, 450, 300);
    dst_rect = DstRectInit(WINDOW_WIDTH-600, 200);
    SDL_BlitSurface(gSetumei, &src_rect, window, &dst_rect);


//マシン
    int i;
    src_rect = SrcRectInit(0, 0, 200, 60);

    for(i = 0; i < 10; i++){
        dst_rect = DstRectInit(300 + 300*(i/5), 250+100*(i%5));
        switch(i) {
        case 0:
            SDL_BlitSurface(gMachine1, &src_rect, window, &dst_rect);
            break;
        case 1:
            SDL_BlitSurface(gMachine2, &src_rect, window, &dst_rect);
            break;
        case 2:
            SDL_BlitSurface(gMachine3, &src_rect, window, &dst_rect);
            break;
        case 3:
            SDL_BlitSurface(gMachine4, &src_rect, window, &dst_rect);
            break;
        case 4:
            //SDL_BlitSurface(gMachine5, &src_rect, window, &dst_rect);
            SDL_BlitSurface(gMachine1, &src_rect, window, &dst_rect);
            break;
        case 5:
            SDL_BlitSurface(gMachine6, &src_rect, window, &dst_rect);
            break;
        case 6:
            SDL_BlitSurface(gMachine7, &src_rect, window, &dst_rect);
            break;
        case 7:
            SDL_BlitSurface(gMachine8, &src_rect, window, &dst_rect);
            break;
        case 8:
            SDL_BlitSurface(gMachine9, &src_rect, window, &dst_rect);
            break;
        case 9:
            //SDL_BlitSurface(gMachine10, &src_rect, window, &dst_rect);
            SDL_BlitSurface(gMachine6, &src_rect, window, &dst_rect);
            break;
        }
    }

//マーク
    for(i = 0; i < num; i++) {
        if(i != myid) {
            MarkDraw(i);
        }
    }
    MarkDraw(myid);

}

static void MarkDraw(int n){
//薄暗くする
    if(pla_sele[n].kPflag == 1) { 
        src_rect = SrcRectInit(0, 0, 200, 60);
        dst_rect = DstRectInit(300 + 300*(pla_sele[n].kndP/5), 250+100*(pla_sele[n].kndP%5));
    }

//マーク
    src_rect = SrcRectInit(0, 0, 215, 75);
    dst_rect = DstRectInit(292 + 300*(pla_sele[n].kndP/5), 242+100*(pla_sele[n].kndP%5));
    switch(n) {
    case 0:
        SDL_BlitSurface(gMark1, &src_rect, window, &dst_rect);
        break;
    case 1:
        SDL_BlitSurface(gMark2, &src_rect, window, &dst_rect);
        break;
    case 2:
        SDL_BlitSurface(gMark3, &src_rect, window, &dst_rect);
        break;
    case 3:
        SDL_BlitSurface(gMark4, &src_rect, window, &dst_rect);
        break;
    }
}



void PlSeFree() {
    SDL_FreeSurface(gHaikei);

    SDL_FreeSurface(gSubTitle);

    SDL_FreeSurface(gSetumei);

    SDL_FreeSurface(gMachine1);
    SDL_FreeSurface(gMachine2);
    SDL_FreeSurface(gMachine3);
    SDL_FreeSurface(gMachine4);
    SDL_FreeSurface(gMachine5);
    SDL_FreeSurface(gMachine6);
    SDL_FreeSurface(gMachine7);
    SDL_FreeSurface(gMachine8);
    SDL_FreeSurface(gMachine9);
    SDL_FreeSurface(gMachine10);

    SDL_FreeSurface(gMark1);
    SDL_FreeSurface(gMark2);
    SDL_FreeSurface(gMark3);
    SDL_FreeSurface(gMark4);
}
