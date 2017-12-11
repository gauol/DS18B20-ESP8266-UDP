#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LED 10

#define serverPort 90
const char* serverIP = "192.168.10.20";
String sensorID = "KUCHNIA";

const char* ssid = "nazwa_sieci";
const char* password = "chaslo";
boolean wifiConnected = false;

// UDP variables
unsigned int localPort = serverPort;
WiFiUDP UDP;
boolean udpConnected = false;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged"; // a string to send back
char udpBuffer[126];
#define ONE_WIRE_BUS 12
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1, Thermometer2;


int liczba = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  sensors.begin();
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  if (!sensors.getAddress(Thermometer1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(Thermometer2, 1)) Serial.println("Unable to find address for Device 1");
  sensors.setResolution(Thermometer1, TEMPERATURE_PRECISION);
  sensors.setResolution(Thermometer2, TEMPERATURE_PRECISION);
  initWifi();
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);

  sensors.requestTemperatures();
  Serial.println(generateUDPresponse());
  UDP.beginPacket(serverIP, serverPort);
  generateUDPresponse().toCharArray(udpBuffer, 126);
  UDP.write(udpBuffer);
  UDP.endPacket();
  delay(1000);
}

void initWifi() {
   wifiConnected = connectWifi();
  
   if (wifiConnected) {
     udpConnected = connectUDP();
     if (udpConnected) {
       Serial.println("Wifi Connected");
     }
   }
}

// connect to UDP – returns true if successful or false if not
boolean connectUDP() {
  boolean state = false;

  Serial.println("");
  Serial.println("Connecting to UDP");

  if (UDP.begin(localPort) == 1) {
    Serial.println("Connection successful");
    state = true;
  }
  else {
    Serial.println("Connection failed");
  }

  return state;
}
// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  return state;
}

String generateUDPresponse(){
  float tempC1 = sensors.getTempC(Thermometer1);
  float tempC2 = sensors.getTempC(Thermometer2);  
  String data = "<SEN=" + sensorID +":TEMP1="+ String(tempC1, 2) +":TEMP2="+ String(tempC2, 2) +"> ";
  return data;
}
