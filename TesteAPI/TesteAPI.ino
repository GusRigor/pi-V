#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
 
// Definições da rede Wifi
const char* SSID = "TrocoSenhaPorCerveja";
const char* PASSWORD = "renatobike";
  
 
// servidor que disponibiliza serviço de geolocalização via IP    
const char* IpApiHost = "ip-api.com";
 
WiFiClient client;
 
/*
 * função que conecta o NodeMCU na rede Wifi
 * SSID e PASSWORD devem ser indicados nas variáveis
 */
void reconnectWiFi() 
{
  if(WiFi.status() == WL_CONNECTED)
    return;
 
  WiFi.begin(SSID, PASSWORD);
 
  while(WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
 
  Serial.println();
  Serial.print("Conectado com sucesso na rede: ");
  Serial.println(SSID);
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());  
}
 
void initWiFi()
{
  Serial.println("\nIniciando configuração WiFi\n");
  delay(10);
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");
 
  reconnectWiFi();
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
    Serial.println("A");
    String data = client.readStringUntil('\n'); // lê uma linha até o "\n"
    Serial.println(data);
    if (data == "\r") {     // a ultima linha do header é vazia, ou seja apenas "\r\n", se lermos até "\n", temos apenas "\r"
      break;
    }
  }
  // recebe os dados de geolocalização em formato JSON e guarda na variável data
  String data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  data = client.readStringUntil('\n');Serial.println(data);
  Serial.println("Dados de geolocalização recebidos\n");
  Serial.println(data);  
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
  Serial.println("\nEnviando geolocalização ao servidor\n");
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
  Serial.println(lat);
  Serial.print("Lon: ");
  lon += location[21];
  lon += location[22];
  lon += location[23];
  lon += location[24];
  lon += location[25];
  lon += location[26];
  lon += location[27];
  lon += location[28];
  Serial.println(lon);
}
 
void setup() {  
  Serial.begin(115200);   
  initWiFi();
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeMsgPack(doc, input);

  // Test if parsing succeeded.
  if (error) {
    Serial.print("deserializeMsgPack() failed: ");
    Serial.println(error.c_str());
    return;
  }
  double latitude = doc["lat"];
  double longitude = doc["lon"];
  Serial.println(latitude)
  Serial.println(longitude)
  makePOSTlocation();
}
 
void loop() {
 
}
