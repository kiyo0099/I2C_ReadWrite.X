/*! @file I2C_WR_XC8.c
*   @brief RS232C�o�R��I2C�f�o�C�X�𐧌�pPIC
*   @author K.Matsuda
*   @date 2017.11.02
*   @details �ڍׂȐ���
*  -------------------------------------------------------\n
*  I2C_WR PIC                                             \n
*  -------------------------------------------------------\n
*  Company: KURABO INDUSTRIES LTD.                        \n
*  Engineer: K.Matsuda                                    \n
*  Create Date: 11/02/2017                                \n
*  Additional Comments:                                   \n
*  Change for PIC16F1827                                  \n
* --------------------------------------------------------\n
*               PIC16F1827                                \n
*               +-----------------------+                 \n
*           _1__| RA2               RA1 |_18_             \n
*     AN0_AD_2__| RA3/AN3           RA0 |_17_             \n
*     AN1_AD_3__| RA4               RA7 |_16_             \n
*     AN2_AD_4__| RA5/MCLR          RA6 |_15_             \n
*     AN3_AD_5__| VSS(GND)          VDD |_14_             \n
*           _6__| RB0               RB7 |_13_             \n
*    I2C_SDA_7__| RB1/SDA           RB6 |_12_             \n
*       RX  _8__| RB2/RX         RB5/TX |_11_  TX         \n
*           _9__| RB3           RB4/SCL |_10_  I2C_SCL    \n
*               +-----------------------+                 \n
* --------------------------------------------------------\n
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rs232.h"
#include "i2c_lib1.h"
#include <xc.h>

//! 8bit uc�^���`���āAunsigned char��Z�k
#define uc unsigned char    // 8 it
//! 16bit ui�^���`���āAunsigned int��Z�k
#define ui unsigned int     // 16bit
//! 32bit ul�^���`���āAunsigned long��Z�k
#define ul unsigned long    // 32bit

#define TIMER_INTERVAL (0xffff - 20000) // TMR���W���[���̃J�E���g�����l
                                        // 8MHz, 1/1�v���X�P�[���ŁA10msec���ƂɊ��荞�݂�����
#define IEEPROM 1
#define HELP_DISP 1
// config for 16F1827
// CONFIG1
    #pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
    #pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
    #pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
    #pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
    #pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
    #pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
    #pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
    #pragma config CLKOUTEN = ON    // Clock Out Enable (CLKOUT function is enabled on the CLKOUT pin)
    #pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
    #pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)
    // CONFIG2
    #pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
    #pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
    #pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
    #pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
    #pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)


//�֐��̃v���g�^�C�v�錾
void buffer_clear();
void arg_delay_ms(unsigned int x);
//void rs_puts(unsigned char *buff);
void rs_puts(const uc *buff);/* �V���A��������̑��M */
void rs_gets(char *buff);
void dump(uc sel);									/* Dump�֐� */
void I2C_write(uc chip, uc subadd, uc data);		/* I2C�f�o�C�X�����݊֐�(�T�u�A�h���X�Ή�) */
uc I2C_read(uc chip, uc subadd);					/* I2C�f�o�C�X�Ǐo���֐�(�T�u�A�h���X8bit, 8bit�Ή�) */
ui I2C_read2(uc chip, ui subadd);					/* I2C�f�o�C�X�Ǐo���֐�(�T�u�A�h���X16bit, �f�[�^16bit�Ή�) */
ui I2C_read3(uc chip, uc subadd);					/* I2C�f�o�C�X�Ǐo���֐�(�T�u�A�h���X8bit, �f�[�^16bit�Ή�) */
uc rcv_Flag;				/* �������M�t���O(CR+LF) */
static uc pram_error[] = "paramater error\r\n";	// Error Message
//uc i2c_add = 0xA0;
uc i2c_add = 0x54;
uc rcv_count;
uc timec;
uc disp_flag;
//! ���͂���R�}���h���C���̕������Max�l
#define MAX_STR			24						// �ő啶����
static uc Buffer[] = "I2C_PIC v0.5";			// Opening Message
uc res1,res2,res3,res4;
uc get_str[MAX_STR];		/* ��M������p�z�� */

