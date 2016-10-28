
#include <ESP8266WiFi.h>

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "sparkfun";

/////////////////////
// Pin Definitions //
/////////////////////
const int OPEN = 14; // Открытие
const int CLOSE = 12; // Закрытие
const int GERKON = 13; // Digital pin to be read
  int closed;// Флаги закрытия/открытия. 1 - истина.
int opened;//

WiFiServer server(80);

void setup() 
{
  closed = 1;
  opened = 1;
  initHardware();
  setupWiFi();
  server.begin();

}

void loop() 
{
  String s = " ";
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  if (req.indexOf("/gpio/0") != -1 and closed == 1 ) // открываем штору  and openclosed == 0
    {
    digitalWrite(OPEN, 0);                                // включаем реле открытия
    delay(3000);                                       // Расчётное время открытия штор
    digitalWrite(OPEN, 1);                                // выключаем реле открытия
    opened = 1;
    closed = 0;
    client.flush();}
  else if (req.indexOf("/gpio/1") != -1 and opened == 1 ) // закрываем штору  and openclosed == 1
    {                                     
    digitalWrite(CLOSE, 0);     
    delay(3000);                                       // Расчётное время открытия штор
    digitalWrite(CLOSE, 1);                                
    opened = 0;
    closed = 1;
    client.flush();}
  else if (req.indexOf("/main") != -1){
  // Prepare the response. Start with the common header:
  s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "Состояние штор: ";
    if (closed){
       s += "Шторы закрыты";
     }
    else if(opened){
       s += "Шторы открыты";
     }
  }
  else
  {
    s += "Неправильный запрос!<br> Открыть шторы /gpio/0 <br> Закрыть шторы /gpio/1";
      client.stop();
    return;
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
 client.flush();
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}


void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "Shtori " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(GERKON, INPUT);
  pinMode(OPEN, OUTPUT);
  pinMode(CLOSE, OUTPUT);
  digitalWrite(OPEN, 1);
  digitalWrite(CLOSE, 1);
  digitalWrite(GERKON, HIGH);
  int openclosed = digitalRead(GERKON); // проверяем открыты или закрыты шторы. Если геркон разомкнут - выводится "1", если замкнут - выводится "0".

  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}
