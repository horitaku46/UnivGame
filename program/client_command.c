/*****************************************************************
�ե�����̾	: client_command.c
��ǽ		: ���饤����ȤΥ��ޥ�ɽ���
*****************************************************************/

#include"common.h"
#include"client_func.h"

static void SetIntData2DataBlock(void *data,int intData,int *dataSize);
static void SetCharData2DataBlock(void *data,char charData,int *dataSize);
static void RecvUpData(void);
static void RecvDownData(void);
static void RecvLeftData(void);
static void RecvRightData(void);
static void RecvSepaUdData(void);
static void RecvSepaLrData(void);
static void RecvShotData(void);
static void RecvShotFinishData(void);
static void RecvDiamondData(void);

/*****************************************************************
�ؿ�̾	: ExecuteCommand
��ǽ	: �����С����������Ƥ������ޥ�ɤ򸵤ˡ�
		  ����������������¹Ԥ���
����	: char	command		: ���ޥ��
����	: �ץ�����ཪλ���ޥ�ɤ��������Ƥ������ˤ�0���֤���
		  ����ʳ���1���֤�
*****************************************************************/
int ExecuteCommand(char command)
{
    int	endFlag = 1;
#ifndef NDEBUG
    printf("#####\n");
    printf("ExecuteCommand()\n");
    printf("command = %c\n",command);
#endif
    switch(command){
    case END_COMMAND:
        endFlag = 0;
        break;
    case UP_COMMAND:
        RecvUpData();
        break;
    case DOWN_COMMAND:
        RecvDownData();
        break;
    case LEFT_COMMAND:
        RecvLeftData();
        break;
    case RIGHT_COMMAND:
        RecvRightData();
        break;
    case SEPARATE_UPDO_COMMAND:
        RecvSepaUdData();
        break;
    case SEPARATE_LERI_COMMAND:
        RecvSepaLrData();
        break;
    case SHOT_COMMAND:
        RecvShotData();
        break;
    case SHOT_FINISH_COMMAND:
        RecvShotFinishData();
        break;
    }
    return endFlag;
}


/*****************************************************************
�ؿ�̾	: SendUpCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendUpCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,UP_COMMAND,&dataSize);
    /* ���饤������ֹ�Υ��å� */
    //SetIntData2DataBlock(data,pos,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendDownCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendDownCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,DOWN_COMMAND,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendLeftCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendLeftCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,LEFT_COMMAND,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendRightCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendRightCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,RIGHT_COMMAND,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendSeparateCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendSeparateCommand(int pos, int muki)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    if(muki == 0) //����
        SetCharData2DataBlock(data,SEPARATE_LERI_COMMAND,&dataSize);
    else if(muki == 1)
        SetCharData2DataBlock(data,SEPARATE_UPDO_COMMAND,&dataSize);
    SetIntData2DataBlock(data, muki, &dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendShotCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendShotCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,SHOT_COMMAND,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendShotFinishCommand
��ǽ	: ���饤����Ȥ˱ߤ�ɽ�������뤿��ˡ�
		  �����С��˥ǡ���������
����	: int		pos	    : �ߤ�ɽ�������륯�饤������ֹ�
����	: �ʤ�
*****************************************************************/
void SendShotFinishCommand(int pos)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

    /* �����������å� */
    assert(0<=pos && pos<MAX_CLIENTS);

#ifndef NDEBUG
    printf("#####\n");
    printf("SendCircleCommand()\n");
    printf("Send Circle Command to %d\n",pos);
#endif

    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,SHOT_FINISH_COMMAND,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}


/*****************************************************************
�ؿ�̾	: SendEndCommand
��ǽ	: �ץ������ν�λ���Τ餻�뤿��ˡ�
		  �����С��˥ǡ���������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
void SendEndCommand(void)
{
    unsigned char	data[MAX_DATA];
    int			dataSize;

#ifndef NDEBUG
    printf("#####\n");
    printf("SendEndCommand()\n");
#endif
    dataSize = 0;
    /* ���ޥ�ɤΥ��å� */
    SetCharData2DataBlock(data,END_COMMAND,&dataSize);

    /* �ǡ��������� */
    SendData(data,dataSize);
}

