#define TONE_OUT_PIN 2
void setup() {
  // put your setup code here, to run once:
  pinMode(TONE_OUT_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  tone(TONE_OUT_PIN, 900);
  delay(1000);
  noTone(TONE_OUT_PIN);
  delay(1000);
}
