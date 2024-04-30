#define TONE_OUT_PIN 2

//チャンネル数
#define CH_COUNT 8

//音階の周波数(単位: Hz)
float toneheight[12] = {523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77};

// (現在時刻 / 500ms)の値。これが変わったときに音高を変える
int step = 0;
// 現在の音階(ドレミ)
int height[CH_COUNT];

//周期(単位: us) = 1000000 / 周波数[Hz]
unsigned long interv[CH_COUNT];
//最後に0↔1を切り替えたときの時刻[us]。ここから現在時刻までの差分を取って処理する
unsigned long lasttime[CH_COUNT];
//各チャンネルの出力データ。0～256
int chout[CH_COUNT];
//チャンネルゲイン。0～256
int chgain[CH_COUNT];
// 出力デジタルデータ。0～256
int mixout = 0;

// MIDI入力
char midi_indata;
char midi_stat = 0;
char midi_stat_type = 0;
char midi_stat_ch = 0;
char midi_data_index = 0;
char midi_data_queue[256];


//セットアップ
void setup() {
  pinMode(TONE_OUT_PIN, OUTPUT);
  analogWriteFreq(1000000);
  analogWriteResolution(8);
  for(int i = 0; i < CH_COUNT; i += 1){
    height[i] = 128;
    interv[i] = 0;
    chout[i] = 0;
    chgain[i] = 64;
  }

  // USBシリアルポートを開く
  Serial.begin(115200);

  // シリアルポートが開くまで待つ(トリッキーなfor)
  delay(100);
  pinMode(LED_BUILTIN, OUTPUT);
  for(int i = 0; !Serial; i += 1){
    digitalWrite(LED_BUILTIN, i % 2);
    delay(100);
  }
  digitalWrite(LED_BUILTIN, LOW);
}


//ループ
void loop() {

  while(Serial.available() > 0){

    // シリアル入力がある
    midi_indata = Serial.read();
    if(midi_indata >= 0x80 && midi_indata <= 0xF7){
      // ステータスバイト
      midi_stat = midi_indata;
      midi_stat_type = midi_stat / 16;
      midi_stat_ch = midi_stat % 16;
      midi_data_index = 0;
    }else if(midi_indata <= 0x7F){
      // データバイト
      midi_data_queue[midi_data_index] = midi_indata;
      if(midi_stat == 0x00){
        // ステータス未設定(異常)
      }else if(midi_stat_type ==8){
        // ノートオフのとき
        if(midi_stat_ch < CH_COUNT){
          if(midi_data_index % 2 == 1){
            // データを2バイト受け取った→反映しにいく
            if(midi_data_queue[midi_data_index - 1] == height[midi_stat_ch]){
              // ノートオフ指定が、現在鳴らしてるトーンに対してである(この条件が無いと、MIDI引っ張ってきたソフトによってはバグる)
              height[midi_stat_ch] = 128;
              interv[midi_stat_ch] = 0;
              lasttime[midi_stat_ch] = micros();
            }
          }
        }
      }else if(midi_stat_type == 9){
        // ノートオンのとき
        if(midi_stat_ch < CH_COUNT){
          if(midi_data_index % 2 == 1){
            // データを2バイト受け取った→反映しにいく
            if(midi_data_queue[midi_data_index] != 0){
              // ノートオン
              height[midi_stat_ch] = midi_data_queue[midi_data_index - 1];
              interv[midi_stat_ch] = 1000000 / (int)(toneheight[height[midi_stat_ch] % 12] * pow(2, height[midi_stat_ch] / 12 - 5)) / 2;
              lasttime[midi_stat_ch] = micros();
            }else{
              // 実質ノートオフ
              if(midi_data_queue[midi_data_index - 1] == height[midi_stat_ch]){
                // ノートオフ指定が、現在鳴らしてるトーンに対してである(この条件が無いと、MIDI引っ張ってきたソフトによってはバグる)
                height[midi_stat_ch] = 128;
                interv[midi_stat_ch] = 0;
              }
            }
          }
        }
      }else if(midi_stat_type == 11){
        // コントロールチェンジのとき
        if(midi_stat_ch < CH_COUNT){
          if(midi_data_index % 2 == 1){
            // データを2バイト受け取った→反映しにいく
            if(midi_data_queue[midi_data_index - 1] == 7){
              chgain[midi_stat_ch] = midi_data_queue[midi_data_index] * 2;
            }
          }
        }
      }
      midi_data_index += 1;
    }

  }


  //各オシレータの処理(毎ループ実行)
  for(int i = 0; i < CH_COUNT; i += 1){
    if (interv[i] == 0){
      // 発音停止
      chout[i] = 0;
    }else{
      if(lasttime[i] + interv[i] < micros()){
        // 0 <-> 1 の切り替わり
        lasttime[i] += interv[i];
        if(chout[i] == 0){
          chout[i] = 255;
        }else{
          chout[i] = -chout[i];
        }
      }
    }
  }

  //ミキシング
  mixout = 0;
  for(int i = 0; i < CH_COUNT; i += 1){
    mixout += (chout[i] * chgain[i] / 2) / 256;
  }
  analogWrite(TONE_OUT_PIN, mixout + 128);
}
