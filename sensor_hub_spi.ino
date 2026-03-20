#include <SPI.h>

const int DRY_VALUE = 1023; 
const int WET_VALUE = 0; 
const int NUM_SENSORS = 5;

const int sensorPins[] = {A0, A1, A2, A3, A4}; 

volatile byte moistureData[NUM_SENSORS];
volatile byte spiIndex = 0;
unsigned long lastUpdateTime = 0;

void setup() {
  Serial.begin(115200); 

  pinMode(MISO, OUTPUT); 
  pinMode(SS, INPUT_PULLUP);

  // Turn on SPI in Slave Mode and enable interrupts
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  
  Serial.println("\nUno SPI Slave Hub Started. Debug mode ON.");
}

// SPI Interrupt - Fires automatically when the ESP8266 asks for a byte
ISR(SPI_STC_vect) {
  byte dummy = SPDR; 

  spiIndex++;
  if (spiIndex >= NUM_SENSORS) {
    spiIndex = 0; 
  }
  SPDR = moistureData[spiIndex];
}

void loop() {
  // Reset the index when the ESP finishes its 5-byte transaction
  if (digitalRead(SS) == HIGH) {
    spiIndex = 0;
    SPDR = moistureData[0]; 
  }

  // Read sensors and update the array every 1 second
  if (millis() - lastUpdateTime > 1000) {
    
    // We create a string to hold our output
    String debugOutput = "Moisture -> ";
    
    for (int i = 0; i < NUM_SENSORS; i++) {
      int raw = analogRead(sensorPins[i]);
      
      // Map based on 1023 (Dry) to 0 (Wet)
      int percent = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
      percent = constrain(percent, 0, 100);
      
      moistureData[i] = (byte)percent;
      
      // Build the debug text with the percentage and the (raw value)
      debugOutput += "P";
      debugOutput += (i + 1);
      debugOutput += ": ";
      debugOutput += percent;
      debugOutput += "% (";
      debugOutput += raw;
      debugOutput += ")   ";
    }
    
    // Safety Valve: Only print if the serial buffer has room. 
    // This prevents the Uno from freezing when running headless on the 9V jack.
    //if (Serial.availableForWrite() > debugOutput.length()) {
      Serial.println(debugOutput);
    //}

    lastUpdateTime = millis();
  }
}