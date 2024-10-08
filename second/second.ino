#include <SPI.h>
#include <LoRa.h> 
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(4, 5); // RX, TX for HC-05
// Define unique addresses for each LoRa device
#define LOCAL_ADDRESS 0x02  // Address of this device
#define DESTINATION_ADDRESS 0x01  // Address of the device to send 
// sendLoRaMessage(DESTINATION_ADDRESS, LOCAL_ADDRESS, outgoingMessage);

#define BUZZER_PIN 3  // Define the buzzer pin
#define LORA_SS 10    // LoRa SS (select) pin
#define LORA_RST 9    // LoRa reset pin
#define LORA_DIO0 2   // LoRa DIO0 pin

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  BTSerial.begin(9600);  // Bluetooth serial initialization

  // Initialize LoRa transceiver module
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  // Check if LoRa module begins successfully
  if (!LoRa.begin(433E6)) { // Set frequency (adjust for your region)
    Serial.println("Starting LoRa failed!");
    while (1);  // Halt if LoRa initialization fails
  }
  
  Serial.println("LoRa initialized successfully.");
  pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as OUTPUT
}

void loop() {
  // Serial Input Handling
  if (Serial.available()) {
    String outgoingMessage = Serial.readString(); // Read incoming data from Serial 
    sendLoRaMessage(DESTINATION_ADDRESS, LOCAL_ADDRESS, outgoingMessage);
    // Buzzer activation based on incoming message
    if (outgoingMessage.equals("a")) {
      activateBuzzer();
    } else {
      digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer is off if no match
    }
  }

  // Bluetooth Input Handling
  if (BTSerial.available()) {
    String outgoingMessage = BTSerial.readString(); // Read incoming message
    sendLoRaMessage(DESTINATION_ADDRESS, LOCAL_ADDRESS, outgoingMessage);

    if (outgoingMessage.equals("a")) {
      activateBuzzer();
    } else {
      digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer is off if no match
    }
  }
  
  // Check for incoming LoRa messages
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Read the destination and sender addresses
    byte destination = LoRa.read();
    byte sender = LoRa.read();
    
    // Check if the message is intended for this device
    if (destination == LOCAL_ADDRESS) {
      // Read the message content
      String incomingMessage = "";
      while (LoRa.available()) {
        incomingMessage += (char)LoRa.read();
      }
      delay(2000);
      Serial.print("Received from 0x");
      Serial.print(sender, HEX);
      Serial.print(": ");
      Serial.println(incomingMessage);
      
      // Buzzer activation based on incoming message
      if (incomingMessage.equals("a")) {
        activateBuzzer();
      } else {
        digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer is off if no match
      }

      BTSerial.println(incomingMessage); // Send to Bluetooth
    } else {
      Serial.println("Message not for this device.");
    }
  }

  delay(500);  // Delay for stability
}

void sendLoRaMessage(byte destination, byte sender, String message) {
  LoRa.beginPacket();  // Start a new packet
  LoRa.write(destination);  // Write the destination address
  LoRa.write(sender);  // Write the sender address
  LoRa.print(message);  // Write the actual message
  LoRa.endPacket();  // Send the packet
  Serial.println("Sent: " + message);
}

// Helper function to activate the buzzer
void activateBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);  // Turn on the buzzer
  delay(2000);                     // Keep buzzer on for 2 seconds
  digitalWrite(BUZZER_PIN, LOW);   // Turn off the buzzer
}
