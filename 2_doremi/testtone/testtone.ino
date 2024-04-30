#define TONE_OUT_PIN 2
void setup() {
  // put your setup code here, to run once:
  pinMode(TONE_OUT_PIN, OUTPUT);
}

float toneheight[12] = {523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77};

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 12; i += 1){
  tone(TONE_OUT_PIN, (int)toneheight[i] );
  delay(500);
  }
}
