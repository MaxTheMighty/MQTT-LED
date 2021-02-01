9/*
 * 
 * 
 * 
 * 
 * I stole this code from an MQTT library example, but it works very well so im not complaining
 * 
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>  
#include <ESP8266WebServer.h>
#include <EasyDDNS.h>


ESP8266WebServer server(80);
// Update these with values suitable for your network.
const char* mqttSendTopic = "fill_in";
const String mqttSendTopicS = String(mqttSendTopic);
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "broker.mqtt-dashboard.com";
int brightness = 1023;
WiFiClient espClient;
PubSubClient client(espClient);


unsigned long startTime = 0;
//unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (1)
char msg[MSG_BUFFER_SIZE];


void onRoot(){
  server.send(200, "text/html", " <meta name='viewport' content='width=device-width, initial-scale=1.0'><body style = 'background-color: #36454f; font-family: Tahoma, Geneva, sans-serif'><div style = 'width:400px; margin:0 auto; color:white' ><div style = 'font-size: 30px'>Sophia's LED Homepage!</div><br><hr>This is where you can see info about your LED<br><hr>Uptime: " + formatTime(timeClient.getEpochTime()-startTime) + "<hr><br>This is a slider for setting LED brightness, try it out<br><form action = '/brightness' method = 'POST'> <div><input style = 'width: 100%' type='range' name = 'slider' min='1' max='1023' value='0' id='myRange' onchange=printValue(this.value)'><br><br><input type = 'submit' value = 'Update'><br><br>Current Brightness: " + String(((double)brightness/1023)*100) + "%<br><hr></div></div>");
  //this is very gross but if it works it works
 }

void setValue(){
  brightness = (server.arg("slider").toInt());
  delay(40);
  server.sendHeader("Location","/");
  server.send(303);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(!MDNS.begin("myled")){
      Serial.println("MDNS Error!");
   
   }
   else{ 
    Serial.println("MDNS active!");}
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/",HTTP_GET,onRoot);
  server.on("/brightness",HTTP_POST,setValue);
  server.begin();

}

void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.println();
  //brightness set by HTML webserver
  if(payload[0] == 1){
    analogWrite(12,0);
    }
   else{
    analogWrite(12,brightness);
    }


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqttSendTopic, "hello world");
      // ... and resubscribe
      client.subscribe("fill_in");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED,HIGH);
  pinMode(13,INPUT);
  pinMode(12,OUTPUT);
  Serial.begin(9600);
  setup_wifi();

}

void loop() {
  MDNS.update();
  server.handleClient();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  msg[0] = (digitalRead(13));
  digitalWrite(BUILTIN_LED,digitalRead(13));
  client.publish(mqttSendTopic, msg);
  delay(100);
  Serial.println(digitalRead(13));
  

   
}
