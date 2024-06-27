#include "WiFi.h"

/*
  PM2.5 
  
  PM1 are extremely fine particulates with a diameter of fewer than 1 microns.
  PM2.5 (also known as fine particles) have a diameter of less than 2.5 microns.
  PM10 means the particles have a diameter less than 10 microns, or 100 times smaller than a millimeter.
*/

NetworkServer server(80);

uint16_t datapm10_standard,datapm25_standard,datapm100_standard,datapm10_env,datapm25_env,datapm100_env,dataparticles_03um,dataparticles_05um,dataparticles_10um,dataparticles_25um,dataparticles_50um,dataparticles_100um;
long timestarted;
bool isAwake;

// Serial Port connections for PMS5003 Sensor
#define RXD2 16 // To sensor TXD
#define TXD2 17 // To sensor RXD

void setup() {
  // debugging output
  Serial.begin(115200);

  // Set up UART connection to PMS5003
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);

//Init WiFi as Station, start SmartConfig
  WiFi.mode(WIFI_AP_STA);

  int tmp=0;
  Serial.println("Waiting for WiFi");
  while ((WiFi.status() != WL_CONNECTED) && (tmp<10)){
    delay(500);
    Serial.print(".");
    tmp=tmp+1;
  }
  Serial.println("");


  WiFi.beginSmartConfig();

//Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  timestarted=millis();
  isAwake=true;
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

void loop() {
  if (isAwake)
  {
    if (readPMSdata(&Serial1)) {
      // reading data was successful!
      datapm10_standard=data.pm100_standard;
      datapm25_standard=data.pm25_standard;
      datapm100_standard=data.pm100_standard;
      datapm10_env=data.pm100_env;
      datapm25_env=data.pm25_env;
      datapm100_env=data.pm100_env;
      dataparticles_03um=data.particles_03um;
      dataparticles_05um=data.particles_05um;
      dataparticles_10um=data.particles_10um;
      dataparticles_25um=data.particles_25um;
      dataparticles_50um=data.particles_50um;
      dataparticles_100um=data.particles_100um;
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
    }
    else
    {
      timestarted=millis();
      isAwake=true;
    }
  }

  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New WiFi Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<head>");
            client.println("<TITLE>:: Magla Air Quality Sensor ::</TITLE>");
            client.println("<meta http-equiv=\”refresh\” content=\”5\">");
            client.println("</TITLE>");
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
            client.print("</STYLE>");
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
    Serial.println("Client Disconnected.");
  }

/*

  if ((millis()-timestarted) > 60000)    //180000) 
  {
    
    if (!isAwake)
    {
      Serial.println("");
      Serial.println("Wake Up!");
      timestarted=millis();
      isAwake=true;
      uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 }; // wakeup
      Serial1.write(command, sizeof(command));
    }
  } else {
    if (isAwake)
    {
      Serial.println("");
      Serial.println("Sleeping...");
      isAwake=false;
      timestarted=millis();      
      uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 }; // sleep
      Serial1.write(command, sizeof(command));
    }
  }
*/
}

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
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}