// -------------------------
// BIBLIOTECI (LIBRARIES)
// -------------------------
#include <WiFi.h>             // Permite plăcuței (ex. ESP32) să se conecteze la rețele Wi-Fi.
#include   <HTTPClient.h>      // Facilitează trimiterea de cereri HTTP (cum ar fi accesarea unui link web).
#include <ArduinoJson.h>      // Ajută la procesarea și extragerea datelor din formatul JSON primit de la API.

#include <Wire.h>             // Permite comunicarea I2C (necesară pentru ecranul OLED).
#include <Adafruit_GFX.h>     // Biblioteca de bază pentru grafică (desenare linii, text, forme).
#include <Adafruit_SSD1306.h> // Biblioteca specifică pentru controlul ecranelor OLED bazate pe driverul SSD1306.

// -------------------------
// Wi-Fi
// -------------------------
const char* ssid = "DIGI_86f4d7";   // Numele rețelei Wi-Fi la care te conectezi.
const char* password = "21d15123";  // Parola rețelei Wi-Fi.

// -------------------------
// OLED
// -------------------------
#define SCREEN_WIDTH 128      // Lățimea ecranului OLED în pixeli.
#define SCREEN_HEIGHT 64      // Înălțimea ecranului OLED în pixeli.

// Inițializarea obiectului "display" pentru ecranul OLED. 
// Parametrii: lățime, înălțime, adresa de comunicare I2C (&Wire) și pinul de reset (-1 înseamnă că împarte reset-ul cu placa).
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// -------------------------
// Setup (Rulează o singură dată la pornire)
// -------------------------
void setup() {
  // Pornim comunicarea serială pentru a putea vedea mesaje pe calculator (Serial Monitor).
  Serial.begin(115200);
  delay(1000); // Așteptăm 1 secundă pentru a lăsa sistemul să se stabilizeze.

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

  // -------------------------
  // Conectare Wi-Fi
  // -------------------------
  WiFi.mode(WIFI_STA);         // Setăm placa în modul "Station" (se conectează la un router, nu emite propriul Wi-Fi).
  WiFi.begin(ssid, password);  // Inițiem conexiunea cu datele introduse la început.

  Serial.print("Connecting");

  // Cât timp statusul Wi-Fi NU este conectat, rulăm această buclă.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");         // Printăm un punct pe Serial Monitor pentru a arăta că lucrează.
    delay(500);                // Așteptăm jumătate de secundă între verificări.
  }

  // După ce iese din buclă (s-a conectat), trecem pe un rând nou și anunțăm succesul.
  Serial.println();
  Serial.println("WiFi connected!");

  // -------------------------
  // HTTP request (Cererea către serverul meteo)
  // -------------------------
  HTTPClient http; // Creăm un obiect de tip HTTPClient.

  // Configurăm adresa web (URL-ul) de unde cerem datele. 
  // Folosim coordonatele pentru Timișoara (lat 45.75, long 21.23) și cerem temperatura curentă.
  http.begin(
    "https://api.open-meteo.com/v1/forecast?"
    "latitude=45.75&"
    "longitude=21.23&"
    "current=temperature_2m"
  );

  // Efectuăm cererea GET (cerem datele) și salvăm codul de răspuns (ex. 200 înseamnă OK, 404 înseamnă eroare).
  int httpCode = http.GET();

  // Afișăm codul HTTP în Serial Monitor pentru depanare (debugging).
  Serial.print("HTTP code: ");
  Serial.println(httpCode);

  // Dacă codul este mai mare ca 0, înseamnă că am primit un răspuns de la server.
  if (httpCode > 0) {

    // Salvăm întregul răspuns primit (care este în format text/JSON) în variabila 'response'.
    String response = http.getString();

    // -------------------------
    // Parsare JSON (Transformăm textul într-un format ușor de citit pentru cod)
    // -------------------------
    JsonDocument doc; // Creăm un document JSON în memoria plăcuței.

    // Procesăm textul 'response' și îl stocăm în 'doc'.
    DeserializationError error = deserializeJson(doc, response);

    // Dacă a apărut o eroare în timpul procesării JSON-ului (ex: text corupt sau incomplet).
    if (error) {
      Serial.print("JSON error: ");
      Serial.println(error.c_str()); // Afișăm motivul erorii.

      // Afișăm eroarea și pe ecranul OLED.
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("JSON error!");
      display.display();

      http.end(); // Închidem conexiunea web.
      return;     // Oprim execuția funcției setup().
    }

    // -------------------------
    // Extragem datele
    // -------------------------
    // Căutăm în documentul JSON valoarea "temperature_2m" aflată în interiorul blocului "current".
    float temperature = doc["current"]["temperature_2m"];

    // Extragem data și ora (care vin sub formă de text, ex: "2026-09-12T16:00").
    String time = doc["current"]["time"];

    // -------------------------
    // Serial Monitor (Afișăm datele extrase pe calculator)
    // -------------------------
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Time: ");
    Serial.println(time);

    // -------------------------
    // Extragem doar ora
    // -------------------------
    // Din textul complet (ex: "2026-09-12T16:00"), tăiem doar caracterele de la poziția 11 până la 16.
    // Aceasta ne va da strict "16:00".
    String hour = time.substring(11, 16);

    // -------------------------
    // OLED (Afișarea pe ecran)
    // -------------------------
    display.clearDisplay(); // Curățăm ecranul pentru a desena noile date.

    // Titlu (Orașul)
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("TIMISOARA");

    // Desenăm o linie orizontală de separare sub titlu (de la x=0, y=12 până la x=127, y=12).
    display.drawLine(0, 12, 127, 12, WHITE);

    // Temperatură (setăm textul mai mare)
    display.setTextSize(2);
    display.setCursor(0, 20);

    // Printăm temperatura cu o singură zecimală (ex: 24.5).
    display.print(temperature, 1);
    display.print(" C");

    // Ora (revenim la text mic)
    display.setTextSize(1);
    display.setCursor(0, 50);

    display.print("Time: ");
    display.print(hour);

    // Trimitem toate instrucțiunile de desenare cumulate către ecran pentru afișare.
    display.display();
  }
  // Dacă codul HTTP primit este <= 0 (conexiune picată, lipsă net, etc.)
  else {
    Serial.println("HTTP request failed");

    // Afișăm eroarea pe ecran.
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("HTTP error!");
    display.display();
  }

  // Închidem conexiunea HTTP pentru a elibera resursele (foarte important pentru stabilitate).
  http.end();
}

// -------------------------
// Loop (Rulează la infinit)
// -------------------------
void loop() {
  // Rămâne gol deoarece am scris tot codul în `setup()`.
  // Datele se descarcă și se afișează o singură dată la pornirea sau resetarea plăcuței.
}