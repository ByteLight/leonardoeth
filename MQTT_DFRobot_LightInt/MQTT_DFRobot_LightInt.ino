/*
  UDPSendReceive.pde
 */

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
const int relay =  PD7;// the number of the Relay pin
int relayState = LOW;    // ledState used to set the LED

//Interrupt to detect when LED is switched on/off
const int interruptPin = 3;
int intrruptFlag = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#define NODENUMBER  0x67 //last 3 digit of IP address
#define CLIENTID "client" STR(NODENUMBER)
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, NODENUMBER};
unsigned int localPort = 8888;      // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back

IPAddress ip(192, 168, 1, NODENUMBER);     //W5500 client address
IPAddress server(192, 168, 1, 77);         //MQTT Server address
IPAddress dns1(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
EthernetClient ethClient;
PubSubClient client(ethClient);

EthernetUDP Udp;    //UDP object


void callback(char* topic, byte* payload, unsigned int length) 
{
  if ((char)payload[0] == '0') {
    client.publish("ack", STR(NODENUMBER));
    digitalWrite(relay, LOW);   // Turn the LED on (Note that LOW is the voltage level
    relayState = LOW;
  }
  else if ((char)payload[0] == '1') {
    client.publish("ack", STR(NODENUMBER));
    digitalWrite(relay, HIGH);  // Turn the LED off by making the voltage HIGH
    relayState = HIGH;
  } 
}


void setup() {
  
  Serial.begin(9600);
    delay(2500);

  //while (!Serial){;}
    Serial.print("After 2500 delay");

  //Serial1.begin(9600);

  //Interrupt pin setup
  pinMode(interruptPin, INPUT);  
  
  //Setup
  pinMode(SS, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(SS, LOW);
  digitalWrite(RST,HIGH);  //Reset Ethernet
  digitalWrite(RST,LOW); 
  digitalWrite(RST,HIGH);  
  
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip, dns1, subnet);
  delay(1500);
  client.setServer(server,1883);
  client.setCallback(callback);
  Udp.begin(localPort);
  
  Serial.print(ip);
  Serial.println("End of setup");
  attachInterrupt(digitalPinToInterrupt(interruptPin), pin_ISR, LOW);

  //Serial1.print(0x55);

}

void pin_ISR() {
  intrruptFlag = 1;
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(" Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(CLIENTID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","Connected!");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void loop() {
  
  
  if (!client.connected()) {
    reconnect();
  }
  
  if (1 == intrruptFlag) {
    Serial.print(" begin pin_ISR()");
    client.publish("ack", STR(NODENUMBER));
    digitalWrite(relay, HIGH); 
    delay(1000);
    digitalWrite(relay, LOW);
    Serial.println("end of pin_ISR()");
    attachInterrupt(digitalPinToInterrupt(interruptPin), pin_ISR, LOW);
    intrruptFlag = 0;
  }
  
  client.loop();  
  
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


