// Marília Zeniti Hoshino
//https://www.mariliah.dev/
//https://linktr.ee/mariliah
//https://github.com/mariliahoshino

//https://github.com/mariliahoshino/Monitor-and-test-Internet-connectivity

//Sistema de monitoramento de múltiplas conexões de internet usando ESP32 + W5500, com alarme local e notificações via Telegram.

#include <SPI.h>
#include <EthernetESP32.h>
#include <MacAddress.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Telegram (mantendo UniversalTelegramBot) - vai trafegar via Ethernet
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// ==================== TELEGRAM ====================
// Dica: se esse token vazar, gere outro no @BotFather.
#define BOT_TOKEN "1111122223333:AAASSSSDDDFFFGGGHHH"  //Token do seu bot no telegram /////////
#define CHAT_ID  "123456789"   // ID do chat no telegram

static const uint32_t TG_POLL_MS = 5000; // a cada 5s
uint32_t lastTgPollMs = 0;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ==================== W5500 (3 modulos) ====================
W5500Driver driver;        // CS = 5 (padrao do driver)
W5500Driver driver1(17);   // CS = 17
W5500Driver driver2(16);   // CS = 16

EthernetClass Ethernet1;
EthernetClass Ethernet2;

// ===== OLED =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static const int I2C_SDA = 21;
static const int I2C_SCL = 22;
static const uint8_t OLED_ADDR = 0x3C;

// ====== CONFIG ======
static const uint8_t NUM_LINKS = 3;
static const int CS_PINS[NUM_LINKS] = { 5, 17, 16 };

static const int PIN_SCK  = 18;
static const int PIN_MISO = 19;
static const int PIN_MOSI = 23;

static const uint32_t DHCP_TIMEOUT_MS = 6000;
static const uint32_t TEST_TIMEOUT_MS = 1500;
static const uint32_t INTERVAL_MS     = 1000;   // tempo entre ciclos (ms)

static const int FAIL_THRESHOLD = 2;

// ===== ALARME (12V) =====
static const int PIN_SINALEIRA = 14; // GPIO14
static const int PIN_BUZZER    = 12; // GPIO12 (strapping pin - manter LOW no boot!)
static const int PIN_ACK_BTN   = 27; // GPIO27 (botao para GND)

bool ack = false;

// padrão do beep
static const uint32_t BEEP_ON_MS  = 200;
static const uint32_t BEEP_OFF_MS = 800;
uint32_t lastBeepMs = 0;
bool buzzerState = false;

// Notificacao por mudanca de quais links falharam (bitmask)
// bit0=R1, bit1=R2, bit2=R3
uint8_t lastFailMaskNotified = 0xFF; // força notificar na primeira mudanca real

// ====== ESTADO ======
int failCount[NUM_LINKS] = {0, 0, 0};
bool linkFail[NUM_LINKS] = {false, false, false};

byte macs[NUM_LINKS][6] = {
  {0xDE,0xAD,0xBE,0xEF,0x00,0x01},
  {0xDE,0xAD,0xBE,0xEF,0x00,0x02},
  {0xDE,0xAD,0xBE,0xEF,0x00,0x03}
};

IPAddress DNS1(8,8,8,8);
IPAddress DNS2(1,1,1,1);

// ====== HELPERS ======
void setAllCSHigh() {
  for (uint8_t i = 0; i < NUM_LINKS; i++) digitalWrite(CS_PINS[i], HIGH);
}

