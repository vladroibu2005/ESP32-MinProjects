#include <WiFi.h>
#include "time.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// =========================
// Wi-Fi + NTP
// =========================

const char* ssid = "DIGI_86f4d7";
const char* parola = "21d15123";

const char* serverNTP = "pool.ntp.org";

const long decalajGMT_sec = 7200;
const int decalajOraVara_sec = 3600;


// =========================
// OLED
// =========================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);


// =========================
// Buton
// =========================

#define BUTON_PIN 27

bool oledPornit = false;
bool butonAnterior = HIGH;


// =========================
// Timer OLED
// =========================

unsigned long momentPornireOLED = 0;

const unsigned long durataAfisare = 15000;


// =========================
// Timer actualizare
// =========================

unsigned long ultimaAfisare = 0;

const unsigned long intervalAfisare = 1000;


// =========================
// SETUP
// =========================

void setup() {

  Serial.begin(115200);

  pinMode(BUTON_PIN, INPUT_PULLUP);


  // =========================
  // OLED
  // =========================

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED failed!");

    while (true) {
      delay(1000);
    }
  }


  // =========================
  // Wi-Fi
  // =========================

  Serial.println("Connecting WiFi...");

  WiFi.begin(ssid, parola);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("Wi-Fi conectat.");


  // =========================
  // NTP
  // =========================

  configTime(
    decalajGMT_sec,
    decalajOraVara_sec,
    serverNTP
  );

  Serial.println("Astept sincronizarea NTP...");


  // Asteptam pana cand avem o ora valida
  struct tm infoOra;

  while (!getLocalTime(&infoOra)) {

    Serial.println("Inca astept ora...");

    delay(500);
  }


  Serial.println("Ora sincronizata!");


  // Afisam ora obtinuta pentru verificare
  Serial.println(
    &infoOra,
    "%A, %d.%m.%Y %H:%M:%S"
  );


  // =========================
  // Acum putem opri Wi-Fi
  // =========================

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  Serial.println("Wi-Fi oprit.");
  Serial.println("Sistem gata.");


  // OLED OFF la pornire
  display.clearDisplay();
  display.display();
}


// =========================
// LOOP
// =========================

void loop() {

  // =========================
  // BUTON
  // =========================

  bool butonAcum = digitalRead(BUTON_PIN);


  if (butonAnterior == HIGH && butonAcum == LOW) {

    oledPornit = true;

    momentPornireOLED = millis();

    Serial.println("OLED ON");


    // Afisam imediat ora
    afisareOraLocala();


    // Debounce
    delay(100);
  }


  butonAnterior = butonAcum;


  // =========================
  // ACTUALIZARE OLED
  // =========================

  if (oledPornit) {

    if (millis() - ultimaAfisare >= intervalAfisare) {

      ultimaAfisare = millis();

      afisareOraLocala();
    }
  }


  // =========================
  // OPRIRE DUPA 15 SECUNDE
  // =========================

  if (oledPornit) {

    if (millis() - momentPornireOLED >= durataAfisare) {

      oledPornit = false;

      display.clearDisplay();
      display.display();

      Serial.println("OLED OFF");
    }
  }
}


// =========================
// AFISARE ORA
// =========================

void afisareOraLocala() {

  struct tm infoOra;


  if (!getLocalTime(&infoOra)) {

    Serial.println("Nu s-a putut obtine ora");

    return;
  }


  display.clearDisplay();


  // =========================
  // Ora
  // =========================

  display.setTextSize(2);

  display.setTextColor(WHITE);

  display.setCursor(16, 16);

  display.println(
    &infoOra,
    "%H:%M:%S"
  );


  // =========================
  // Data
  // =========================

  display.setTextSize(1);

  display.setCursor(25, 42);

  display.println(
    &infoOra,
    "%d.%m.%Y"
  );


  display.display();


  // Serial Monitor
  Serial.println(
    &infoOra,
    "%A, %d.%m.%Y %H:%M:%S"
  );
}