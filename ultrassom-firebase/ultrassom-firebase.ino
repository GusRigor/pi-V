#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

//Definicoes
#define FIREBASE_HOST "teste-89a4a.firebaseio.com"
#define FIREBASE_AUTH "g7UEAxozoXIl6XhZy3Zqspyw7Rw4TMjVp6OIyuR9"
#define WIFI_SSID "TrocoSenhaPorCerveja"
#define WIFI_PASSWORD "renatobike"

// Leitura HC-SR04
const uint8_t trig_pin = 10;
const uint8_t echo_pin = 9;
uint32_t print_timer;
 

 
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

  // Configuração do estado inicial dos pinos Trig e Echo.
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  digitalWrite(trig_pin, LOW);
}
void loop() {
  
  float valorTratado = trataValor();
  pushDistancia(valorTratado);
  wait(5);
  
}

void pushDistancia(float dist){
  Firebase.pushFloat("distancia",dist);
}

void wait(int sec){
  delay(sec * 1000);
}


float trataValor(){

  if (millis() - print_timer > 500) {
  print_timer = millis();
 
  // Pulso de 5V por pelo menos 10us para iniciar medição.
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(11);
  digitalWrite(trig_pin, LOW);
 
  /* Mede quanto tempo o pino de echo ficou no estado alto, ou seja,
  o tempo de propagação da onda. */
  uint32_t pulse_time = pulseIn(echo_pin, HIGH);
 
  /* A distância entre o sensor ultrassom e o objeto será proporcional a velocidade
  do som no meio e a metade do tempo de propagação. Para o ar na
  temperatura ambiente Vsom = 0,0343 cm/us. */
  float distance = 0.01715 * pulse_time;
  return distance;
  }
}
