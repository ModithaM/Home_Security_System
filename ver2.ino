//test mystyle sky sms
/* use following pins

arduino           GSM Modem
D10  ------------>  GSMR
Ground  -------------> Ground

Arduino                Circuit                
D03 ----------------->   Data IN
5V ------------------>     5V
G ------------------->    G
*/
#include <SoftwareSerial.h>
#include <String.h>
#include <TimerOne.h>
#include <EEPROM.h>

SoftwareSerial mySerial(11,10); //7Rx, 8Tx

const int Read_Alarm_State  = 1;
const int Send_SMS          = 2;
const int Read_SMS          = 3;

int CurrState;
int CurrAlmState[8] ={0,0,0,0,0,0,0,0};
int  SMS_Sent[8]    ={0,0,0,0,0,0,0,0};
//---------------------------------
char  inChar;
int   NoOfRcvdBytes;

int buttonPin1 = 2;  //button pin
int buttonPin2 = 3;
int buttonPin3 = 4;

int Buzzer = 6;
int ledheart = 12; //heart beat led
int checkled = 13;
int intSystemStatus = 0;

char SerialReadBuf[32];
int  SerialChar = 65;

String MobNumber[] = { "0752628852", "0756238536"};
//String MobNumber1[10];
//String MobNumber2[10];

char   charMobNumber[10];
String  TextMobNo;

int    MobNoLimit = 2;
//String ZoneName[] = {"Faci Block-C(G/R)","Prod Area(BD)","Prod Area (CP/Det)","F Good Area","Mezz Floor","Team 15-20 Area","F Good Area-Mezz","Faci Block-C(Kitch)"};

//String ZoneName[] = {"Facility Block-C(G/Room)","Production Area (BD)","Zone C","Zone D","Zone E","Zone F","Zone G","Zone H"};
String ZoneName[] = {"01.Front Door","02.Back Door","03.Stores"};

char   charMsg[120];

String  TextMsg;
int     DelaySec =0;
int     DelayBuzzer = 0;
int     i;
int     PwCorrect =0;
int     DelaySMSread =0;

void setup() 
{
  delay(5000);                 // 10 Seconds Delay while turned ON Modem
  //Timer1.initialize(150000);
  Timer1.initialize(1000000);   //1 Second
  Timer1.attachInterrupt(funInterrupt);
  // put your setup code here, to run once:

  pinMode(buttonPin1, INPUT_PULLUP); //input the buttonPin
  pinMode(buttonPin2, INPUT_PULLUP); //input the buttonPin
  pinMode(buttonPin3, INPUT_PULLUP); //input the buttonPin

  pinMode(Buzzer, OUTPUT);//heart led
  pinMode(ledheart, OUTPUT);//heart led
  pinMode(checkled, OUTPUT);//CHECK led
  
  mySerial.begin(9600);    // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate 
  delay(1000); 
  //text mode gsm command
  mySerial.println("ATZ"); 
  DelaySec = 2;
  while(DelaySec !=0)
  {    
      if(mySerial.available())
      {
        inChar = (char)mySerial.read();
        Serial.print(inChar);
      }
  } 
  delay(1000);  
  mySerial.println("AT+CMGF=1");    //Because we want to send the SMS in text mode
  DelaySec = 2;
  while(DelaySec !=0)
  {    
      if(mySerial.available())
      {
        inChar = (char)mySerial.read();
        Serial.print(inChar);
      }
  }
    delay(1000);
    digitalWrite(Buzzer, HIGH);
    delay(500); 
    digitalWrite(Buzzer, LOW);
    delay(500); 
    digitalWrite(Buzzer, HIGH);
    delay(500); 
    digitalWrite(Buzzer, LOW);
    delay(500); 
    digitalWrite(Buzzer, HIGH);
    delay(500); 
    digitalWrite(Buzzer, LOW);
    delay(500); 

    //------------------------------------------------
    FunSendAckSMS("System Powered On");
    CurrState = Read_Alarm_State;
}

