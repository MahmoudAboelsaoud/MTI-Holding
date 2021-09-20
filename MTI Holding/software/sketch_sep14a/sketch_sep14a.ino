
/***********************************************/
/*Author  : Eng.Mahmoud Aboelsaoud             */
/*DATA    : 20 Aug 2021                        */
/*Version : V01                                */
/*Author  : MAXEL HEATER SOFTWARE              */
/***********************************************/

#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 6, 5, 4, 3);

signed long temptot = 0;
const long eventTime_tem = 2000;
unsigned long previousTime_tem = 0;


volatile int flow_frequency; // Measures flow sensor pulses Calculated litres/hour
float vol = 0.0, l_minute = 0.0;
unsigned char flowsensor  = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

unsigned char start_button  = 7;
unsigned char up_button     = A4;
unsigned char down_button   = A5;
unsigned char ok_button     = 8;
float temp_connter = 25; 

unsigned char SetUserTemprature_Flag = 0;

int tempSensor   = A1; // Assigning analog pin A5 to variable 'sensor'
float tempc      = 0.0; //variable to store temperature in degree Celsius
float vout       = 0.0;

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
  delay(2000);
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
void tempSensorInit()
{
  //pinMode(tempSensor,INPUT);
  /*analogReference(EXTERNAL);*/
  analogReference(INTERNAL);
}

void tempSensorRead()
{
  /*float temp = ((analogRead(tempSensor)/1023.0)*5000);
  float mv = temp/10;
  lcd.clear();
  lcd.print("   Water Temp ");
  lcd.setCursor(7,1);
  lcd.print((int)mv);
  lcd.print(" C");
  delay(500);*/
  
  vout = analogRead(tempSensor); //Reading the value from sensor
  vout = vout*1100/(1024*10); // Storing value in Degree Celsius
  tempc = vout;
  lcd.clear();
  lcd.print("   Water Temp ");
  lcd.setCursor(7,1);
  lcd.print((int)(vout));
  lcd.print(" C");
  delay(500);
  /*int x;
  unsigned long currentTime = millis();
  unsigned long temptot = 0;
  for(x=0;x<100;x++)
  {
    temptot+=analogRead(A1);
  }

  float sensorValue = temptot/100;
  float voltage = sensorValue *(1100/1023);
  float temp = voltage*0.1;
  if(currentTime - previousTime_tem >= eventTime_tem )
  {
     lcd.clear();
  lcd.print("   Water Temp ");
  lcd.setCursor(7,1);
  lcd.print(temp);
  lcd.print(" C");
  previousTime_tem = currentTime;
  }*/
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
 /* pinMode(PwmPin9 , OUTPUT); // declare pwm pin to be an output:
  pinMode(PwmPin10, OUTPUT); // declare pwm pin to be an output:
  pinMode(PwmPin11, OUTPUT); // declare pwm pin to be an output:*/
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
   //digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   //Serial.begin(9600);
   attachInterrupt(digitalPinToInterrupt(flowsensor), flow, FALLING); // Setup Interrupt /*FALLING*/
   //lcd.clear();
   //lcd.setCursor(0,0);
   //lcd.print("Water Flow Meter");
   //lcd.setCursor(0,1);
   currentTime = millis(); // millis function returns the number of milliseconds that your Arduino board has been powered up
   cloopTime = currentTime;
}

