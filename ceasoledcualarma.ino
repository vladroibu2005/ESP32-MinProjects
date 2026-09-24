#include <WiFi.h>
#include "time.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


const char* ssid   = "DIGI_86f4d7";
const char* parola   = "21d15123";

const char* serverNTP = "pool.ntp.org";
const long  decalajGMT_sec = 7200;
const int   decalajOraVara_sec = 3600;


// ALARMA
int alarma_ora = 13;
int alarma_minut = 10;
int durata_alarma = 10; // secunde


// BUZZER
#define BUZZER_PIN 25


// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {

  Serial.begin(115200);

  // Pornire buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);


  // Pornire OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED failed!");

    while (true) {
      delay(1000);
    }
  }


  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("Connecting WiFi...");
  display.display();


  // Conectare la Wi-Fi
  Serial.print("Se conecteaza la ");
  Serial.println(ssid);

  WiFi.begin(ssid, parola);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectat.");


  // Initializare ora prin NTP
  configTime(decalajGMT_sec, decalajOraVara_sec, serverNTP);

  afisareOraLocala();


  // Oprim Wi-Fi dupa sincronizarea orei
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}


void loop() {

  delay(1000);

  afisareOraLocala();
}


void afisareOraLocala() {

  struct tm infoOra;

  if (!getLocalTime(&infoOra)) {

    Serial.println("Nu s-a putut obtine ora");
    return;
  }


  // -------------------------
  // AFISARE OLED
  // -------------------------

  display.clearDisplay();


  // Ora
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(16, 16);

  display.println(&infoOra, "%H:%M:%S");


  // Data
  display.setTextSize(1);
  display.setCursor(25, 42);

  display.println(&infoOra, "%B:%d:%Y");


  display.display();


  // -------------------------
  // SERIAL MONITOR
  // -------------------------

  Serial.println(&infoOra, "%A, %B %d %Y %H:%M:%S");


  // -------------------------
  // ALARMA
  // -------------------------

  if (infoOra.tm_hour == alarma_ora &&
      infoOra.tm_min == alarma_minut &&
      infoOra.tm_sec == 0) {

    Serial.println("ALARMA!");

    digitalWrite(BUZZER_PIN, HIGH);

    delay(durata_alarma * 1000);

    digitalWrite(BUZZER_PIN, LOW);
  }
}