/*****
static
*****/
/*****************************************************************
�ؿ�̾	: SetIntData2DataBlock
��ǽ	: int ���Υǡ����������ѥǡ����κǸ�˥��åȤ���
����	: void		*data		: �����ѥǡ���
		  int		intData		: ���åȤ���ǡ���
		  int		*dataSize	: �����ѥǡ����θ��ߤΥ�����
����	: �ʤ�
*****************************************************************/
static void SetIntData2DataBlock(void *data,int intData,int *dataSize)
{
    int tmp;

    /* �����������å� */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    tmp = htonl(intData);

    /* int ���Υǡ����������ѥǡ����κǸ�˥��ԡ����� */
    memcpy(data + (*dataSize),&tmp,sizeof(int));
    /* �ǡ��������������䤹 */
    (*dataSize) += sizeof(int);
}

/*****************************************************************
�ؿ�̾	: SetCharData2DataBlock
��ǽ	: char ���Υǡ����������ѥǡ����κǸ�˥��åȤ���
����	: void		*data		: �����ѥǡ���
		  int		intData		: ���åȤ���ǡ���
		  int		*dataSize	: �����ѥǡ����θ��ߤΥ�����
����	: �ʤ�
*****************************************************************/
static void SetCharData2DataBlock(void *data,char charData,int *dataSize)
{
    /* �����������å� */
    assert(data!=NULL);
    assert(0<=(*dataSize));

    /* char ���Υǡ����������ѥǡ����κǸ�˥��ԡ����� */
    *(char *)(data + (*dataSize)) = charData;
    /* �ǡ��������������䤹 */
    (*dataSize) += sizeof(char);
}

/*****************************************************************
�ؿ�̾	: RecvUpData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvUpData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    //RecvIntData(&x);
    RecvIntData(&pos);

    //fprintf(stderr, "x = %d\n", x);
    fprintf(stderr, "UP:pos = %d\n", pos);

    //RecvIntData(&y);
    //RecvIntData(&r);

    /* �ߤ�ɽ������ */
    DrawUpCircle(pos);
    player[pos].command.up = 1;
}

/*****************************************************************
�ؿ�̾	: RecvDownData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvDownData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "DOWN:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    DrawDownCircle(pos);
    player[pos].command.down = 1;
}

/*****************************************************************
�ؿ�̾	: RecvLeftData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvLeftData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "LEFT:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    DrawLeftCircle(pos);
    player[pos].command.left = 1;
}

/*****************************************************************
�ؿ�̾	: RecvRightData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvRightData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "RIGHT:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    DrawRightCircle(pos);
    player[pos].command.right = 1;
}

/*****************************************************************
�ؿ�̾	: RecvSepaUdData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvSepaUdData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "SEPA1:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    player[pos].command.up = 0;
    player[pos].command.down = 0;
}

/*****************************************************************
�ؿ�̾	: RecvSepaLrData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvSepaLrData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "SEPA2:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    player[pos].command.left = 0;
    player[pos].command.right = 0;
}

/*****************************************************************
�ؿ�̾	: RecvShotData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvShotData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "SHOT:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    PlayerBulletEnter(pos); //client_PlayerData.c �ˤ���
    player[pos].command.b5 = 1;
}


/*****************************************************************
�ؿ�̾	: RecvShotFinishData
��ǽ	: �ߤ�ɽ�����뤿��Υǡ������������ɽ������
����	: �ʤ�
����	: �ʤ�
*****************************************************************/
static void RecvShotFinishData(void)
{
    int	pos;

    /* �ߥ��ޥ�ɤ��Ф����������������� */
    RecvIntData(&pos);

    fprintf(stderr, "SHOT_FINISH:pos = %d\n", pos);

    /* �ߤ�ɽ������ */
    player[pos].command.b5 = 0;
}