/*! @struct command
 * command��͌�̃f�[�^���i�[����\����                \n
 * ��Fi2cwr a0 40 33 ����͂��ĕϐ��ɃZ�b�g����   \n
 * command = "i2cwr"                                  \n
 * param1 = 0xa0                                      \n
 * param2 = 0x40                                      \n
 * param3 = 0x33                                      \n
**/
struct command {
	uc command[8];	 		//!< �R�}���h������̔z��/* ���̓R�}���h */
	ui param1;				//!< ���p�����[�^�ϐ�/* parameter1 */
	ui param2;				//!< ���p�����[�^�ϐ�/* parameter2 */
	ui param3;				//!< ��O�p�����[�^�ϐ�/* parameter3 */
};

/*! @brief printf�֐��̏o�͐�̃X�^�u�֐�
*   @param[in] ch �o�͕����R�[�h
*   @return �Ȃ�
*/
void putch(unsigned char ch){
    Send(ch);
   return;
}
/*! @fn void pic_ini()
*   @brief PIC�ˑ��̏������֐� for 16F1827
*   @return �Ȃ�
*/
void pic_ini(){

	// for 16F1827
	OSCCONbits.IRCF = 0xE;        // �����N���b�N��8MHz�쓮�ɂ���
	//  RA7, RA6, RA5, RA4, RA3, RA2, RA1, RA0
	//  Out, Out,  In,  In, Out,  In, Out,  In
	//    0,   0,   1,   1,   0,   1,   0,   1
	TRISA  = 0x35;          // PortA={0,0,1,1,0,1,0,1}
	//  RB7, RB6, RB5, RB4, RB3, RB2, RB1, RB0
	//  Out, Out,  Out, Out,  In,  In, Out,  In
	//    0,   0,   0,   0,   1,   1,   0,   1
	TRISB  = 0x0D;          // PortA={0,0,0,0,1,1,0,1}
        WPUB = 0xFF;
        ANSELA = 0x00;
        ANSELB = 0x00;
        // TX/RX pin select
        APFCON1bits.TXCKSEL=1;  // 1 = TX/CK function is on RB5
        APFCON0bits.RXDTSEL=1;  // 1 = RX/DT function is on RB2

//	ADCON0 = 0x41;
	INTCONbits.GIE  = 1;    // Grobal interrupt��L���ɂ���
	INTCONbits.PEIE = 1;    // �O�������݂�L���ɂ���
}

/* 16 �i������� 10 �i���ɕϊ����� */
int ToDec(char str[ ])
{
    int i = 0;        /* �z��̓Y���Ƃ��Ďg�p */
    int n;
    int x = 0;
    char c;

    while (str[i] != '\0') {        /* ������̖����łȂ���� */
            /* '0' ���� '9' �̕����Ȃ� */
        if ('0' <= str[i] && str[i] <= '9')
            n = str[i] - '0';        /* �����ɕϊ� */
            /* 'a' ���� 'f' �̕����Ȃ� */
        else if ('a' <= (c = tolower(str[i])) && c <= 'f')
            n = c - 'a' + 10;        /* �����ɕϊ� */
        else {        /* ����ȊO�̕����Ȃ� */
            printf("error\n");
            break;        /* �v���O�������I�������� */
        }
        i++;        /* ���̕������w�� */
        x = x *16 + n;    /* ���オ�� */
    }
   return (x);
}

/* ���������J�E���g���� */
int strlen_o(const char *src){
	int i=0;
	while(*src++)
		i++;
	return(i);
}
/* �������copy���� */
void strcopy_o( char *dst, const char *src ){
    while( *src != '\0' )			//*src��������̏I���łȂ��Ԃ͌J��Ԃ�
	{
        *dst = *src;				//1�����R�s�[
        dst++;						//�R�s�[��̏ꏊ���P������
        src++;						//�R�s�[���̏ꏊ���P������
    }
    *dst = '\0';					//�R�s�[��̕�����̏I�����Z�b�g
}
/* 16 �i������� 10 �i���ɕϊ����� */
ui A16ToDec(const uc str[ ]){
	uc i = 0;		 /* �z��̓Y���Ƃ��Ďg�p */
	uc n, c;
	ui x = 0;

	while (str[i] != '\0') {								/* ������̖����łȂ���� */
		if ('0' <= str[i] && str[i] <= '9')					/* '0' ���� '9' �̕����Ȃ� */
			n = str[i] - '0';								/* �����ɕϊ� */
		else if ('a' <= (c = tolower(str[i])) && c <= 'f')	/* 'a' ���� 'f' �̕����Ȃ� */
			n = c - 'a' + 10;								/* �����ɕϊ� */
		else {												/* ����ȊO�̕����Ȃ� */
			printf("error\n");
			break;											/* �v���O�������I�������� */
		}
		i++;		/* ���̕������w�� */
		x = x*16 + n;	  /* ���オ�� */
	}
   return (x);
}

