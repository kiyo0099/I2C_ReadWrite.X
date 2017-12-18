/*****************************************
*  RS232C送受信ライブラリ　ヘッダファイル
*   Send()
*   Receive()
*****************************************/
#include <xc.h>

#define RS232_HW   0       // SW=0. HW=1
/***** ハード設定　****/
#define BAUD_SW        51      // 9600bps Fosc = 8MHz
#define BAUD_HW        12      // 9600bps Fosc = 8MHz
#define DTIME       10      // 検出遅延時間
#define TXPIN       RA3
#define RXPIN       RA4

/**** 関数プロトタイプ宣言 ****/
void Send(char code);
char Receive(void);
void rs232_ini(void);
