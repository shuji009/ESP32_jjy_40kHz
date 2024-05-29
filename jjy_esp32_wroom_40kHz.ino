// jjy_esp32_wroom_40kHz.ino
// 	ver.0.10    (c) 2024/05/16 by Nash　Shuji009
//
// G5---> 220ohm ---> lead(40cm) ----> GND ///暫定アンテナ
///   -> 330ohm -> 30cm loop antenna -> GND   
// M(P1~P4) = -1 / 255  200ms
// 0 = 0                800ms
// 1= 1                 500ms
//
#include <Arduino.h>
#include <driver/ledc.h>
#include <WiFi.h>

// hardware
const int ledChannel = 0; // 使用するLEDチャンネル ---> JJT antena 
const int ledPin = 5;     // PWM出力するGPIOピン ANT G-5


char P0,P1,P2,P3,P4,P5;
const char M = P0 = P1 = P2 = P3 = P4 = P5 = -1;
char PA1,PA2, SU1,  LS1,LS2;

char  sg[60];
///char  min40, min20, min10, min8, min4, min2, min1;
///char  hr20, hr10, hr8, hr4, hr2, hr1;
///char  dy200, dy100, dy80, dy40, dy20, dy10, dy8, dy4, dy2, dy1;
///char  yr80, yr40, yr20, yr10, yr8, yr4, yr2, yr1;
///char  wd4, wd2, wd1;

const char* ssid     = "YOUR_SSID"; 	// ご自身のWi-FiネットワークのSSIDを入力してください
const char* password = "YOUR_PASSWORD";	// Wi-Fiネットワークのパスワードを入力してください

void setup() {
    
  ///debug
  /// for(int i=0;i<61;i++) sg[i] = 89;
  ///

    set_fix();  // mark etc set to sg[]

    ledcSetup(ledChannel, 40000, 8); // チャンネルのセットアップ (40kHz, 8bit分解能)
    ledcAttachPin(ledPin, ledChannel); // GPIOピンとチャンネルを関連付け

    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (WiFi.begin(ssid, password) != WL_DISCONNECTED) {
        ESP.restart();
    }

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }

    Serial.println("Connected to the WiFi network!");

    // NTPサーバーの設定
    configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");

}

void loop() {
    struct tm timeInfo;
    char s[20];
    char t[50];
    int se,mi,sh;

     // 現在時刻を取得
    getLocalTime(&timeInfo);

    // 時刻を人間が読める形式に変換
    sprintf(s, "%04d/%02d/%02d %02d:%02d:%02d",
            timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);

    // 時刻をシリアルモニタに出力
    Serial.println(s);

    sprintf(t, "Y:%02d  D:%03d WD:%01d",
            timeInfo.tm_year + 1900 - 2000, timeInfo.tm_yday + 1, timeInfo.tm_wday);

    Serial.println(t);


  /// 閏秒補正
    se = timeInfo.tm_sec;
    sh = 0;
    if(timeInfo.tm_sec == 60) { //  閏秒＋1
      sg[53]=LS1=1;
      sg[54]=LS2=0;
      se = timeInfo.tm_sec - 1;
      sh=1;
    }
    
    if(timeInfo.tm_sec == 61) {
      sg[53]=LS1=1;
      sg[54]=LS2=1;
      se = timeInfo.tm_sec - 2;
      sh=2;
    }

    set_min(timeInfo.tm_min);
    set_hour(timeInfo.tm_hour);
    set_day(timeInfo.tm_yday + 1);              //１月１日を１とした通算の日
    set_wday(timeInfo.tm_wday);
    set_year(timeInfo.tm_year + 1900 - 2000);   //西暦年の下２桁

    ///for(int i=0;i<60; i++) {
    ///     sprintf(t, "%02d :: %2d   ", i,sg[i]);
    ///     Serial.print(t);
    ///}
    ///Serial.println();

    for( int i = se; i < 60 + sh; i++){
      sprintf(t, "%02d ",sg[i]);
      Serial.print(t);
      switch(sg[i]){
        case -1:
          mark();
          break;
       case 255:
          mark();
          break;
         case 0 :
          zero();
          break;
        case 1 :
          one();
          break;
        default:    ///// BUG!!!
          zero();
          sprintf(t, "%02d  %02d",i, sg[i]);
          Serial.print("***BUG data ***  ");
          Serial.println(t);
          break;
      }
    
    }        
    delay(5);
}


