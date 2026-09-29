#include <WiFi.h>
#include <WebServer.h>

// ==========================
// Wi-Fi
// ==========================

const char* ssid = "DIGI_86f4d7";
const char* password = "21d15123";

// ==========================
// Pinii LED-ului RGB
// ==========================

const int ledRed = 27;
const int ledGreen = 32;
const int ledBlue = 33;

// Web server pe portul 80
WebServer server(80);

// ==========================
// Pagina web
// ==========================

const char* html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <title>ESP32 RGB Controller</title>

  <style>
    body {
      font-family: Arial;
      text-align: center;
      margin-top: 40px;
    }

    .slider {
      width: 80%;
      max-width: 500px;
    }

    .colorBox {
      width: 150px;
      height: 150px;
      margin: 30px auto;
      border-radius: 15px;
      border: 2px solid black;
      background: rgb(0, 0, 0);
    }

    p {
      font-size: 20px;
    }
  </style>
</head>

<body>

  <h1>ESP32 RGB LED</h1>

  <div class="colorBox" id="colorBox"></div>

  <p>
    Red:
    <span id="redValue">0</span>
  </p>

  <input
    class="slider"
    type="range"
    min="0"
    max="255"
    value="0"
    oninput="changeColor()"
    id="red"
  >

  <p>
    Green:
    <span id="greenValue">0</span>
  </p>

  <input
    class="slider"
    type="range"
    min="0"
    max="255"
    value="0"
    oninput="changeColor()"
    id="green"
  >

  <p>
    Blue:
    <span id="blueValue">0</span>
  </p>

  <input
    class="slider"
    type="range"
    min="0"
    max="255"
    value="0"
    oninput="changeColor()"
    id="blue"
  >

  <script>

    function changeColor() {

      let red = document.getElementById("red").value;
      let green = document.getElementById("green").value;
      let blue = document.getElementById("blue").value;

      // Afișăm valorile
      document.getElementById("redValue").innerHTML = red;
      document.getElementById("greenValue").innerHTML = green;
      document.getElementById("blueValue").innerHTML = blue;

      // Schimbăm culoarea pătratului din browser
      document.getElementById("colorBox").style.backgroundColor =
        "rgb(" + red + "," + green + "," + blue + ")";

      // Trimitem valorile către ESP32
      fetch("/rgb?r=" + red + "&g=" + green + "&b=" + blue);
    }

  </script>

</body>
</html>
)rawliteral";

// ==========================
// Pagina principală
// ==========================

void handleRoot() {
  server.send(200, "text/html", html);
}

// ==========================
// Control RGB
// ==========================

void handleRGB() {

  if (server.hasArg("r")) {
    int red = server.arg("r").toInt();
    analogWrite(ledRed, red);
  }

  if (server.hasArg("g")) {
    int green = server.arg("g").toInt();
    analogWrite(ledGreen, green);
  }

  if (server.hasArg("b")) {
    int blue = server.arg("b").toInt();
    analogWrite(ledBlue, blue);
  }

  server.send(200, "text/plain", "OK");
}

// ==========================
// Setup
// ==========================

void setup() {

  Serial.begin(115200);

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  // LED stins la pornire
  analogWrite(ledRed, 0);
  analogWrite(ledGreen, 0);
  analogWrite(ledBlue, 0);

  // Conectare Wi-Fi
  WiFi.begin(ssid, password);

  Serial.print("Conectare la Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectat!");

  Serial.print("Adresa IP: ");
  Serial.println(WiFi.localIP());

  // Rute HTTP
  server.on("/", handleRoot);
  server.on("/rgb", handleRGB);

  // Pornim serverul
  server.begin();

  Serial.println("Web server pornit!");
}

// ==========================
// Loop
// ==========================

void loop() {
  server.handleClient();
}
