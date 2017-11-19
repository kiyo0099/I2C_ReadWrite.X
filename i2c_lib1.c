/******************************************
*  I2C�ʐM���C�u����
*   I2CStart()
*   I2CStop()
*   I2COut()
*   I2CRcv()
******************************************/
/*! @file i2c_lib1.c
*   @brief �\�t�g�E�F�AI2C����֐�(Master)
*   @author K.Matsuda
*   @date 2017.01.21
*   @details �ڍׂȐ���               \n
*   I2C�ʐM���C�u����                                                  \n
*   PIC����I2C���W���[���͗��p���Ă��Ȃ��A�\�t�g�E�F�A��bit����B         \n
*   delay_us()�Ń^�C�~���O������Ă���̂ŁA���x��200kbps���x�����E�B     \n
*/
#include <xc.h>
#include "i2c_lib1.h"

/*****************************
* �X�^�[�g�����o��
*****************************/
/*! @fn   nI2CStart(void)
*   @brief I2C��n�X�^�[�g��Ԃ𐶐�����֐�
*   @return �Ȃ�
*/
void nI2CStart(void){
        __delay_us(10);
	SCL = 0;						// SCL Low
        __delay_us(10);
	TRISSDA = 0;					// �o�̓��[�h�ɖ߂��@SDA =Low
	SDA = 1;					// SDA��High
        __delay_us(10);
	SCL = 1;						// SCL Low
        __delay_us(10);
 	/* SCL,SDA�͏펞High�Ƃ���@*/
	SDA = 0;						// ���SDA Low
	TRISSDA = 0;					// SDA�o��
        __delay_us(10);
	SCL = 0;						// SCL Low

}
/*****************************
* �X�^�[�g�����o��
*****************************/
/*! @fn   I2CStart(void)
*   @brief I2C�̃X�^�[�g��Ԃ𐶐�����֐�
*   @return �Ȃ�
*/
void I2CStart(void){
	/* SCL,SDA�͏펞High�Ƃ���@*/
	SDA = 0;						// ���SDA Low
	TRISSDA = 0;					// SDA�o��
        __delay_us(10);
	SCL = 0;						// SCL Low

}
/*****************************
* �X�g�b�v�����o�� 
*****************************/
/*! @fn   I2CStop(void)
*   @brief I2C�̃X�g�b�v��Ԃ𐶐�����֐�
*   @return �Ȃ�
*/
void I2CStop(void){
	SCL = 0;						// SCL Low
	__delay_us(10);
	SDA = 0;						// SDA Low
	TRISSDA = 0;					// �o�̓��[�h�ɖ߂��@SDA =Low
	SCL = 1;						// ���SCL��High
	__delay_us(10);
	SDA = 1;						// �ォ��SDA��High
	__delay_us(10);				// �Ԋu�p�x��	
}
/*********************************
* I2C��1�o�C�g�o��
* �X���[�u�����ACK��߂�l�Ƃ��� 
**********************************/
/*! @fn   I2COut(unsigned char data)
*   @brief I2C��1�o�C�g���M�֐�
*   @param data �o�̓f�[�^
*   @return ACK
*   @details �ڍׂȐ���               \n
*   Master�f�o�C�X�Ƃ��āAI2C��1�o�C�g�o�͂���   \n
*   �X���[�u�����ACK��߂�l�Ƃ���             \n
*/
unsigned char I2COut(unsigned char data){
	int i;
	unsigned char BitPos, ACK;

	/* Data Out */
	TRISSDA = 0;					// SDA�o�̓��[�h
	BitPos = 0x80;				// �r�b�g�ʒu�����l
	for(i=0; i<8; i++){			// 8�r�b�g�J��Ԃ�
		SCL = 0;					// SCL Low
		if((data & BitPos) != 0)	// �r�b�g�o��
			SDA = 1;				// SDA High
		else
			SDA = 0;				// SDA Low
		BitPos = BitPos >> 1;		// �r�b�g�ʒu�ړ�
		SCL = 1;					// SCL High�ɖ߂�
	}
	/* Ack �`�F�b�N */
	SCL = 0;						// �N���b�N�PLow
	TRISSDA = 1;					// ���̓��[�h�ɂ���ACK����
	__delay_us(2);
	SCL = 1;						// �N���b�NHigh�ɖ߂�
	__delay_us(2);
	ACK = SDA;					// ACK�`�F�b�N
        return(ACK);					// ACK��߂�l�Ƃ���
}
/********************************
* I2C��1�o�C�g����
* �p�����[�^��ACK/NAK��ԑ�
********************************/
/*! @fn   I2CRcv(unsigned char Ack)
*   @brief I2C��1�o�C�g��M�֐�
*   @param Ack ACK/NACK���M�w��
*   @return data
*   @details �ڍׂȐ���               \n
*   Master�f�o�C�X�Ƃ��āAI2C��1�o�C�g���͂𐧌䂷��B   \n
*   �f�[�^��M����������ƁAACK/NACK���X���[�u�ɑ���      \n
*/
unsigned char I2CRcv(unsigned char Ack){
	int	i;
	unsigned char BitPos, data;

	data = 0;
	BitPos =0x80;					// �r�b�g�ʒu���Z�b�g
	for(i=0; i<8; i++){			// 8�r�b�g�J��Ԃ�
		SCL = 0;					// SCL Low
		TRISSDA = 1;				// ���̓��[�h
		__delay_us(3);			// ���m��
		SCL = 1;					// SCL High
		if(SDA)					// �r�b�g����
			data |= BitPos;
		BitPos = BitPos >> 1;		// �r�b�g�V�t�g
	}
	/* ACK/NAK�o�� */
	SCL = 0;						// SCL Low
	SDA = Ack;					// ACK ot NAK �o��
	TRISSDA = 0;					// SDA�o�̓��[�h
	__delay_us(2);				// �p���X���m��
	SCL = 1;						// SCL High
	return(data);				// ��M�f�[�^��߂�
}