///構造体 tm はtime.h
///struct tm {
///  int tm_sec;      /// 秒 [0-61] 最大2秒までのうるう秒を考慮
///  int tm_min;      /// 分 [0-59]
///  int tm_hour;     /// 時 [0-23]
///  int tm_mday;     /// 日 [1-31]
///  int tm_mon;      /// 月 [0-11] 0から始まることに注意
///  int tm_year;     /// 年 [1900からの経過年数]
///  int tm_wday;     /// 曜日 [0:日 1:月 ... 6:土]
///  int tm_yday;     /// 年内の通し日数 [0-365]
///  int tm_isdst;    /// 夏時間フラグ　[夏時間を採用しているときに正、採用していないときに 0、この情報が得られないときに負]
///};


void set_year(int n){
  
  int m = dec2BCD(n);

  sg[48] = m % 2; m = m >> 1;
  sg[47] = m % 2; m = m >> 1;
  sg[46] = m % 2; m = m >> 1;
  sg[45] = m % 2; m = m >> 1;

  sg[44] = m % 2; m = m >> 1;
  sg[43] = m % 2; m = m >> 1;
  sg[42] = m % 2; m = m >> 1;
  sg[41] = m % 2;
}

void set_day(int n){
  
  int m = dec2BCD(n);

  sg[33] = m % 2; m = m >> 1;
  sg[32] = m % 2; m = m >> 1;
  sg[31] = m % 2; m = m >> 1;
  sg[30] = m % 2; m = m >> 1;

  sg[28] = m % 2; m = m >> 1;
  sg[27] = m % 2; m = m >> 1;
  sg[26] = m % 2; m = m >> 1;
  sg[25] = m % 2; m = m >> 1;

  sg[23] = m % 2; m = m >> 1;
  sg[22] = m % 2;
}

void set_wday(int m){

  sg[52] = m % 2; m = m >> 1;
  sg[51] = m % 2; m = m >> 1;
  sg[50] = m % 2;
}

void set_hour(int n){
  
  int m = dec2BCD(n);

  sg[18] = m % 2; m = m >> 1;
  sg[17] = m % 2; m = m >> 1;
  sg[16] = m % 2; m = m >> 1;
  sg[15] = m % 2; m = m >> 1;
  
  sg[13] = m % 2; m = m >> 1;
  sg[12] = m % 2;
  
  char PA1 = sg[18] ^ sg[17] ^ sg[16] ^ sg[15] ^ sg[13] ^ sg[12]; //PA1 = (20h+10h+8h+4h+2h+1h) mod 2
  sg[36] = PA1;
}

void set_min(int n){
  
  int m = dec2BCD(n);


  sg[8] = m % 2; m = m >> 1;
  sg[7] = m % 2; m = m >> 1;
  sg[6] = m % 2; m = m >> 1;
  sg[5] = m % 2; m = m >> 1;

  sg[3] = m % 2; m = m >> 1;
  sg[2] = m % 2; m = m >> 1;
  sg[1] = m % 2;

  char PA2 = sg[8] ^ sg[7] ^ sg[6] ^ sg[5] ^ sg[3] ^ sg[2] ^ sg[1]; //PA2 = (40m+20m+10m+8m+4m+2m+1m) mod 2
  sg[37] = PA2;
}


void set_fix(){
  sg[0] = sg[9] = sg[19] = sg[29] = sg[39] = sg[49] = sg[59] = M;
  sg[4] = sg[10] = sg[11] = sg[14] = sg[20] = sg[21] = sg[24] = sg[34] = sg[35] = sg[55] = sg[56] = sg[57] = sg[58] = 0;
  sg[38] = sg[40] = 0;
  sg[53] = sg[54] = 0;
  
}

int dec2BCD(int decimal) {
    int bcd = 0;
    int multiplier = 1;

    while (decimal > 0) {
        int digit = decimal % 10;
        bcd += digit * multiplier;
        multiplier *= 16; // 10進数の各桁を4ビットのBCDに変換するために16倍する
        decimal /= 10;
    }

    return bcd;
    
    ///printf("BCD: %X\n", bcd);
}


void mark() {   // 0.2sec
  ledcWrite(ledChannel, 127);
  delay(200);
  ledcWrite(ledChannel, 0);
  delay(800);
}

void zero() {  // 0.8sec
  ledcWrite(ledChannel, 127);
  delay(800);
  ledcWrite(ledChannel, 0);
  delay(200);
}

void one() {    // 0.5sec
  ledcWrite(ledChannel, 127);
  delay(500);
  ledcWrite(ledChannel, 0);
  delay(500);
}