void loop()
{       
    //heart HB led
    digitalWrite(ledheart, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);               // wait for a second
    digitalWrite(ledheart, LOW);    // turn the LED off by making the voltage LOW
    delay(500);               // wait for a second
  
   switch (CurrState) 
   {
      case Read_Alarm_State:        //  Read Alarm State
        FunReadAlarmState();
        CurrState = Send_SMS;
        break;
        
      case Send_SMS:               // Send SMS function    
         
          for(i=0;i<3;i++)
          {           
            Serial.println("Checking Input : " + String(i));           
            if(CurrAlmState[i] == 0)
            {
                if(SMS_Sent[i] == 0)
                {
                  if(intSystemStatus == 1)  FunSendAlertSMS(i);        // Send Alert SMS
                  SMS_Sent[i] = 1;
                }
            }
            else
            {
              if(SMS_Sent[i] == 1)
                {
                    if(intSystemStatus == 1) FunSendAlertClearSMS(i);      // Send Clear SMS
                  SMS_Sent[i] = 0;
                }
            }
          }          
               
        CurrState = Read_SMS;        
        //delay(1500);
        break;
        
       case Read_SMS:
          if(DelaySMSread == 0)
          {
              DelaySMSread = 10;
               FunReadSMS();
          }
          // delay(10000);
          //do something when var equals 2
          CurrState = Read_Alarm_State;  
        break;   
        
      default:
        // if nothing else matches, do the default
        // default is optional
      break;
  }
}
//----------------------------------------------------
void funInterrupt()   // 1 Second interrupt
{
    if(DelaySec !=0)      DelaySec--;
    if(DelaySMSread !=0)  DelaySMSread--;
    if(DelayBuzzer != 0)  DelayBuzzer--;

    Serial.print("Alm Timer: ");
    Serial.println(DelayBuzzer);
    if(intSystemStatus == 1)
    {
        if(DelayBuzzer > 0)
        {
            digitalWrite(Buzzer, HIGH);
        }
        else
        {
            digitalWrite(Buzzer, LOW);
        }
    }
    else
    {
        digitalWrite(Buzzer, LOW);
    }    
}
//------------ Read SMS Function --------------------
void FunReadSMS()
{
    Serial.println("Read SMS....");
    for(i=0;i<120;i++) charMsg[i] = '0';
    while(mySerial.available())   inChar = (char)mySerial.read(); 
    
    mySerial.println("AT+CMGR=1"); //send sms message, insert your own phone number including the country code
    //Serial.println("AT+CMGR=1"); //send sms message, insert your own phone number including the country code
       
    DelaySec =5;
    NoOfRcvdBytes=0;
    PwCorrect = 0;   
    i=0;
    while(DelaySec !=0)
    {    
        if(mySerial.available())
        {
          inChar = (char)mySerial.read();
          if(inChar == '*') PwCorrect = 1;
          if(PwCorrect == 1)
          {
              if(i<120) charMsg[i] = inChar;  //String(inChar);
              i++;
          }
          Serial.print(inChar);
          NoOfRcvdBytes++;
        }
    }
    Serial.print("MSG: ");   
    Serial.print(charMsg);
    Serial.println("END");    
   
    //-------------- Delete Receved SMS ------------------------
    if(NoOfRcvdBytes > 80)
    {
        Serial.println("Delete SMS....");
        mySerial.println("AT+CMGD=1,4");
        //Serial.println("AT+CMGD=1");
        delay(100);
        mySerial.println((char)26);
        //Serial.println((char)26);    
            
        DelaySec =6;
        NoOfRcvdBytes=0;
        while(DelaySec !=0)
        {
            if(mySerial.available())
            {
              inChar = (char)mySerial.read();
              Serial.print(inChar);
              NoOfRcvdBytes++;
            }
        }
    }
    Serial.print("No Of Received Byte :");
    Serial.println(NoOfRcvdBytes);
    Serial.print("No Of Received i :");
    Serial.println(i);
   
    if((charMsg[1]=='1')&&(charMsg[2]=='3')&&(charMsg[3]=='7')&&(charMsg[4]=='9')&&(charMsg[5]=='G')&&(charMsg[6]=='1')&&(charMsg[7]=='?'))
    {
      Serial.println("View Group 1 Numbers");
      FunSendAllNumbersG1SMS();      
    } 
    else if((charMsg[1]=='A')&&(charMsg[2]=='l')&&(charMsg[3]=='e')&&(charMsg[4]=='x')&&(charMsg[5]=='a'))
    {
      Serial.println("I am working..");
      FunSendAckSMS("I am working..");      
    }
    else if((charMsg[1]=='S')&&(charMsg[2]=='y')&&(charMsg[3]=='s')&&(charMsg[5]=='o')&&(charMsg[6]=='n'))
    {
        intSystemStatus = 1;
        Serial.println("System Turned On");
        FunSendAckSMS("System Turned On");      
    }
    else if((charMsg[1]=='S')&&(charMsg[2]=='y')&&(charMsg[3]=='s')&&(charMsg[5]=='o')&&(charMsg[6]=='f')&&(charMsg[7]=='f'))
    {
        intSystemStatus = 0;
        Serial.println("System Turned Off");
        FunSendAckSMS("System Turned Off");      
    }
    else
    {
        Serial.println("No Category");
    }
}
//------------ ReadAlarmState Function --------------------
void FunReadAlarmState()
{
    //--------- Read Input 1 ---------------------------------   
    if(digitalRead(buttonPin1) == HIGH)
    {
      CurrAlmState[0] = 1;
      Serial.println("Input1 : " + String(CurrAlmState[0]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
    } 
    else
    {
        DelayBuzzer = 300;
        CurrAlmState[0] = 0;
        Serial.println("Input1 : " + String(CurrAlmState[0]));
    }   
    //--------- Read Input 2 ---------------------------------     
    if(digitalRead(buttonPin2) == HIGH)
    {
      CurrAlmState[1] = 1;
      Serial.println("Input2 : " + String(CurrAlmState[1]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
    } 
    else
    {
        DelayBuzzer = 300;
      CurrAlmState[1] = 0;
      Serial.println("Input2 : " + String(CurrAlmState[1]));
    }    
    //--------- Read Input 3 ---------------------------------   
    if(digitalRead(buttonPin3) == HIGH)
    {
      CurrAlmState[2] = 1;
      Serial.println("Input3 : " + String(CurrAlmState[2]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
    } 
    else
    {
        DelayBuzzer = 300;
        CurrAlmState[2] = 0;
        Serial.println("Input3 : " + String(CurrAlmState[2]));
    } 
}
//--------------------------Send Alert SMS Function -------------------    
 void FunSendAlertSMS(int tmpZoneNo)
{
    //check if the button is pressed, send text message 
     Serial.println("Send Alert SMS");
    TextMsg = "Thief Alert..!, Zone : " ;
    TextMsg = TextMsg + ZoneName[tmpZoneNo]; 
    //Serial.println(TextMsg);
    
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber[list] + "\"\r"); //send sms message, insert your own phone number including the country code
          //Serial.println("AT+CMGS=\"" + MobNumber1[list] + "\"\r"); //send sms message, insert your own phone number including the country code
          DelaySec = 2;
          while(DelaySec !=0)
          {    
              if(mySerial.available())
              {
                inChar = (char)mySerial.read();
                Serial.print(inChar);
              }
          }
          mySerial.println(TextMsg);//the content of the message
          delay(75); //d2
          mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
          DelaySec = 6;
          while(DelaySec !=0)
          {    
              if(mySerial.available())
              {
                inChar = (char)mySerial.read();
                Serial.print(inChar);
              }
          }       
      }
      else
      {
        Serial.println("Mobile Number Incorrect");
      }     
   }
    
}
//--------------------------Send Alert Clear SMS Function -------------------    
void FunSendAlertClearSMS(int tmpZoneNo)
{
    //check if the button is pressed, send text message 
     Serial.println("Send Alert Clear SMS");
    TextMsg = "Thief Alert Clear.., Zone : " ;
    TextMsg = TextMsg + ZoneName[tmpZoneNo]; 
    //Serial.println(TextMsg);
    
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber[list] + "\"\r"); //send sms message, insert your own phone number including the country code
          DelaySec = 2;
          while(DelaySec !=0)
          {    
              if(mySerial.available())
              {
                inChar = (char)mySerial.read();
                Serial.print(inChar);
              }
          }
          mySerial.println(TextMsg);//the content of the message
          delay(75);
          mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
          DelaySec = 6;
          while(DelaySec !=0)
          {    
              if(mySerial.available())
              {
                inChar = (char)mySerial.read();
                Serial.print(inChar);
              }
          }
      }
      else
      {
        Serial.println("Mobile Number Incorrect");
      }    
   }
     
}
//--------------------------Send All SMS Numbers Group 1 -------------------    
void FunSendAllNumbersG1SMS()
{
    Serial.println("*1379G1:..... SENDING All NOS...");
    
    while(mySerial.available())   inChar = (char)mySerial.read();    
    mySerial.println("AT+CMGS=\"" + MobNumber[0] + "\"\r"); //send sms message, insert your own phone number including the country code
    delay(800);
    mySerial.print("*1379G1:");
    for(i=0;i<10;i++)
    {
        mySerial.print(MobNumber[i]);
        mySerial.print(',');
    }       
    delay(100);
    mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
    Serial.println((char)26);//the ASCII code of the ctrl+z is 26   
    DelaySec = 6;
    while(DelaySec !=0)
    {    
        if(mySerial.available())
        {
          inChar = (char)mySerial.read();
          Serial.print(inChar);
        }
    }        
}
  
//--------------------------Send Acknoladge SMS -------------------    
void FunSendAckSMS(String strMessage)
{
    //check if the button is pressed, send text message 
    Serial.println("");
    Serial.print("Send Message :");
    Serial.println(strMessage);
    
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber[list] + "\"\r"); //send sms message, insert your own phone number including the country code
          //Serial.println("AT+CMGS=\"" + MobNumber1[list] + "\"\r"); //send sms message, insert your own phone number including the country code
          DelaySec = 2;
          while(DelaySec !=0)
          {    
              if(mySerial.available())
              {
                inChar = (char)mySerial.read();
                Serial.print(inChar);
              }
          }
          mySerial.println(strMessage);//the content of the message
          delay(75); //d2
          mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
          DelaySec = 6;
          while(DelaySec !=0)
          {    
              if(mySerial.available())
              {
                inChar = (char)mySerial.read();
                Serial.print(inChar);
              }
          }       
      }
      else
      {
        Serial.println("Mobile Number Incorrect");
      }     
   }    
}