/*! @fn    parse(unsigned char *sbuffer, struct command *com)
*   @brief �R�}���h����͂���֐�
*   @param sbuffer : ������̐擪�|�C���^
*   @param com : �R�}���h��͗p��COM�\����
*   @details �ڍׂȐ���                           \n
*   �V���A���ʐM�Ŏ�M������������X�y�[�X��؂�ŉ�͂���֐��B \n
*   �菇�F                                                   \n
*   1.�X�y�[�X��؂�ŃR�}���h�𕪂���B                      \n
*   2.Comand���@����1 ����1 ����1 �ƕ���ł���B              \n
*   3.Command�͂��̂܂ܕ�����Ɋi�[����B                     \n
*   4.����1,2,3�͂��ꂼ��16�i����->����(ui)�ɕϊ����Ċi�[����B\n
*/
int parse(unsigned char *sbuffer, struct command *com)
{
	int i,j,k;
	char com_buff[10]="";

	i=0;j=0;k=0;
	printf("\r\n");
	/* strlen(str) �� str �Ɋi�[����Ă��镶����̒��� */
	while (  sbuffer[i] != '\0' ){
		while ( sbuffer[i] == ' ' )
			i++;   /* str[i] ���󔒕����ł������C�P�� i �� +1 ���� */
		/* ��� while �����I��������_�� str[i] �͋󔒕����ł͂Ȃ��D */
		while ( (sbuffer[i] != ' ') && ( sbuffer[i] != '\0') ){
			com_buff[j++] = sbuffer[i++];
		}
		com_buff[j]='\0';
		j=0;
		if(k==0){
				strcopy_o( com->command, com_buff);
		}
		else if(k==1){
			// param1���
			if(com_buff[0] == '0' && com_buff[1]=='x')
				com->param1 = A16ToDec(&com_buff[2]);   // 0x���΂�
			else
				com->param1 = A16ToDec(&com_buff[0]);
		}else if(k==2){
			// param2���
			if(com_buff[0] == '0' && com_buff[1]=='x')
				com->param2 = A16ToDec(&com_buff[2]);   // 0x���΂�
			else
				com->param2 = A16ToDec(&com_buff[0]);
		}else if(k==3){			// param3���
			if(com_buff[0] == '0' && com_buff[1]=='x')
				com->param3 = A16ToDec(&com_buff[2]);   // 0x���΂�
			else
				com->param3 = A16ToDec(&com_buff[0]);
		}
		k++;
	}
	return k;
}
/*! @fn    help_list()
*   @brief Help��\������֐�
*/
void help_list()
{
#if HELP_DISP
	printf("\r\n> %s for 16F1827.\r\n",Buffer);
	printf(">i2cadd <add>             Set I2C address.(hex)\r\n");
	printf(">i2crd <chip> <add>       I2C data read.(hex)\r\n");
	printf(">i2cwr <chip> <add> <dat> I2C data write.(hex)\r\n");
	printf(">li2crd <chip> <add>       Lepton I2C data read.(hex)\r\n");
	printf(">li2cwr <chip> <add> <dat> Lepton I2C data write.(hex)\r\n");
	printf(">si2crd <chip> <add>      I2C data read(16bit).(hex)\r\n");
	printf(">eprd <add>               iEEPROM read.(hex)\r\n");
	printf(">epwr <add> <dat>         iEEPROM write.(hex)\r\n");
	printf(">dump <sel>               Data Dump.0=I2C,1=iEPROM\r\n");
	printf(">help                     Help command\r\n");
#endif
}


