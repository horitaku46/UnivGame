#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "../constants.h"
#include "../common.h"
#include "server_common.h"

static GAME_STATE gstate;

static CLIENT clients[MAX_NUM_CLIENTS]; 
static int num_clients;
static int num_socks;
static fd_set mask; //fd_set:FD(ファイルディスクリプタ)の集合
static CONTAINER data;

void setup_server(int, u_short);
int control_requests();
void terminate_server();

static void send_data(int, void *, int);
static int receive_data(int, void *, int);
static void handle_error(char *);

static void PlayerDataLoad();
static PlayerData PlayerEnter(int myid, int knd);
static void EnemyDataLoad();
static void EnemyEnter(int num);
static int EnemyDamage();
static ItemData ItemEnter(EnemyData a);
static void PowerChange(int pow);
static SDL_Rect SrcRectInit(int x, int y, int w, int h);
static SDL_Rect DstRectInit(int x, int y);
//static void PlayerShotEnter(int n, int num);

/**************************
void setup_server(int num_cl, u_short port)
引数：num_client 利用するクライアント数
      port サーバーが使用するポート番号
機能：サーバーのセットアップ
 **************************/
void setup_server(int num_cl, u_short port) {
  int rsock, sock = 0;
  struct sockaddr_in sv_addr, cl_addr;

  fprintf(stderr, "Server setup is started.\n");

  num_clients = num_cl;

  rsock = socket(AF_INET, SOCK_STREAM, 0); //ソケット生成
  if (rsock < 0) handle_error("socket()");
  fprintf(stderr, "sock() for request socket is done successfully.\n"); //成功
    
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(port); //IPネットワークバイトオーダーの（16bit）形式で変換
  sv_addr.sin_addr.s_addr = INADDR_ANY; //任意アドレスから受付可

  int opt = 1;
//ソケットのオプションの設定と取得を行う
  setsockopt(rsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (bind(rsock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) handle_error("bind()"); //bind:ソケットに設定を結びつける
  fprintf(stderr, "bind() is done successfully.\n");

  if (listen(rsock, num_clients) != 0) handle_error("listen()"); //接続ソケットの準備
  fprintf(stderr, "listen() is started.\n");

//クライアントの入室
  int i, max_sock = 0;
  socklen_t len;
  char src[MAX_LEN_ADDR];
  for (i = 0; i < num_clients; i++) { //クライアントが全員入ったら抜ける
    len = sizeof(cl_addr);
    sock = accept(rsock, (struct sockaddr *)&cl_addr, &len); //クライアントアドレスと接続の確立
    if (sock < 0) handle_error("accept()");

    if (max_sock < sock) max_sock = sock;

    if(read(sock, clients[i].name, MAX_LEN_NAME) == -1) handle_error("read()"); //read:データの読み込み(引数１：接続ソケット　引数２：読み込み先のバッファ　引数３：読み込みバイト数)
    clients[i].cid = i;
    clients[i].sock = sock;
    clients[i].addr = cl_addr;
    memset(src, 0, sizeof(src));
//inet_ntop:引数２のアドレスをバイナリ形式→テキスト形式に変換し、引数３に返す
    inet_ntop(AF_INET, (struct sockaddr *)&cl_addr.sin_addr, src, sizeof(src));
    fprintf(stderr, "Client %d is accepted (name=%s, address=%s, port=%d).\n", i, clients[i].name, src, ntohs(cl_addr.sin_port)); //ntohs:IPネットワークバイトオーダー→ホストバイトオーダーに変換
  }
  
  close(rsock);

  int j;
  for (i = 0; i < num_clients; i++) {
      send_data(i, &num_clients, sizeof(int));
    send_data(i, &i, sizeof(int));
    for (j = 0; j < num_clients; j++) {
      send_data(i, &clients[j], sizeof(CLIENT));
    }
  }

  num_socks = max_sock + 1;
  FD_ZERO(&mask); //引数をゼロクリア
  FD_SET(0, &mask); //0番目のFDに対応する値を1にセット

  for(i = 0; i < num_clients; i++) {
    FD_SET(clients[i].sock, &mask);
  }
  fprintf(stderr, "Server setup is done.\n"); //サーバーのセットアップ完了


//各機体のプレイヤーデータの読み込み
  PlayerDataLoad();

//敵データの読み込み
  EnemyDataLoad();
  for(i = 0; i < ENEMY_MAX; i++)
      enemy[i].flag = 0;

  gstate = GAME_TITLE;
  pla_sele.kPflag = 0;
  
  Total_Score = 0;
  power = 1;
  speed = 1;
}

/************************
int control_requests()
引数：なし
機能：サーバー中の動作
返値：1…メッセージ送信　0…チャットプログラム終了
************************/
int numF = 0; //カウントの間隔
int numC[4] = {0}; //送信したクライアント数（全員が揃うかの確認）
int control_requests() {
    fd_set read_flag = mask;
    memset(&data, 0, sizeof(CONTAINER)); //dataの先頭から"sizeof(CONTAINER)"バイト分'0'をセット(全て0)
  
    //fprintf(stderr, "select() is started.\n");
//select:FDの集合から読み込み可(引数２)/書き込み可(引数３)/例外発生(引数４)のFDを発見
    if(select(num_socks, (fd_set *)&read_flag, NULL, NULL, NULL) == -1) handle_error("select()");

    //EnemyData eneA;

    int i, result = 1;
    for (i = 0; i < num_clients; i++) {
        if(FD_ISSET(clients[i].sock, &read_flag)) {//FD_ISSET:[引数１]番目のFDが1かどうかの確認
            receive_data(i, &data, sizeof(data)); //読み込み先から引数２へのデータの読み込み
            //fprintf(stderr, "%d : %c\n", data.cid, data.command);

            switch(gstate) {
//GAME_TITLE
            case GAME_TITLE:
                if(data.state != gstate){
                    fprintf(stderr, "not state.[GAME_TITLE]\n");
                    exit(1);
                    break;
                }
                switch(data.command) {
                case FOUR_COMMAND:
                    gstate = GAME_SELECT;
                    data.state = gstate;
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1;

                    /*numC[data.cid] = 1;
                    fprintf(stderr, "numC[%d] = 1\n", data.cid);

                    int j;
                    for(j = 0; j < num_clients; j++){
                        if(numC[j] == 0)
                            break;
                        if(j == num_clients-1){
                            gstate = GAME_SELECT;
                            data.state = gstate;
                            send_data(BROADCAST, &data, sizeof(data));
                            result = 1;

                            for(j = 0; j < 4; j++)
                                numC[j] = 0;

                            break;
                        }
                        }*/
                    break;
                case END_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 0; //チャットプログラムの終了
                    break;
                default:
                    fprintf(stderr, "control_requests(): %c is not a valid command.[GAME_TITLE]\n", data.command);
                    exit(1);
                    break;
                }
                break;

//GAME_SELECT
            case GAME_SELECT:
                if(data.state != gstate){
                    fprintf(stderr, "not state.[GAME_SELECT]\n");
                    exit(1);
                    break;
                }
                if(pla_sele.kPflag != data.cid) {
                    fprintf(stderr, "kPflag[%d] : player[%d] not select\n",pla_sele.kPflag, data.cid);
                    break;
                }

                switch(data.command) {
                case LEFT_COMMAND:
                case RIGHT_COMMAND:
                    player[data.cid].command.kndP = (player[data.cid].command.kndP + 5)%10;
                    data.kndP = player[data.cid].command.kndP;
                    send_data(BROADCAST, &data, sizeof(data));
                    fprintf(stderr, "player[%d].command.kndP = %d\n", data.cid, player[data.cid].command.kndP);
                    result = 1;
                    break;
                case UP_COMMAND:
                    player[data.cid].command.kndP = (player[data.cid].command.kndP + 9)%10;
                    data.kndP = player[data.cid].command.kndP;
                    send_data(BROADCAST, &data, sizeof(data));
                    fprintf(stderr, "player[%d].command.kndP = %d\n", data.cid, player[data.cid].command.kndP);
                    result = 1;
                    break;
                case DOWN_COMMAND:
                    player[data.cid].command.kndP = (player[data.cid].command.kndP + 1)%10;
                    data.kndP = player[data.cid].command.kndP;
                    send_data(BROADCAST, &data, sizeof(data));
                    fprintf(stderr, "player[%d].command.kndP = %d\n", data.cid, player[data.cid].command.kndP);
                    result = 1;
                    break;
                case FOUR_COMMAND: //決定
                    //numC[data.cid] = 1;
                    pla_sele.kPflag = data.cid;
                    fprintf(stderr, "pla_sele.kPflag = %d\n", pla_sele.kPflag);


                    if(pla_sele.kPflag != num_clients-1){ //まだ決定していない人がいるとき
                        data.kPflag = data.cid+1;
                        pla_sele.kPflag = data.kPflag;
                        data.flag = 3;
                        send_data(BROADCAST, &data, sizeof(data));
                        fprintf(stderr, "check %d\n", data.kPflag);
                        break;
                    }

                    else/*if(i == num_clients-1)*/{
                        fprintf(stderr, "check in!\n");
                        gstate = GAME_MAIN;
                        data.state = gstate;
                        data.flag = 4;
                        send_data(BROADCAST, &data, sizeof(data));
                        result = 1;

                        stage = 1;
                        data.num = 0;
                        EnemyEnter(data.num);

                        pla_sele.kPflag = 0;

                        for(i = 0; i < 4; i++){
                            numC[i] = 0;
                        }
                    }

                    break;
                case THREE_COMMAND: //戻る・選び直し

                    break;
                case END_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 0; //チャットプログラムの終了
                    break;
                default:
                    fprintf(stderr, "control_requests(): %c is not a valid command.[GAME_SELECT]\n", data.command);
                    exit(1);
                    break;
                }
                break;

//GAME_LOAD
            case GAME_LOAD: //各プレイヤーの機種等のデータを送信
                if(data.state != gstate){
                    fprintf(stderr, "not state.[GAME_LOAD]\n");
                    exit(1);
                    break;
                }
                switch(data.command) {
                case END_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 0; //チャットプログラムの終了
                    break;
                default:
                    fprintf(stderr, "control_requests(): %c is not a valid command.[GAME_LOAD]\n", data.command);
                    exit(1);
                    break;
                }
                break;

//GAME_MAIN
            case GAME_MAIN:
                if(data.state != gstate){
                    fprintf(stderr, "'%c' %d not state.[%d:GAME_MAIN]\n", data.command, data.state, gstate);
                    exit(1);
                    break;
                }

//サーバーでも管理データの管理時間を決める
                for(i = 0; i < ITEM_MAX; i++){
                    item[i].tx -= 5;
                    if(item[i].tx < -60)
                        item[i].flag = 0;
                }

                switch(data.command) {
                case LEFT_COMMAND:
                case RIGHT_COMMAND:
                case UP_COMMAND:
                case DOWN_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case SEPARATE_UPDO_COMMAND:
                case SEPARATE_LERI_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case UP_ROTA:
                case RIGHT_ROTA:
                case LEFT_ROTA:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case UP_SEPA_ROTA:
                case RIGHT_SEPA_ROTA:
                case LEFT_SEPA_ROTA:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case SHOT_COMMAND:
                case SHOT_FINISH_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case BARRIER_COMMAND:
                case BARRIER_FINISH_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case PLAYER_HIT:
                    HP_Num--;
                    data.hp = HP_Num;
                    data.player.flag2 = 180;
                    if(HP_Num <= 0) {
                        gstate = GAME_OVER;
                        data.state = gstate;
                    }
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case PLAYER_HIT2:
                    HP_Num-=5;
                    data.hp = HP_Num;
                    data.player.flag2 = 180;
                    if(HP_Num <= 0) {
                        gstate = GAME_OVER;
                        data.state = gstate;
                    }
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case ENEMY_HIT:
                    stage = data.stage;
                    //fprintf(stderr, "stage = %d\n", stage);
                    enemy[data.ene_num].flag = data.enemy.flag;
                    data.num = EnemyDamage();
                    if(data.num > 0 && data.enemy.item > 0){ //アイテムの出現
                        data.item = ItemEnter(data.enemy);
                    }
                    send_data(BROADCAST, &data, sizeof(data));

                    fprintf(stderr, "data.num = %d\n", data.num);
                    if(data.num > 0){
                        switch(data.num){
                        case 6:
                        case 12:
                        case 18:
                        case 25:
                        case 32:
                        case 39:
                        case 47:
                        case 56:
                            EnemyEnter(0);
                        break;
                        default:
                            EnemyEnter(0);
                            break;
                        }
                    }
                    result = 1; //メッセージの送信
                    break;
                case BARRIER_HIT:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1; //メッセージの送信
                    break;
                case DATA_PULL:
                    switch(data.flag){
                    case 1: //HPの設定
                        HP_Max = data.hp;
                        HP_Num = HP_Max;
                        break;
                    case 2: //プレイヤーの書き込み・送信
                        data.player = PlayerEnter(data.cid, data.player.knd);
                        fprintf(stderr, "data.player.knd = %d\n", data.player.knd);
                        numF++;
                        if(numF == num_clients){
                            HP_Num = HP_Max;
                            data.hp = HP_Num;
                        }
                        send_data(BROADCAST, &data, sizeof(data));
                        break;
                    case 3: //敵の書き込み
                        //EnemyData eneA;
                        //Shot ene_shoA;

                        //EnemyEnter();
                        break;
                    case 4: //毎秒のプレイヤーデータの送信(現在のHPも)
                        data.hp = HP_Num;
                        send_data(BROADCAST, &data, sizeof(data));
                        result = 1;
                        break;
                    case 5: //アイテムの獲得
                        item[data.m].flag = 0;
                        data.item.flag = item[data.m].flag;
                        switch(item[data.m].knd) {
                        case 1: //攻撃力アップ
                            power++;
                            data.power = power;
                            PowerChange(power); //攻撃力の変化
                            break;
                        case 2: //体力回復
                            HP_Num+=10;
                            if(HP_Num > HP_Max)
                                HP_Num = HP_Max;
                            data.hp = HP_Num;
                            break;
                        case 3: //スピードアップ
                            speed++;
                            data.speed = speed;
                            break;
                        }
                        send_data(BROADCAST, &data, sizeof(data));
                        result = 1;
                        break;
                    }
                    result = 1;
                    break;
                case END_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 0; //チャットプログラムの終了
                    break;
                default:
                    fprintf(stderr, "control_requests(): %c is not a valid command.[GAME_MAIN]\n", data.command);
                    exit(1);
                    break;
                }
                break;

//GAME_OVER
            case GAME_OVER:
                if(data.state != gstate){
                    fprintf(stderr, "not state.[GAME_OVER]\n");
                    exit(1);
                    break;
                }
                switch(data.command) {
                case FOUR_COMMAND:
                    data.state = GAME_TITLE;
                      /*この間にランキングの処理を入れる*/
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1;
                    break;
                case END_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 0; //チャットプログラムの終了
                    break;
                default:
                    fprintf(stderr, "control_requests(): %c is not a valid command.[GAME_OVER]\n", data.command);
                    exit(1);
                    break;
                }
                break;

//GAME_CLEAR
            case GAME_CLEAR:
                if(data.state != gstate){
                    fprintf(stderr, "not state.[GAME_CLEAR]\n");
                    exit(1);
                    break;
                }
                switch(data.command) {
                case FOUR_COMMAND:
                    data.state = GAME_TITLE;
                      /*この間にランキングの処理を入れる*/
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 1;
                    break;
                case END_COMMAND:
                    send_data(BROADCAST, &data, sizeof(data));
                    result = 0; //チャットプログラムの終了
                    break;
                default:
                    fprintf(stderr, "control_requests(): %c is not a valid command.[GAME_CLEAR]\n", data.command);
                    exit(1);
                    break;
                }
                break;
            }
        }
    }

    return result;   
}

/***********************
static void send_data(int cid, void *data, int size)
引数：cid クライアントの番号(サーバー側：BROADCAST)
      *data 書きこむデータ
      size データの大きさ
機能：データをクライアントソケットに入れる
***********************/
static void send_data(int cid, void *data, int size) {
    if ((cid != BROADCAST) && (0 > cid || cid >= num_clients)) { //クライアントのエラー
    fprintf(stderr, "send_data(): client id is illeagal.\n");
    exit(1);
  }
    if ((data == NULL) || (size <= 0)) { //サーバーのエラー
        fprintf(stderr, "send_data(): data is illeagal.\n");
	exit(1);
  }

    if (cid == BROADCAST) { //サーバー
    int i;
    for (i = 0; i < num_clients; i++) {
//write:引数１のファイルディスクリプタに引数２バッファの引数３バイト分書きこむ
      if (write(clients[i].sock, data, size) < 0) handle_error("write()");
    }
    } else { //クライアント
    if (write(clients[cid].sock, data, size) < 0) handle_error("write()");
  }  
}

/**********************
static int receive_data(int cid, void *data, int size)
引数：cid クライアント番号
      *data 読み込みデータ
      size 読み込みデータのサイズ
機能：コネクションからバッファへのデータの読み込み
返値：読み込み結果
***********************/
static int receive_data(int cid, void *data, int size) {
  if ((cid != BROADCAST) && (0 > cid || cid >= num_clients)) {
    fprintf(stderr, "receive_data(): client id is illeagal.\n");
    exit(1);
  }
  if ((data == NULL) || (size <= 0)) {
        fprintf(stderr, "receive_data(): data is illeagal.\n");
	exit(1);
  }

  return read(clients[cid].sock, data, size); //引数１：接続ソケット　引数２：読み込み先のバッファ　引数３：読み込みバイト数
}

/***************************************
static void handle_error(char *message)
引数：*message 送るメッセージ
機能：エラーメッセージの送信
****************************************/
static void handle_error(char *message) {
  perror(message);
  fprintf(stderr, "%d\n", errno);
  exit(1);
}

/***************************************
void terminate_server(void)
引数：なし
機能：サーバーの終了
****************************************/
void terminate_server(void) {
  int i;
  for (i = 0; i < num_clients; i++) {
    close(clients[i].sock);
  }
  fprintf(stderr, "All connections are closed.\n");
  exit(0);
}




static void PlayerDataLoad(){
    FILE *fp;//ファイルを読み込む型
    int input[64];
    char inputc[64];
    int i;

    if((fp = fopen("PlayerData.csv", "r")) == NULL){//ファイル読み込み
        
        exit(-1);
    }
    for (i = 0; i < 2; i++)//   2      
        while (getc(fp) != '\n');

    int n = 0;//行
    int num = 0;//列

    while (1) {
        for (i = 0; i < 64; i++) {
            input[i] = getc(fp);//1文字取得
            inputc[i] = input[i];
            if (inputc[i] == '/') {//スラッシュがあれば
                while (getc(fp) != '\n');
                i = -1;//カウンタを最初に戻す
                continue;
            }
            if (input[i] == ',' || input[i] == '\n') {//カンマか改行なら
                inputc[i] = '\0';

                break;
            }
            if (input[i] == EOF) {//ファイルの終わりなら
                goto EXFILE;
            }
        }

        switch (num) {
        case 0: playerOrder[n].knd = atoi(inputc); break; //プレイヤーの種類
        case 1: playerOrder[n].knd2 = atoi(inputc); break; //戦闘機か戦車か
        case 2: playerOrder[n].sp = atoi(inputc); break; //動く速度
        case 3: playerOrder[n].pattern2 = atoi(inputc); break; //攻撃パターン
        case 4: playerOrder[n].w = atoi(inputc); break; //（画像の）幅
        case 5: playerOrder[n].h = atoi(inputc); break; //（画像の）高さ
        case 6: playerOrder[n].hp_max = atoi(inputc); break; //体力
        case 7: playerOrder[n].power = atoi(inputc); break; //初期攻撃力
        case 8: playerOrder[n].w2 = atoi(inputc); break; //（砲台画像の）幅
        case 9: playerOrder[n].h2 = atoi(inputc); break; //（砲台画像の）高さ
        }
        num++;
        if (num == 10) {
            num = 0;
            n++;
        }
    }
EXFILE:
    fclose(fp);
}


static PlayerData PlayerEnter(int myid, int knd){
    PlayerData tmp;
    int t;
    for(t = 0; t < PLAYER_ORDER_MAX; t++){
        if(knd == playerOrder[t].knd){
            tmp.knd = knd;
            tmp.flag = 1;
            tmp.knd2 = playerOrder[t].knd2;
            tmp.sp = playerOrder[t].sp;
            //tmp.power = playerOrder[t].power;
            power = 1;
            tmp.pattern2 = playerOrder[t].pattern2;

            HP_Max += playerOrder[t].hp_max;

            tmp.flag2 = 0;
            tmp.num = myid;

            switch (tmp.knd2) {
            case 1:
                tmp.tx = 100;
                tmp.ty = 100*(tmp.num+1);
                tmp.ang = 0;
                break;
            case 2:
                tmp.src2 = SrcRectInit(0, 0, playerOrder[t].w2, playerOrder[t].h2);
                tmp.tx = 100*(tmp.num+1);
                tmp.ty = 800+20*(tmp.num+1);
                tmp.ang = -PI / 6;
                tmp.rad = tmp.ang*180/PI;
                break;
            }
            tmp.src = SrcRectInit(0, 0, playerOrder[t].w, playerOrder[t].h);
            tmp.dst = DstRectInit(tmp.tx - tmp.src.w / 2, tmp.ty - tmp.src.h / 2);
            if(tmp.dst.x < 0){
                tmp.tx = tmp.src.w/2+30;
                tmp.dst = DstRectInit(tmp.tx - tmp.src.w / 2, tmp.ty - tmp.src.h / 2);
            }

            switch(knd){ //砲台の初期位置
            case 5:
                tmp.dst2 = DstRectInit(tmp.tx - tmp.src.w / 2 + 80, tmp.ty - tmp.src.h / 2 + 5);
                break;
            case 6:
                tmp.dst2 = DstRectInit(tmp.tx - tmp.src.w / 2 + 100, tmp.ty - tmp.src.h / 2 + 10);
                break;
            case 7:
                tmp.dst2 = DstRectInit(tmp.tx - tmp.src.w / 2 + 100, tmp.ty - tmp.src.h / 2 + 10);
                break;
            case 8:
                tmp.dst2 = DstRectInit(tmp.tx - tmp.src.w / 2 + 110, tmp.ty - tmp.src.h / 2 +5);
                break;
            case 9:
                tmp.dst2 = DstRectInit(tmp.tx - tmp.src.w / 2 + 32, tmp.ty - tmp.src.h / 2 -5);
                break;
            }

            //PlayerShotEnter(myid, tmp2); //クライアントで行う

            fprintf(stderr, "knd = %d, w = %d, h = %d\n", tmp.knd, tmp.src.w, tmp.src.h);

            break;
        }
    }
    /*HP = HP_M;

    if(HP_M > 0){
        CONTAINER data;
        memset(&data, 0, sizeof(CONTAINER));

        data.command = DATA_PULL;
        data.hp = HP;
        data.flag = 1; //データの送信のみ

        send_data(&data, sizeof(CONTAINER), sock);
    }*/

    return tmp;
}

/*****************************************
static void EnemyDataLoad()
引数：
機能：敵データの読み込み
返値：
 *****************************************/
static void EnemyDataLoad() {
    FILE *fp;//ファイルを読み込む型
    int input[64];
    char inputc[64];
    int i;

    if((fp = fopen("EnemyData.csv", "r")) == NULL){//ファイル読み込み
        
        exit(-1);
    }
    for (i = 0; i < 2; i++)//   2      
        while (getc(fp) != '\n');

    int n = 0;//行
    int num = 0;//列

    while (1) {
        for (i = 0; i < 64; i++) {
            input[i] = getc(fp);//1文字取得
            inputc[i] = input[i];
            if (inputc[i] == '/') {//スラッシュがあれば
                while (getc(fp) != '\n');
                i = -1;//カウンタを最初に戻す
                continue;
            }
            if (input[i] == ',' || input[i] == '\n') {//カンマか改行なら
                inputc[i] = '\0';

                break;
            }
            if (input[i] == EOF) {//ファイルの終わりなら
                goto EXFILE;
            }
        }

        switch (num) {
        case 0: enemyOrder[n].mission = atoi(inputc); break; //ミッション  //atoi:文字列を数式に変える
        case 1: enemyOrder[n].stage = atoi(inputc); break; //ステージ
        case 2: enemyOrder[n].wait = atoi(inputc); break; //待機時間
        case 3: enemyOrder[n].knd = atoi(inputc); break; //敵の種類
        case 4: enemyOrder[n].pattern = atoi(inputc); break; //敵の動くパターン
        case 5: enemyOrder[n].sp = atoi(inputc); break; //敵の動く速度
        case 6: enemyOrder[n].pattern2 = atoi(inputc); break; //敵の攻撃するパターン
        case 7: enemyOrder[n].blknd = atoi(inputc); break; //敵が打つ弾の種類
        case 8: enemyOrder[n].blW = atoi(inputc); break; //弾画像の幅
        case 9: enemyOrder[n].blH = atoi(inputc); break; //弾画像の高さ
        case 10: enemyOrder[n].blCnt = atoi(inputc); break; //弾打ちの間隔
        case 11: enemyOrder[n].bltime = atoi(inputc); break; //敵が出現してから打つまでの時間
        case 12: enemyOrder[n].tx = atoi(inputc); break; //x座標
        case 13: enemyOrder[n].ty = atoi(inputc); break; //y座標
        case 14: enemyOrder[n].w = atoi(inputc); break; //幅
        case 15: enemyOrder[n].h = atoi(inputc); break; //高さ
        case 16: enemyOrder[n].hp_max = atoi(inputc); break; //最大HP
        case 17: enemyOrder[n].item = atoi(inputc); break; //落とすアイテムの種類
        case 18: enemyOrder[n].item2 = atoi(inputc); break; //item=1のとき、変更するショットの番号
        case 19: enemyOrder[n].score = atoi(inputc); break; //高さ
        }
        num++;
        if (num == 20) {
            num = 0;
            enemyOrder[n].flag = 1;
            n++;
        }
        //fprintf(stderr, "n = %d\n", n);
    }
EXFILE:
    fclose(fp);
}

static void EnemyEnter(int num) {
        int i, t;
    for(t = 0; t < ENEMY_ORDER_MAX; t++){
        if (enemyOrder[t].flag == 1 && enemyOrder[t].mission == 1 /*&& enemyOrder[t].stage == stage*/){
            for(i = 0; i < ENEMY_MAX; i++){
                if(enemy[i].flag == 0){
                    fprintf(stderr, "Order:%d\n", t);
                    if(num == 0)
                        enemy[i].flag = 1;
                    else
                        enemy[i].flag = 2;
                    enemy[i].stage = enemyOrder[t].stage;
                    enemy[i].wait = enemyOrder[t].wait;
                    enemy[i].knd = enemyOrder[t].knd;
                    enemy[i].pattern = enemyOrder[t].pattern;
                    enemy[i].sp = enemyOrder[t].sp;
                    enemy[i].cnt = 0;
                    enemy[i].pattern2 = enemyOrder[t].pattern2;
                    enemy[i].blknd = enemyOrder[t].blknd;
                    enemy[i].blW = enemyOrder[t].blW;
                    enemy[i].blH = enemyOrder[t].blH;
                    enemy[i].blCnt = enemyOrder[t].blCnt;
                    enemy[i].bltime = enemyOrder[t].bltime; //弾打ちまでの時間
                    enemy[i].tx = enemyOrder[t].tx;
                    enemy[i].ty = enemyOrder[t].ty;
                    enemy[i].hp_max = enemyOrder[t].hp_max;
                    enemy[i].item = enemyOrder[t].item;
                    enemy[i].item2 = enemyOrder[t].item2;
                    enemy[i].score = enemyOrder[t].score;

                    enemy[i].src = SrcRectInit(0, 0, enemyOrder[t].w, enemyOrder[t].h);
                    enemy[i].dst = DstRectInit(enemy[i].tx - enemy[i].src.w / 2, enemy[i].ty - enemy[i].src.h / 2);
                    enemy[i].hp = enemy[i].hp_max;

                    enemyOrder[t].flag = 0;

//データの送信
                    memset(&data, 0, sizeof(CONTAINER));
                    data.command = DATA_PULL;
                    data.flag = 3;
                    data.state = GAME_MAIN;
                    data.enemy = enemy[i];
                    data.ene_num = i;
                    send_data(BROADCAST, &data, sizeof(data));

                    fprintf(stderr, "%d : (hp) = (%4d)\n", i, enemy[i].hp);
                    break;
                }
            }
        }
    }
}

int num = 0;
static int EnemyDamage() {
    if(enemy[data.ene_num].flag == 1) {
        enemy[data.ene_num].hp -= power;
        if(enemy[data.ene_num].hp<=0) { //倒したとき
            enemy[data.ene_num].flag = 0;//2
            enemy[data.ene_num].flag2 = 0;

//更新しないもの
            enemy[data.ene_num].tx = data.enemy.tx;
            enemy[data.ene_num].ty = data.enemy.ty;
            enemy[data.ene_num].cnt = data.enemy.cnt;
            enemy[data.ene_num].wait = data.enemy.wait;

            data.enemy = enemy[data.ene_num];
            num++;

            Total_Score += enemy[data.ene_num].score;
            data.scoreM = Total_Score;
            return num;
        }
        enemy[data.ene_num].flag2 = 60;
//更新しないもの
        enemy[data.ene_num].tx = data.enemy.tx;
        enemy[data.ene_num].ty = data.enemy.ty;
        enemy[data.ene_num].cnt = data.enemy.cnt;
        enemy[data.ene_num].wait = data.enemy.wait;
        fprintf(stderr, "[%d](tx, ty) = (%4d, %3d)\n", data.ene_num, enemy[data.ene_num].tx, enemy[data.ene_num].ty);
        data.enemy = enemy[data.ene_num];
    }
    return -1;
}


/*新しいアイテムの出現*/
static ItemData ItemEnter(EnemyData a){
    int i;
    for(i = 0; i < ITEM_MAX; i++){
        if(item[i].flag == 0) {
            item[i].tx = a.tx;
            item[i].ty = a.ty;
            item[i].flag = 1;
            item[i].knd = a.item;
            item[i].num = i;
            return item[i];
        }
    }
    return item[0];
}


static void PowerChange(int pow) {

}


static SDL_Rect SrcRectInit(int x, int y, int w, int h){
    SDL_Rect tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.w = w;
    tmp.h = h;

    return tmp;
}


static SDL_Rect DstRectInit(int x, int y){
    SDL_Rect tmp;

    tmp.x = x;
    tmp.y = y;
    
    return tmp;
}



