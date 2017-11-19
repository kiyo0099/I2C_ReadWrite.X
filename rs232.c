/*! @file rs232.c
*   @brief PIC�̃V���A���ʐM�p�t�@�C��
*   @author K.Matsuda
*   @date 2017.01.21
*   @details �ڍׂȐ���      \n
*   RS232C����M���C�u����                               \n
*   PIC����UART�̗��p��Timer2���p�́Adefice�Ő؂�ւ��B   \n
*   define RS232_HW   1       // SW=0, HW=1             \n
*   SW=Timer2�𗘗p                                     \n
*   HW=����UART���p                                     \n
*
*
*    RS232C����M���C�u���� (rs232c.c)�@Timer2 �g�p
*
*******************************************************************************/
#include <xc.h>
#include "rs232.h"

static volatile char dat;
static volatile char bCnt;
static volatile char bPos;

extern unsigned char rcv_Flag;
extern unsigned char get_str[20];
extern unsigned char rcv_count;

/* RS232C ������ */
/*! @fn   rs232_ini(void)
*   @brief RS232�������֐�
*   @return �Ȃ�
*   @details �ڍׂȐ���    \n
*   PIC�̃��W�X�^��ʐM�{�[���[�g�ɍ��킹�Đݒ肷��֐�
*/
void rs232_ini(void)
{
#if RS232_HW
     // USART�̏����ݒ�
     TXSTA = 0x20;         // TXEN=1, SYNC=0, BRGH=0,
     RCSTA = 0x90;
     SPBRG = BAUD_HW;     // �{�[���[�g�ݒ�
     RCIF = 0;              // ��M�t���O������
     RCIE = 1;              // ��M���荞�ݗL��
#else
     T2CON  = 0x01;                  // 1/4�v��,1/1�|�X�g
     TMR2IF = 0;                     // ���荞�݃t���O�N���A
     TXPIN = 1;
#endif
}

/* 1�o�C�g���M�T�u�֐� */
/* I2C�f�o�C�X�����݊֐� */
/*! @fn  Send(unsigned char code)
*   @brief 1�o�C�g���M�T�u�֐�
*   @param code : �����R�[�h
*   @return �Ȃ�
*   @details �ڍׂȐ���             \n
*   RS232_HW==1�Ȃ��                                                      \n
*     PIC������UART�ʐM�@�\���g���̂ŁA���W�X�^�ݒ�ɕ����R�[�h���Z�b�g����B  \n
*   RS232_HW==0�Ȃ��                                                      \n
*     UART�ʐM�@�\���^�C�}�[�Ŏ�������̂ŁA1bit���Ƀs���𐧌䂷��B          \n
*/
void Send(unsigned char code)
{
#if RS232_HW
    // TRMT:Transmit Shift Register Status bit
    // 1 = TSR empty
    // 0 = TSR full
    while(!TRMT)
        continue;
    TXREG = code;
#else
    dat = code;                 // ���M�f�[�^�Z�b�g
    bCnt = 0;                   // �r�b�g�J�E���^���Z�b�g
    bPos = 0x01;                // �r�b�g�ʒu���Z�b�g
    PR2 = BAUD_SW;                 // �{�[���[�g�ݒ�
    TXPIN = 0;                  // �X�^�[�g�r�b�g�o��
    TMR2 = 0;                   // �^�C�}2���Z�b�g
    T2CON = 0x05;               // 1/4�v��,1/1�|�X�g
    while(bCnt < 10){           // ���M�����҂�
        while(!TMR2IF)          // TMR2�t���O���o�҂�
            ;
        TMR2IF = 0;
        bCnt++;
        if(bCnt > 0 && bCnt < 9){           // 1?8bit
            TXPIN = (dat & bPos)? 1: 0;     // �f�[�^�r�b�g�o��
            bPos <<= 1;                     // �r�b�g�V�t�g
        }
        if(bCnt == 9)                       // �f�[�^�I��
            TXPIN = 1;                      // �X�g�b�v�r�b�g�o��
        if(bCnt == 10){                     // �X�g�b�v�r�b�g�I��
            T2CON = 0x01;                   // �^�C�}�Q��~
        }
    }
#endif

}

#if RS232_HW
/* ��M�Ɋ��荞�݂��g�p���� */
void interrupt InterReceiver( void )
{
    unsigned char c;

    if(RCIF==1){   // UART���荞�݃`�F�b�N
        c = RCREG;  // ���W�X�^�����M�f�[�^���o��
        if(c!=0x0a && c!=0x0d){
           get_str[rcv_count++] = c;
        }
        if(c==0x0d){      //0x0d='\r'�iCarriage return:���s�j
            if(rcv_count)
                rcv_Flag = 1;
            get_str[rcv_count++] = 0x00;
            rcv_count = 0;
        }
        if(c==0x0a){    //  0x0a='\n'�iLine feed:���A�j
            rcv_count = 0;
        }
        if(rcv_count >=sizeof(get_str))
             rcv_count = 0;
        RCIF = 0;             // ���荞�ݎ�M�t���O�����Z�b�g
    }
}
#else
/* 1�o�C�g��M�T�u�֐� */
unsigned char Receive(void)
{
    while(RXPIN)                // �X�^�[�g�r�b�g�҂�
        ;
    bCnt = 0;                   // �r�b�g�J�E���^������
    dat = 0;                    // ��M�f�[�^�N���A
    bPos = 0x01;                // �r�b�g�ʒuyuipppouyyyy������
    TMR2 = 0;                   // �^�C�}�Q������
    PR2 = DTIME;                // 1/2�r�b�g���ɐݒ�
    TMR2IF = 0;
    T2CON = 0x05;               // 1/4�v��,1/1�|�X�g�A�J�n
    while(bCnt < 10){           // ��M�����҂�
        while(!TMR2IF)
            ;
        TMR2IF = 0;
        switch(bCnt){
            case 0:                         // �X�^�[�g�r�b�g�̏���
                if(RXPIN) bCnt = 0xFF;
                else{
                    PR2 = BAUD_SW;
                    bCnt++;
                }
                break;
            case 9:                         // �X�g�b�v�r�b�g�̏���
                if(RXPIN) bCnt++;
                else bCnt = 0xFF;
                T2CON = 0x01;               // TMR2��~
                break;
            default:                        // 1?8bit�ڂ̃f�[�^���̏���
                if(RXPIN) dat |= bPos;
                bPos <<= 1;
                bCnt++;
        }
        // GP1 = ~GP1;      // �f�o�b�O
    }
    if(bCnt == 10)
        return(dat);            // ����f�[�^��Ԃ�
    else
        return(0xFF);           // �G���[�t���O��Ԃ�
}
#endif

