
/***********************************************/
/*Author  : Eng.Mahmoud Aboelsaoud             */
/*DATA    : 20 Aug 2021                        */
/*Version : V01                                */
/*Author  : MAXEL HEATER SOFTWARE              */
/***********************************************/

/*********************LCD********************************************/
#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 6, 5, 4, 3);
/********************************************************************/

/******************Temperature Sensors *******************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin A0 on the Arduino 
#define ONE_WIRE_BUS  8 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/


volatile int flow_frequency; // Measures flow sensor pulses Calculated litres/hour
float vol = 0.0, l_minute = 0.0;
unsigned char flowsensor  = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

unsigned char start_button  = A3;
unsigned char up_button     = A4;
unsigned char down_button   = A5;
unsigned char ok_button     = A2;
float         temp_connter  = 25; 

unsigned char SetUserTemprature_Flag = 0;

int waterTempInput  = 0;
int waterTempOutput = 0;

unsigned char PwmPin9  = 9;
unsigned char PwmPin10 = 10;
unsigned char PwmPin11 = 11;

/*****************************INTERRUPT***************************************/
void flow () // Interrupt function
{
  flow_frequency++;
}
/*****************************************************************************/
/**********************************LCD****************************************/
void LcdInit()
{
  lcd.begin(16, 2);
  lcd.print("  MAXEL HEATER");
  delay(3000);
}
/******************************************************************************/
/***************************************BUTTONS*********************************/
void ControlButtonInit()
{
  pinMode(ok_button   , INPUT);
  pinMode(start_button, INPUT);
  pinMode(up_button   , INPUT); // declare pwm pin to be an INPUT:
  pinMode(down_button , INPUT); // declare pwm pin to be an INPUT:
}

void SetUserTemprature()
{
  SetUserTemprature_Flag = 1;
  lcd.clear();
  lcd.print("Set Desired Temp ");
  lcd.setCursor(6,1);
  lcd.print((int)temp_connter);
  lcd.print(" C");
  while(digitalRead(ok_button)==0) 
  {
     if(digitalRead(up_button)==1)
      {
        delay(170);
        temp_connter++;
        if(temp_connter == 41)
            temp_connter = 40;
        lcd.setCursor(6,1);
        lcd.print((int)temp_connter);
        lcd.print(" C");
      }
    else if((digitalRead(down_button)==1))
      {
        delay(170);
        temp_connter--;
        if(temp_connter <= 25)
            temp_connter = 25;
        lcd.setCursor(6,1);
        lcd.print((int)temp_connter);
        lcd.print(" C");
      }
  }
      lcd.clear();
      lcd.print(" TEMP IS SET TO ");
      lcd.setCursor(6,1);
      lcd.print((int)temp_connter);
      lcd.print(" C");
      delay(500);
}
/*******************************************************************************/
/********************************TEMP Sensor************************************/
void tempSensorsInit()
{
  sensors.begin(); 
}

int tempInputSensorRead()
{
  sensors.requestTemperatures(); // Send the command to get temperature readings
  return (int)(sensors.getTempCByIndex(0));
}

int tempOuputSensorRead()
{
   sensors.requestTemperatures(); // Send the command to get temperature readings
   return (int)(sensors.getTempCByIndex(1));
}

/*******************************************************************************/
/*********************************Temp Difrance********************************/
int TempDiffranceRequired(int currentTemp, int desiredTemp)
{
  int DeltaTemp = currentTemp - desiredTemp;
  return DeltaTemp;
}
/******************************************************************************/

/*******************************  PWM  *****************************************/
void PwmInit()
{
  pinMode(PwmPin9 , OUTPUT); // declare pwm pin to be an output:
  pinMode(PwmPin10, OUTPUT); // declare pwm pin to be an output:
  pinMode(PwmPin11, OUTPUT); // declare pwm pin to be an output:*/
  analogWrite(PwmPin9 , 0);
  analogWrite(PwmPin10, 0);
  analogWrite(PwmPin11, 0);
}
void PwmFunction()
{
  
}
/*******************************************************************************/
/******************************* Flow Meter ************************************/


