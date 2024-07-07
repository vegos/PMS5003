#include "WiFi.h"
#include "HTTPClient.h"

/*
  PMS5003 
  
  PM1 are extremely fine particulates with a diameter of fewer than 1 microns.
  PM2.5 (also known as fine particles) have a diameter of less than 2.5 microns.
  PM10 means the particles have a diameter less than 10 microns, or 100 times smaller than a millimeter.

  ©2024, Antonis Maglaras (maglaras@gmail.com)
*/

NetworkServer server(80);

uint16_t datapm10_standard,datapm25_standard,datapm100_standard,datapm10_env,datapm25_env,datapm100_env,dataparticles_03um,dataparticles_05um,dataparticles_10um,dataparticles_25um,dataparticles_50um,dataparticles_100um;
long timestarted,timeforpost;
bool isAwake,SleepSend;
int analogVolts, batteryVolts;

// Serial Port connections for PMS5003 Sensor
#define RXD1 16 // To sensor TXD
#define TXD1 17 // To sensor RXD

#define DEBUG true

#define SleepTime 60000

#define EveryXTime   300000

//----------------------------------------------------------------------------------------------------------

void Sleep()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
  Serial1.write(command, sizeof(command));
  isAwake = false;
}

//----------------------------------------------------------------------------------------------------------

void WakeUp()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
  Serial1.write(command, sizeof(command));
  isAwake = true;
}

//----------------------------------------------------------------------------------------------------------

void ActiveMode()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71 };
  Serial1.write(command, sizeof(command));
}

//----------------------------------------------------------------------------------------------------------

void PassiveMode()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
  Serial1.write(command, sizeof(command));
}

//----------------------------------------------------------------------------------------------------------

