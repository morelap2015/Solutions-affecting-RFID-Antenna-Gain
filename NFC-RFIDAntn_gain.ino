#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 

// Pin configurations for the RC522
constexpr uint8_t RST_PIN=9;
constexpr uint8_t SS_PIN=10;
constexpr int NUM_SAMPLE=10;

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Antenna Gain Array - used to hold our samples so we can do statistics
int ag_array[NUM_SAMPLE];
// Counter Variable to stop printing new data once we measured the appropriate number of samples.
int count = 0;

// Initalize necessary Parameters
void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  // Initialization of RFID Reader
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Scan PICC to see UID, SAK, Type, data blocks and signal level"));
  count = 0;
}
void loop() {
  // TODO: Transition from not using both 'x' and 'ag_reading', one variable should be sufficient  
  int x = 0x01<<4;
  // The Antenna Gain (AG) Value - higher AG means more power is used.
  int ag_reading = 0;
  mfrc522.PCD_SetAntennaGain(x);
  delay(10);
  while (x < (0x10<<4)) { //if it is found
    // If the Antenna Gain is not found increase by 1
    if( mfrc522.PICC_IsNewCardPresent() == 0 ) {
      ag_reading += 1;
    }
    delay(1);
    x = x+1;
    mfrc522.PCD_SetAntennaGain(x);
  }
  ag_array[(count++) % NUM_SAMPLE] = ag_reading;
  if (count == NUM_SAMPLE) {
    int sum = 0;
    int mn = 1000;
    int mx = 0;
    double sd = 0;
    double mean;
    for (int i = 0; i < NUM_SAMPLE; i++) {
      sum += ag_array[i];
      mn = ag_array[i] < mn? ag_array[i]:mn;
      mx = ag_array[i] > mx? ag_array[i]:mx;
      Serial.print(ag_array[i]);
      Serial.print(", ");
    }
    Serial.println();
    mean = (double) sum / NUM_SAMPLE;

    for (int i = 0; i < NUM_SAMPLE; i++) {
      sd += (ag_array[i] - mean)*(ag_array[i] - mean);
    }
    sd = sqrt(sd / NUM_SAMPLE);
    Serial.print("  Average: ");
    Serial.println(mean);
    Serial.print("  Min: ");
    Serial.println(mn);
    Serial.print("  Max: ");
    Serial.println(mx);
    Serial.print("  SD: ");
    Serial.println(sd);
  } 
  else if (count < NUM_SAMPLE) {
    Serial.print("Antenna Gain is ");
    Serial.println(ag_reading);
  }
   
}
