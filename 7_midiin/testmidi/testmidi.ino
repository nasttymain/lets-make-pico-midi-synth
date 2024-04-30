// MIDI入力
char midi_indata;
char midi_stat = 0;
char midi_stat_type = 0;
char midi_stat_ch = 0;
char midi_data_index = 0;
char midi_data_queue[256];

void setup() {
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

void loop() {

  if(Serial.available() > 0){

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
        if(midi_stat_ch == 0){
          if(midi_data_index % 2 == 1){
            // データを2バイト受け取った→反映しにいく
            // 今回は、ch0にノートオフが来たらLEDを消灯する
            digitalWrite(LED_BUILTIN, LOW);
          }
        }
      }else if(midi_stat_type == 9){
        // ノートオンのとき
        if(midi_stat_ch == 0){
          if(midi_data_index % 2 == 1){
            // データを2バイト受け取った→反映しにいく
            // 今回は、ch0にノートオンが来たらLEDを点灯する。ただし、ベロシティが0のノートオンは = ノートオフなので、消灯する
            if(midi_data_queue[midi_data_index] != 0){
              // ノートオン
              digitalWrite(LED_BUILTIN, HIGH);
            }else{
              // 実質ノートオフ
              digitalWrite(LED_BUILTIN, LOW);
            }
          }
        }
      }
      midi_data_index += 1;
    }

  }

}
