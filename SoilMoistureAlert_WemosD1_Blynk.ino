#define BLYNK_TEMPLATE_ID "TMPL69UJIYZTa"
#define BLYNK_TEMPLATE_NAME "Monitoring DHT11"
#define BLYNK_AUTH_TOKEN "G43b7JjbuUWv1NTvn5f-0CYvKhISUKnr"

//#define BLYNK_PRINT Serial
 
 
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ngabrur";
char pass[] = "ngabrur123";

//Mux control pins
int s0 = D6;
int s1 = D5;

//Mux in "SIG" pin
#define SIG_pin A0

int led = D7;
int MaxValue = 1024;  // Corresponds to 0% soil moisture
int MinValue = 0;     // Corresponds to 100% soil moisture
int sliderValue = 0;
float finalValue;     // For soil moisture
float lux;

BlynkTimer timer;

void sendSensor()
{
  int sensorValue = readMux(1);
  lux = -64.95 * (float)sensorValue + 66509;
  int sM = readMux(0);
  finalValue = 100.0 * ((float)(MaxValue - sM) / (MaxValue - MinValue));

  if (isnan(sM)) {
    Serial.println("Failed to read from Soil Moisture sensor!");
    return;
  } else { 
    Serial.print("Soil Moisture Value: ");  
    Serial.print(finalValue);  
    Serial.println("%");
    Serial.print("Lux Value: ");  
    Serial.print(lux);
    Serial.println(" lx");
  }
  //Serial.println(sensorValue);
  
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V10, round(finalValue));
  Blynk.virtualWrite(V5, round(lux));
}

int readMux(int channel) {
  int controlPin[] = {s0, s1};

  int muxChannel[4][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
  };

  //loop through the 4 sig
  for(int i = 0; i < 2; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(SIG_pin);

  //return the value
  return val;
}

BLYNK_WRITE(V0) {
  sliderValue = param.asInt(); // Get the value from the slider
}

void setup() {
  // Debug console
  Serial.begin(115200);
 
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
  
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);

  pinMode(SIG_pin, INPUT);
  pinMode(led, OUTPUT);
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
  
  if (round(finalValue) > sliderValue) {
    Serial.println("WET");
  } else {
    Blynk.logEvent("notify", "It's time to water your plants");
    Serial.println("DRY");
    
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
    
  }
  delay(1000);
  
}