void SmartConfig()
{
  WiFi.mode(WIFI_STA);
  if (DEBUG) Serial.println("\r\nWait for Smartconfig...");
  WiFi.beginSmartConfig();
  while (1)
  {
#ifdef DEBUG 
    Serial.print(".");
#endif    
    delay(500);                   // wait for a second
    if (WiFi.smartConfigDone())
    {
#ifdef DEBUG       
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
#endif      
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------

bool AutoConfig()
{
    WiFi.begin();
    for (int i = 0; i < 20; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
#ifdef DEBUG           
            Serial.println("WIFI SmartConfig Success");
            Serial.printf("SSID:%s", WiFi.SSID().c_str());
            Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
            Serial.print("LocalIP:");
            Serial.print(WiFi.localIP());
            Serial.print(" ,GateIP:");
            Serial.println(WiFi.gatewayIP());
#endif            
            return true;
        }
        else
        {
#ifdef DEBUG           
            Serial.print("WIFI AutoConfig Waiting......");
            Serial.println(wstatus);
#endif
            delay(1000);            
        }
    }
#ifdef DEBUG     
    Serial.println("WIFI AutoConfig Failed!" );
#endif    
    return false;
}

//----------------------------------------------------------------------------------------------------------

void setup() {
  // debugging output
  Serial.begin(115200);

  // Set up UART connection to PMS5003
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  
  // Set the resolution to 12 bits (0-4096)
  analogReadResolution(12);

  // Autoconfig WiFi etc
  if (!AutoConfig())
  {
      SmartConfig();
  }

  server.begin();
  timestarted = millis() + SleepTime;
  timeforpost = millis();
  isAwake = true;
  SleepSend = false;

  WakeUp();
  delay(1000);
  ActiveMode();
  delay(1000);
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

//----------------------------------------------------------------------------------------------------------

void loop() {
  if (readPMSdata(&Serial1)) 
  {
    // We have a reading
    if (!((data.pm10_standard == 0) && (data.pm25_standard == 0) && (data.pm100_standard == 0) 
    && (data.pm10_env == 0) && (data.pm25_env == 0) && (data.pm100_env == 0)
    && (data.particles_03um == 0) && (data.particles_05um == 0) && (data.particles_10um == 0) 
    && (data.particles_25um == 0) && (data.particles_50um == 0) && (data.particles_100um == 0)))
    {
      datapm10_standard = data.pm10_standard;
      datapm25_standard = data.pm25_standard;
      datapm100_standard = data.pm100_standard;
      datapm10_env = data.pm10_env;
      datapm25_env = data.pm25_env;
      datapm100_env = data.pm100_env;
      dataparticles_03um = data.particles_03um;
      dataparticles_05um = data.particles_05um;
      dataparticles_10um = data.particles_10um;
      dataparticles_25um = data.particles_25um;
      dataparticles_50um = data.particles_50um;
      dataparticles_100um = data.particles_100um;
      analogVolts = analogReadMilliVolts(0);
      batteryVolts = analogVolts*2;
    }
  }

  if ((isAwake) && ((millis() % 10000) == 0))
  {
#ifdef DEBUG       
    Serial.println();
    Serial.println("----------------------------------------------------------------");
    Serial.println("Concentration Units (standard)");
    Serial.print("PM 1.0: "); Serial.print(datapm10_standard);
    Serial.print("\t\tPM 2.5: "); Serial.print(datapm25_standard);
    Serial.print("\t\tPM 10: "); Serial.println(datapm100_standard);
    Serial.println("----------------------------------------------------------------");
    Serial.println("Concentration Units (environmental)");
    Serial.print("PM 1.0: "); Serial.print(datapm10_env);
    Serial.print("\t\tPM 2.5: "); Serial.print(datapm25_env);
    Serial.print("\t\tPM 10: "); Serial.println(datapm100_env);
    Serial.println("----------------------------------------------------------------");
    Serial.print("Particles > 0.3um / 0.1L air: "); Serial.println(dataparticles_03um);
    Serial.print("Particles > 0.5um / 0.1L air: "); Serial.println(dataparticles_05um);
    Serial.print("Particles > 1.0um / 0.1L air: "); Serial.println(dataparticles_10um);
    Serial.print("Particles > 2.5um / 0.1L air: "); Serial.println(dataparticles_25um);
    Serial.print("Particles > 5.0um / 0.1L air: "); Serial.println(dataparticles_50um);
    Serial.print("Particles > 10.0um / 0.1L air: "); Serial.println(dataparticles_100um);
    Serial.println("----------------------------------------------------------------");
    Serial.print("Battery voltage: "); Serial.print(batteryVolts); Serial.println(" mV");
#endif      
  }

  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
#ifdef DEBUG   
    Serial.println("New WiFi Client.");  // print a message out the serial port
#endif    
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
#ifdef DEBUG         
        Serial.write(c);            // print it out the serial monitor
#endif        
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<HTML>");
            client.println("<HEAD>");
            client.println("<meta http-equiv=\"refresh\" content=\"180\">");
            client.println("<TITLE>:: Magla Air Quality Sensor ::</TITLE>");
            client.print("<STYLE TYPE=\"text/css\">");
            client.print("html {");
            client.print("font-size: 16 px;");
            client.print("  font-family: Helvetica;");
            client.print("  background-color: #FFFFFF;");
            client.print("  color: #404040;");
            client.print("}");
            client.print("body {");
            client.print("  margin: 0 auto;");
            client.print("padding: 0;");
            client.print("  padding-top: 20px;");
            client.print("  font-size: 16 px;");
            client.print("  font-family: Helvetica;");
            client.print("  color: #404040;");
            client.print("  vertical-align: baseline;");
            client.print("}");
            client.println("</STYLE>");
            client.println("</HEAD>");
            client.println("<CENTER>");
            client.println("<b><font color=\"blue\">Magla Air Quality Monitor</font></b>");
            client.println("<BR><BR>");
            client.println("<b><font color=\"blue\">Concentration Units (standard)</font></b>");
            client.println("<BR>");
            client.print("PM 1.0: <font color=\"red\">"); client.print(datapm10_standard);
            client.println("</font><BR>");
            client.print("PM 2.5: <font color=\"red\">"); client.print(datapm25_standard);
            client.println("</font><BR>");
            client.print("PM 10: <font color=\"red\">"); client.print(datapm100_standard);
            client.println("</font><BR>");
            client.println("<BR>");
            client.println("<b><font color=\"blue\">Concentration Units (environmental)</font></b>");
            client.println("<BR>");
            client.print("PM 1.0: <font color=\"red\">"); client.print(datapm10_env);
            client.println("</font><BR>");
            client.print("PM 2.5: <font color=\"red\">"); client.print(datapm25_env);
            client.println("</font><BR>");
            client.print("PM 10: <font color=\"red\">"); client.print(datapm100_env);
            client.println("</font><BR>");
            client.println("<BR>");
            client.print("<b><font color=\"blue\">Particles</font></b>");
            client.println("<BR>");
            client.print("Particles > 0.3um / 0.1L air: <font color=\"red\">"); client.print(dataparticles_03um);
            client.println("</font><BR>");
            client.print("Particles > 0.5um / 0.1L air: <font color=\"red\">"); client.print(dataparticles_05um);
            client.println("</font><BR>");
            client.print("Particles > 1.0um / 0.1L air: <font color=\"red\">"); client.print(dataparticles_10um);
            client.println("</font><BR>");
            client.print("Particles > 2.5um / 0.1L air: <font color=\"red\">"); client.print(dataparticles_25um);
            client.println("</font><BR>");
            client.print("Particles > 5.0um / 0.1L air: <font color=\"red\">"); client.print(dataparticles_50um);
            client.println("</font><BR>");
            client.print("Particles > 10.0um / 0.1L air: <font color=\"red\">"); client.print(dataparticles_100um);
            client.println("</font><BR>");
            client.println("<BR>");
            client.print("<b><font size=-1><font color=\"blue\">Battery</font></b>");
            client.println("<BR>");
            client.print("Battery voltage: <font color=\"blue\">"); client.print(batteryVolts);
            client.println("</FONT>mV<BR>");
            client.println("</FONT></FONT>");
            client.println("<BR>");
            client.println("</CENTER>");
            client.println("<BR>");
            client.println("</HTML>");
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
#ifdef DEBUG     
    Serial.println("Client Disconnected.");
#endif    
  }

  if ((millis() - timeforpost) > EveryXTime)
  {
    if (PostToServer())
    {
#ifdef DEBUG
      Serial.println("Post to server!");
#endif
    } else {
#ifdef DEBUG      
      Serial.println("Post failed!");
#endif      
    }
  }
  
/* 
// -- need some finetuning -- not ready yet
if ((millis() - timestarted) > SleepTime)
{
  WakeUp();
  ActiveMode();
  timestarted = millis();
  isAwake = true;
  SleepSend = false;
#ifdef DEBUG
  Serial.println("Wake Up!");
#endif
} else {
  if ((!SleepSend) && ((millis() - timestarted) > 20000))
  {
    Sleep();
    timestarted = millis();
    SleepSend = true;    
#ifdef DEBUG
    Serial.println("Sleep!");
#endif    
  }
  else
  {
    // DO NOTHING JUST WAIT
  }
}
*/
}

//----------------------------------------------------------------------------------------------------------

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  /* debugging
    for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
    }
    Serial.println();
  */

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
#ifdef DEBUG     
    Serial.println("Checksum failure");
#endif    
    return false;
  }
  // success!
  return true;
}

//----------------------------------------------------------------------------------------------------------

boolean PostToServer()
{
  const String serverName = "http://192.168.100.5/post-sensor-data.php";
  const String apiKeyValue = "58619089-8dcd-444d-b8aa-2803a300471c";

  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;

    http.begin(serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&pm10_standard=" + datapm10_standard + "&pm25_standard=" + datapm25_standard +
                             "&pm100_standard=" + datapm100_standard + "&pm10_env=" + datapm10_env + "&pm25_env=" + datapm25_env + "&pm100_env=" + datapm100_env +
                             "&particles_03um=" + dataparticles_03um + "&particles_05um=" + dataparticles_05um + "&particles_10um=" + dataparticles_10um +
                             "&particles_25um=" + dataparticles_25um + "&particles_50um=" + dataparticles_50um + "&particles_100um=" + dataparticles_100um +
                             "&batteryvolts=" + batteryVolts;

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
        
    if (httpResponseCode>0) {
#ifdef DEBUG      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
#endif      
      http.end();
      timeforpost = millis();
      return true;
    }
    else {
#ifdef DEBUG      
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
#endif      
      http.end();
      timeforpost = millis();
      return false;
    }    
  }
}