void FlowMeterFunction()
{
  /*set the output of the pwm to zero*/
  analogWrite(PwmPin9 , 0);
  analogWrite(PwmPin10, 0);
  analogWrite(PwmPin11, 0);
  
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
     /* lcd.setCursor(0,0);
      lcd.print("Rate: ");
      lcd.print(l_minute);
      lcd.print(" L/M");
      lcd.setCursor(0,1);
      vol = vol +l_minute;
      lcd.print("Vol :");
      lcd.print(vol);
      lcd.print(" L");*/
      flow_frequency = 0; // Reset Counter
      vol = 0;
      //Serial.print(l_minute, DEC); // Print litres/hour
      //Serial.println(" L/Sec");
    }
    else 
    { 
     // Serial.println(" flow rate = 0 ");
      flow_frequency = 0; // Reset Counter
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Water Flow Rate ");
      lcd.setCursor(0,1);
      lcd.print("  Rate: ");
      lcd.print( flow_frequency );
      lcd.print(" L/M");
      
      /*lcd.setCursor(0,0);
      lcd.print("Rate: ");
      lcd.print( flow_frequency );
      lcd.print(" L/M");
      lcd.setCursor(0,1);
      lcd.print("Vol :");
      lcd.print(vol);
      lcd.print(" L");*/
    }
   }
}
/*******************************************************************************/
/*******************************************************************************/
void setup()
{
   ControlButtonInit();/*Init Buttons used POWER ON, RESTERT, UP, DOWN, OK */
   while(!digitalRead(start_button));/*START Button*/
   LcdInit(); /*logo and delay*/
   tempSensorInit(); /*Temp sensor Init*/
   PwmInit(); /*Init PWM*/
   if(SetUserTemprature_Flag == 0) SetUserTemprature(); /* Set the user desired temp*/ 
   FlowMeterInit(); /*Init the flow meter and LCD*/
}

void loop ()
{
  FlowMeterFunction(); /*Get the flow meter value */
  if(l_minute > 5.0)  /*check the flow rate till reach the setting value */ 
    {
      tempSensorRead(); /*read the temp sensor value*/
      int data  = TempDiffranceRequired(temp_connter, tempc); /*calc the temp diffrence between the desiered temp  and the water current temp */
      lcd.clear();
      lcd.print("Temp Diffrent = ");
      lcd.setCursor(8,1);
      
      switch((int)data)
         {
          case 0:
                {
                  analogWrite(PwmPin9 , 0);
                  analogWrite(PwmPin10, 0);
                  analogWrite(PwmPin11, 0);
                  lcd.print((int)data);
                  lcd.print(" C"); 
                  delay(500);
                  break;
                 } 
          case 1:
                {
                  analogWrite(PwmPin9 , 50);
                  analogWrite(PwmPin10, 50);
                  analogWrite(PwmPin11, 50);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
                  break;
                }
          case 2:  
                {
                  analogWrite(PwmPin9 , 80);
                  analogWrite(PwmPin10, 80);
                  analogWrite(PwmPin11, 80);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
                  break;
                }
          case 3:
                {     
                  analogWrite(PwmPin9 , 110);
                  analogWrite(PwmPin10, 110);
                  analogWrite(PwmPin11, 110);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
                  break;
                }
          case 4:
                {
                  analogWrite(PwmPin9 , 140);
                  analogWrite(PwmPin10, 140);
                  analogWrite(PwmPin11, 140);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
                  break;
                }
          case 5:
                {
                 
                  /*lcd.print((int)data);
                  lcd.print(" C"); 
                  for(char i =0;i<255;i++)
                   {
                  analogWrite(9 , i);
                  delay(20);
                  analogWrite(11, i);
                  delay(20);
                  analogWrite(10, i);
                  delay(20);
                   }*/

                  analogWrite(PwmPin9 , 255);
                  analogWrite(PwmPin11, 127);
                  analogWrite(PwmPin10, 63);
                  
                  lcd.print((int)data);
                  lcd.print(" C"); 
                   delay(1000);
                  break;
                }
          case 6:
                {
                  analogWrite(PwmPin9 , 200);
                  analogWrite(PwmPin10, 200);
                  analogWrite(PwmPin11, 200);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
                  break;
                }
          case 7:
                {
                  analogWrite(PwmPin9 , 230);
                  analogWrite(PwmPin10, 230);
                  analogWrite(PwmPin11, 230);
        
                 lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
                  break;
                }
          case 8:
                {
                  analogWrite(PwmPin9 , 255);
                  analogWrite(PwmPin10, 255);
                  analogWrite(PwmPin11, 255);
        
                lcd.print((int)data);
                 lcd.print(" C"); 
                  delay(500);
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

