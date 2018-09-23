/*
   HID RFID Reader Wiegand Interface for Arduino Uno
   Originally by  Daniel Smith, 2012.01.30 -- http://www.pagemac.com/projects/rfid/arduino_wiegand

   Updated 2016-11-23 by Jon "ShakataGaNai" Davis.
   See https://obviate.io/?p=7470 for more details & instructions
*/

#include <DS3231_Simple.h>
#include <SPI.h>


DS3231_Simple Clock;
String date;

#define MAX_BITS 100                 // max number of bits 
#define WEIGAND_WAIT_TIME  3000      // time to wait for another weigand pulse.  

unsigned char databits[MAX_BITS];    // stores all of the data bits
unsigned char databits1[MAX_BITS];    // stores all of the data bits
unsigned char bitCount;              // number of bits currently captured
unsigned char bitCount1;              // number of bits currently captured
unsigned char flagDone;  // goes low when data is currently being captured
unsigned char flagDone1;
unsigned int weigand_counter;        // countdown until we assume there are no more bits
unsigned int weigand_counter1;        // countdown until we assume there are no more bits

unsigned long facilityCode = 0;      // decoded facility code
unsigned long cardCode = 0;          // decoded card code
unsigned long facilityCode1 = 0;      // decoded facility code
unsigned long cardCode1 = 0;          // decoded card code

int LED_GREEN = 11;
int LED_RED = 12;
int BEEP_BEEP = 10;

// interrupt that happens when INTO goes low (0 bit)
void ISR_INT0() {
  //Serial.print("0");   // uncomment this line to display raw binary
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;

}

// interrupt that happens when INT1 goes low (1 bit)
void ISR_INT1() {
  //Serial.print("1");   // uncomment this line to display raw binary
  databits[bitCount] = 1;
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;
}
void ISR_INT01() {
  //Serial.print("01");   // uncomment this line to display raw binary
  bitCount1++;
  flagDone1 = 0;
  weigand_counter1 = WEIGAND_WAIT_TIME;

}
void ISR_INT11() {
  //Serial.print("11");   // uncomment this line to display raw binary
  databits1[bitCount1] = 1;
  bitCount1++;
  flagDone1 = 0;
  weigand_counter1 = WEIGAND_WAIT_TIME;

}

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BEEP_BEEP, OUTPUT);
  digitalWrite(LED_RED, HIGH); // High = Off
  digitalWrite(BEEP_BEEP, HIGH); // High = off
  digitalWrite(LED_GREEN, LOW);  // Low = On
  pinMode(2, INPUT);     // DATA0 (INT0)
  pinMode(3, INPUT);     // DATA1 (INT1)
  pinMode(18, INPUT);     // DATA01 (INT0)
  pinMode(19, INPUT);     // DATA11 (INT1)

  Serial.begin(9600);
  Serial.println("RFID Readers");
  
  Clock.begin();

  // binds the ISR functions to the falling edge of INTO and INT1
  attachInterrupt(5, ISR_INT0, FALLING);
  attachInterrupt(4, ISR_INT1, FALLING);
  attachInterrupt(1, ISR_INT01, FALLING);
  attachInterrupt(0, ISR_INT11, FALLING);


  weigand_counter = WEIGAND_WAIT_TIME;
  weigand_counter1 = WEIGAND_WAIT_TIME;
}

void loop() {
  prin();
  // prin(bitCount1, flagDone1, weigand_counter1);

}




void prin() {

  // This waits to make sure that there have been no more data pulses before processing data
  if (!flagDone) {
    if (--weigand_counter == 0)
      flagDone = 1;
  }

  // if we have bits and we the weigand counter went out
  if (bitCount > 0 && flagDone) {
    unsigned char i;

    Serial.print("Read ");
    Serial.print(bitCount);
    Serial.print(" bits. ");

    if (bitCount == 32) {
      // 35 bit HID Corporate 1000 format
      // facility code = bits 2 to 14
      for (i = 0; i < 8; i++) {
        facilityCode <<= 1;
        facilityCode |= databits[i];
      }

      // card code = bits 15 to 34
      for (i = 8; i < 31; i++) {
        cardCode <<= 1;
        cardCode |= databits[i];
      }
      Serial.println("from 1st reader");
      printBits();
    }


    // cleanup and get ready for the next card
    bitCount = 0;
    facilityCode = 0;
    cardCode = 0;
    for (i = 0; i < MAX_BITS; i++)
    {
      databits[i] = 0;
    }
  }

  if (!flagDone1) {
    if (--weigand_counter1 == 0)
      flagDone1 = 1;
  }

  // if we have bits and we the weigand counter went out
  if (bitCount1 > 0 && flagDone1) {
    unsigned char i;

    Serial.print("Read ");
    Serial.print(bitCount1);
    Serial.print(" bits. ");

    if (bitCount1 == 32) {
      // 35 bit HID Corporate 1000 format
      // facility code = bits 2 to 14
      for (i = 0; i < 8; i++) {
        facilityCode1 <<= 1;
        facilityCode1 |= databits1[i];
      }

      // card code = bits 15 to 34
      for (i = 8; i < 31; i++) {
        cardCode1 <<= 1;
        cardCode1 |= databits1[i];
      }
      Serial.println("from 2nd reader");
      printBits();
    }

    // cleanup and get ready for the next card
    bitCount1 = 0;
    facilityCode1 = 0;
    cardCode1 = 0;
    for (i = 0; i < MAX_BITS; i++)
    {
      databits1[i] = 0;
    }
  }
}

void printBits() {
  if (facilityCode == 0) {
    Serial.print("FC = ");
    Serial.print(facilityCode1);
    Serial.print(", CC = ");
    Serial.println(cardCode1);
  } else {
    Serial.print("FC = ");
    Serial.print(facilityCode);
    Serial.print(", CC = ");
    Serial.println(cardCode);
  }
  Serial.println(DT(0));
  // Now lets play with some LED's for fun:
  digitalWrite(LED_RED, LOW); // Red
  if (cardCode == 12345) {
    // If this one "bad" card, turn off green
    // so it's just red. Otherwise you get orange-ish
    digitalWrite(LED_GREEN, HIGH);
  }
  delay(500);
  digitalWrite(LED_RED, HIGH);  // Red Off
  digitalWrite(LED_GREEN, LOW);  // Green back on

  // Lets be annoying and beep more
  digitalWrite(BEEP_BEEP, LOW);
  delay(500);
  digitalWrite(BEEP_BEEP, HIGH);
  delay(500);
  digitalWrite(BEEP_BEEP, LOW);
  delay(500);
  digitalWrite(BEEP_BEEP, HIGH);
}

String  DT(int x) {
  DateTime MyDateAndTime;
  MyDateAndTime = Clock.read();
  if (x == 0) {

    date = String(MyDateAndTime.Hour) + ':' + String(MyDateAndTime.Minute) + ':' + String(MyDateAndTime.Second) + ' ' + '-' + ' ' + String(MyDateAndTime.Year) + '/' + String(MyDateAndTime.Month) + '/' + String(MyDateAndTime.Day);

    return date;
  }
  return ;
}
