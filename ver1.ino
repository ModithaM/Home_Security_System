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
int buttonPin4 = 5;
int buttonPin5 = 6;
int buttonPin6 = 7;
int buttonPin7 = 8;
int buttonPin8 = 9;

int ledheart = 12; //heart beat led

int checkled = 13;

char SerialReadBuf[32];
int  SerialChar = 65;

//String MobNumber[] = { "0772628859", "0773116850", "0773684424", "0714897480", "0778297098", "0765404905", "0766595497", "0774982063", "0759969609", "0716231575", "0772222357", "0770899803", "0773467615", "0771013461", "0775068933", "0717750277",  "0714900405"};
String MobNumber1[10];
String MobNumber2[10];

char   charMobNumber[10];
String  TextMobNo;

int    MobNoLimit = 2;
//String ZoneName[] = {"Faci Block-C(G/R)","Prod Area(BD)","Prod Area (CP/Det)","F Good Area","Mezz Floor","Team 15-20 Area","F Good Area-Mezz","Faci Block-C(Kitch)"};

//String ZoneName[] = {"Facility Block-C(G/Room)","Production Area (BD)","Zone C","Zone D","Zone E","Zone F","Zone G","Zone H"};
String ZoneName[] = {"01- Facility Block-C(G/Room to Canteen)","02- Production Area (Beam Detectors)","03- Production Area (Call Points/Detectors)","04- Finished Goods Area","05- Mezzanine Floor","06- Team 15-20 Area","07- Finished Good Area-Mezzanine","08- Facility Block-C(Kitchen to Generator)"};

char   charMsg[120];

