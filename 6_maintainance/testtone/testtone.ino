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
}


//ループ
void loop() {
  // 500ms 毎に音階を変える
  if(step != micros() / 500 / 1000){
    step = micros() / 500 / 1000;
    height[0] = (height[0] + 1) % 84;
    height[1] = height[0] + 4;
    height[2] = height[0] + 7;
    for(int i = 0; i < 3; i += 1){
      interv[i] = 1000000 / (int)(toneheight[height[i] % 12] * pow(2, height[i] / 12 - 4)) / 2;
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
