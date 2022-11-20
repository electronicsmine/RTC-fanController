#include<LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
const int button = 14;
const int m1 = 5;
const int m2 = 6;
int rpm = 0;
int rpm_temp = 0;

#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
bool LED_STATE = true;
char sec_1 = 0;

ISR(TIMER1_COMPA_vect) {
  sec_1++;
  TCNT1  = 0;
}

void isr_1() //interrupt service routine
{
  rpm++;
}

void setup () {
  while (!Serial); // for Leonardo/Micro/Zero
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(button, INPUT);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  lcd.setCursor(0, 0);

  cli();                      //stop interrupts for till we make the settings
  /*1. First we reset the control register to amke sure we start with everything disabled.*/
  TCCR1A = 0;                 // Reset entire TCCR1A to 0
  TCCR1B = 0;                 // Reset entire TCCR1B to 0

  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */
  TCCR1B |= B00000100;        //Set CS12 to 1 so we get prescalar 256

  /*3. We enable compare match mode on register A*/
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A

  /*4. Set the value of register A to 31250*/
  OCR1A = 31250;             //Finally we set compare register A to this value
  attachInterrupt(0, isr_1, RISING);
  sei();                     //Enable back the interrupts
  lcd.print("   Wel Come");

  delay(1000);
}
void loop () {
  if (digitalRead(button) == HIGH)
  {
    digitalWrite(m1, LOW);
    digitalWrite(m2, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(" CC  ");
    if (rpm_temp > 1000)
      lcd.print("Full");
    else if (rpm_temp > 750)
      lcd.print("3/4 ");
    else if (rpm_temp <750)
      lcd.print("1/2 ");
    else if (rpm_temp < 1)
      lcd.print("0   ");
  }
  else
  {
    digitalWrite(m2, LOW);
    digitalWrite(m1, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("  C  ");
    if (rpm_temp > 1000)
      lcd.print("Full");
    else if (rpm_temp > 750)
      lcd.print("3/4 ");
    else if (rpm_temp < 750)
      lcd.print("1/2 ");
    else if (rpm_temp < 1)
      lcd.print("0   ");

  }
  if (sec_1 >= 2)
  {
    sec_1 = 0;
    //First, set the timer back to 0 so it resets for next interrupt
    LED_STATE = !LED_STATE;      //Invert LED state
    digitalWrite(4, LED_STATE); //Write new state to the LED on pin D5

    DateTime now = rtc.now();
    lcd.setCursor(0, 0);
    lcd.print(now.day());
    lcd.print(":");
    lcd.print(now.month());
    lcd.print(":");
    lcd.print(now.year() - 2000);
    lcd.print(" ");
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());
    lcd.print(":");
    lcd.print(now.second());
    lcd.print("  ");
    rpm_temp = rpm;
    rpm = 0;
  }


  //  Serial.print(now.year(), DEC);
  //  Serial.print('/');
  //  Serial.print(now.month(), DEC);
  //  Serial.print('/');
  //  Serial.print(now.day(), DEC);
  //  Serial.print("  ");
  //  Serial.print(now.hour(), DEC);
  //  Serial.print(':');
  //  Serial.print(now.minute(), DEC);
  //  Serial.print(':');
  //  Serial.print(now.second(), DEC);
  //  Serial.println();
}