void FlowMeterInit()
{
   pinMode(flowsensor, INPUT);
   pinMode(13, OUTPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   //Serial.begin(9600);
   attachInterrupt(digitalPinToInterrupt(flowsensor), flow, FALLING); // Setup Interrupt /*FALLING*/
   currentTime = millis(); // millis function returns the number of milliseconds that your Arduino board has been powered up
   cloopTime = currentTime;
}

void FlowMeterFunction()
{  
  currentTime = millis();
   /*Every second, calculate and print litres/hour*/
   if(currentTime >= (cloopTime + 1000)) 
   {
    cloopTime = currentTime; // Updates cloopTime
    if(flow_frequency != 0)
    {                                       // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_minute = (flow_frequency / 7.5);    // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Water Flow Rate ");
      lcd.setCursor(0,1);
      lcd.print("  Rate: ");
      lcd.print( l_minute );
      lcd.print(" L/M");
     
      flow_frequency = 0; // Reset Counter
      vol = 0;
    }
    else 
    { 
      l_minute=0.0;
      flow_frequency = 0; // Reset Counter
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Water Flow Rate ");
      lcd.setCursor(0,1);
      lcd.print("  Rate: ");
      lcd.print( l_minute );
      lcd.print(" L/M");
      
      analogWrite(PwmPin9 , 0);
      analogWrite(PwmPin10, 0);
      analogWrite(PwmPin11, 0);
    }
   }
}
/*******************************************************************************/
/*******************************************************************************/
void setup()
{
   tempSensorsInit(); /*Temp sensor Init*/
   PwmInit(); /*Init PWM*/
   ControlButtonInit();/*Init Buttons used POWER ON, RESTERT, UP, DOWN, OK */
   while(!digitalRead(start_button));/*START Button*/
   LcdInit(); /*logo and delay*/
   if(SetUserTemprature_Flag == 0) SetUserTemprature(); /* Set the user desired temp*/ 
   FlowMeterInit(); /*Init the flow meter and LCD*/
}

void loop ()
{
while((tempOuputSensorRead())< temp_connter)
{
  FlowMeterFunction(); /*Get the flow meter value */
  if(l_minute > 5.0)  /*check the flow rate till reach the setting value */ 
    {
      waterTempInput  = tempInputSensorRead();  /*read the temp of the Input Water*/
      waterTempOutput = tempOuputSensorRead(); /*read the temp of the Output Water*/
       
    lcd.clear();
      lcd.print("Input Water=");
    lcd.print(waterTempInput);
      lcd.print("C");
      lcd.setCursor(0,1);
      lcd.print("Output Water=");
    lcd.print(waterTempOutput);
      lcd.print("C");
    delay(1000);
    
      int data  = TempDiffranceRequired(waterTempOutput, waterTempInput); /*calc the temp diffrence between the desiered temp  and the water current temp */
      lcd.clear();
      lcd.print("Temp Diffrent = ");
      lcd.setCursor(8,1);
      lcd.print(data);
      lcd.print(" C");
      delay(1000);
     switch(data)
         {
          case 0:
                {
                  analogWrite(PwmPin9 , 255);
                  analogWrite(PwmPin10, 100);
                  analogWrite(PwmPin11, 50);
                  lcd.print((int)data);
                  lcd.print(" C"); 
                  break;
                 } 
          case 1:
                {
                  analogWrite(PwmPin9 , 250);
                  analogWrite(PwmPin10, 200);
                  analogWrite(PwmPin11, 50);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
          case 2:  
                {
                  analogWrite(PwmPin9 , 230);
                  analogWrite(PwmPin10, 5);
                  analogWrite(PwmPin11, 5);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
          case 3:
                {     
                  analogWrite(PwmPin9 , 220);
                  analogWrite(PwmPin10, 2);
                  analogWrite(PwmPin11, 5);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
          case 4:
                {
                  analogWrite(PwmPin9 , 200);
                  analogWrite(PwmPin10, 5);
                  analogWrite(PwmPin11, 5);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
          case 5:
                {
                  analogWrite(PwmPin9 , 180);
                  analogWrite(PwmPin11, 5);
                  analogWrite(PwmPin10, 5);
                  
                  lcd.print((int)data);
                  lcd.print(" C"); 
                  break;
                }
          case 6:
                {
                  analogWrite(PwmPin9 , 150);
                  analogWrite(PwmPin10, 5);
                  analogWrite(PwmPin11, 5);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
          case 7:
                {
                  analogWrite(PwmPin9 , 140);
                  analogWrite(PwmPin10, 245);
                  analogWrite(PwmPin11, 245);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
          case 8:
                {
                  analogWrite(PwmPin9 , 120);
                  analogWrite(PwmPin10, 245);
                  analogWrite(PwmPin11, 245);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  break;
                }
           case 9:
                {
                  analogWrite(PwmPin9 , 110);
                  analogWrite(PwmPin10, 180);
                  analogWrite(PwmPin11, 180);
        
                lcd.print((int)data);
                lcd.print(" C"); 
                  break;
                }
           case 10:
                {
                  analogWrite(PwmPin9 , 100);
                  analogWrite(PwmPin10, 170);
                  analogWrite(PwmPin11, 170);
        
                 lcd.print((int)data);
                 lcd.print(" C");
                  break;
                }
        default :
                 {
                  analogWrite(PwmPin9 , 0);
                  analogWrite(PwmPin10, 0);
                  analogWrite(PwmPin11, 0);
                  break;
                 } 
                 
         } 
    }
 } 
                  analogWrite(PwmPin9 , 0);
                  analogWrite(PwmPin10, 0);
                  analogWrite(PwmPin11, 0);
                  lcd.clear();
                  lcd.print(" Reach Desired ");
                  lcd.setCursor(0,1);
                  lcd.print("    Temp ");
                  lcd.print(tempOuputSensorRead());
                  lcd.print(" C");
                  
}