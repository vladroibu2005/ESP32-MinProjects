#include <Wire.h>
#define WIRE Wire

void setup() {
  WIRE.begin();

  Serial.begin(9600);
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Scanner");
}


void loop() {
  byte eroare, adresa;
  int nDispozitive;

  Serial.println("Scanare...");

  nDispozitive = 0;
  for(adresa = 1; adresa < 127; adresa++ )
  {
    WIRE.beginTransmission(adresa);
    eroare = WIRE.endTransmission();

    if (eroare == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (adresa<16)
        Serial.print("0");
      Serial.print(adresa,HEX);
      Serial.println("  !");

      nDispozitive++;
    }
    else if (eroare==4)
    {
      Serial.print("Unknown error at address 0x");
      if (adresa<16)
        Serial.print("0");
      Serial.println(adresa,HEX);
    }
  }
  if (nDispozitive == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // astept 5s pana la urmatoarea scanare
