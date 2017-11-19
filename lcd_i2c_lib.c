/***********************************************
*  液晶表示器ライブラリ
*  I2Cインターフェース
*    lcd_init()    ----- 初期化
*    lcd_cmd(cmd)  ----- コマンド出力
*    lcd_data(data) ----- １文字表示出力
*    lcd_str(ptr)  ----- 文字列表示出力
*    lcd_clear()   ----- 全消去
**********************************************/
#include <xc.h>
#include "lcd_i2c_lib.h"
#include "i2c_lib1.h"

/*********************************
* 液晶へ1文字表示データ出力
*********************************/
void lcd_data(unsigned char data)
{
	I2CStart();					// スタート
	I2COut(ADD_IC);					// アドレス
	I2COut(0x40);					// 表示データ指定
	I2COut(data);					// 表示データ出力
	I2CStop();					// ストップ
	__delay_us(30);				// 遅延
}

/*******************************
* 液晶へ１コマンド出力 
*******************************/
void lcd_cmd(unsigned char cmd)
{
	I2CStart();					// スタート
	I2COut(ADD_IC);					// アドレス
	I2COut(0x00);					// コマンド指定
	I2COut(cmd);					// コマンド出力
	I2CStop();					// ストップ
	/* ClearかHomeか */
	if((cmd == 0x01)||(cmd == 0x02))
		__delay_ms(2);			// 2msec待ち
	else
		__delay_us(30);			// 30μsec待ち	
}
/*******************************
*  初期化関数 
*******************************/
void lcd_init(void)
{
	delay_100ms(1);
	lcd_cmd(0x38);				// 8bit 2line Normal mode
	lcd_cmd(0x39);				// 8bit 2line Extend mode	
	lcd_cmd(0x14);				// OSC 183Hz BIAS 1/5
	/* コントラスト設定 */
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
* 全消去関数
******************************/
void lcd_clear(void)
{
	lcd_cmd(0x01);				//初期化コマンド出力
}
/*****************************
* 文字列表示関数
*****************************/
void lcd_str(const unsigned char* ptr)
{
	while(*ptr != 0)				//文字取り出し 
		lcd_data(*ptr++);			//文字表示
}
#if 1
/*****************************
*  アイコン表示制御関数
*****************************/
void lcd_icon(unsigned char num, unsigned char onoff)
{
	lcd_cmd(0x39);				// Extend mode
	lcd_cmd(0x40 | ICON[num][0]);	// アイコンアドレス指定
	if(onoff)
		lcd_data(ICON[num][1]);	// アイコンオンデータ
	else
		lcd_data(0x00);			// アイコンオフデータ
	lcd_cmd(0x38);				// Normal Mode
}
#endif

/******************************
*  100msec遅延関数
******************************/
void delay_100ms(unsigned int time)
{
	time *= 4;					// 4倍
	while(time){
		__delay_ms(25);			// 25msec
		time--;					// 100msec x time
	}
}