String ipToString(const IPAddress &ip) {
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

bool tcpCheck(IPAddress ip, uint16_t port, uint32_t timeoutMs) {
  EthernetClient ethClient;
  ethClient.setTimeout(timeoutMs);
  bool ok = ethClient.connect(ip, port);
  ethClient.stop();
  return ok;
}

bool internetOk() {
  bool a = tcpCheck(DNS1, 53, TEST_TIMEOUT_MS);
  bool b = tcpCheck(DNS2, 53, TEST_TIMEOUT_MS);
  return a || b;
}

void printHeader() {
  Serial.println();
  Serial.println("========================================");
  Serial.println(" Monitor 3 Links - ESP32 + W5500 + OLED  ");
  Serial.println(" Telegram via Ethernet (sem WiFi)        ");
  Serial.println("========================================");
}

void updateDisplay(const char* footerMsg = nullptr) {
  if (!display.width()) return; // se OLED não iniciou, não faz nada

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Monitor de Links");
  display.println();

  for (uint8_t i = 0; i < NUM_LINKS; i++) {
    display.print("R");
    display.print(i + 1);
    display.print(": ");
    display.print(linkFail[i] ? "FALHA" : "OK");
    display.print(" (");
    display.print(failCount[i]);
    display.println(")");
  }

  display.println();
  bool anyFail = linkFail[0] || linkFail[1] || linkFail[2];
  display.print("Status: ");
  if (!anyFail) display.println("NORMAL");
  else if (ack) display.println("ALERTA (ACK)");
  else display.println("ALERTA");

  if (footerMsg) {
    display.println();
    display.println(footerMsg);
  }

  display.display();
}

// ====== Ethernet bring-up / bring-down (para usar em envio do Telegram) ======
bool bringUpLink(uint8_t idx) {
  setAllCSHigh();
  digitalWrite(CS_PINS[idx], LOW);
  delay(50);

  if (idx == 0) Ethernet.init(driver);
  if (idx == 1) Ethernet.init(driver1);
  if (idx == 2) Ethernet.init(driver2);

  int dhcpOk = Ethernet.begin(macs[idx], DHCP_TIMEOUT_MS);
  if (dhcpOk == 0) return false;

  // opcional: log rapido
  Serial.printf("[R%d] IP: %s\n", idx + 1, ipToString(Ethernet.localIP()).c_str());
  return true;
}

void bringDownLink(uint8_t idx) {
  Ethernet.end();
  Ethernet1.end();
  Ethernet2.end();
  digitalWrite(CS_PINS[idx], HIGH);
}
////////////////////////////////////////////////////////////
void checkTelegramCommandsViaAnyLink() {
  // só tenta no intervalo definido
  if (millis() - lastTgPollMs < TG_POLL_MS) return;
  lastTgPollMs = millis();

  // Se todos falharam, não adianta tentar
  bool anyOk = false;
  for (int i = 0; i < NUM_LINKS; i++) if (!linkFail[i]) { anyOk = true; break; }
  if (!anyOk) return;

  // Sobe uma interface disponível (primeiro link OK)
  int chosen = -1;
  for (int i = 0; i < NUM_LINKS; i++) {
    if (!linkFail[i]) { chosen = i; break; }
  }
  if (chosen < 0) return;

  if (!bringUpLink(chosen)) {
    bringDownLink(chosen);
    return;
  }

  // garante TLS ok
  client.stop();
  client.setInsecure();

  // busca mensagens
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      if (text == "/status") {
        String msg = getStatusMessage();
        bot.sendMessage(chat_id, msg, "");
      }
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  bringDownLink(chosen);
}
///////////////////////////////////////////////////////

bool sendTelegramViaAnyLink(const String& text) {
  // 1a passada: tenta só links marcados como OK
  // 2a passada: tenta todos
  for (uint8_t pass = 0; pass < 2; pass++) {
    for (uint8_t i = 0; i < NUM_LINKS; i++) {
      if (pass == 0 && linkFail[i]) continue;

      Serial.printf("[TG] Tentando enviar via R%d...\n", i + 1);

      if (!bringUpLink(i)) {
        Serial.printf("[TG] R%d: DHCP falhou\n", i + 1);
        bringDownLink(i);
        continue;
      }

      if (!internetOk()) {
        Serial.printf("[TG] R%d: sem internet (teste TCP falhou)\n", i + 1);
        bringDownLink(i);
        continue;
      }

      bool ok = bot.sendMessage(CHAT_ID, text, "");
      Serial.printf("[TG] R%d: %s\n", i + 1, ok ? "ENVIADO ✅" : "FALHOU ❌");

      bringDownLink(i);
      if (ok) return true;
    }
  }

  Serial.println("[TG] Nao foi possivel enviar por nenhum link.");
  return false;
}

void testLink(uint8_t idx) {
  checkAckButton();
  Serial.println();
  Serial.printf("[R%d] Selecionando W5500 (CS=%d)\n", idx + 1, CS_PINS[idx]);

  setAllCSHigh();
  digitalWrite(CS_PINS[idx], LOW);
  delay(50);

  if (idx == 0) Ethernet.init(driver);
  if (idx == 1) Ethernet.init(driver1);
  if (idx == 2) Ethernet.init(driver2);

  Serial.printf("[R%d] DHCP...\n", idx + 1);

  int dhcpOk = Ethernet.begin(macs[idx], DHCP_TIMEOUT_MS);

  if (dhcpOk == 0) {
    failCount[idx]++;
    linkFail[idx] = (failCount[idx] >= FAIL_THRESHOLD);
    Serial.printf("[R%d] FALHA DHCP (failCount=%d)\n", idx + 1, failCount[idx]);

    Ethernet.end();
    Ethernet1.end();
    Ethernet2.end();
    digitalWrite(CS_PINS[idx], HIGH);
    return;
  }

  IPAddress ip = Ethernet.localIP();
  IPAddress gw = Ethernet.gatewayIP();
  IPAddress dns = Ethernet.dnsServerIP();
  Serial.printf("[R%d] IP:  %s\n", idx + 1, ipToString(ip).c_str());
  Serial.printf("[R%d] GW:  %s\n", idx + 1, ipToString(gw).c_str());
  Serial.printf("[R%d] DNS: %s\n", idx + 1, ipToString(dns).c_str());

  Serial.printf("[R%d] Teste Internet (TCP 53 em 8.8.8.8/1.1.1.1)...\n", idx + 1);
  bool ok = internetOk();

  if (ok) {
    failCount[idx] = 0;
    linkFail[idx] = false;
    Serial.printf("[R%d] OK ✅\n", idx + 1);
  } else {
    failCount[idx]++;
    linkFail[idx] = (failCount[idx] >= FAIL_THRESHOLD);
    Serial.printf("[R%d] FALHA Internet (failCount=%d)\n", idx + 1, failCount[idx]);
  }

  Ethernet.end();
  digitalWrite(CS_PINS[idx], HIGH);
}

void printSummary() {
  Serial.println();
  Serial.println("------ RESUMO ------");
  for (uint8_t i = 0; i < NUM_LINKS; i++) {
    Serial.printf("R%d: %s (failCount=%d)\n",
                  i + 1,
                  linkFail[i] ? "FALHA" : "OK",
                  failCount[i]);
  }
  Serial.println("--------------------");
}

String getStatusMessage() {
  String msg = "Status atual dos links:\n\n";

  for (int i = 0; i < NUM_LINKS; i++) {
    msg += "R" + String(i+1) + ": ";
    msg += linkFail[i] ? "FALHA\n" : "OK\n";
  }

  return msg;
}
////////////////////////////// novo 2026/02/08

void checkAckButton() {
  static uint32_t lastChange = 0;
  static bool lastReading = true; // pullup => solto = HIGH
  static bool stableState = true;

  bool reading = digitalRead(PIN_ACK_BTN);

  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }

  // debounce ~50ms
  if ((millis() - lastChange) > 50 && reading != stableState) {
    stableState = reading;

    // detecta pressionado (LOW)
    if (stableState == false) {
      ack = true;
      Serial.println("[ACK] Alarme reconhecido: buzzer silenciado.");
    }
  }
}