String  TextMsg;
int     DelaySec =0;
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

  pinMode(buttonPin1, INPUT); //input the buttonPin
  pinMode(buttonPin2, INPUT); //input the buttonPin
  pinMode(buttonPin3, INPUT); //input the buttonPin
  pinMode(buttonPin4, INPUT); //input the buttonPin
  pinMode(buttonPin5, INPUT); //input the buttonPin
  pinMode(buttonPin6, INPUT); //input the buttonPin
  pinMode(buttonPin7, INPUT); //input the buttonPin
  pinMode(buttonPin8, INPUT); //input the buttonPin
  
  pinMode(ledheart, OUTPUT);//heart led
  pinMode(checkled, OUTPUT);//CHECK led
  
  mySerial.begin(9600);    // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate 
  delay(100); 
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
  //------   Read EEPROM -------------------------------------
  for(i=0;i<10;i++)
  {
    MobNumber1[i] = "";
  }
  for(i=0;i<10;i++)
  {
    MobNumber1[0] += char(EEPROM.read(i));
    MobNumber1[1] += char(EEPROM.read(i+11));
    MobNumber1[2] += char(EEPROM.read(i+22)); 
    MobNumber1[3] += char(EEPROM.read(i+33));
    MobNumber1[4] += char(EEPROM.read(i+44));
    MobNumber1[5] += char(EEPROM.read(i+55)); 
    MobNumber1[6] += char(EEPROM.read(i+66));
    MobNumber1[7] += char(EEPROM.read(i+77)); 
    MobNumber1[8] += char(EEPROM.read(i+88));
    MobNumber1[9] += char(EEPROM.read(i+99)); 
  }
  for(i=0;i<10;i++)
  {
    Serial.println(MobNumber1[i]);
  }   
  for(i=0;i<10;i++)
  {
    MobNumber2[i] = "";
  }
  for(i=110;i<120;i++)
  {
    MobNumber2[0] += char(EEPROM.read(i));
    MobNumber2[1] += char(EEPROM.read(i+11));
    MobNumber2[2] += char(EEPROM.read(i+22)); 
    MobNumber2[3] += char(EEPROM.read(i+33));
    MobNumber2[4] += char(EEPROM.read(i+44));
    MobNumber2[5] += char(EEPROM.read(i+55)); 
    MobNumber2[6] += char(EEPROM.read(i+66));
    MobNumber2[7] += char(EEPROM.read(i+77)); 
    MobNumber2[8] += char(EEPROM.read(i+88));
    MobNumber2[9] += char(EEPROM.read(i+99)); 
  }
  for(i=0;i<10;i++)
  {
    Serial.println(MobNumber2[i]);
  }          
  
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
         
        for(i=0;i<8;i++)
        {           
          Serial.println("Checking Input : " + String(i));           
          if(CurrAlmState[i] == 1)
          {
              if(SMS_Sent[i] == 0)
              {
                 FunSendAlertSMS(i);        // Send Alert SMS
                 SMS_Sent[i] = 1;
              }
          }
          else
          {
             if(SMS_Sent[i] == 1)
              {
                 FunSendAlertClearSMS(i);      // Send Clear SMS
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
    //Serial.print("Sec Timer Working.: ");
    //Serial.println(DelaySec);
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
    else if((charMsg[1]=='1')&&(charMsg[2]=='3')&&(charMsg[3]=='7')&&(charMsg[4]=='9')&&(charMsg[5]=='G')&&(charMsg[6]=='2')&&(charMsg[7]=='?'))
    {
      Serial.println("View Group 2 Numbers");
      FunSendAllNumbersG2SMS();      
    }
    else if((charMsg[1]=='1')&&(charMsg[2]=='3')&&(charMsg[3]=='7')&&(charMsg[4]=='9')&&(charMsg[5]=='G')&&(charMsg[6]=='1')&&(charMsg[7]=='='))
    {
      Serial.println("Update Group 1 Numbers");
      for(i=0;i<110;i++)
      {
          EEPROM.write(i, charMsg[i+8]);
      }
      for(i=0;i<10;i++)
      {
        MobNumber1[i] = "";
      }
      for(i=0;i<10;i++)
      {
        MobNumber1[0] += char(EEPROM.read(i));
        MobNumber1[1] += char(EEPROM.read(i+11));
        MobNumber1[2] += char(EEPROM.read(i+22)); 
        MobNumber1[3] += char(EEPROM.read(i+33));
        MobNumber1[4] += char(EEPROM.read(i+44));
        MobNumber1[5] += char(EEPROM.read(i+55)); 
        MobNumber1[6] += char(EEPROM.read(i+66));
        MobNumber1[7] += char(EEPROM.read(i+77)); 
        MobNumber1[8] += char(EEPROM.read(i+88));
        MobNumber1[9] += char(EEPROM.read(i+99)); 
      }
      for(i=0;i<10;i++)
      {
        Serial.println(MobNumber1[i]);
      }
      FunSendAllNumbersG1SMS();       
    }
    else if((charMsg[1]=='1')&&(charMsg[2]=='3')&&(charMsg[3]=='7')&&(charMsg[4]=='9')&&(charMsg[5]=='G')&&(charMsg[6]=='2')&&(charMsg[7]=='='))
    {
      Serial.println("Update Group 2 Numbers");
      for(i=0;i<110;i++)
      {
          EEPROM.write(i+110, charMsg[i+8]);
      }
      for(i=0;i<10;i++)
      {
        MobNumber2[i] = "";
      }
      for(i=110;i<120;i++)
      {
        MobNumber2[0] += char(EEPROM.read(i));
        MobNumber2[1] += char(EEPROM.read(i+11));
        MobNumber2[2] += char(EEPROM.read(i+22)); 
        MobNumber2[3] += char(EEPROM.read(i+33));
        MobNumber2[4] += char(EEPROM.read(i+44));
        MobNumber2[5] += char(EEPROM.read(i+55)); 
        MobNumber2[6] += char(EEPROM.read(i+66));
        MobNumber2[7] += char(EEPROM.read(i+77)); 
        MobNumber2[8] += char(EEPROM.read(i+88));
        MobNumber2[9] += char(EEPROM.read(i+99)); 
      }
      for(i=0;i<10;i++)
      {
        Serial.println(MobNumber2[i]);
      } 
      FunSendAllNumbersG2SMS();      
    }

    else
    {
      Serial.println("No Category");
    }
}
//------------ ReadAlarmState Function --------------------
void FunReadAlarmState()
{
    //-- Send Alarm SMS to Mob numbers  
 
   //--------- Read Input 1 ---------------------------------   
    if(digitalRead(buttonPin1) == HIGH)
    {
      CurrAlmState[0] = 1;
      Serial.println("Input1 : " + String(CurrAlmState[0]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[0] = 0;
      Serial.println("Input1 : " + String(CurrAlmState[0]));
    }   
  
     //--------- Read Input 2 ---------------------------------   
  
    if(digitalRead(buttonPin2) == HIGH)
    {
      CurrAlmState[1] = 1;
      Serial.println("Input2 : " + String(CurrAlmState[1]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[1] = 0;
      Serial.println("Input2 : " + String(CurrAlmState[1]));
    } 
   
    //--------- Read Input 3 ---------------------------------   
    if(digitalRead(buttonPin3) == HIGH)
    {
      CurrAlmState[2] = 1;
      Serial.println("Input3 : " + String(CurrAlmState[2]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[2] = 0;
      Serial.println("Input3 : " + String(CurrAlmState[2]));
    }  
    
     //--------- Read Input 4 ---------------------------------   
    if(digitalRead(buttonPin4) == HIGH)
    {
      CurrAlmState[3] = 1;
      Serial.println("Input4 : " + String(CurrAlmState[3]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[3] = 0;
      Serial.println("Input4 : " + String(CurrAlmState[3]));
    } 
    
     //--------- Read Input 5 ---------------------------------   
    if(digitalRead(buttonPin5) == HIGH)
    {
      CurrAlmState[4] = 1;
      Serial.println("Input5 : " + String(CurrAlmState[4]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[4] = 0;
      Serial.println("Input5 : " + String(CurrAlmState[4]));
    } 
    
     //--------- Read Input 6 ---------------------------------   
    if(digitalRead(buttonPin6) == HIGH)
    {
      CurrAlmState[5] = 1;
      Serial.println("Input6 : " + String(CurrAlmState[5]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[5] = 0;
      Serial.println("Input6 : " + String(CurrAlmState[5]));
    } 
    
     //--------- Read Input 7 ---------------------------------   
    if(digitalRead(buttonPin7) == HIGH)
    {
      CurrAlmState[6] = 1;
      Serial.println("Input7 : " + String(CurrAlmState[6]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[6] = 0;
      Serial.println("Input7 : " + String(CurrAlmState[6]));
    } 
    
     //--------- Read Input 8 ---------------------------------   
    if(digitalRead(buttonPin8) == HIGH)
    {
      CurrAlmState[7] = 1;
      Serial.println("Input8 : " + String(CurrAlmState[7]));
      digitalWrite(checkled, HIGH);       //CHECK LED ON
      //while(digitalRead(buttonPin1) == HIGH);
       Serial.println("Arduno problem");    
    } 
    else
    {
      CurrAlmState[7] = 0;
      Serial.println("Input8 : " + String(CurrAlmState[7]));
    } 
   
}

//--------------------------Send Alert SMS Function -------------------    
 void FunSendAlertSMS(int tmpZoneNo)
{
    //check if the button is pressed, send text message 
     Serial.println("Send Alert SMS");
    TextMsg = "Fire Alarm Alert..!, Zone : " ;
    TextMsg = TextMsg + ZoneName[tmpZoneNo]; 
    //Serial.println(TextMsg);
    
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber1[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber1[list] + "\"\r"); //send sms message, insert your own phone number including the country code
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
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber2[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber2[list] + "\"\r"); //send sms message, insert your own phone number including the country code
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
    TextMsg = "Fire Alarm Alert Clear.., Zone : " ;
    TextMsg = TextMsg + ZoneName[tmpZoneNo]; 
    //Serial.println(TextMsg);
    
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber1[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber1[list] + "\"\r"); //send sms message, insert your own phone number including the country code
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
   for(int list=0; list < MobNoLimit; list++) 
   {       
      NoOfRcvdBytes = 0;
      TextMobNo = MobNumber2[list];
      Serial.println(TextMobNo);
      TextMobNo.toCharArray(charMobNumber, 10);
      if(charMobNumber[0] == '0')
      {
          Serial.println("Mobile Number Correct");
          mySerial.println("AT+CMGS=\"" + MobNumber2[list] + "\"\r"); //send sms message, insert your own phone number including the country code
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
    mySerial.println("AT+CMGS=\"" + MobNumber1[0] + "\"\r"); //send sms message, insert your own phone number including the country code
    delay(800);
    mySerial.print("*1379G1:");
    for(i=0;i<10;i++)
    {
        mySerial.print(MobNumber1[i]);
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
//--------------------------Send All SMS Numbers Group 2 -------------------    
void FunSendAllNumbersG2SMS()
{
    Serial.println("*1379G2:..... SENDING All NOS...");
    
    while(mySerial.available())   inChar = (char)mySerial.read();    
    mySerial.println("AT+CMGS=\"" + MobNumber1[0] + "\"\r"); //send sms message, insert your own phone number including the country code
    delay(800);
    mySerial.print("*1379G2:");
    for(i=0;i<10;i++)
    {
        mySerial.print(MobNumber2[i]);
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
  




