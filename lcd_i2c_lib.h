/********************************
*  �t���\����p�w�b�_�t�@�C��
*    lcd_init()    ----- ������
*    lcd_cmd(cmd)  ----- �R�}���h�o��
*    lcd_data(data) ----- �P�����\���o��
*    lcd_str(ptr)  ----- ������\���o��
*    lcd_clear()   ----- �S����
********************************/

#define _XTAL_FREQ  8000000	// �N���b�N���g���ݒ�

/********************************
*  �����p�萔
*********************************/
//#define CONTRAST	0x18		// for 5.0V
#define CONTRAST  0x3F		// for 3.3V
#define ADD_IC  0x7C            // I2C�t�� I2C Device address

#if 0
/*********************************
*�@�A�C�R���̒�`
*********************************/
const unsigned char ICON[14][2]={
	{0x00, 0x10},		// �A���e�i
	{0x02, 0x10},		// �d�b
	{0x04, 0x10},		// ����
	{0x06, 0x10},		// �W���b�N
	{0x07, 0x10},		// ��	
	{0x07, 0x08},		// ��
	{0x07, 0x18},		// ����
	{0x09, 0x10},		// ��
	{0x0B, 0x10},		// �s��
	{0x0D, 0x02},		// �d�r����
	{0x0D, 0x12},		// �e�ʏ�
	{0x0D, 0x1A},		// �e�ʒ�
	{0x0D, 0x1E},		// �e�ʑ�
	{0x0F, 0x10}		// ��
};
#endif

void lcd_data(unsigned char data);
void lcd_cmd(unsigned char cmd);
void lcd_init(void);
void lcd_str(const unsigned char* ptr);
void lcd_clear(void);
void delay_100ms(unsigned int time);
void lcd_icon(unsigned char num, unsigned char onoff);
