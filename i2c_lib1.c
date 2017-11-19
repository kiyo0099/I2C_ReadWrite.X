/******************************************
*  I2C通信ライブラリ
*   I2CStart()
*   I2CStop()
*   I2COut()
*   I2CRcv()
******************************************/
/*! @file i2c_lib1.c
*   @brief ソフトウェアI2C制御関数(Master)
*   @author K.Matsuda
*   @date 2017.01.21
*   @details 詳細な説明               \n
*   I2C通信ライブラリ                                                  \n
*   PIC内蔵I2Cモジュールは利用していない、ソフトウェアでbit制御。         \n
*   delay_us()でタイミングを取っているので、速度は200kbps程度が限界。     \n
*/
#include <xc.h>
#include "i2c_lib1.h"

/*****************************
* スタート条件出力
*****************************/
/*! @fn   nI2CStart(void)
*   @brief I2Cのnスタート状態を生成する関数
*   @return なし
*/
void nI2CStart(void){
        __delay_us(10);
	SCL = 0;						// SCL Low
        __delay_us(10);
	TRISSDA = 0;					// 出力モードに戻す　SDA =Low
	SDA = 1;					// SDAをHigh
        __delay_us(10);
	SCL = 1;						// SCL Low
        __delay_us(10);
 	/* SCL,SDAは常時Highとする　*/
	SDA = 0;						// 先にSDA Low
	TRISSDA = 0;					// SDA出力
        __delay_us(10);
	SCL = 0;						// SCL Low

}
/*****************************
* スタート条件出力
*****************************/
/*! @fn   I2CStart(void)
*   @brief I2Cのスタート状態を生成する関数
*   @return なし
*/
void I2CStart(void){
	/* SCL,SDAは常時Highとする　*/
	SDA = 0;						// 先にSDA Low
	TRISSDA = 0;					// SDA出力
        __delay_us(10);
	SCL = 0;						// SCL Low

}
/*****************************
* ストップ条件出力 
*****************************/
/*! @fn   I2CStop(void)
*   @brief I2Cのストップ状態を生成する関数
*   @return なし
*/
void I2CStop(void){
	SCL = 0;						// SCL Low
	__delay_us(10);
	SDA = 0;						// SDA Low
	TRISSDA = 0;					// 出力モードに戻す　SDA =Low
	SCL = 1;						// 先にSCLをHigh
	__delay_us(10);
	SDA = 1;						// 後からSDAをHigh
	__delay_us(10);				// 間隔用遅延	
}
/*********************************
* I2Cで1バイト出力
* スレーブからのACKを戻り値とする 
**********************************/
/*! @fn   I2COut(unsigned char data)
*   @brief I2Cの1バイト送信関数
*   @param data 出力データ
*   @return ACK
*   @details 詳細な説明               \n
*   Masterデバイスとして、I2Cで1バイト出力する   \n
*   スレーブからのACKを戻り値とする             \n
*/
unsigned char I2COut(unsigned char data){
	int i;
	unsigned char BitPos, ACK;

	/* Data Out */
	TRISSDA = 0;					// SDA出力モード
	BitPos = 0x80;				// ビット位置初期値
	for(i=0; i<8; i++){			// 8ビット繰り返し
		SCL = 0;					// SCL Low
		if((data & BitPos) != 0)	// ビット出力
			SDA = 1;				// SDA High
		else
			SDA = 0;				// SDA Low
		BitPos = BitPos >> 1;		// ビット位置移動
		SCL = 1;					// SCL Highに戻す
	}
	/* Ack チェック */
	SCL = 0;						// クロック１Low
	TRISSDA = 1;					// 入力モードにしてACK入力
	__delay_us(2);
	SCL = 1;						// クロックHighに戻す
	__delay_us(2);
	ACK = SDA;					// ACKチェック
        return(ACK);					// ACKを戻り値とする
}
/********************************
* I2Cで1バイト入力
* パラメータでACK/NAKを返送
********************************/
/*! @fn   I2CRcv(unsigned char Ack)
*   @brief I2Cの1バイト受信関数
*   @param Ack ACK/NACK送信指定
*   @return data
*   @details 詳細な説明               \n
*   Masterデバイスとして、I2Cで1バイト入力を制御する。   \n
*   データ受信を完了すると、ACK/NACKをスレーブに送る      \n
*/
unsigned char I2CRcv(unsigned char Ack){
	int	i;
	unsigned char BitPos, data;

	data = 0;
	BitPos =0x80;					// ビット位置リセット
	for(i=0; i<8; i++){			// 8ビット繰り返し
		SCL = 0;					// SCL Low
		TRISSDA = 1;				// 入力モード
		__delay_us(3);			// 幅確保
		SCL = 1;					// SCL High
		if(SDA)					// ビット入力
			data |= BitPos;
		BitPos = BitPos >> 1;		// ビットシフト
	}
	/* ACK/NAK出力 */
	SCL = 0;						// SCL Low
	SDA = Ack;					// ACK ot NAK 出力
	TRISSDA = 0;					// SDA出力モード
	__delay_us(2);				// パルス幅確保
	SCL = 1;						// SCL High
	return(data);				// 受信データを戻す
}
