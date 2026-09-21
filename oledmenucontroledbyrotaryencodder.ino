#include <Wire.h>               // Include biblioteca pentru comunicarea I2C (necesară pentru display)
#include <Adafruit_GFX.h>       // Include biblioteca grafică de bază pentru a desena forme, text și grafice
#include <Adafruit_SSD1306.h>   // Include biblioteca specifică pentru controlul driverului de ecran OLED SSD1306

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128        // Definește lățimea ecranului OLED în pixeli (128 de coloane)
#define SCREEN_HEIGHT 64        // Definește înălțimea ecranului OLED în pixeli (64 de rânduri)
#define OLED_RESET -1           // Specifică pinul de reset (-1 înseamnă că împarte resetul cu microcontrolerul sau nu are pin dedicat)

// Instanțiază obiectul 'display' transmițând dimensiunile, magistrala I2C (&Wire) și pinul de reset
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Encoder ----------------
const int CLK_PIN = 32;         // Definește pinul la care este conectat semnalul de Clock (A) al encoderului rotativ
const int DT_PIN  = 33;         // Definește pinul la care este conectat semnalul de Data (B) al encoderului rotativ
const int SW_PIN  = 25;         // Definește pinul la care este conectat butonul de pe axul encoderului (Switch)

int lastCLK;                    // Variabilă pentru a stoca starea anterioară a pinului CLK (necesară pentru detectarea tranzițiilor)

// ---------------- Menu ----------------
const char* menuItems[] = {     // Declară un array de pointeri la char (șiruri de caractere) pentru elementele meniului
  "Sensors",                    // Elementul cu index 0
  "Outputs",                    // Elementul cu index 1
  "Settings",                   // Elementul cu index 2
  "System Info"                 // Elementul cu index 3
};

const int menuCount = 4;        // Reține numărul total de opțiuni din meniu (folosit mai jos pentru limitarea selecției)
int menuIndex = 0;              // Variabilă globală care ține minte pe ce opțiune se află cursorul (inițial index 0)


// ---------------- Afisare meniu ----------------
void drawMenu() {               // Funcție personalizată pentru a desena interfața meniului principal
  display.clearDisplay();       // Șterge complet buffer-ul de memorie video al display-ului

  display.setTextSize(1);       // Setează mărimea fontului la 1 (dimensiunea standard, 5x7 pixeli pe caracter)
  display.setTextColor(SSD1306_WHITE); // Setează culoarea textului pe alb (pixel aprins, ecranul fiind monocrom)

  // Titlu
  display.setCursor(32, 0);     // Pozitionează cursorul de scriere pe coordonatele x=32, y=0 pentru a centra vizual titlul
  display.println("MAIN MENU"); // Scrie textul titlului în buffer și trece la rândul următor

  // Elemente meniu
  for (int i = 0; i < menuCount; i++) { // Buclează prin toate cele 4 elemente ale array-ului de meniu

    display.setCursor(0, 16 + i * 12);  // Calculează poziția Y pentru fiecare linie de text (spațiate la 12 pixeli între ele, pornind de la y=16)

    if (i == menuIndex) {       // Verifică dacă indexul generat de buclă este cel selectat momentan de utilizator
      display.print("> ");      // Dacă da, afișează un caracter indicator (săgeată) la începutul rândului
    } else {                    
      display.print("  ");      // Dacă nu, afișează spații goale pentru a păstra textul perfect aliniat cu restul
    }

    display.println(menuItems[i]); // Afișează textul opțiunii (ex: "Sensors")
  }

  display.display();            // Comanda finală care trimite buffer-ul generat din RAM către controlerul ecranului prin I2C pentru a fi afișat
}


