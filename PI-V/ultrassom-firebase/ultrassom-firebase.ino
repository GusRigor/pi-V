
#define trigPin D1
#define echoPin D2

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


const char* IpApiHost = "ip-api.com";

//Definicoes
#define FIREBASE_HOST "pi-v-e0ea4.firebaseio.com"
#define FIREBASE_AUTH "WLmxUiiAhhwqlD90Bry38LNZMGo68ApIPaa87F6Z"
#define WIFI_SSID "TrocoSenhaPorCerveja"
#define WIFI_PASSWORD "renatobike"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String folder = "";
String value = "";
String latitude = "";
String longitude = "";

WiFiClient client;

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
  Firebase.setString("products/PIV0069/trashes/0/hMax","25.0");

  timeClient.begin();
  timeClient.setTimeOffset(0);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  makePOSTlocation();
  Firebase.setString("products/PIV0069/trashes/0/location/longitude",longitude);
  Firebase.setString("products/PIV0069/trashes/0/location/latitude",latitude);
}

long duracao;
float distancia;
float aux = 0;
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
  
  delay(10000); // minutos
}


/*
 * Função que realiza GET request no site ip-api.com
 * Esse site disponibiliza uma API de geolocalização via IP
 * A função retorna um JSON com dados de geolocalização
 * Os dados de geolocalização são exibidos na pagina web em um Google Maps
 */
String makeGETlocation()
{
  Serial.println("\nRequisitando Geolocalização via IP\n");
  if ( !client.connect(IpApiHost, 80) ) {
    Serial.println("connection ao ip-api.com falhou");
    return "connection failed";
  }
  
  // Realiza HTTP GET request
  client.println("GET /json/?fields=lat,lon HTTP/1.1");
  client.print("Host: ");
  client.println(IpApiHost);
  client.println("Connection: close");
  client.println();
 
  // recebe o Header de resposta, 
  // cada linha do header termina com "\r\n", a ultima linha é fazia, ou seja, "\r\n" apenas
  while (client.connected()) {
    //Serial.println("A");
    String data = client.readStringUntil('\n'); // lê uma linha até o "\n"
    //Serial.println(data);
    if (data == "\r") {     // a ultima linha do header é vazia, ou seja apenas "\r\n", se lermos até "\n", temos apenas "\r"
      break;
    }
  }
  // recebe os dados de geolocalização em formato JSON e guarda na variável data
  String data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  data = client.readStringUntil('\n');
  //Serial.println("Dados de geolocalização recebidos\n");
  //Serial.println(data);  
  return data; 
}
 
/*
 * Função que envia ao servidor a localização do NodeMCU
 * função realiza um POST request ao servidor no link /location
 * o servidor por sua vez exibe a localização do NodeMCU no Google Maps
 */
void makePOSTlocation()
{
  String location = makeGETlocation(); // guarda o JSON de geolocalização na variável location
  //Serial.println("\nEnviando geolocalização ao servidor\n");
  Serial.println(location);
  //7 ao 13
  String lat;
  String lon; 
  Serial.print("Lat: ");
  lat += location[7];
  lat += location[8];
  lat += location[9];
  lat += location[10];
  lat += location[11];
  lat += location[12];
  lat += location[13];
  lat += location[14];
  Serial.println(lat);
  latitude = lat;
  Serial.print("Lon: ");
  lon += location[22];
  lon += location[23];
  lon += location[24];
  lon += location[25];
  lon += location[26];
  lon += location[27];
  lon += location[28];
  lon += location[29];
  Serial.println(lon);
  longitude = lon;
}