/*! @fn  I2C_write(uc chip, uc subadd, uc data)
*   @brief I2C�f�o�C�X�����݊֐�(�T�u�A�h���X�Ή�)
*   @param chip : I2C�f�o�C�X�̃A�h���X
*   @param subadd : I2C�f�o�C�X�̓������W�X�^�A�h���X
*   @param data : �������ރf�[�^(8bit)
*   @return �Ȃ�
*   @details �ڍׂȐ���
*/
void I2C_write(uc chip, uc subadd, uc data){
	I2CStart();
	res1 = I2COut(chip);		// write mode
	res2 = I2COut(subadd); 		// sub address
	res3 = I2COut(data);		// data
	I2CStop();
	__delay_us(20);				// �x��
	__delay_ms(5);
//	 __delay_us(50);			// �x��
//	 __delay_ms(10);
}
/*! @fn  I2C_write2(uc chip, ui subadd, ui data)
*   @brief I2C�f�o�C�X�����݊֐�(�T�u�A�h���X�Ή�)
*   @param chip : I2C�f�o�C�X�̃A�h���X
*   @param subadd : I2C�f�o�C�X�̓������W�X�^�A�h���X(16bit)
*   @param data : �������ރf�[�^(16bit)
*   @return �Ȃ�
*   @details �ڍׂȐ���
*/
void I2C_write2(uc chip, ui subadd, ui data){
	I2CStart();
	res1 = I2COut(chip);		// write mode
	res2 = I2COut((uc)( subadd >> 8 )); 		// sub address
	res2 = I2COut((uc)( subadd & 0x00FF )); 		// sub address
	res3 = I2COut((uc)(data>>8));		// data
	res3 = I2COut((uc)(data & 0x00FF));		// data
	I2CStop();
	__delay_us(20);				// �x��
	__delay_ms(5);
}
/*! @fn  I2C_read(uc chip, uc subadd)
*   @brief I2C�f�o�C�X�ǂݏo���֐�(�T�u�A�h���X�Ή�)
*   @param chip : I2C�f�o�C�X�̃A�h���X
*   @param subadd : I2C�f�o�C�X�̓������W�X�^�A�h���X
*   @return data : �ǂݏo�����l(8bit)
*   @details �ڍׂȐ���
*/
uc I2C_read(uc chip, uc subadd){
	uc data;

	I2CStart();
	res1 = I2COut(chip);		// write mode
	res2 = I2COut(subadd); 		// sub address
	nI2CStart();
	res3 = I2COut(chip | 0x01);	// read mode
	data = I2CRcv(1);			// get data with no ACK
	I2CStop();
	__delay_us(30); 			// �x��
	return(data);
}

