#include <SPI.h>    // For spi communication with the module
#include <MFRC522.h>  // Initializing RFID module
#include <LiquidCrystal_I2C.h>  //initializing the LCD

#define SS_PIN 10    //SDA or data pin for SPI
#define RST_PIN 9    // reset pin 

MFRC522 rfid(SS_PIN, RST_PIN);  //Declaring the instance of RFID with SDA and reset pins
LiquidCrystal_I2C lcd(0x27, 16, 2);  //Declaring the instance of LCD

MFRC522::MIFARE_Key key;     

byte IDFromCard[4];   
/*
Data from the rfid comes in a bit stream, however to make sense of the bit stream te data can be displayed in HEX or DEC format.RFID contains 1KB of data which is segregated into 
64 lines where each line contains 16 bytes of data. The line 0 is where default ID from the manufacturer is stored and it is not adivsable to write onto the line 0. However it is 
unique within the given number of tags with the module hence each tag can be identified from the data in the line 0. It is worthy to note that from all the tags, only the first 4 
bytes are unique from the 16 bytes. So we are only concerned with the first 4 bytes of data from line 0.
*/ 
int state;  //Saving the state for IR sensor
int startMillis;
int elapsedTime;

void setup() {
Serial.begin(9600);
SPI.begin();
rfid.PCD_Init();
lcd.init();
lcd.backlight();

pinMode(7,INPUT);
pinMode(3,OUTPUT);
pinMode(2,OUTPUT);

  for(byte i=0; i<6; i++)                                      // Simply printing the key for the RFID, this key is set and cant be changed for a given ID reader
  {
    key.keyByte[i]= 0xFF;
  }

  Serial.println("key for the RFID is:");
  for(byte i=0; i<6; i++)
  {
    Serial.print(key.keyByte[i], HEX);
    Serial.print(" ");
  }
}

void loop() {
state=digitalRead(7);                          //checking for if a card is on the RFID sensor module
IRSensor(state);
if ( ! rfid.PICC_IsNewCardPresent())
  return;

if ( ! rfid.PICC_ReadCardSerial())                // verifying that some data is being read from the RFID card reader
  return;

  

  Serial.println("");
  Serial.println(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak); // getting the category of card being used 
  Serial.println(rfid.PICC_GetTypeName(piccType));


    String a,b,c,d;
    a=rfid.uid.uidByte[0];
    b=rfid.uid.uidByte[1];
    c=rfid.uid.uidByte[2];
    d=rfid.uid.uidByte[3];

    String readString=a+b+c+d;       //Taking the 4 bytes of data and concatenating it for comparison 

    printOnLCD(readString);
 
    Serial.println(F("The tag ID is:"));  // testing and printing the UID on the cards in different format to understand the digits that are being read

    Serial.println(F("In bin: "));
    printBin(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    Serial.println(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  
    Serial.println(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

rfid.PICC_HaltA();               // stopping the values from reading and printing constantly, rather having them read and print when a card is placed.

rfid.PCD_StopCrypto1();
 
}

void printBin(byte *buffer , byte bufferSize)   //Function call for printing the input uid in different data types, this part of the code was just meant for testing purposes                                             
{                                               
  for(byte i=0;i<bufferSize;i++)
  { 
    Serial.print(buffer[i],BIN);
    Serial.print(" ");
  }
}

void printHex(byte *buffer, byte bufferSize)
{
  for(byte i=0; i<bufferSize; i++)
  {
    Serial.print(buffer[i],HEX);
    Serial.print(" ");
  }
}

void printDec(byte *buffer, byte bufferSize)
{
  for(byte i=0; i<bufferSize; i++)
  {
    Serial.print(buffer[i],DEC);
    Serial.print(" ");
  }
}

int printOnLCD(String input)       // Function call for printing on LCD
{
 
   int myArr[3]={"14715020148","676088148","115117192167"};
   if(input==myArr[0])
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle A");
      lcd.setCursor(0, 1);
      lcd.print("Timer:");
    }
      else if(input==myArr[1])
      {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle B");
      lcd.setCursor(0, 1);
      lcd.print("Timer:");
      Serial.println("Vehicle B");
      }else if(input==myArr[2])
      {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vehicle C");
      lcd.setCursor(0, 1);
      lcd.print("Timer:");
      Serial.println("Vehicle C");
      }

}

int IRSensor (int CurrentState)
{
  if(state==HIGH)
  {
    
    startMillis=millis();
    digitalWrite(2,LOW);
    digitalWrite(3,HIGH);
  }
  else if(state==LOW)
  {
    elapsedTime = millis()-startMillis;
    lcd.setCursor(7, 1);
    lcd.print(elapsedTime);
    digitalWrite(3,LOW);
    digitalWrite(2,HIGH);
  }
}
