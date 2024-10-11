#include <Arduino.h>
#include <Bounce2.h>
#include <Preferences.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>

#define BUTTON_PIN 0
#define tempoBotaoPressionado 5000
#define MAX_ENTRIES 10 // Definindo o número máximo de entradas

// Configurações do NTP
const char* ssid = "YOUR_SSID"; // Substitua pelo seu SSID
const char* password = "YOUR_PASSWORD"; // Substitua pela sua senha
const long utcOffsetInSeconds = -3 * 3600; // UTC-3 para Brasil (ex: BRT)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

Bounce botao = Bounce(); // Cria o objeto botao
Preferences preferences; // Cria o objeto Preferences

void incrementaContador();
void reset();

int contador = 0; // Variável para armazenar o contador
unsigned long timestamps[MAX_ENTRIES]; // Array para armazenar os timestamps
String formattedDates[MAX_ENTRIES]; // Array para armazenar as datas formatadas
int currentIndex = 0; // Índice atual para armazenar os timestamps

void setup() 
{
  Serial.begin(9600); // Inicia a comunicação serial
  WiFi.begin(ssid, password); // Conecta ao WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");

  timeClient.begin(); // Inicia o cliente NTP
  timeClient.setTimeOffset(utcOffsetInSeconds); // Define o deslocamento de tempo


  botao.attach(BUTTON_PIN, INPUT_PULLUP); // Configura o pino do botão como INPUT_PULLUP
  Serial.println("Contagem iniciada");
  preferences.begin("config", false); // Acesso não somente leitura

  // Recupera o valor armazenado na memória
  contador = preferences.getInt("contador", 0); 
  currentIndex = preferences.getInt("currentIndex", 0); 

  // Recupera os timestamps armazenados
  for (int i = 0; i < MAX_ENTRIES; i++) 
  {
    String key = "timestamp_" + String(i); // Cria a chave como string
    timestamps[i] = preferences.getUInt(key.c_str(), 0); // Usa c_str() para a chave
    formattedDates[i] = preferences.getString(("date_" + String(i)),c_str(), ""); // Recupera a data formatada
  }

  Serial.printf("Contagem inicia em: %d \n\r", contador); // Exibe o contador no monitor serial
}


void loop()
{
  timeClient.update(); // Atualiza o horário do NTP
  botao.update(); // Atualiza o estado do botão

  if (botao.fell()) // Se o botão foi solto
  {
    // if (botao.previousDuration() < tempoBotaoPressionado) {
    //   reset(); // Zera o contador
    //   Serial.println("Contador resetado"); // Mostra mensagem de reset

    // } else {
      incrementaContador(); // Incrementa o contador

      // Armazena o timestamp atual
      timestamps[currentIndex] = millis(); // Armazena o tempo atual (em milissegundos)
      currentIndex = (currentIndex + 1) % MAX_ENTRIES; // Atualiza o índice e faz um loop
 
      // Armazena a data formatada
       String formattedDate = timeClient.getFormattedTime();
      formattedDates[currentIndex] = formattedDate; // Armazena a data formatada

      // Armazena os timestamps na memória

      for (int i = 0; i < MAX_ENTRIES; i++) {
        String key = "timestamp_" + String(i); // Cria a chave como string
        preferences.putUInt(key.c_str(), timestamps[i]); // Armazena cada timestamp

        String dateKey = "date_" + String(i); // Cria a chave para a data
        preferences.putString(dateKey.c_str(), formattedDates[i]); // Armazena a data formatada
        }
      
      preferences.putInt("contador", contador); // Armazena o valor do contador na memoria
      preferences.putInt("currentIndex", currentIndex); // Armazena o índice atual

      Serial.printf("Contagem: %d \n\r", contador); // Exibe o contador no monitor serial
      Serial.printf("Timestamp armazenado: %lu\n\r", timestamps[currentIndex]); // Exibe o timestamp armazenado
      Serial.printf("Data armazenada: %s\n\r", formattedDates[currentIndex].c_str()); // Exibe a data formatada
      Serial.printf("Últimos timestamps: ");
      for (int i = 0; i < MAX_ENTRIES; i++) {
        Serial.printf("Entry %d: %lu (%s) ", i, timestamps[i], formattedDates[i].c_str()); // Identifica cada entrada
//        Serial.printf("%lu ", timestamps[i]);
      }
      Serial.println();
     }
}

  void incrementaContador()
{
  contador++; // Incrementa o contador
   preferences.putInt("contador", contador); // Armazena o contador após o incremento
}

void reset()
{
  contador = 0; // Zera o contador
}


