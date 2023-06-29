#include <SoftwareSerial.h>

#define RX_PIN 2  // UART RX pin
#define TX_PIN 3  // UART TX pin
#define BAUD_RATE 9600

#define  RAND_MAX  3
#define  RANDOM_MAX  30

SoftwareSerial uart(RX_PIN, TX_PIN);  // Initialize SoftwareSerial object

void setup() {
  Serial.begin(9600);  // Initialize the serial monitor for debugging
  uart.begin(BAUD_RATE);  // Initialize UART communication
  pinMode(9, OUTPUT);

}
int theta = 45;  // Sample theta value (in degrees)
int distance = 30;  // Sample distance value (in meters)

void loop() {

  // Convert theta and distance to strings
    if (Serial.available()) {
    char receivedChar = Serial.read();
    analogWrite(9, 255);

    theta = 90;
    if (receivedChar == '1') {
      theta = 0;  // Reset theta to 0
    }
    if (receivedChar == '2') {
      distance = 20;  // Reset theta to 0
    }
    if (receivedChar == '3') {
      theta = 90;  // Reset theta to 0
    }
    
    
  }
  String thetaStr = String(theta);  // 2 decimal places
  String distanceStr = String(distance);  // 2 decimal places

  // Construct the data string in the format "(theta | distance)"
  String data =  thetaStr + " | " + distanceStr;

  // Send the data through UART
  uart.println(data);

  // Print the data to the serial monitor for debugging
  Serial.println(data);

  delay(500);  // Delay for 1 second before sending the next data

  theta = (theta + 1)% 180;

  if(random(8) == 0 ){
    distance = distance  + random(41) - 20;
  };
  distance = distance + random(5) -2;

  if (distance <0)
    distance =0;

}