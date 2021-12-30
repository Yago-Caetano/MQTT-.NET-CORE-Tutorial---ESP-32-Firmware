#include <Arduino.h>

/* 
 *  Projeto: Conexão MQTT com broker feito em .NET Core
 *  Autor: Yago Caetano
 *  
 */
#include <WiFi.h>
#include <PubSubClient.h>
 

 
 
/* Defines do MQTT */
#define TOPICO_SUBSCRIBE "teste/topic2"
#define TOPICO_PUBLISH   "teste/topic1" 

 
#define ID_MQTT  "M-ESP32"   //deve ser unico no broker
 
/* Variaveis, constantes globais */
 
const char* SSID = "jkk"; // SSID Wi-Fi a se conectar
const char* PASSWORD = "akivcnentra"; // Senha Wi-Fi a se conectar
 
const char* BROKER_MQTT = "192.168.15.103"; //URL do broker MQTT 
int BROKER_PORT = 1883; // Porta do Broker MQTT
   
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
 
/* Prototypes */
void initializeWiFi(void);
void initializeMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void checkWiFIEMQTT(void);
 

 
/* Função: inicializa e conecta-se na rede WI-FI desejada
*  Parâmetros: nenhum
*  Retorno: nenhum
*/
void initializeWiFi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se á: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
      
    reconnectWiFi();
}
 
/* Função: Inicia a conexão MQTT
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void initializeMQTT(void) 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //define os parametros de conexão
    MQTT.setCallback(mqtt_callback);            //define o callback
}
 

/* Função: Callback de conexão MQTT
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
  
    /* obtem a string do payload recebido */
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
 
    Serial.print("Mensagem receibda: ");
    Serial.println(msg);
    
}
 
/* Função: reconecta-se ao broker MQTT.
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void reconnectMQTT(void) 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar");
            Serial.println("Tentando novamente em 2s");
            delay(2000);
        }
    }
}
 
/* Função: verifica as conexões Wi-Fi e MQTT
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se broker não está conectado, tenta conectar
      
     reconnectWiFi(); //reconecta a rede Wi-Fi
}
 
/* Função: reconecta-se ao WiFi
 * Parâmetros: nenhum
 * Retorno: nenhum
 */
void reconnectWiFi(void) 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
          
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
      
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
  
/* Função de setup */
void setup() 
{
    Serial.begin(115200);  


    /* Inicializa a conexao wi-fi */
    initializeWiFi();
 
    /* Inicializa a conexao ao broker MQTT */
    initializeMQTT();
}
 
/* Loop principal */
void loop() 
{
     
    /* garante funcionamento das conexões WiFi e ao broker MQTT */
    VerificaConexoesWiFIEMQTT();

    MQTT.loop();

    if(MQTT.connected())
    {
        MQTT.publish(TOPICO_PUBLISH,"Aqui eh o ESP32");
    }
 
    /* Refaz o ciclo após 2 segundos */
    delay(2000);
}