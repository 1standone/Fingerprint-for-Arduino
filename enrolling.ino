// 01_Enrolling_Fingerprint
#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
  // For UNO and others without hardware serial, we must use software serial...
  // pin #2 is IN from sensor   (pin #2 = RX <----- TX fingerprint sensor / AS608).
  // pin #3 is OUT from arduino (pin #3 = TX -----> RX fingerprint sensor / AS608).
  // Set up the serial port to use softwareserial..
  SoftwareSerial mySerial(2, 3); //--> RX, TX.

#else
  // On Leonardo/M0/etc, others with hardware serial, use hardware serial!
  // hardware serial RX <----- TX fingerprint sensor / AS608.
  // hardware serial TX -----> RX fingerprint sensor / AS608.
  #define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;
String userinput;
//________________________________________________________________________________readnumber()
// Function to wait for the user to enter the ID number from the monitor serial.
uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
String readname(void) {
   String name = "";

	while (name == "") {
  while (!Serial.available());
  name = Serial.readString();

  }
  return name;
}
//________________________________________________________________________________

//________________________________________________________________________________getFingerprintEnroll()
// Function to enroll fingerprints.
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.println();
 
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id); Serial.println(userinput);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  Serial.println();
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println();
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println();
  Serial.println("Place same finger again");

  Serial.println();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  Serial.println();
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.println();
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   

  Serial.println();
  Serial.print("ID "); Serial.println(id);
  Serial.println(userinput);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
//________________________________________________________________________________

//________________________________________________________________________________VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial);  //--> For Yun/Leo/Micro/Zero/...
  delay(100);

  Serial.println();
  Serial.println("Adafruit Fingerprint sensor enrollment");

  // Set the data rate for the sensor serial port.
  finger.begin(57600);

  Serial.println();
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
   Serial.println("Enter ID and Name (separated by a space):");
}
//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

   if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    // Split the input into ID and Name
    int spaceIndex = input.indexOf(' ');
    if (spaceIndex != -1) {
      id = input.substring(0, spaceIndex).toInt();
      userinput = input.substring(spaceIndex + 1);
      Serial.print("Enrolling ID #");
      Serial.print(id);
      Serial.print(" with Name ");
      Serial.println(userinput);

      if (id == 0) {
        return;
      }
    while (true) {  // Infinite loop
        if (getFingerprintEnroll()) {
          // Enrolling completed, break the loop
          Serial.println("Enrolling completed.");
          break;
        }
      }
    } else {
      Serial.println("Invalid input format. Please enter ID and Name (e.g., '1 John').");
    }
  }
}