void setup() {                  // Funcția setup, executată o singură dată la boot sau reset
  Serial.begin(115200);         // Inițializează interfața serială la 115200 baud rate (utilă pentru debugging)

  // Encoder
  pinMode(CLK_PIN, INPUT_PULLUP); // Configurează pinul CLK ca intrare și activează rezistența internă de pull-up (ține pinul HIGH by default)
  pinMode(DT_PIN, INPUT_PULLUP);  // Configurează pinul DT ca intrare cu rezistență de pull-up activată
  pinMode(SW_PIN, INPUT_PULLUP);  // Configurează pinul de buton ca intrare cu pull-up (apăsarea va trage pinul la LOW)

  lastCLK = digitalRead(CLK_PIN); // Citește și salvează starea inițială a pinului CLK pentru a avea o referință la prima rotire

  // OLED
  Wire.begin(21, 22);           // Inițializează hardware-ul I2C al ESP32 (SDA pe pinul 21, SCL pe pinul 22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Pornește ecranul (generează tensiunea intern) și caută-l la adresa I2C 0x3C
    Serial.println("OLED not found!");              // Dacă returnează 'false' (ecran neconectat/defect), printează o eroare
    while (true);                                   // Intră într-o buclă infinită blocând programul, deoarece display-ul lipsește
  }

  drawMenu();                   // Apelează funcția definită mai sus pentru a popula ecranul cu meniul inițial
}


void loop() {                   // Funcția loop rulează la nesfârșit, cât timp placa este alimentată

  // -------- ROTIRE ENCODER --------

  int currentCLK = digitalRead(CLK_PIN); // Citește constant starea semnalului de ceas (CLK)

  if (currentCLK != lastCLK) {           // Dacă starea s-a schimbat față de ultima citire (înseamnă că a început o tranziție/rotație)

    if (digitalRead(DT_PIN) != currentCLK) { // Compară starea pinului Data (DT) cu starea curentă de Clock (CLK) pentru a afla sensul de rotație
      menuIndex++;                           // Semnalele sunt defazate diferit (sens orar), deci mergem în jos în meniu (incrementăm indexul)
    } else {
      menuIndex--;                           // Semnalele coincid (sens antiorar), deci mergem în sus în meniu (decrementăm indexul)
    }

    // Circularitate meniu (Wrap-around logic)
    if (menuIndex >= menuCount) {            // Dacă am dat prea mult de rotiță și am depășit elementul "System Info" (index 3)
      menuIndex = 0;                         // Sărim înapoi la primul element "Sensors" (index 0)
    }

    if (menuIndex < 0) {                     // Dacă dăm în sus când suntem pe primul element (indexul devine -1)
      menuIndex = menuCount - 1;             // Sărim direct la ultimul element (index 3)
    }

    drawMenu();                              // Deoarece indexul s-a modificat, forțăm redesenarea meniului pentru a muta indicatorul ">"

    lastCLK = currentCLK;                    // Actualizează starea veche cu starea curentă, pregătind sistemul pentru următorul pas de rotație
  }


  // -------- APASARE ENCODER --------

  if (digitalRead(SW_PIN) == LOW) {          // Verifică dacă butonul de pe encoder este apăsat (coboară de la HIGH la LOW din cauza PULLUP)

    display.clearDisplay();                  // Șterge ecranul pentru a face loc ecranului de confirmare a selecției

    display.setTextSize(1);                  // Revine la text normal (marime 1)
    display.setCursor(0, 0);                 // Pune cursorul în stânga-sus (x=0, y=0)
    display.println("Selected:");            // Scrie cuvântul "Selected:"

    display.setTextSize(2);                  // Mărește fontul de 2 ori pentru a evidenția opțiunea selectată
    display.setCursor(0, 25);                // Mută cursorul la coordonata Y=25, cam pe mijlocul ecranului
    display.println(menuItems[menuIndex]);   // Printează opțiunea curentă bazată pe indexul la care am oprit rotirea

    display.display();                       // Trimite noul buffer în memoria SSD1306 pentru a afișa modificările

    Serial.print("Selected: ");              // Printează același text pe interfața serială (pentru debugging pe PC)
    Serial.println(menuItems[menuIndex]);    // Adaugă opțiunea efectivă selectată și trece la rând nou pe serial

    delay(500);                              // Oprește execuția 500 ms pentru ca utilizatorul să apuce să citească ecranul cu confirmarea

    drawMenu();                              // După pauză, redesenează automat meniul principal pentru a reveni la starea de dinainte de apăsare

    delay(150);                              // Un mic delay adițional pentru 'debouncing' (împiedică înregistrarea unei apăsări duble accidentale când ridici degetul)
  }
}