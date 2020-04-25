
#define trigPin D1
#define echoPin D2

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//Definicoes
#define FIREBASE_HOST "teste-89a4a.firebaseio.com"
#define FIREBASE_AUTH "g7UEAxozoXIl6XhZy3Zqspyw7Rw4TMjVp6OIyuR9"
#define WIFI_SSID "TrocoSenhaPorCerveja"
#define WIFI_PASSWORD "renatobike"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String folder = "";
String value = "";
 
void setup() {
  Serial.begin(9600);
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500); 
  }
  Serial.println();
  Serial.print("conectado: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  timeClient.begin();
  timeClient.setTimeOffset(0);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

long duracao;
float distancia, aux;

void loop() {
  
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  folder = "distancia/" + String(epochTime);
  Serial.println(folder);
  
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duracao = pulseIn(echoPin, HIGH ,1000000);
  aux = duracao;
  distancia = (aux*340/10000)/2;
  Serial.print("Distancia: ");
  Serial.println(distancia);
  value = String(distancia);
  Firebase.setString(folder,value);
  
  Serial.print("Duracao: ");
  Serial.println(duracao);

  delay(12000000); //dois minutos
}
