/*! @file rs232.c
*   @brief PICのシリアル通信用ファイル
*   @author K.Matsuda
*   @date 2017.01.21
*   @details 詳細な説明      \n
*   RS232C送受信ライブラリ                               \n
*   PIC内蔵UARTの利用とTimer2利用は、deficeで切り替え。   \n
*   define RS232_HW   1       // SW=0, HW=1             \n
*   SW=Timer2を利用                                     \n
*   HW=内蔵UART利用                                     \n
*
*
*    RS232C送受信ライブラリ (rs232c.c)　Timer2 使用
*
*******************************************************************************/
#include <xc.h>
#include "rs232.h"

static volatile char dat;
static volatile char bCnt;
static volatile char bPos;

extern unsigned char rcv_Flag;
extern unsigned char get_str[20];
extern unsigned char rcv_count;

/* RS232C 初期化 */
/*! @fn   rs232_ini(void)
*   @brief RS232初期化関数
*   @return なし
*   @details 詳細な説明    \n
*   PICのレジスタを通信ボーレートに合わせて設定する関数
*/
void rs232_ini(void)
{
#if RS232_HW
     // USARTの初期設定
     TXSTA = 0x20;         // TXEN=1, SYNC=0, BRGH=0,
     RCSTA = 0x90;
     SPBRG = BAUD_HW;     // ボーレート設定
     RCIF = 0;              // 受信フラグ初期化
     RCIE = 1;              // 受信割り込み有効
#else
     T2CON  = 0x01;                  // 1/4プリ,1/1ポスト
     TMR2IF = 0;                     // 割り込みフラグクリア
     TXPIN = 1;
#endif
}

/* 1バイト送信サブ関数 */
/* I2Cデバイス書込み関数 */
/*! @fn  Send(unsigned char code)
*   @brief 1バイト送信サブ関数
*   @param code : 文字コード
*   @return なし
*   @details 詳細な説明             \n
*   RS232_HW==1ならば                                                      \n
*     PIC内蔵のUART通信機能を使うので、レジスタ設定に文字コードをセットする。  \n
*   RS232_HW==0ならば                                                      \n
*     UART通信機能をタイマーで実現するので、1bit毎にピンを制御する。          \n
*/
void Send(unsigned char code)
{
#if RS232_HW
    // TRMT:Transmit Shift Register Status bit
    // 1 = TSR empty
    // 0 = TSR full
    while(!TRMT)
        continue;
    TXREG = code;
#else
    dat = code;                 // 送信データセット
    bCnt = 0;                   // ビットカウンタリセット
    bPos = 0x01;                // ビット位置リセット
    PR2 = BAUD_SW;                 // ボーレート設定
    TXPIN = 0;                  // スタートビット出力
    TMR2 = 0;                   // タイマ2リセット
    T2CON = 0x05;               // 1/4プリ,1/1ポスト
    while(bCnt < 10){           // 送信完了待ち
        while(!TMR2IF)          // TMR2フラグ検出待ち
            ;
        TMR2IF = 0;
        bCnt++;
        if(bCnt > 0 && bCnt < 9){           // 1?8bit
            TXPIN = (dat & bPos)? 1: 0;     // データビット出力
            bPos <<= 1;                     // ビットシフト
        }
        if(bCnt == 9)                       // データ終了
            TXPIN = 1;                      // ストップビット出力
        if(bCnt == 10){                     // ストップビット終了
            T2CON = 0x01;                   // タイマ２停止
        }
    }
#endif

}

#if RS232_HW
/* 受信に割り込みを使用する */
void interrupt InterReceiver( void )
{
    unsigned char c;

    if(RCIF==1){   // UART割り込みチェック
        c = RCREG;  // レジスタから受信データ取り出し
        if(c!=0x0a && c!=0x0d){
           get_str[rcv_count++] = c;
        }
        if(c==0x0d){      //0x0d='\r'（Carriage return:改行）
            if(rcv_count)
                rcv_Flag = 1;
            get_str[rcv_count++] = 0x00;
            rcv_count = 0;
        }
        if(c==0x0a){    //  0x0a='\n'（Line feed:復帰）
            rcv_count = 0;
        }
        if(rcv_count >=sizeof(get_str))
             rcv_count = 0;
        RCIF = 0;             // 割り込み受信フラグをリセット
    }
}
#else
/* 1バイト受信サブ関数 */
unsigned char Receive(void)
{
    while(RXPIN)                // スタートビット待ち
        ;
    bCnt = 0;                   // ビットカウンタ初期化
    dat = 0;                    // 受信データクリア
    bPos = 0x01;                // ビット位置yuipppouyyyy初期化
    TMR2 = 0;                   // タイマ２初期化
    PR2 = DTIME;                // 1/2ビット幅に設定
    TMR2IF = 0;
    T2CON = 0x05;               // 1/4プリ,1/1ポスト、開始
    while(bCnt < 10){           // 受信完了待ち
        while(!TMR2IF)
            ;
        TMR2IF = 0;
        switch(bCnt){
            case 0:                         // スタートビットの処理
                if(RXPIN) bCnt = 0xFF;
                else{
                    PR2 = BAUD_SW;
                    bCnt++;
                }
                break;
            case 9:                         // ストップビットの処理
                if(RXPIN) bCnt++;
                else bCnt = 0xFF;
                T2CON = 0x01;               // TMR2停止
                break;
            default:                        // 1?8bit目のデータ部の処理
                if(RXPIN) dat |= bPos;
                bPos <<= 1;
                bCnt++;
        }
        // GP1 = ~GP1;      // デバッグ
    }
    if(bCnt == 10)
        return(dat);            // 正常データを返す
    else
        return(0xFF);           // エラーフラグを返す
}
#endif

