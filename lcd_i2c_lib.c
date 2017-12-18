/***********************************************
*  �t���\���탉�C�u����
*  I2C�C���^�[�t�F�[�X
*    lcd_init()    ----- ������
*    lcd_cmd(cmd)  ----- �R�}���h�o��
*    lcd_data(data) ----- �P�����\���o��
*    lcd_str(ptr)  ----- ������\���o��
*    lcd_clear()   ----- �S����
**********************************************/
#include <xc.h>
#include "lcd_i2c_lib.h"
#include "i2c_lib1.h"

/*********************************
* �t����1�����\���f�[�^�o��
*********************************/
void lcd_data(unsigned char data)
{
	I2CStart();					// �X�^�[�g
	I2COut(ADD_IC);					// �A�h���X
	I2COut(0x40);					// �\���f�[�^�w��
	I2COut(data);					// �\���f�[�^�o��
	I2CStop();					// �X�g�b�v
	__delay_us(30);				// �x��
}

/*******************************
* �t���ւP�R�}���h�o�� 
*******************************/
void lcd_cmd(unsigned char cmd)
{
	I2CStart();					// �X�^�[�g
	I2COut(ADD_IC);					// �A�h���X
	I2COut(0x00);					// �R�}���h�w��
	I2COut(cmd);					// �R�}���h�o��
	I2CStop();					// �X�g�b�v
	/* Clear��Home�� */
	if((cmd == 0x01)||(cmd == 0x02))
		__delay_ms(2);			// 2msec�҂�
	else
		__delay_us(30);			// 30��sec�҂�	
}
/*******************************
*  �������֐� 
*******************************/
void lcd_init(void)
{
	delay_100ms(1);
	lcd_cmd(0x38);				// 8bit 2line Normal mode
	lcd_cmd(0x39);				// 8bit 2line Extend mode	
	lcd_cmd(0x14);				// OSC 183Hz BIAS 1/5
	/* �R���g���X�g�ݒ� */
	lcd_cmd(0x70 + (CONTRAST & 0x0F));
	lcd_cmd(0x5C + (CONTRAST >> 4));
//	lcd_cmd(0x6A);				// Follower for 5.0V
	lcd_cmd(0x6B);				// Ffollwer for 3.3V
	delay_100ms(3);
	lcd_cmd(0x38);				// Set Normal mode 
	lcd_cmd(0x0C);				// Display On 
	lcd_cmd(0x01);				// Clear Display 
}
/******************************
* �S�����֐�
******************************/
void lcd_clear(void)
{
	lcd_cmd(0x01);				//�������R�}���h�o��
}
/*****************************
* ������\���֐�
*****************************/
void lcd_str(const unsigned char* ptr)
{
	while(*ptr != 0)				//�������o�� 
		lcd_data(*ptr++);			//�����\��
}
#if 1
/*****************************
*  �A�C�R���\������֐�
*****************************/
void lcd_icon(unsigned char num, unsigned char onoff)
{
	lcd_cmd(0x39);				// Extend mode
	lcd_cmd(0x40 | ICON[num][0]);	// �A�C�R���A�h���X�w��
	if(onoff)
		lcd_data(ICON[num][1]);	// �A�C�R���I���f�[�^
	else
		lcd_data(0x00);			// �A�C�R���I�t�f�[�^
	lcd_cmd(0x38);				// Normal Mode
}
#endif

/******************************
*  100msec�x���֐�
******************************/
void delay_100ms(unsigned int time)
{
	time *= 4;					// 4�{
	while(time){
		__delay_ms(25);			// 25msec
		time--;					// 100msec x time
	}
}
