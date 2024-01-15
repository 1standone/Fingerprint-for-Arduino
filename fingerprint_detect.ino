//02_Fingerprint_Detect
#include <Adafruit_Fingerprint.h>
#define lock 12
#define pinRed 11
#define pinGreen 10
#define buzz 8
SoftwareSerial mySerial(2, 3); //--> RX, TX.
//tone(9, 1200, 500);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

volatile int finger_status = -1;

byte cnt_fail_ImageTooMessy = 0;
byte cnt_fail_Invalid_Fingerprint = 0;
int failedAttempts = 0;
// getFingerprintIDez()
// Function to scan fingerprints and match them with the database on the fingerprint sensor module.
// returns -1 if failed, otherwise returns ID #.
int getFingerprintIDez() {

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    cnt_fail_ImageTooMessy = 0;
    cnt_fail_Invalid_Fingerprint = 0;
    return -1;
  }
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    cnt_fail_ImageTooMessy++;
    if (cnt_fail_ImageTooMessy > 5) {
      cnt_fail_ImageTooMessy = 0;
      cnt_fail_Invalid_Fingerprint = 0;

      Serial.println();
      Serial.println("The fingerprint image is too messy !");
      Serial.println("Clean your finger and Clean the fingerprint sensor.");
      Serial.println("Lift your finger from the fingerprint sensor.");
      Serial.println("Then place your finger again on the fingerprint sensor.");
    }
    delay(50);
    return -1;
  }
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    cnt_fail_Invalid_Fingerprint++;
    if (cnt_fail_Invalid_Fingerprint > 2) {
      failedAttempts++;
      if(failedAttempts >= 3) {
        tone(buzz, 1000);
        delay(1000);
        noTone(buzz);
        failedAttempts = 0;        
       }      
      cnt_fail_Invalid_Fingerprint = 0;
      cnt_fail_ImageTooMessy = 0;

      Serial.println();
      Serial.println("Invalid fingerprint !");
      Serial.println("Make sure your fingerprint is already enrolled.");
      Serial.println("Clean your finger and Clean the fingerprint sensor.");
      Serial.println("Lift your finger from the fingerprint sensor.");
      Serial.println("Then place your finger again on the fingerprint sensor.");
      digitalWrite(pinRed, HIGH);
      delay(2000);
      digitalWrite(pinRed, LOW);
      tone(buzz, 1000);
      delay(500);
      noTone(buzz);      
    }
    delay(50);
    return -1;
  }

  cnt_fail_ImageTooMessy = 0;
  cnt_fail_Invalid_Fingerprint = 0;
  
  // found a match!
  Serial.println();
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  
  if (finger.confidence>90){
    digitalWrite(lock, LOW);
    digitalWrite(pinGreen, HIGH);
    delay(2000);
    digitalWrite(lock, HIGH);
    digitalWrite(pinGreen, LOW);    
  }
  
  return finger.fingerID; 
}
//________________________________________________________________________________

//________________________________________________________________________________VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  while (!Serial);  //--> For Yun/Leo/Micro/Zero/..  
  delay(100);
  pinMode(lock, OUTPUT);
  digitalWrite(lock, HIGH); 
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(buzz, OUTPUT);
  noTone(buzz);
 
    
  Serial.println();
  Serial.println("Adafruit fingerprint detect test");

  // Set the data rate for the sensor serial port.
  finger.begin(57600);

  Serial.println();
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  //Serial.println();
  //Serial.println(F("Reading sensor parameters"));
  //finger.getParameters();
  //Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  //Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  //Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  //Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  //Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  //Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  //Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  Serial.println();
  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data.");
  }
  else {
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    Serial.println("Waiting for valid finger...");
  }
}

//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  finger_status = getFingerprintIDez();
  delay(50);  //--> don't ned to run this at full speed.

}