/******************************************
*  I2C通信ライブラリ用ヘッダファイル
*   I2CStart()
*   I2CStop()
*   I2COut()
*   I2CRcv()
******************************************/

/** クロック周波数設定　遅延関数用 **/
#define _XTAL_FREQ  8000000	// クロック周波数設定

/** I/Oピンの設定 **/
#define	SDA		RB1
#define	SCL		RB4
#define TRISSDA 	TRISB1

/** 関数プロトタイピング **/
void I2CStart(void);
void nI2CStart(void);
void I2CStop(void);
unsigned char I2COut(unsigned char data);
unsigned char I2CRcv(unsigned char Ack);