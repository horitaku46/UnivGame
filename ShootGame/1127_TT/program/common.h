/*****************************************************************
�ե�����̾	: common.h
��ǽ		: �����С��ȥ��饤����Ȥǻ��Ѥ�������������Ԥ�
*****************************************************************/

/*
�Х�
��client_win.c : DrawLeftCircle�ؿ� : �ߤ����˹Ԥ��ʤ�
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<assert.h>
#include<math.h>

//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>


#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>
//#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_mixer.h>

#define PORT			(u_short)8888	/* �ݡ����ֹ� */

#define MAX_CLIENTS		4				/* ���饤����ȿ��κ����� */
#define MAX_NAME_SIZE	10 				/* �桼����̾�κ�����*/

#define MAX_DATA		200				/* ����������ǡ����κ����� */

#define END_COMMAND		'E'		  		/* �ץ�����ཪλ���ޥ�� */
#define LEFT_COMMAND	'L'				/* �����ޥ�� */
#define RIGHT_COMMAND	'R'				/* �����ޥ�� */
#define UP_COMMAND	'U'				/* �女�ޥ�� */
#define DOWN_COMMAND	'D'				/* �����ޥ�� */
#define SEPARATE_UPDO_COMMAND	'S'				/* �����ƥ��å���Υ�����ޥ�� */
#define SEPARATE_LERI_COMMAND	'T'				/* �����ƥ��å���Υ�����ޥ�� */

typedef struct{
    CvPoint center;
    int x, y, r;
    int command;
}PlayerData;

PlayerData player[2];

#endif