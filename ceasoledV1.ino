#include <WiFi.h>
#include "time.h"

#include <Wire.h>             // Permite comunicarea I2C (necesară pentru ecranul OLED).
#include <Adafruit_GFX.h>     // Biblioteca de bază pentru grafică (desenare linii, text, forme).
#include <Adafruit_SSD1306.h> // Biblioteca specifică pentru controlul ecranelor OLED bazate pe driverul SSD1306.


const char* ssid     = "DIGI_86f4d7";
const char* parola   = "21d15123";

const char* serverNTP = "pool.ntp.org";
const long  decalajGMT_sec = 7200;
const int   decalajOraVara_sec = 3600;

int alarma_ora=12;
int alarma_minut=30;
int durata_alarma=10; //secunde

// OLED
// -------------------------
#define SCREEN_WIDTH 128      // Lățimea ecranului OLED în pixeli.
#define SCREEN_HEIGHT 64      // Înălțimea ecranului OLED în pixeli.

// Inițializarea obiectului "display" pentru ecranul OLED. 
// Parametrii: lățime, înălțime, adresa de comunicare I2C (&Wire) și pinul de reset (-1 înseamnă că împarte reset-ul cu placa).
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup(){
  Serial.begin(115200);

  // Încercăm să pornim OLED-ul la adresa I2C 0x3C (adresa standard pentru aceste ecrane).
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed!"); // Dacă eșuează, afișăm o eroare în Serial Monitor.

    // Blocăm execuția într-o buclă infinită dacă ecranul nu funcționează.
    while (true) {
      delay(1000);
    }
  }

  // Curățăm orice a rămas pe ecran din utilizările anterioare (memoria buffer).
  display.clearDisplay();
  // Setăm culoarea textului pe ALB (ecranele monocrome folosesc WHITE pentru pixeli aprinși).
  display.setTextColor(WHITE);

  // Setăm mărimea textului la 1 (cea mai mică dimensiune vizibilă).
  display.setTextSize(1);
  // Mutăm cursorul de unde va începe să scrie textul la coordonatele X=0, Y=0 (colțul stânga-sus).
  display.setCursor(0, 0);
  // Pregătim mesajul de stare.
  display.println("Connecting WiFi...");
  // Trimitem efectiv datele grafice din memoria plăcuței către ecran pentru a fi afișate.
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
  if (infoOra.tm_hour == alarma_ora &&
    infoOra.tm_min == alarma_minut{

    // pornește buzzer
    // aprinde LED
}
}

void afisareOraLocala(){
  struct tm infoOra;

  if(!getLocalTime(&infoOra)){
    Serial.println("Nu s-a putut obtine ora");
    return;
  }

  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(16, 16);

  display.println(&infoOra, "%H:%M:%S");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 42);

  display.println(&infoOra, "%B:%d:%Y");

  display.display();

  Serial.println(&infoOra, "%A, %B %d %Y %H:%M:%S");
}