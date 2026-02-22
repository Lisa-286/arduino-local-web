/*
  Arduino Local Web - UNO WiFi R4
  Web server standalone che crea hotspot WiFi e serve pagina web locale.
  Non richiede connessione a rete esterna.
*/

#include <WiFiS3.h>

// Configurazione hotspot (Access Point)
char apSSID[] = "ArduinoTempLocal";
char apPass[] = "password123";  // password per l'hotspot (min 8 caratteri)

WiFiServer server(80);

// Pin sensore temperatura
const int tempPin = A0;
float currentTemp = 22.5;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 3000; // 3 secondi

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Avvio hotspot WiFi...");
  
  // Crea Access Point
  if (WiFi.beginAP(apSSID, apPass) != WL_AP_LISTENING) {
    Serial.println("Creazione AP fallita!");
    while (true);
  }
  
  delay(1000);
  
  // Avvia server web
  server.begin();
  
  Serial.println("Hotspot WiFi attivo!");
  Serial.print("SSID: ");
  Serial.println(apSSID);
  Serial.print("Password: ");
  Serial.println(apPass);
  Serial.print("IP Access Point: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.println("Connettiti al WiFi 'ArduinoTempLocal' con password 'password123'");
  Serial.println("Poi apri nel browser: http://192.168.4.1");
  Serial.println("========================================");
}

void loop() {
  // Leggi temperatura ogni 3 secondi
  unsigned long now = millis();
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    currentTemp = readTemperature();
    lastUpdate = now;
    Serial.print("Temperatura: ");
    Serial.print(currentTemp, 1);
    Serial.println(" °C");
  }
  
  // Gestisci client web
  WiFiClient client = server.available();
  if (client) {
    handleClient(client);
    client.stop();
  }
}

float readTemperature() {
  // Lettura simulata - per sensore reale sostituisci
  int raw = analogRead(tempPin);
  // Simula 0-1023 -> 18-28°C
  float temp = 18.0 + (raw / 1023.0) * 10.0;
  // Variazione casuale leggera
  temp += (random(-15, 15) / 100.0);
  return temp;
}

