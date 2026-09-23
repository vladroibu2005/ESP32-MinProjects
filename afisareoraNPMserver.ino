#include <WiFi.h>
#include "time.h"

const char* ssid     = "DIGI_86f4d7";
const char* parola   = "21d15123";

const char* serverNTP = "pool.ntp.org";
const long  decalajGMT_sec = 7200;
const int   decalajOraVara_sec = 3600;

void setup(){
  Serial.begin(115200);

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
  
  // Inițializare și obținere oră
  configTime(decalajGMT_sec, decalajOraVara_sec, serverNTP);
  afisareOraLocala();

  // Deconectare Wi-Fi deoarece nu mai este necesar
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop(){
  delay(1000);
  afisareOraLocala();
}

void afisareOraLocala(){
  struct tm infoOra;
  if(!getLocalTime(&infoOra)){
    Serial.println("Nu s-a putut obtine ora");
    return;
  }
  
  Serial.println(&infoOra, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Ziua saptamanii: ");
  Serial.println(&infoOra, "%A");
  Serial.print("Luna: ");
  Serial.println(&infoOra, "%B");
  Serial.print("Ziua din luna: ");
  Serial.println(&infoOra, "%d");
  Serial.print("Anul: ");
  Serial.println(&infoOra, "%Y");
  Serial.print("Ora: ");
  Serial.println(&infoOra, "%H");
  Serial.print("Ora (format 12 ore): ");
  Serial.println(&infoOra, "%I");
  Serial.print("Minutul: ");
  Serial.println(&infoOra, "%M");
  Serial.print("Secunda: ");
  Serial.println(&infoOra, "%S");

  Serial.println("Variabile de timp:");
  char variabilaOra[3];
  strftime(variabilaOra, 3, "%H", &infoOra);
  Serial.println(variabilaOra);
  
  char variabilaZi[10];
  strftime(variabilaZi, 10, "%A", &infoOra);
  Serial.println(variabilaZi);
  Serial.println();
}