#include <ESP8266WiFi.h>
const char* ssid     = "RCwifiModule";
const char* password = "PWSPROJECT13";

WiFiServer server(80);

String header;
String outputState = "?"; //initially set to off
String outputDirection = "?"; //initially no direction
String lightState = "?"; //initially set to normal

// Motor Inputs (INL=left in INR=right in)
const int INL1 = D2;
const int INL2 = D7;
const int INR1 = D6;
const int INR2 = D5;
const int RL = 5;
const int RLa = 4;

void connectToWifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(INL1, OUTPUT);
  pinMode(INL2, OUTPUT);
  pinMode(INR1, OUTPUT);
  pinMode(INR2, OUTPUT);
  pinMode(RL, OUTPUT);
  pinMode(RLa, OUTPUT);
  stopMotors();
  Serial.begin(115200);
  connectToWifi(); 

  digitalWrite(D0, LOW);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);   
}

void relayon(){
  Serial.println("Lights: ON");
  digitalWrite(RL, HIGH);  
}

void relayoff(){
  Serial.println("Lights: OFF");
  digitalWrite(RL, LOW);  
}

void relaybut(){
  Serial.println("Button: Signal");
  digitalWrite(RLa, HIGH); 
  delay(1000);
  digitalWrite(RLa, LOW);  
}

void right(){
  Serial.println("Beweging: →");
  digitalWrite(INL1, HIGH);
  analogWrite(INL2, LOW);  
  digitalWrite(INR1, HIGH);
  analogWrite(INR2, LOW);
}

void left(){
  Serial.println("Beweging: ←");
  digitalWrite(INL1, LOW);
  digitalWrite(INL2, HIGH);  
  digitalWrite(INR1, LOW);
  digitalWrite(INR2, HIGH);
}

void stopMotors(){
  Serial.println("Beweging: O");
  digitalWrite(INL1, LOW);
  digitalWrite(INL2, LOW);  
  digitalWrite(INR1, LOW);
  digitalWrite(INR2, LOW);
}

void forward(){
  Serial.println("Beweging: ↑");
  digitalWrite(INL1, HIGH);
  digitalWrite(INL2, LOW);  
  digitalWrite(INR1, LOW);
  digitalWrite(INR2, HIGH);
}

void backward(){
  Serial.println("Beweging: ↓");
  digitalWrite(INL1, LOW);
  digitalWrite(INL2, HIGH);  
  digitalWrite(INR1, HIGH);
  digitalWrite(INR2, LOW);
}

void createWebServer(){
  WiFiClient client = server.available();   
  if (client) {                            
    Serial.println("New Client.");        
    String currentLine = "";               
    while (client.connected()) {           
      if (client.available()) {          
        char c = client.read();            
        Serial.write(c);                 
        header += c;
        if (c == '\n') {                 
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Motor Control (On & Off)
            if (header.indexOf("GET /control/forward") >= 0) {
              outputState = "<h2 class=\"green\">AAN</h2>";
              outputDirection = "<h2 class=\"green\">&#8593;</h2>";
              forward();
              
            } else if (header.indexOf("GET /control/stop") >= 0) {
              outputState = "<h2 class=\"red\">UIT</h2>";
              outputDirection = "<h2 class=\"red\">O</h2>";
              stopMotors();
              
            } else if (header.indexOf("GET /control/backward") >= 0) {
              outputState = "<h2 class=\"green\">AAN";
              outputDirection = "<h2 class=\"green\">&#8595;</h2>";
              backward();
              
            } else if (header.indexOf("GET /control/left") >= 0) {
              outputState = "<h2 class=\"green\">AAN";
              outputDirection = "<h2 class=\"green\">&#8592;</h2>";
              left();  
                         
            } else if (header.indexOf("GET /control/right") >= 0) {
              outputState = "<h2 class=\"green\">AAN";
              outputDirection = "<h2 class=\"green\">&#8594;</h2>";
              right();  
                         
            } else if (header.indexOf("GET /light/stop") >= 0) {
              lightState = "<h2 class=\"red\">Stopped";
              relayon();   
                 
            } else if (header.indexOf("GET /light/run") >= 0) {
              lightState = "<h2 class=\"green\">Running";
              relayoff(); 
              
            } else if (header.indexOf("GET /relay/button") >= 0) {
              relaybut(); 

            }
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
           
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #2ca545; border: none; color: white; padding: 16px 40px; margin: 25px; box-shadow: 0px 0px 15px 1px #aaaaaa;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttoncenter { background-color: #ba1e1e;}");
            client.println(".red { color: #DC143C;}");
            client.println(".green { color: #7CFC00;}");
            client.println(".yellow { color: #FFFF00;}");
            client.println("</style></head>");
            
            
            client.println("<title>PWS WIFI controlled Vehicle</title>");
            client.println("");
            client.println("");
            client.println(" ");
            client.println(" ");
            
            
            client.println("<h2>Status: </h2>" + outputState);
            client.println("<h2>Beweging: </h2>"+ outputDirection);
            
            client.println("<p><a href=\"/control/forward\"><button class=\"button\">&#8593;</button></a></p>");
            client.print("<p><a href=\"/control/left\"><button class=\"button\">&#8592;</button></a> <a href=\"/control/stop\"><button class=\"button\" class=\"buttoncenter\">O</button></a> <a href=\"/control/right\"><button class=\"button\">&#8594;</button></a></p>");
            client.println("<p><a href=\"/control/backward\"><button class=\"button\">&#8595;</button></a></p>");
            client.println("");
            client.println("<h2>Lights: </h2>"+ lightState);
            client.println("<p><a href=\"/light/run\"><button class=\"button\">GO</button></a> <a href=\"/light/stop\"><button class=\"button\">STOP</button></a></p>");
            client.println("<a href=\"/relay/button\"><button class=\"button\">LIGHTS</button></a> ");
            
               
            client.println("</body></html>");
            
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;    
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void loop(){
  createWebServer();
}
