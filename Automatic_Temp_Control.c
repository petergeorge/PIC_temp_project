/*
author peter george
date 16/10/2016
*/
char  keypadPort at PORTB;



sbit LCD_RS at LATC4_bit;
sbit LCD_EN at LATC5_bit;
sbit LCD_D4 at LATC0_bit;
sbit LCD_D5 at LATC1_bit;
sbit LCD_D6 at LATC2_bit;
sbit LCD_D7 at LATC3_bit;

sbit LCD_RS_Direction at TRISC4_bit;
sbit LCD_EN_Direction at TRISC5_bit;
sbit LCD_D4_Direction at TRISC0_bit;
sbit LCD_D5_Direction at TRISC1_bit;
sbit LCD_D6_Direction at TRISC2_bit;
sbit LCD_D7_Direction at TRISC3_bit;


#define HEATER PORTD.RD0
#define FAN PORTD.RD1
#define ENTER 15
#define CLEAR 13
#define ON 1
#define OFF 0

void main() {
  unsigned short kp,Txt[14];
  unsigned short Temp_Ref ;
  unsigned char inTemp;
  unsigned int temp;
  float mV, ActualTemp;
  
  Keypad_Init();                           // Initialize Keypad
  ANSELC = 0;                              // Configure PORTC as digital I/O
  ANSELB = 0;                              // Configure PORTB as digital I/O
  ANSELD = 0;                              // Configure PORTD as digital I/O
  TRISA0_bit = 1;                          //Configure AN0 (RA0) as input
  TRISC = 0;                               //PORTC are outputs (LCD)
  TRISD0_bit=0;                            //RD0 is output (Heater)
  TRISD1_bit=0;                            //RD1 is output (Fan)
  
  Lcd_Init();                              // Initialize LCD
  Lcd_Cmd(_LCD_CLEAR);                     // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);                // Cursor off
  Lcd_Out(1, 4, "Automatic");
  Lcd_Out(2, 2, "Temp Control");
  delay_ms(2000);                          //2s delay
  
  HEATER = OFF;
  FAN = OFF;

  //ON startup, read the Referance Temperature from the Keypad
        START:
       Lcd_Cmd(_LCD_CLEAR);                     // Clear display
       Lcd_Out(1, 1, "Enter Temp Ref");
       Temp_Ref=0;
        Lcd_Out(2, 1, "Temp Ref: ");
        while(1)
        {
         do
         kp = Keypad_Key_Click();             // Store key code in kp variable
         while (!kp);
         if ( kp == ENTER )break;
         if (kp > 3 && kp < 8) kp = kp-1;
         if (kp > 8 && kp < 12) kp = kp-2;
         if (kp ==14)kp = 0;
         if ( kp == CLEAR )goto START;
         Lcd_Chr_Cp(kp + '0');
         Temp_Ref =(10*Temp_Ref) + kp;
       }
      Lcd_Cmd(_LCD_CLEAR);                     // Clear display
      Lcd_Out(1, 1, "Temp Ref: ");
      intToStr( Temp_Ref,Txt);         //Convert to String
      inTemp=Ltrim(Txt);
       Lcd_Out_CP(inTemp);                  //Display Ref Temp
      Lcd_Out(2, 1, "Press # to Cont.");
      //Wait until # is pressed
      kp =0;
      while(kp!=ENTER)
      {
         do
           kp = Keypad_Key_Click();             // Store key code in kp variable
         while(!kp);
      }
       Lcd_Cmd(_LCD_CLEAR);              // Clear display
       
   Lcd_Out(1, 1, "Temp Ref: ");
   Lcd_Chr(1,15,223);                  // Different LCD displays have different
                                     //   char code for degree
  Lcd_Chr(1,16,'C');                  // Display "C" for Celsius
   //Program loop
  while(1) {
     //Display Referance Temperature and Actual Temperature
     temp = ADC_Read(0);               //Read temperature from AN0
     mV = temp * 5000.0/1024.0;        //Convert to mV
     ActualTemp = mV/10.0 ;              // Convert to degrees Celcius
     intToStr( Temp_Ref,Txt);         //Convert to String
     inTemp=Ltrim(Txt);
     //Lcd_Out(1, 1, "Temp Ref: ");
     Lcd_Out(1, 11, inTemp);        //Display Ref Temp
     Lcd_Out(2, 1, "Temp= ");
     FloatToStr(ActualTemp,Txt);         //Convert to string
     Txt[4] = 0;
     Lcd_Out(2,7,Txt);
     Lcd_Out(2,12,"   ");

      //Compare ref temp with actual emp
      if (Temp_Ref >  ActualTemp)  //If Temp Ref is less than actual Temp, Switch ON Heater
      {
      HEATER = ON,
      FAN = OFF;
      }
       if (Temp_Ref <  ActualTemp)  //If Temp Ref is greater than actual Temp, Switch ON Fan
      {
      HEATER = OFF,
      FAN = ON;
      }
      if (Temp_Ref ==  ActualTemp)  //If Temp Ref is equal to actual Temp, Switch OFF Fan and Heater
      {
      HEATER = OFF,
      FAN = OFF;
      }
      Delay_ms(10000);        //Wait 10 s then repeat
  }
}