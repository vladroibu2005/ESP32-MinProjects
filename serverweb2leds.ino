// ============================================================
// ESP32 WEB SERVER - CONTROL DOUĂ LED-URI
// ============================================================

// Biblioteca pentru conexiunea Wi-Fi a ESP32.
#include <WiFi.h>

// Biblioteca pentru crearea serverului web.
#include <WebServer.h>


// ============================================================
// WI-FI
// ============================================================

// Numele rețelei Wi-Fi.
const char* ssid = "DIGI...";

// Parola rețelei Wi-Fi.
const char* password = "....";


// ============================================================
// SERVER
// ============================================================

// Creăm serverul web pe portul 80.
// Portul 80 este portul standard pentru HTTP.
WebServer server(80);


// ============================================================
// LED-URI
// ============================================================

// LED-ul 1 este conectat la GPIO 18.
const int led1Pin = 18;

// LED-ul 2 este conectat la GPIO 19.
const int led2Pin = 19;


// ============================================================
// STAREA LED-URILOR
// ============================================================

// Variabila păstrează starea LED-ului 1.
// false = stins
// true  = aprins
bool led1State = false;


// Variabila păstrează starea LED-ului 2.
bool led2State = false;


// ============================================================
// GENERAREA PAGINII WEB
// ============================================================

String getHTML()
{
  // Creăm un String gol în care vom construi pagina.
  String html = "";


  // ----------------------------------------------------------
  // ÎNCEPUTUL DOCUMENTULUI HTML
  // ----------------------------------------------------------

  html += "<!DOCTYPE html>";

  html += "<html>";

  html += "<head>";

  // Permite afișarea corectă a caracterelor.
  html += "<meta charset='UTF-8'>";

  // Face pagina adaptabilă pentru telefon.
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";

  // Titlul paginii.
  html += "<title>ESP32 Control</title>";


  // ----------------------------------------------------------
  // CSS - ASPECTUL PAGINII
  // ----------------------------------------------------------

  html += "<style>";

  // Stilul general al paginii.
  html += "body {";
  html += "font-family: Arial;";
  html += "text-align: center;";
  html += "background-color: #f2f2f2;";
  html += "padding: 20px;";
  html += "}";


  // Stilul cardurilor LED-urilor.
  html += ".card {";
  html += "background-color: white;";
  html += "padding: 25px;";
  html += "margin: 20px auto;";
  html += "max-width: 400px;";
  html += "border-radius: 15px;";
  html += "box-shadow: 0 4px 10px rgba(0,0,0,0.15);";
  html += "}";


  // Stilul butoanelor.
  html += "button {";
  html += "font-size: 18px;";
  html += "padding: 12px 30px;";
  html += "border: none;";
  html += "border-radius: 10px;";
  html += "cursor: pointer;";
  html += "}";


  // Stilul pentru LED aprins.
  html += ".on {";
  html += "color: green;";
  html += "font-weight: bold;";
  html += "}";


  // Stilul pentru LED stins.
  html += ".off {";
  html += "color: red;";
  html += "font-weight: bold;";
  html += "}";


  // Închidem CSS-ul.
  html += "</style>";


  // Închidem HEAD.
  html += "</head>";


  // ----------------------------------------------------------
  // CORPUL PAGINII
  // ----------------------------------------------------------

  html += "<body>";


  // Titlul principal.
  html += "<h1>ESP32 Control Panel</h1>";


  // ==========================================================
  // LED 1
  // ==========================================================

  html += "<div class='card'>";

  // Numele LED-ului.
  html += "<h2>LED 1</h2>";


  // Verificăm starea LED-ului 1.
  if (led1State)
  {
    // Dacă LED-ul este aprins, afișăm ON.
    html += "<p>Stare: <span class='on'>● APRINS</span></p>";

    // Butonul va avea acțiunea de stingere.
    html += "<a href='/toggle1'>";
    html += "<button>STINGE</button>";
    html += "</a>";
  }
  else
  {
    // Dacă LED-ul este stins, afișăm OFF.
    html += "<p>Stare: <span class='off'>● STINS</span></p>";

    // Butonul va avea acțiunea de aprindere.
    html += "<a href='/toggle1'>";
    html += "<button>APRINDE</button>";
    html += "</a>";
  }

  // Închidem cardul LED-ului 1.
  html += "</div>";


  // ==========================================================
  // LED 2
  // ==========================================================

  html += "<div class='card'>";

  // Numele LED-ului.
  html += "<h2>LED 2</h2>";


  // Verificăm starea LED-ului 2.
  if (led2State)
  {
    // Dacă LED-ul este aprins.
    html += "<p>Stare: <span class='on'>● APRINS</span></p>";

    // Butonul îl va stinge.
    html += "<a href='/toggle2'>";
    html += "<button>STINGE</button>";
    html += "</a>";
  }
  else
  {
    // Dacă LED-ul este stins.
    html += "<p>Stare: <span class='off'>● STINS</span></p>";

    // Butonul îl va aprinde.
    html += "<a href='/toggle2'>";
    html += "<button>APRINDE</button>";
    html += "</a>";
  }

  // Închidem cardul LED-ului 2.
  html += "</div>";


  // Închidem BODY.
  html += "</body>";

  // Închidem HTML.
  html += "</html>";


  // Returnăm pagina completă.
  return html;
}