/*! @fn  I2C_read2(uc chip, ui subadd)
*   @brief I2C�f�o�C�X�ǂݏo���֐�(�T�u�A�h���X�Ή�)
*   @param chip : I2C�f�o�C�X�̃A�h���X
*   @param subadd : I2C�f�o�C�X�̓������W�X�^�A�h���X(16bit)
*   @return data : �ǂݏo�����l(16bit)
*   @details �ڍׂȐ���
*/
ui I2C_read2(uc chip, ui subadd){
	uc ret1,ret2;
	ui data;

	I2CStart();
	res1 = I2COut(chip);		// write mode
	res2 = I2COut((uc)(subadd >> 8)); 		// sub address
	res2 = I2COut( subadd & 0x00FF); 		// sub address
	nI2CStart();
	res3 = I2COut(chip | 0x01);	// read mode
	ret1 = I2CRcv(0);			// get data with ACK
	ret2 = I2CRcv(1);			// get data with no ACK
	I2CStop();
	__delay_us(30); 			// �x��
	data = ret1<<8 | ret2;
	return(data);
}
/*! @fn  I2C_read3(uc chip, uc subadd)
*   @brief I2C�f�o�C�X�ǂݏo���֐�(�T�u�A�h���X�Ή�)
*   @param chip : I2C�f�o�C�X�̃A�h���X
*   @param subadd : I2C�f�o�C�X�̓������W�X�^�A�h���X(8bit)
*   @return data : �ǂݏo�����l(16bit)
*   @details �ڍׂȐ���
*/
ui I2C_read3(uc chip, uc subadd){
	uc ret1,ret2;
	ui data;
        ui count;

	I2CStart();
	res1 = I2COut(chip);		// write mode
	res2 = I2COut(subadd); 		// sub address
	nI2CStart();
	res3 = I2COut(chip | 0x01);	// read mode
        count=0;
        while(res3!=0){
            res3 = I2COut(chip | 0x01);	// read mode
            count++;
            if (count>1024)
                break;
            else
                printf("res3=%d, count=%04d\r\n" ,res3, count);
        }
	ret1 = I2CRcv(0);			// get data with ACK
	ret2 = I2CRcv(1);			// get data with no ACK
	I2CStop();
	__delay_us(30); 			// �x��
	data = ret1<<8 | ret2;
	return(data);
}
#if IEEPROM
/*! @fn  write_int_eeprom(uc addr, uc data)
*   @brief PIC������EEPROM�����݊֐�
*   @param addr : �A�h���X
*   @param data : �ǂݏo�����l(8bit)
*   @return �Ȃ�
*/
void write_int_eeprom(uc addr, uc data){
	while(WR)
		continue;
	eeprom_write( addr, data );
}
/*! @fn   read_int_eeprom(uc addr)
*   @brief PIC������EEPROM�ǂݏo���֐�
*   @param addr : �A�h���X
*   @return data : �ǂݏo�����l(8bit)
*/
uc read_int_eeprom(uc addr){
	uc data=0;

	while(WR)
		continue;
	data = eeprom_read( addr );
	return(data);
}
#endif
void main( void )
{
	struct command com;
	static int add_val;
	static int wdata,rdata;
	char temp_str[31];
	uc params;					// Parameter��
//	char get_str[31];
	int i=0;
	int h_val,l_val,t_val;
	int param;
	int read_val;
	
    pic_ini();		/* PIC�ˑ��̏����� */
	rs232_ini();	/* �V���A�������� */
	
	__delay_ms(1000);
	// �o��TEST
	help_list();
        timec=0;
	
	while(1){
		rs_gets(get_str);
		while(rcv_Flag){
			// command���
//                        printf("com.command[]=%s\r\n", com.command);
			params = parse(get_str, &com);
			// command����
			if(!strcmp( com.command, "i2cwr")){
					if(params != 4)
							printf("%s",pram_error);
					else{
							printf("I2C_write(%d%d%d)\r\n", res1, res2, res3);
							I2C_write( (uc)com.param1, com.param2, com.param3 );
					}
			}
			if(!strcmp( com.command, "li2cwr")){
					if(params != 4)
							printf("%s",pram_error);
					else{
//							printf("%s %02x %04x %04x\r\n", com.command, com.param1, com.param2 , com.param3);
							printf("I2C_write(%d%d%d)\r\n", res1, res2, res3);
							I2C_write2( (uc)com.param1, com.param2, com.param3 );
					}
			}
			else if(!strcmp(  com.command, "i2crd")){
					if(params != 3)
							printf("%s", pram_error);
					else{
						 read_val = I2C_read( com.param1, com.param2);
							sprintf(temp_str, "val(%d%d%d)=0x%02X\r\n", res1, res2, res3, read_val );
							printf("%s",temp_str);
					}
			}
			else if(!strcmp(  com.command, "li2crd")){
					if(params != 3)
							printf("%s", pram_error);
					else{
//						printf("%s %02x %04x \r\n", com.command, com.param1, com.param2 );
						read_val = I2C_read2( (uc)com.param1, com.param2);
						sprintf(temp_str, "val(%d%d%d%d)=0x%04X\r\n", res1, res2, res3, res4, read_val );
						printf("%s",temp_str);
					}
			}
			else if(!strcmp(  com.command, "si2crd")){
					if(params != 3)
							printf("%s", pram_error);
					else{
//						printf("%s %02x %04x \r\n", com.command, com.param1, com.param2 );
						read_val = I2C_read3( (uc)com.param1, (uc)com.param2);
						sprintf(temp_str, "val(%d%d%d%d)=0x%04X\r\n", res1, res2, res3, res4, read_val );
						printf("%s",temp_str);
					}
			}
			else if(!strcmp( com.command, "i2cadd")){
					if(params == 1)
						printf("I2C_Adress = 0x%02x\r\n", i2c_add);
					else if(params == 2){
						i2c_add = com.param1;
						printf("I2C_Adrress set 0x%02x\r\n", com.param1);
					}else{
						printf("%s",pram_error);
					}
			}
#if 0
			else if(!strcmp( com.command, "epwr")){
					if(params != 3)
							printf("%s",pram_error);
					else{
							write_int_eeprom(  com.param1,	com.param2 );
							printf("eep_write(0x%02X, 0x%02X)\r\n", com.param1,  com.param2 );
					}
			}
			else if(!strcmp(  com.command, "eprd")){
					if(params != 2)
							printf("%s",pram_error);
					else{
							read_val = read_int_eeprom( com.param1);
							sprintf(temp_str, "val(0x%02X)=0x%02X\r\n",  com.param1, read_val );
							printf("%s",temp_str);
					}
			}
#endif
                        else if(!strcmp( com.command, "dump")){
					if(params != 2)
							printf("%s",pram_error);
					else
							dump(com.param1);
			}
			else if(!strcmp( com.command, "loop")){
					disp_flag=1;
			}
                        else if(!strcmp( com.command, "help")){
					help_list();
                                        disp_flag=0;
                                        timec=0;
			}
			else{
					printf("command error\r\n");
			}
			rcv_Flag = 0;				// ��M�t���O�E�N���A
                        buffer_clear();
                }
                if(disp_flag)
                	printf("test %d\r\n", timec++);
	}
}
/*! @fn buffer_clear()
*   @brief ��M�o�b�t�@���N���A����֐�
*   @return �Ȃ�
*/
void buffer_clear(){
    uc i;

    for(i=0;i<MAX_STR;i++)
        get_str[i]=0;
}
/*! @fn arg_delay_ms(ui x)
*   @brief ms�P�ʂ�wait����֐�
*   @param x wait���鎞�ԁF�P�ʂ�ms
*   @return �Ȃ�
*   @details �ڍׂȐ���                         \n
* �҂����Ԃ��ϐ��ݒ�ł���arg_delay_ms()�֐����` \n
* �i__delay_ms()�͈����ɕϐ��ݒ肪�o���Ȃ��ׁj     \n
*/
void arg_delay_ms(unsigned int x) {
	while(x) {
		__delay_ms(1);
		x--;
	}
}
/* �V���A��������̑��M */
void rs_puts(const uc *buff){
	while(*buff)
		Send(*buff++);
	Send(0x0d);			// CR(Carrige Return)
	Send(0x0a);			// LF/NL(LineFeed/NewLine)���I���R�[�h
}
/* �V���A��������̎�M */
void rs_gets(char *buff)
{
#if RS232_HW
// HW���g���Ɗ��荞�ݏ����ŕ�������擾����̂ŁA�����ł͉������Ȃ��B
#else
	char *p, chr;
	p = buff;
	while(1){
		chr = Receive();
		if(chr == 0xFF){
			buff[0] = 'E';
			buff[1] = 'R';
			buff[2] = 'R';
			buff[3] = 0x00;
			break;
		}
		if(chr != 0x0d && chr != 0x0a)
			*p++ = chr;
		if(chr == 0x0d)
			*p = 0x00;
		if(chr == 0x0a){	// LF/NL���I���R�[�h
			*p = 0x00;
			break;
		}
	}
	rcv_Flag = 1;		// ��M������t���O���Z�b�g����B
#endif
}
/*! @fn     dump(uc sel)
*   @brief �e��Dump����֐�
*   @param sel : �Z���N�g�M��
*   @details �ڍׂȐ���                    \n
*   sel�M���̒l�ɂ���Ă��ȉ��̋�Ԃ�dump�\������B \n
*   0:I2C�f�o�C�X�̓������W�X�^��Dump����B  \n
*   1:PIC����EEPROM�̃f�[�^��Dump����B        \n
*   �\����16�i8bit��256��                     \n
*/
void dump(uc sel){
	uc i, j;
	uc vol, add, read_val;

	if(sel){
		vol=8; printf("EEPROM Register Dump\r\n");		// 0x00-0x7F�܂�Dump����B
	}else{
		vol=8; printf("I2C(add=0x%02x) Register Dump\r\n", i2c_add);		// 0x00-0xFF�܂�Dump����B
	}
	add=0;
	printf("   ");
	for(i=0;i<16;i++)
		printf("  %X",i);					// 0 1 2 3 4 �E�E�E�\��
	for (i=0; i<vol; i++){
		printf("\r\n%2X  ", add);
		for(j=0; j<16; j++){
			if(sel)
#if IEEPROM
				read_val = read_int_eeprom( add );
#else
                               ;
#endif
                        else
				read_val = I2C_read( i2c_add, add );
			printf("%02X ", read_val);
			add++;
		}
	}
	printf("\r\n");
}
