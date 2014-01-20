/*
 CAOS
 	
 The circuit:
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK  - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS   - depends on your SD card shield or module. 
 	       Pin 7 used here for consistency with other Arduino examples
  
  *for Arduino Nano v3.0 (ATmega328)
 **SPI: 7 (CS), 11 (MOSI), 12 (MISO), 13 (SCK).

 created 20 Jan 2014
 by BlueCocoa 
 */

// include the SD library:
#include <SD.h>
//Our user's settings file
/*
 *  Settings file's e.g
 *  GMT,8
 *  WiN,AirPort
 *  WiP,bluecocoa
 *  CL1,0700
 *  CL2,0710
 *  LAZ,5
 *  
 */
File userDefault;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 10;  

void printError(const char *error){
  Serial.println(error);
}

int initSD(){
  pinMode(10, OUTPUT);     // change this to 53 on a mega
  if (!SD.begin(7)) {
    printError("Initialization Failed!");
    return -1;
  }
  Serial.println("Initialization Done.");
  return 0;
}

void printUserDefault(){
  userDefault = SD.open("userPref.txt");
  if (userDefault) {
    Serial.println();
    Serial.println("userPref.txt:");
    // read from the file until there's nothing else in it:
    while (userDefault.available()) {
    	Serial.write(userDefault.read());
    }
    // close the file:
    userDefault.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening userPref.txt");
  }
}

void writeUserDefaultValueWithKey(const char *value, const char *key){
    Serial.print("Preparing To Writing User Default Value:\"");
    Serial.print(value);
    Serial.print("\" With Key:\"");
    Serial.print(key);
    Serial.println("\"");
    userDefault = SD.open("userPref.txt", FILE_WRITE);
    if (userDefault) {
        int check[3] = {0};
        userDefault.seek(0);
        while (userDefault.available()) {
            for(int i = 0; i < 3; i++){
                check[i] = userDefault.read();
            }
            if(check[0] == key[0] && check[1] == key[1] && check[2] == key[2]){
                break;
            }else{
                while(userDefault.read() != '\n' && userDefault.available()){
                    ;
                }
            }
        }
        uint32_t positionMark = userDefault.position();
        if(positionMark == userDefault.size()){
            userDefault.print(key);
            userDefault.print(",");
            userDefault.println(value);
            userDefault.close();
            Serial.println("Done.");
            return;
        }else{
            userDefault.seek(0);
            File tmp = SD.open("tmp.txt", FILE_WRITE);
            for(int i = 0; i < positionMark+1; i++){
                tmp.write(userDefault.read());
            }
            tmp.println(value);
            while(userDefault.read() != '\n'){
                ;
            }
            while(userDefault.available()){
                tmp.write(userDefault.read());
            }
            tmp.close();
            userDefault.close();
            SD.remove("userPref.txt");
            tmp.seek(0);
            userDefault = SD.open("userPref.txt", FILE_WRITE);
            tmp = SD.open("tmp.txt", FILE_READ);
            while(tmp.available()){
                userDefault.write(tmp.read());
            }
            tmp.close();
            userDefault.close();
            SD.remove("tmp.txt");
            Serial.println("Done.");
            return;
        }
    } else {
        if(SD.exists("userPref.txt")){
            printError("Error Opening userPref.txt");
        }else{
            printError("userPref.txt does not exist");
        }
    }
}

char * readUserDefaultWithKey(const char *key){
  Serial.print("Searching User Default With key:\"");
  Serial.print(key);
  Serial.println("\"");
  int check[3] = {0};
  userDefault = SD.open("userPref.txt", FILE_READ);
  if (userDefault) {
    userDefault.seek(0);
    while (userDefault.available()) {
      for(int i = 0; i < 3; i++){
        check[i] = userDefault.read();
      }
      if(check[0] == key[0] && check[1] == key[1] && check[2] == key[2]){
        break;
      }else{
        while(userDefault.read() != '\n' && userDefault.available()){
          ;
        }
      }
    }
    uint32_t positionMark = userDefault.position();
    if(positionMark >= userDefault.size() - 2){
      Serial.print("Sorry,  we didn't find the value for \"");
      Serial.print(key);
      Serial.println("\"");
      // close the file:
      userDefault.close();
      return NULL;
    }
    int count = 0;
    userDefault.read();
    while(userDefault.read() != '\n'){
      count++;
    }
    char value[count+1];
    userDefault.seek(positionMark+1);
    for(int i = 0;i < count;i++){
      value[i] = (char)userDefault.read();
    }
    value[count] = '\0';
    // close the file:
    userDefault.close();
    Serial.print("Done, value is \"");
    Serial.print(value);
    Serial.println("\"");
    Serial.println();
    return value;
  } else {
    // if the file didn't open, print an error:
    if(SD.exists("userPref.txt")){
      printError("Error Opening userPref.txt");
    }else{
      printError("userPref.txt does not exist");
    }
  }
  return NULL;
}

void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  if(initSD() != 0){
    printError("SD Error!");
    return;
  }
  //for test
  readUserDefaultWithKey("GMT");
  readUserDefaultWithKey("CL1");
  writeUserDefaultValueWithKey("0720","CL1");
  readUserDefaultWithKey("CL1");
  readUserDefaultWithKey("NON");
  writeUserDefaultValueWithKey("NON's Value","NON");
  readUserDefaultWithKey("NON");
  writeUserDefaultValueWithKey("NON's New Value","NON");
  readUserDefaultWithKey("NON");
  printUserDefault();
}

void loop(void) {
  
}