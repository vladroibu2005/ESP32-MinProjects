#define LED_PIN 18
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
   if (Serial.available()) {
    char data = Serial.read();

    if (data == '1') {
        digitalWrite(LED_PIN, HIGH);
    }
    if (data == '0') {
        digitalWrite(LED_PIN, LOW);
}
   }
}