void handleClient(WiFiClient client) {
  String request = client.readStringUntil('\r');
  Serial.print("Client: ");
  Serial.println(request);
  client.flush();
  
  // Endpoint dati semplice
  if (request.indexOf("GET /data") != -1) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println(currentTemp);
    return;
  }
  
  // Risposta HTTP per pagina HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  // Pagina HTML completa inline
  client.println("<!DOCTYPE html>");
  client.println("<html lang='it'>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<title>Temperatura Locale Arduino</title>");
  client.println("<style>");
  client.println("html, body { height: 100%; margin: 0; padding: 0; font-family: 'Segoe UI', system-ui, sans-serif; background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%); color: #f8fafc; }");
  client.println(".container { max-width: 800px; margin: 0 auto; padding: 20px; min-height: 100vh; display: flex; flex-direction: column; justify-content: center; }");
  client.println("header { text-align: center; margin-bottom: 40px; }");
  client.println("h1 { font-size: 2.8rem; margin-bottom: 10px; background: linear-gradient(90deg, #60a5fa, #3b82f6); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }");
  client.println(".subtitle { font-size: 1.2rem; color: #94a3b8; }");
  client.println(".temp-card { background: rgba(30, 41, 59, 0.7); backdrop-filter: blur(10px); border-radius: 24px; padding: 40px; text-align: center; border: 1px solid rgba(96, 165, 250, 0.2); box-shadow: 0 20px 40px rgba(0, 0, 0, 0.3); margin-bottom: 30px; }");
  client.println(".temp-value { font-size: 5.5rem; font-weight: 900; color: #60a5fa; line-height: 1; margin: 20px 0; text-shadow: 0 0 30px rgba(96, 165, 250, 0.5); }");
  client.println(".unit { font-size: 2.2rem; color: #cbd5e1; margin-left: 10px; }");
  client.println(".update-info { color: #94a3b8; margin-top: 15px; font-size: 0.95rem; }");
  client.println(".info-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-top: 30px; }");
  client.println(".info-card { background: rgba(30, 41, 59, 0.5); border-radius: 16px; padding: 20px; border: 1px solid #334155; }");
  client.println(".info-card h3 { color: #60a5fa; margin-top: 0; margin-bottom: 15px; font-size: 1.2rem; }");
  client.println(".info-list { list-style: none; padding: 0; margin: 0; }");
  client.println(".info-list li { padding: 10px 0; border-bottom: 1px solid #334155; display: flex; justify-content: space-between; }");
  client.println(".info-list li:last-child { border-bottom: none; }");
  client.println(".badge { display: inline-block; padding: 4px 12px; border-radius: 20px; font-size: 0.85rem; font-weight: 600; }");
  client.println(".badge-online { background: rgba(16, 185, 129, 0.2); color: #10b981; }");
  client.println("footer { text-align: center; margin-top: 40px; color: #64748b; font-size: 0.9rem; }");
  client.println("@media (max-width: 600px) { h1 { font-size: 2.2rem; } .temp-value { font-size: 4rem; } .container { padding: 15px; } }");
  client.println("</style>");
  client.println("<script>");
  client.println("let lastTemp = " + String(currentTemp, 1) + ";");
  client.println("function updateDisplay() {");
  client.println("  fetch('/data')");
  client.println("    .then(r => r.text())");
  client.println("    .then(t => {");
  client.println("      const temp = parseFloat(t);");
  client.println("      if (!isNaN(temp)) {");
  client.println("        document.getElementById('temp').textContent = temp.toFixed(1);");
  client.println("        document.getElementById('time').textContent = new Date().toLocaleTimeString('it-IT');");
  client.println("        lastTemp = temp;");
  client.println("      }");
  client.println("    })");
  client.println("    .catch(e => console.log('Aggiornamento fallito (offline?)'));");
  client.println("}");
  client.println("setInterval(updateDisplay, 3000);");
  client.println("updateDisplay();");
  client.println("</script>");
  client.println("</head>");
  client.println("<body>");
  client.println("<div class='container'>");
  client.println("<header>");
  client.println("<h1>🌡️ Temperatura Locale</h1>");
  client.println("<p class='subtitle'>Web server standalone · Hotspot Arduino · Nessuna rete esterna</p>");
  client.println("</header>");
  client.println("<div class='temp-card'>");
  client.println("<div class='temp-value' id='temp'>" + String(currentTemp, 1) + "</div>");
  client.println("<div><span class='unit'>°C</span></div>");
  client.println("<div class='update-info'>Ultimo aggiornamento: <span id='time'>" + String(millis() / 1000) + "s</span></div>");
  client.println("</div>");
  client.println("<div class='info-grid'>");
  client.println("<div class='info-card'>");
  client.println("<h3>📡 Connessione</h3>");
  client.println("<ul class='info-list'>");
  client.println("<li>SSID:<span><strong>ArduinoTempLocal</strong></span></li>");
  client.println("<li>Password:<span>password123</span></li>");
  client.println("<li>IP Server:<span>192.168.4.1</span></li>");
  client.println("<li>Stato:<span class='badge badge-online'>ATTIVO</span></li>");
  client.println("</ul>");
  client.println("</div>");
  client.println("<div class='info-card'>");
  client.println("<h3>⚙️ Sistema</h3>");
  client.println("<ul class='info-list'>");
  client.println("<li>Dispositivo:<span>Arduino UNO R4</span></li>");
  client.println("<li>Sensore:<span>Pin A0</span></li>");
  client.println("<li>Aggiornamento:<span>3 secondi</span></li>");
  client.println("<li>Modalità:<span>Hotspot</span></li>");
  client.println("<li>Client:<span>1 connesso</span></li>");
  client.println("</ul>");
  client.println("</div>");
  client.println("</div>");
  client.println("<footer>");
  client.println("<p>Server web standalone · Generato automaticamente da Arduino · " + String(__DATE__) + "</p>");
  client.println("</footer>");
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");
}
