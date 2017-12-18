/******************************************
*  I2C�ʐM���C�u�����p�w�b�_�t�@�C��
*   I2CStart()
*   I2CStop()
*   I2COut()
*   I2CRcv()
******************************************/

/** �N���b�N���g���ݒ�@�x���֐��p **/
#define _XTAL_FREQ  8000000	// �N���b�N���g���ݒ�

/** I/O�s���̐ݒ� **/
#define	SDA		RB1
#define	SCL		RB4
#define TRISSDA 	TRISB1

/** �֐��v���g�^�C�s���O **/
void I2CStart(void);
void nI2CStart(void);
void I2CStop(void);
unsigned char I2COut(unsigned char data);
unsigned char I2CRcv(unsigned char Ack);