void notifyTelegram(bool alarm) {
  String msg;

  if (alarm) {
    msg = "🚨 ALERTA: Falha em link da loja!\n";
  } else {
    msg = "✅ OK: Todos os links normalizados.\n";
  }

  for (int i = 0; i < 3; i++) {
    msg += "R";
    msg += String(i + 1);
    msg += ": ";
    msg += linkFail[i] ? "FALHA\n" : "OK\n";
  }

  sendTelegramViaAnyLink(msg);
}

uint8_t getFailMask() {
  uint8_t m = 0;
  for (uint8_t i = 0; i < NUM_LINKS; i++) {
    if (linkFail[i]) m |= (1 << i);
  }
  return m;
}

// Envia status detalhado (R1/R2/R3) no Telegram usando a porta Ethernet disponivel
// Retorna true se conseguiu enviar
bool notifyTelegramMask(uint8_t mask) {
  String msg;

  if (mask == 0) {
    msg = "✅ OK: Todos os links normalizados.\n";
  } else {
    msg = "🚨 ALERTA: Mudanca no status dos links.\n";
  }

  for (uint8_t i = 0; i < NUM_LINKS; i++) {
    msg += "R" + String(i + 1) + ": " + ((mask & (1 << i)) ? "FALHA" : "OK") + "\n";
  }

  // Ajuda a evitar reaproveitar uma conexao TLS antiga
  client.stop();
  delay(50);

  return sendTelegramViaAnyLink(msg);
}

