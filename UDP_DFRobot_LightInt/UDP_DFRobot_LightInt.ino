

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet2.h>
#include <EthernetUdp2.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#define NODENUMBER 248              //last 3 digit of IP address
#define CLIENTID "client" STR(NODENUMBER)
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, NODENUMBER};

//For W5500 Ethernet
#define _STR(X)  #X
#define STR(X)  _STR(X)
#define SS     10U    //D10----- SS
#define RST    11U    //D11----- Reset
//#define UDP_TX_PACKET_MAX_SIZE 32

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

//UDP
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send backM
int packetsize;


IPAddress ip(192, 168, 1, NODENUMBER);      //W5500 client address
IPAddress server(192, 168, 1, 230);         // Server address
IPAddress dns1(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetUDP Udp;    //UDP object
unsigned int destinationPort = 5005;  // the destination port
unsigned int localPort = 5004;              // local port to listen on


//'n' for normal, 'c' for closed (powered)
//payload[0] is nth relay [1] for closed or normal.
//ex. publishing n1 to relay sets relay1 to normal.


/*
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
*/

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
  Ethernet.begin(mac, ip, dns1, dns1, subnet);
  delay(1500);
  Udp.begin(localPort);
  Serial.println(ip);


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


void loop() {

  if(Udp.parsePacket() == 2)
  {
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);

    if (packetBuffer[0] == '1')
    {
      if (packetBuffer[1] == 'n')
        digitalWrite(relay1,LOW);
      else if (packetBuffer[1] == 'c')
        digitalWrite(relay1, HIGH);
    }
    else if (packetBuffer[0] == '2')
    {
      if (packetBuffer[1] == 'n')
        digitalWrite(relay2,LOW);
      else if (packetBuffer[1] == 'c')
        digitalWrite(relay2, HIGH);
    }
    else if (packetBuffer[0] == '3')
    {
      if (packetBuffer[1] == 'n')
        digitalWrite(relay3,LOW);
      else if (packetBuffer[1] == 'c')
        digitalWrite(relay3, HIGH);
    }
    else if (packetBuffer[0] == '4')
    {
      if (packetBuffer[1] == 'n')
        digitalWrite(relay4,LOW);
      else if (packetBuffer[1] == 'c')
        digitalWrite(relay4, HIGH);
    }
  }

  if (1 == intrruptFlag){
    Udp.beginPacket(server, destinationPort);
    Udp.write(STR(NODENUMBER));
    Udp.endPacket();

  
    delay(4000);
    intrruptFlag = 0;
  }

  
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
    
  }
  delay(10);
  */


