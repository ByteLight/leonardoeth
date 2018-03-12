
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <EthernetUdp2.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <PubSubClient.h>


//For W5500 Ethernet
#define _STR(X)  #X
#define STR(X)  _STR(X)
#define SS     10U    //D10----- SS
#define RST    11U    //D11----- Reset

//Relay state
const int relay = 6;       //  the number of the Relay pin
int relayState = LOW;     // ledState used to set the LED

//Interrupt for light sensor
const int interruptPin = SDA;
const int powerPin = 12;
int intrruptFlag = 0;
byte testByte = 0x55;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:

#define NODENUMBER 250                      //last 3 digit of IP address

#define CLIENTID "client" STR(NODENUMBER)
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, NODENUMBER};
unsigned int localPort = 8888;              // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send backM

IPAddress ip(192, 168, 1, NODENUMBER);      //W5500 client address
IPAddress server(192, 168, 1, 77);          //MQTT Server address
IPAddress dns1(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

EthernetUDP Udp;    //UDP object


void callback(char* topic, byte* payload, unsigned int length) 
{
  // b for bytelight, c for casambi
  if (strcmp(topic,"firmware") == 0 && payload[0] == 98) {
     mqttClient.publish("bytelight", STR(NODENUMBER));
    digitalWrite(relay, LOW);   
    Serial.println(F("bytelight"));
  } 
    
  else if (strcmp(topic,"firmware") == 0 && payload[0] == 99) {
     mqttClient.publish("casambi", STR(NODENUMBER));
    digitalWrite(relay, HIGH);   
    Serial.println(F("casambi"));
  } 
  
}


void setup() {
  
  //Serial port setup
  Serial.begin(115200);
  Serial.println("USB Serial OK");

  Serial1.begin(9600);
  Serial.println("Serial Pin1 OK");

  //W5500 Ethernet setup
  pinMode(SS, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(SS, LOW);
  digitalWrite(RST,HIGH);  //Reset Ethernet
  digitalWrite(RST,LOW); 
  digitalWrite(RST,HIGH);  
  
  //Ethernet UDP steup:
  Ethernet.begin(mac, ip, dns1, subnet);
  delay(1500);
  Udp.begin(localPort);
  Serial.println(ip);

  //MQTT client setup
  mqttClient.setServer(server,1883);
  mqttClient.setCallback(callback);
  Serial.println(F("Setup Complete1"));

  //Light interrupt pin setup
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin,HIGH);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pin_ISR, HIGH);
 
  Serial.println(F("Interrupt setup complete"));


}

void pin_ISR() {
  intrruptFlag = 1;
}

void reconnect() 
{
  while (!mqttClient.connected()) {
    Serial.print(F(" Attempting MQTT connection..."));
    
    if (mqttClient.connect(CLIENTID)) {
      Serial.println(F("connected"));
      mqttClient.publish("outTopic","Connected!");
      // ... and resubscribe
      mqttClient.subscribe("firmware");
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(F(" try again in 2~3 seconds"));
      // Wait 2 seconds before retrying
      delay(random(2000,3000));
    }
    
  }
}

void loop() {
  
  
  if (!mqttClient.connected()) {
    reconnect();
  }
  
  if (1 == intrruptFlag) {
    intrruptFlag = 0;
    mqttClient.publish("lightOn", STR(NODENUMBER));
    //attachInterrupt(digitalPinToInterrupt(interruptPin), pin_ISR, LOW);
    Serial.println(F("I see light!"));
    delay(1000);
    //Serial1.print(testByte);
  }
  
  mqttClient.loop();  
  
}

  /*
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
  delay(10);
  */


/*
  Processing sketch to run with this example
 =====================================================

 // Processing UDP example to send and receive string data from Arduino
 // press any key to send the "Hello Arduino" message


 import hypermedia.net.*;

 UDP udp;  // define the UDP object


 void setup() {
 udp = new UDP( this, 6000 );  // create a new datagram connection on port 6000
 //udp.log( true ); 		// <-- printout the connection activity
 udp.listen( true );           // and wait for incoming message
 }

 void draw()
 {
 }

 void keyPressed() {
 String ip       = "192.168.1.177";	// the remote IP address
 int port        = 8888;		// the destination port

 udp.send("Hello World", ip, port );   // the message to send

 }

 void receive( byte[] data ) { 			// <-- default handler
 //void receive( byte[] data, String ip, int port ) {	// <-- extended handler

 for(int i=0; i < data.length; i++)
 print(char(data[i]));
 println();
 }
 */


