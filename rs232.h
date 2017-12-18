/*****************************************
*  RS232C����M���C�u�����@�w�b�_�t�@�C��
*   Send()
*   Receive()
*****************************************/
#include <xc.h>

#define RS232_HW   0       // SW=0. HW=1
/***** �n�[�h�ݒ�@****/
#define BAUD_SW        51      // 9600bps Fosc = 8MHz
#define BAUD_HW        12      // 9600bps Fosc = 8MHz
#define DTIME       10      // ���o�x������
#define TXPIN       RA3
#define RXPIN       RA4

/**** �֐��v���g�^�C�v�錾 ****/
void Send(char code);
char Receive(void);
void rs232_ini(void);
