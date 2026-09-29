const int ledRed = 27;
const int ledGreen = 32;
const int ledBlue = 33;

void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

}

void loop() {
  for (int r = 0; r <= 1; r++) {
      for (int g = 0; g <= 1; g++) {
         for (int b = 0; b <= 1; b++) {

           digitalWrite(ledRed, r);
           digitalWrite(ledGreen, g);
           digitalWrite(ledBlue, b);

           delay(500);
    }
  }
}

}
