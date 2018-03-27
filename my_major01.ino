//written for Arduino NANO by praveen kumar
#include <SoftwareSerial.h>                                       //hedder file for software serial
#include <TinyGPS++.h>                                            //for gps(conversion of NMEA format to understandable)
#include <MQ2.h>                                                  //for smoke sensor
#define RXPIN  4                                                  //
#define TXPIN  5                                                  //
#define L     A0
#define T     A1
#define SMOKE A2
MQ2 mq2(SMOKE);
char   str[100],msg[32],phh[100];
int    i=0,x=0,k=0,temp = 0,smk=0,lpg=0,co=0,n=0,p=0,b=0,count=0;
float  celcius=0.0;
String stringOne, stringTwo, stringThree,stringZero,psw;
String CO,LPG,SMK,replay,sendd,COO,LPGG,SMKK,cel,cell;
SoftwareSerial gpsSerial(RXPIN, TXPIN);                           //
TinyGPSPlus gps;                                                  //
void setup()
{
  gpsSerial.begin(9600);
  Serial.begin(9600);  
  mq2.begin();                                                   //
  gsm_init();
}


void loop()
{
  celcius = analogRead(T);
  celcius = (celcius*600)/1023.0;
  celcius = celcius/2;
  cel = (String)celcius;
  int li =analogRead(L);
  smk= mq2.readSmoke();
  SMK=(String)smk;
  lpg = mq2.readLPG(); 
  co  = mq2.readCO();
  LPG =(String)lpg;
  CO  =(String)co;
  LPGG="\nLPG="+LPG+"ppm";
  COO="\nCO="+CO+"ppm"; 
  SMKK="\nsmoke="+SMK+"ppm";
  cell = cel+"'C";
  if(li<50.0)SendMessage1();
  if(celcius>=32.0)SendMessage1();
  if(co>100.0)SendMessage1();
  while(gpsSerial.available() > 0) 
  gps.encode(gpsSerial.read()); 
  if (gps.location.isUpdated())
 {
  Serial.print("LAT=");  Serial.println(gps.location.lat(),6);
  Serial.print("LNG=");  Serial.println(gps.location.lng(),6);
  stringOne   = String(gps.location.lat(),8);
  stringTwo   = String(gps.location.lng(),8);
  stringThree = String();

  stringZero  = String("https://www.google.com/maps/search/?api=1&query=");
  stringThree = stringZero + stringOne + ',' + stringTwo+COO+LPGG+SMKK+"\ntemp="+cell;
  }
  
 if(temp == 0)RecieveMessage();
 for(unsigned int t=0;t<6000;t++)
  {
   serialEvent();
  if(temp==5)
  {
    x=0,k=0,temp=0;
    while(x<i)
    {
      while(str[x]=='#')
      {
        x++;
        while(str[x]!='*')
        {
          msg[k++]=str[x++];
        }
      }
      x++;
    }
    msg[k]='\0';
      Serial.println(msg);                               //prints received message on serial monitor
       if(p>0)                                           //this loop gets the phone number
        {
         for(i=0;i<13;i++)                               //including +91 total 13 chareters
         {
          phh[b++]=str[p++];
         }
        }
    
    //Serial.println(msg);
    psw= (String)msg;
    replay=(String)phh;
    delay(1000);
    if(psw == "getdata")SendMessage();
    p=0,i=0,x=0,k=0,n=0,b=0;
   }
  } 
 }

 void SendMessage()
{
  Serial.println("AT+CMGF=1");    
  delay(1000);  
  sendd=String("AT+CMGS=\"")+replay+String("\"\r"); 
  Serial.println(sendd);
  delay(1000);
  Serial.println("REQUEST: ");
  Serial.println(stringThree);
  delay(100);
  Serial.println((char)26);
  delay(1000);
  temp=0;
}
void SendMessage1()
{
  Serial.println("AT+CMGF=1");    
  delay(1000);  
  sendd=String("AT+CMGS=\"+91**********")+String("\"\r"); 
  delay(1000);
  Serial.println("ALERT: ");
  Serial.println(stringThree);
  
  delay(100);
  Serial.println((char)26);
  delay(1000);
  temp=0;
}

 void RecieveMessage()
 {
  Serial.println("AT+CNMI=2,2,0,0,0"); 
  delay(500);

  temp=2;
 
}
 
void serialEvent()
{
  while(Serial.available())
  {
    char ch=(char)Serial.read();
    str[i++]=ch;
       if(ch== '+')
    {
     if(i-n==7)
      {
        p=i-1,n=0;
      }  
      n=i;
    }
    if(ch == '*')
    {
      temp=5;
   
      Serial.println("Message Received");
      
    }
  }
}
void gsm_init()
{
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    delay(1000);
  }
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
      
    }
    delay(1000);
  }
  
  
}
