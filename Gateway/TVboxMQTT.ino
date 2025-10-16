#include <WiFi.h> // <- A biblioteca para ESP32
#include <PubSubClient.h>

// --- CONFIGURAÇÕES DA REDE WI-FI ---
const char* ssid = "LCADE_CEAMAZON_2.4G";
const char* password = "pquality";

// --- CONFIGURAÇÕES DO BROKER MQTT (NA TXBOX) ---
const char* mqtt_server = "192.168.0.165"; 
const int mqtt_port = 1883;
const char* mqtt_user = "petee";     // O usuário que você criou
const char* mqtt_password = "petee123";   // A senha que você criou

// --- TÓPICOS MQTT ---
const char* topico_publicar = "teste/local";
const char* topico_subscrever = "teste/local";

// O LED interno na maioria das placas ESP32 fica no pino 2
#define LED_BUILTIN 2

// Inicializa os clientes
WiFiClient espClient;
PubSubClient client(espClient);

// Função chamada quando uma mensagem chega em um tópico que subscrevemos
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  String mensagem;
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Mensagem: ");
  Serial.println(mensagem);

  // Exemplo: Acender ou apagar o LED interno com base na mensagem
  if (mensagem == "ON") {
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (mensagem == "OFF") {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

// Função para reconectar ao broker MQTT se a conexão cair
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao Broker MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      client.publish(topico_publicar, "ESP32 Conectado");
      client.subscribe(topico_subscrever);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(10);

  // Conectando ao Wi-Fi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n--------------------");
  Serial.println("WiFi conectado!");
  Serial.print("Endereço IP da ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println("--------------------");

  // Configurando a conexão com o broker MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Essencial para manter a conexão e processar mensagens
}