// ============================================================
// SETUP
// ============================================================

void setup()
{
  // Pornim Serial Monitor la 115200 baud.
  Serial.begin(115200);

  // Așteptăm o secundă.
  delay(1000);


  // ----------------------------------------------------------
  // CONFIGURAREA GPIO-URILOR
  // ----------------------------------------------------------

  // GPIO 18 devine ieșire.
  pinMode(led1Pin, OUTPUT);

  // GPIO 19 devine ieșire.
  pinMode(led2Pin, OUTPUT);


  // La pornire, LED-ul 1 este stins.
  digitalWrite(led1Pin, LOW);

  // La pornire, LED-ul 2 este stins.
  digitalWrite(led2Pin, LOW);


  // ----------------------------------------------------------
  // CONECTAREA LA WI-FI
  // ----------------------------------------------------------

  // Începem conectarea la Wi-Fi.
  WiFi.begin(ssid, password);

  // Afișăm mesajul în Serial Monitor.
  Serial.println("Conectare la Wi-Fi...");


  // Așteptăm până când ESP32 se conectează.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Așteptăm 500 ms.
    delay(500);

    // Afișăm un punct.
    Serial.print(".");
  }


  // Trecem pe linie nouă.
  Serial.println();

  // Confirmăm conectarea.
  Serial.println("Wi-Fi conectat!");


  // Afișăm adresa IP.
  Serial.print("Adresa IP: ");

  // Afișăm IP-ul ESP32.
  Serial.println(WiFi.localIP());


  // ==========================================================
  // PAGINA PRINCIPALĂ
  // ==========================================================

  server.on("/", []()
  {
    // Trimitem pagina web către browser.
    server.send(200, "text/html", getHTML());
  });


  // ==========================================================
  // TOGGLE LED 1
  // ==========================================================

  server.on("/toggle1", []()
  {
    // Inversăm starea LED-ului.
    //
    // false -> true
    // true  -> false
    led1State = !led1State;


    // Aplicăm noua stare pe GPIO 18.
    digitalWrite(led1Pin, led1State);


    // Trimitem pagina actualizată.
    server.send(200, "text/html", getHTML());
  });


  // ==========================================================
  // TOGGLE LED 2
  // ==========================================================

  server.on("/toggle2", []()
  {
    // Inversăm starea LED-ului 2.
    led2State = !led2State;


    // Aplicăm noua stare pe GPIO 19.
    digitalWrite(led2Pin, led2State);


    // Trimitem pagina actualizată.
    server.send(200, "text/html", getHTML());
  });


  // Pornim serverul.
  server.begin();

  // Afișăm confirmarea.
  Serial.println("Server web pornit!");
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
  // Verificăm permanent dacă browserul a trimis o cerere.
  server.handleClient();
}