void alarmControl() {
  bool anyFail = linkFail[0] || linkFail[1] || linkFail[2];

  if (!anyFail) {
    // voltou ao normal: reseta ack e desliga tudo
    ack = false;
    digitalWrite(PIN_SINALEIRA, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    buzzerState = false;
    return;
  }

  // tem falha: sinaleira ligada
  digitalWrite(PIN_SINALEIRA, HIGH);

  if (ack) {
    // reconhecido: buzzer desligado
    digitalWrite(PIN_BUZZER, LOW);
    buzzerState = false;
    return;
  }else{
    buzzerState = true;
  }

  digitalWrite(PIN_BUZZER,  buzzerState);

  // não reconhecido: buzzer pulsante
  /*
  uint32_t now = millis();
  uint32_t interval = buzzerState ? BEEP_ON_MS : BEEP_OFF_MS;

  if (now - lastBeepMs >= interval) {
    buzzerState = !buzzerState;
    digitalWrite(PIN_BUZZER, buzzerState ? HIGH : LOW);
    lastBeepMs = now;
  }
  */
}

void checkTelegramCommands() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      Serial.print("[TG CMD] Recebido: ");
      Serial.println(text);

      if (text == "/status") {
        String msg = getStatusMessage();
        sendTelegramViaAnyLink(msg);
      }
    }

    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}
/////////////////////////////// novo 2026/02/08



void setup() {
  // Saidas: setar LOW antes de virar OUTPUT (ajuda no boot do GPIO12)
  digitalWrite(PIN_SINALEIRA, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_SINALEIRA, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  pinMode(PIN_ACK_BTN, INPUT_PULLUP);

  Serial.begin(115200);
  delay(500);

  // TLS: sem validar certificado (mais simples)
  client.setInsecure();

  for (uint8_t i = 0; i < NUM_LINKS; i++) {
    pinMode(CS_PINS[i], OUTPUT);
    digitalWrite(CS_PINS[i], HIGH);
  }

  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);

  // OLED
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("ERRO: OLED SSD1306 nao encontrado (0x3C).");
  } else {
    updateDisplay("Iniciando...");
  }

  printHeader();
}

void loop() {
  uint32_t start = millis();
  Serial.printf("\n[CICLO] inicio em %lu ms\n", (unsigned long)start);

  for (uint8_t i = 0; i < NUM_LINKS; i++) {
    testLink(i);

    checkAckButton();
    alarmControl();
    checkTelegramCommands();   // novo 2026/02/08

    updateDisplay("Testando links...");
    delay(200);
  }

  printSummary();
  updateDisplay("Ciclo completo");

  // ===== Telegram: notificar a cada mudanca de quais links falharam =====
  // Ex.: cai R1 -> envia; cai R2 -> envia; cai R3 (tudo offline) -> nao tem como enviar
  uint8_t mask = getFailMask();

  // Se tudo caiu, nao adianta tentar enviar (vai falhar e atrasar o ciclo).
  // Ainda assim, atualizamos o lastFailMaskNotified para evitar tentar toda hora.
  if (mask == 0b111) {
    lastFailMaskNotified = mask;
  } else if (mask != lastFailMaskNotified) {
    bool sent = notifyTelegramMask(mask);
    if (sent) {
      lastFailMaskNotified = mask;
    }
  }

  // Espera ativa: mantém alarme e lê botão durante INTERVAL_MS
  uint32_t t0 = millis();
  while (millis() - t0 < INTERVAL_MS) {
    checkAckButton();
    alarmControl();
    //checkTelegramCommands();  // <- novo   status
    checkTelegramCommandsViaAnyLink();
    delay(10);
  }

  uint32_t elapsed = millis() - start;
  Serial.printf("[CICLO] fim (duracao=%lu ms).\n", (unsigned long)elapsed);
}
