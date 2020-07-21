/*******************************************************************
 *  this is a basic example how to program a Telegram Bot          *
 *  using TelegramBOT library on ESP8266                           *
 *                                                                 *
 *  Open a conversation with the bot, it will echo your messages   *
 *  https://qsy.by/telegram-bot-na-esp8266/                        *
 *                                                                 * 
 *                                                                 *
 *  written by Pavel V/ Hladkih                                    *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266TelegramBOT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Подключаем пин данных DS18B20 к GPIO 15 на ESP8266
#define ONE_WIRE_BUS 15
WiFiClientSecure client;
// Initialize Wifi connection to the router
char ssid[] = "ZTE-cead1a";              // SSID (name)
char password[] = "f4b8a7ce";            // your network key
int status = 0;


// Initialize Telegram BOT

#define BOTtoken "520178827:AAHPaUTwk5Vfgb38L12345_oikswMZQ_MQ4"  //Токен бота полученного от @BotFather
#define BOTname "Ботаник" // Имя бота
#define BOTusername "BotanikRobot" // Логин бота
TelegramBOT bot(BOTtoken, BOTname, BOTusername);

int Bot_mtbs = 1000; //среднее время между сканированием сообщений
long Bot_lasttime;   //

bool Start = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Thermometer1 = {0x28, 0x42, 0x2B, 0x6C, 0x06, 0x00, 0x00, 0xDE}; //ONE WIRE адрес датчика температуры.

float getTemperature(){
  float temp;
  sensors.setResolution(Thermometer1, 10); // Точность занков после запятой
  sensors.requestTemperatures(); // Команда получения температуры
  //Serial.print("Temperature for the device 1 (index 0) is: ");
  //Serial.println(sensors.getTempCByIndex(0)); 
  temp = sensors.getTempC(Thermometer1);  // Сохраняем значение температуры в переменной
  return temp; // Возвращаем значение температуры функции 
}

/**************************************************
 * Функция обработки команд ботом от пользователя *
 **************************************************/
void Bot_EchoMessages() {
  for (int i = 1; i < bot.message[0][0].toInt() + 1; i++)      {
    bot.sendMessage(bot.message[i][4], bot.message[i][5], "");
    bot.message[i][5]=bot.message[i][5].substring(1,bot.message[i][5].length());
       
    if (bot.message[i][5] == "start") {
      String wellcome = "Wellcome from FlashLedBot, your personal Bot on ESP8266 board";
      bot.sendMessage(bot.message[i][4], wellcome, "");
      Start = true;
    }
  }
  bot.message[0][0] = "";   // All messages have been replied - reset new messages
}

void setup() {
  sensors.begin(); // инициируем работу датчика
  Serial.begin(115200);
  delay(3000);
  
  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, password) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
 
  Serial.println(ip);
  client.setInsecure();
  bot.begin();      // Включаем бота
}



void loop() { 
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    bot.getUpdates(bot.message[0][1]);   // Включаем API и получаем новые сообщения    
    if (getTemperature() < 23){
    Serial.print("ALARM: "); // Проверяем,если температура ниже указанной, тогда шлем сообщение
    bot.sendMessage("216473175", "Котел скоро потухнет. Подкинь дровишек браток!!!", ""); // Здесь вписываем ID чата с ботом
  }    
    Bot_EchoMessages();   // reply to message with Echo
    Bot_lasttime = millis();
  }
}
