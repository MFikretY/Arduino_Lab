#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

const int inputpin=A1; //pin A1 takes signal from circuit
const int outputpin=5; //pin A2 puts out signal
int input=0; // set input initially equal to 0
int output=0; // set output initially equal to 0
float inputvoltage=0;  // set voltage coming out of circuit into arduino=0
float actualvoltage=0;  // initialize corrected voltage value
float absvoltage=0;  // initialize absolute value of voltage
int bin1=0; // initialize 1st digit of binary number (2^2)
int bin2=0;  // 2nd binary digit (2^1)
int bin3=0; // 3rd binary digit (2^0)
int bin4=0; // 4th binary digit (2^-1)
int bin5=0; // 5th binary digit (2^-2)
int bin6=0; // 6th binary digit (2^-3)
int time=0;  // initialize time to mark each second
int time1=0;  // initialize time to track time since last lcd update
float shiftvoltage=-2.5; // value of voltage used to shift signal going into and out of arduino
int absvoltageint=0;

void setup()
{
  lcd.begin(16,2);  // initialize lcd
  pinMode(inputpin,INPUT);  // set A0 to input
  pinMode(outputpin,OUTPUT); // set A1 to output
  Serial.begin(9600);
}

void loop()
{
  input=analogRead(inputpin); // read input voltage of a scale of 0 to 1023
  inputvoltage=input*5*.00097752;  // convert input from 0 to 1023 to 0 to 5
  actualvoltage=(inputvoltage+shiftvoltage)*2.5; // unshift and undo gain to obtain input to circuit
  if (actualvoltage>5)  // if the calculated voltage is above 5V
  {
    actualvoltage=5;  // read values greater than 5 V as 5V
  }
  else if (actualvoltage<-5) // if the calculated voltage is below -5V
  {
    actualvoltage=-5;  // read values less than -5 V as -5 V
  }
  absvoltage=abs(actualvoltage); //absolute value of voltage;
  absvoltageint=8*absvoltage+.25; // begin rounding to nearest 1/8
  absvoltage=(absvoltageint)*.125; // finish rounding
  if (absvoltage>=4)  // if |Vin|>4
  {
    bin1=1; // first binary voltage=1
    absvoltage=absvoltage-4; // subtract 4 from voltage for next comparison stage
  }
  else
  {
    bin1=0;  // if less than 4, largest bit=0
  }
  if (absvoltage>=2)  // if voltage is more than 2 after deciding first binary digit
  {
    bin2=1;  // second binary digit=1
    absvoltage=absvoltage-2;  // subtract 2 from voltage for next comparison stage
  }
  else
  {
    bin2=0;  // if less than 2, second largest bit=0
  }
  if (absvoltage>=1)  // if magnitude of voltage remaining after prior stages>1
  {
    bin3=1;  // 3rd digit of binary number=1
    absvoltage=absvoltage-1;  // subtract 1 from voltage for next comparison stage
  }
  else
  {
    bin3=0; // if less than 1, third largest bit=0
  }
  if (absvoltage>=.5) // if magnitude of voltage remaining after prior stages>.5
  {
    bin4=1; // 4th digit of binary number=1
    absvoltage=absvoltage-.5;  // subtract .5 from voltage for next comparison stage
  }
  else
  {
    bin4=0; // if less than .5, fourth largest bit=0
  }
  if (absvoltage>=.25) // if magnitude of voltage remaining after prior stages>.25
  {
    bin5=1; // 5th digit of binary number=1
    absvoltage=absvoltage-.25; // subtract .25 from voltage for next comparison stage
  }
  else
  {
    bin5=0; // if less than .25, fifth largest bit=0
  }
  if (absvoltage>=.125) // if magnitude of voltage remaining after prior stages>.125
  {
    bin6=1; // 6th digit of binary number=1
    absvoltage=absvoltage-.125;
  }
  else
  {
    bin6=0; // if less than .125, sixth largest bit=0
  }
  time1=millis()-time; // time elapsed since last lcd update
  if (time1>=1000) // if a second has passed since the last update
  {
    lcd.setCursor(0,0); // set lcd to overwrite at top left
    lcd.print("Voltage="); // print "Voltage="
    if (actualvoltage>=0) // if voltage is positive
    {
      lcd.print("+"); // print "+" in front of binary number
    }
    else // if voltage is negative
    {
      lcd.print("-"); // print "-" in front of binary number
    }
    lcd.print(bin1);  // print digits of binary number in order with decimal point in appropriate position
    lcd.print(bin2);
    lcd.print(bin3);
    lcd.print(".");
    lcd.print(bin4);
    lcd.print(bin5);
    lcd.print(bin6);
    time=millis(); // reset reference time to current time
  }
  output=(actualvoltage*.5-shiftvoltage)*255*.2; // convert -5 to 5 volt scale to 0 to 255 scale
  analogWrite(outputpin,output);  // send the output to PWM pin
  Serial.println(actualvoltage);
}
