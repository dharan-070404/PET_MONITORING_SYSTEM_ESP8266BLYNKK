#define BLYNK_TEMPLATE_ID "TMPL3DIv1xA4_"
#define BLYNK_TEMPLATE_NAME "PULSE"
#define BLYNK_AUTH_TOKEN "9e40VPru5Xh_FeMMhHIt2VkKLob7BC_v"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MLX90614.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Dharani";  // type your WiFi name
char pass[] = "dh070404";  // type your WiFi password

BlynkTimer timer;
const int pulseSensorPin = A0;  // Connect the heart rate sensor to A0 pin
const int soundPin = A0;  // Connect the sound sensor to D1 pin

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
const int numReadings = 10;
int soundReadings[numReadings];  // Array to store sound readings
int soundIndex = 0;              // Index for storing sound readings
int soundCount = 0; 

void sendSensorData()
{
  int heart = analogRead(pulseSensorPin);  // Read heart rate data from the sensor
  delay(10);
  int soundValue = analogRead(soundPin); 
  soundReadings[soundIndex] = soundValue; // Store current sound reading in the array

  if (soundValue > 470) {   // Adjust the threshold value as per your sensor's sensitivity
    soundCount++;          // Increment sound count
  }

  soundIndex = (soundIndex + 1) % numReadings;  // Update sound index, cyclically

  if (soundCount > 7) {    // Check if sound count exceeds the threshold
    Blynk.logEvent("dog_bark","Dog barking detected!!!!!!");
    Blynk.virtualWrite(V4,"Emergency alert!!!");
    Serial.println("Emergency!");
    soundCount=0;
  }

  if (heart == 0) {
    Serial.println("Failed to read heart rate from sensor!");
    return;
  }

  int heartRate = map(heart, 0, 1023, 0, 200);
  float ambientTemperature = mlx.readAmbientTempC();
  float objectTemperature = mlx.readObjectTempC();

  Blynk.virtualWrite(V0, heartRate);  // Send heart rate data to Blynk app
  Blynk.virtualWrite(V1, ambientTemperature);  // Send ambient temperature data to Blynk app
  Blynk.virtualWrite(V2, objectTemperature);  // Send object temperature data to Blynk app
  Blynk.virtualWrite(V3, soundValue);  // Send sound level to Blynk app

  Serial.print("Heart Rate: ");
  Serial.println(heartRate);
  Serial.print("Ambient Temperature: ");
  Serial.println(ambientTemperature);
  Serial.print("Object Temperature: ");
  Serial.println(objectTemperature);
  Serial.print("Sound Level: ");
  Serial.println(soundValue);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  mlx.begin();
  pinMode(soundPin, INPUT);  // Initialize sound sensor pin as input
  timer.setInterval(1000L, sendSensorData);

}

void loop()
{
  Blynk.run();
  timer.run();
}
