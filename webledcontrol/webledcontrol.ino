#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>

#ifndef STASSID
#define STASSID "Nintendo"
#define STAPSK  "papahbaik"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

bool lampState = false;

void handleRoot() {
  String rootPage  = "<html>";
  rootPage        += "  <head>";
  rootPage        += "    <title>Riptek IOT Aye!!!</title>";
  rootPage        += "    <style>";
  rootPage        += "      body {";
  rootPage        += "        min-width: 100%;";
  rootPage        += "        min-height: 100%;";
  rootPage        += "        background-color: #dcdde1;";
  rootPage        += "        margin: 0;";
  rootPage        += "      }";

  rootPage        += "      h1, span {";
  rootPage        += "          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;";
  rootPage        += "          color: #2f3640;";
  rootPage        += "      }";

  rootPage        += "      h1 {";
  rootPage        += "          font-weight: 600;";
  rootPage        += "      }";

  rootPage        += "      h6 {";
  rootPage        += "          margin-top: 100px;";
  rootPage        += "      }";

  rootPage        += "      .body-wrapper {";
  rootPage        += "        min-width: 100%;";
  rootPage        += "        display: flex;";
  rootPage        += "        align-items: center;";
  rootPage        += "        flex-direction: column;";
  rootPage        += "      }";

  rootPage        += "      .logo {";
  rootPage        += "        margin-top: 24px;";
  rootPage        += "      }";

  rootPage        += "      .lamp-status {";
  rootPage        += lampState ? "color: green;" : "color: red;";
  rootPage        += "        font-weight: bold;";
  rootPage        += "      }";

  rootPage        +="      .lamp-switch-wrapper {";
  rootPage        += "        padding-top: 12px;";
  rootPage        += "      }";

  rootPage        += "      .temperature-box {";
  rootPage        += "        margin-top: 32px;";
  rootPage        += "        padding: 24px;";
  rootPage        += "        border: 2px solid #2f3640;";
  rootPage        += "        border-radius: 12px;";
  rootPage        += "        display: flex;";
  rootPage        += "        flex-direction: column;";
  rootPage        += "        align-items: center;";
  rootPage        += "        box-shadow: 1px 1px 10px 0px rgba(0,0,0,0.4);";
  rootPage        += "      }";

  rootPage        += "      .temperature-title {";
  rootPage        += "        font-weight: bold;";
  rootPage        += "        font-size: 24px;";
  rootPage        += "      }";
  
  rootPage        += "      .temperature-value {";
  rootPage        += "        font-weight: bold;";
  rootPage        += "        font-size: 24px;";
  rootPage        += "        color: #40739e;";
  rootPage        += "      }";

  rootPage        += "      .temperature-refresh {";
  rootPage        += "        margin-top: 24px;";
  rootPage        += "      }";
  rootPage        += "    </style>";
  rootPage        += "  </head>";
  rootPage        += "  <body>";
  rootPage        += "    <div class=\"body-wrapper\">";
  rootPage        += "      <h1>Baca Sensor dan Kontrol LED</h1>";
  rootPage        += "      <div>";
  rootPage        += "        <span>Status lampu: </span>";
  rootPage        += lampState ? "<span class=\"lamp-status\">hidup &#9673</span><br />" : "<span class=\"lamp-status\">mati &#9673</span><br />";
  rootPage        += "        <div class=\"lamp-switch-wrapper\">";
  rootPage        += "          <button onclick=\"window.location.href='/ledon'\">Nyalakan LED</button>";
  rootPage        += "          <button onclick=\"window.location.href='/ledoff'\">Matikan LED</button>";
  rootPage        += "        </div>";
  rootPage        += "      </div>";
  rootPage        += "      <div class=\"temperature-box\">";
  rootPage        += "        <span class=\"temperature-title\">";
  rootPage        += "            Suhu dan kelembaban saat ini:";
  rootPage        += "        </span><br/>";
  rootPage        += "        <span class=\"temperature-value\">";
  rootPage        += String(dht.readTemperature()) + "&#8451 ";
  rootPage        += String(dht.readHumidity()) + "%";
  rootPage        += "        </span>";
  rootPage        += "        <button class=\"temperature-refresh\" onclick=\"location.reload()\">&#8635</button>";
  rootPage        += "      </div>";
  rootPage        += "      <h6>FikriRP &copy; 2020</h6>";
  rootPage        += "    </div>";
  rootPage        += "  </body>";
  rootPage        += "</html>";

  server.send(200, "text/html", rootPage);
}

void handleLedOn() {
  digitalWrite(LED_BUILTIN, !HIGH);
  lampState = true;
  server.send(200, "text/html", "<script>window.location.replace(\"/\");</script>");
}

void handleLedOff() {
  digitalWrite(LED_BUILTIN, !LOW);
  lampState = false;
  server.send(200, "text/html", "<script>window.location.replace(\"/\");</script>");
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, !LOW);
  
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("iotriptek")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound( []() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started");

  dht.begin();
  Serial.println("DHT sensor initialized");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
