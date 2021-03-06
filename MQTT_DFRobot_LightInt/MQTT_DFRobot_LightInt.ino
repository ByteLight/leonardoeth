

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <EthernetUdp2.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <PubSubClient.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#define NODENUMBER 247              //last 3 digit of IP address
#define CLIENTID "client" STR(NODENUMBER)
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, NODENUMBER};

//For W5500 Ethernet
#define _STR(X)  #X
#define STR(X)  _STR(X)
#define SS     10U    //D10----- SS
#define RST    11U    //D11----- Reset

//Relay state
const int relay1 = 6;       //  the number of the Relay pin
const int relay2 = 5;
const int relay3 = 4;
const int relay4 = 3;

int relayState = LOW;     // ledState used to set the LED

//Interrupt for light sensor
const int interruptPin = SDA; // high when no light
const int powerPin = 12;
int intrruptFlag = 0;
byte testByte = 0x55;


unsigned int localPort = 8888;              // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send backM

IPAddress ip(192, 168, 1, NODENUMBER);      //W5500 client address
IPAddress server(192, 168, 1, 66);          //MQTT Server address
IPAddress dns1(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

EthernetUDP Udp;    //UDP object

//'n' for normal, 'c' for closed (powered)
//payload[0] is nth relay [1] for closed or normal.
//ex. publishing n1 to relay sets relay1 to normal.

void callback(char* topic, byte* payload, unsigned int length) 
{
    Serial.println(payload[1]);
  //Relay 1 
  if (strcmp(topic,"relay") == 0 && payload [0] == '1' ) {
    if (payload[1] == 'n') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay1, LOW);   
    }

    else if (payload[1] == 'c') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay1, HIGH);
    }
  }
  //Relay 2
  if (strcmp(topic,"relay") == 0 && payload [0] == '2' ) {
    if (payload[1] == 'n') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay2, LOW);   
    }
    else if (payload[1] == 'c') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay2, HIGH);
    }
  }

  //Relay 3
  if (strcmp(topic,"relay") == 0 && payload [0] == '3' ) {
    if (payload[1] == 'n') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay3, LOW);   
    }
    else if (payload[1] == 'c') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay3, HIGH);
    }
  }

  //Relay 4
  if (strcmp(topic,"relay") == 0 && payload [0] == '4' ) {
    if (payload[1] == 'n') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay4, LOW);   
    }

    else if (payload[1] == 'c') {
      mqttClient.publish("relayStatus", STR(NODENUMBER));
      digitalWrite(relay4, HIGH);
    }
  }
  
}


void setup() {
  
  //Serial port setup
  Serial.begin(115200);
  Serial.println("USB Serial OK");

  Serial1.begin(9600);
  Serial.println("Serial Pin1 OK");

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  //W5500 Ethernet setup
  pinMode(SS, OUTPUT);
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
  Serial.println(F("Setup Complete"));

  //Light interrupt pin setup
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin,HIGH);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pin_ISR, FALLING);
 
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
      mqttClient.subscribe("relay");
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
    digitalWrite(relay3, HIGH);

  if (!mqttClient.connected()) {
    reconnect();
  }

 
  if (1 == intrruptFlag) {
        digitalWrite(relay2, LOW);

    mqttClient.publish("lightOn", STR(NODENUMBER));
        digitalWrite(relay2, HIGH);

    //delay(3000);
    intrruptFlag = 0;
  }
  
  mqttClient.loop();
    digitalWrite(relay3, LOW);

  
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


