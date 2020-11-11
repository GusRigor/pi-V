#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
 
// Set these to run example.
#define FIREBASE_HOST "pi-v-e0ea4.firebaseio.com"
#define FIREBASE_AUTH "WLmxUiiAhhwqlD90Bry38LNZMGo68ApIPaa87F6Z"
#define WIFI_SSID "RockBar"
#define WIFI_PASSWORD "renatobike"
#define trigPin D1
#define echoPin D2

String folder = "";
String value = "";
String latitude = "-23.56519673";
String longitude = "-46.78712636";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  
  Serial.begin(9600);

  // connect to wifi.
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

  Firebase.setString("products/PIV0069/trashes/0/location/longitude",longitude);
  Firebase.setString("products/PIV0069/trashes/0/location/latitude",latitude);
}

long duracao;
float distancia, aux;
int count = 0;
float vetorPosicao[10];

void loop() {
   timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  folder = "products/PIV0069/trashes/0/level/timestamp";
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

  if(distancia < 25.0){
    count++;
    aux = distancia;
    vetorPosicao[count-1] = aux;
  
    Serial.print("Contador: ");
    Serial.println(count);
    if(count > 6 ){
      aux = 0;
      for(int i = 0; i != count-1; i++){
        aux += vetorPosicao[i];
        Serial.print("aux: ");
        Serial.println(aux);
      }
      aux += distancia;
      aux = aux/7;
      Serial.print("aux: ");
      Serial.println(aux);
      folder = String(epochTime);
      Firebase.setString("products/PIV0069/trashes/0/level/timestamp",folder);
      Firebase.setString("products/PIV0069/trashes/0/level/measure",value);
      Serial.println("Postado no firebase");
      count = 0;
    }
    Serial.print("Contador: ");
    Serial.println(count);
    
    Serial.print("Duracao: ");
    Serial.println(duracao);
    Serial.println("========================================");
  }
  
  delay(10000);// minutos
}
