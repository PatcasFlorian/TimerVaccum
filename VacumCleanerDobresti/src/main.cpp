/* ----- C Program for Arduino based Alarm Clock ---- */
#include <Arduino.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include<EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal_PCF8574.h>
#include <OneWire.h>
#define DS1307_I2C_ADDRESS 0x68
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 ori 0x3F for a 16 chars and 2 line display

DS1307 RTC;
int HOUR, MINUT,SECOND2,ZIUA,LUNA, meniu = 1;
int HOUR3, MINUT3, SECOND3,ziua3, luna, an, temp3 = 1;
//setare in-out Pini
int next = 10;
int inc = 11;
int startProgram = 12;
 volatile int coinPin = 3;
int inhibitCoin = 4;
int outputRell = 5;
//............................
//set variabile globale
int timeCoin = 0;
volatile int coin = 0 ;
unsigned int totalCoin;
const int limitCoin = 10;
volatile boolean insert = false;
// rezervare zone de memorie in eeprom
int totalCoins = 100,timeCoins = 110; 
// ...................................
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
void setDateDs1307(
  byte second,        // 0-59
  byte minute,        // 0-59
  byte hour,          // 1-23
  byte dayOfWeek,     // 1-7 1=Mon, 7=Sun
  byte dayOfMonth,    // 1-28/29/30/31
  byte month,         // 1-12
  byte year          // 0-99
)
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//interrupt
void coinInterrupt(void) {
 if(digitalRead(coinPin)==LOW){
     coin ++ ;
  insert = true;
  delay(21500);
 
 }
}
void setup() {
   Serial.begin(9600); 
   
  Wire.begin();
  sensors.begin();
  RTC.begin();
 //Set pin mode Arduino nano
  pinMode(inc, INPUT);
  pinMode(next, INPUT);
  pinMode(startProgram,INPUT);
  pinMode(coinPin, INPUT);
  pinMode(inhibitCoin, OUTPUT);
  pinMode(outputRell, OUTPUT);
  //Set val Pin Arduino nano
 digitalWrite(coinPin,HIGH);
  digitalWrite(inhibitCoin,LOW);
  digitalWrite(outputRell,LOW);
  digitalWrite(next, HIGH);
  digitalWrite(inc, HIGH);
  digitalWrite(startProgram,HIGH);
   attachInterrupt(digitalPinToInterrupt(3), coinInterrupt, LOW);
 
//set LCD instance
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH); 
  lcd.setCursor(0, 0);
  lcd.print("Auto VacuumCleaner ");
  lcd.setCursor(0, 1);
  lcd.print(" V 2.00EE ");
  delay(3000); lcd.clear();
  timeCoin = EEPROM.get(timeCoins,timeCoin);

}

//Meniu Program
void meniuprog(void)
{
  delay(600);
  meniu = 5;
  lcd.clear();
  while (meniu == 5) {
    meniu = 1;
           delay(400);      
         lcd.setCursor(0, 0);  
          lcd.print(" * SetProg/Timp  "  );
          
          lcd.setCursor(1, 1); lcd.print(" AfisConta");delay(400);  
          lcd.setCursor(1,0); lcd.blink();
 do{
        if (digitalRead(next) == LOW) { 
          delay(200);
          lcd.clear();lcd.noBlink();
          //coin_time();
           meniu = 2;
        } 
        if (digitalRead(inc) == LOW) 
          {
            delay(200);
          meniu = 2;
          lcd.clear();
          }
      }while(meniu==1);
       

 delay(400);
    
      lcd.print( " * AfisConta ");
      lcd.setCursor(1, 1);
      lcd.print("  SetData");lcd.setCursor(1,0); lcd.blink();
                                                                         
      
      do
      {

        if (digitalRead(next) == LOW) { 
          delay(200);lcd.noBlink();
          //  displayConta();
          meniu = 3;
          lcd.clear();
        }
           delay(200);
        if (digitalRead(inc) == LOW) {
          delay(200);
          meniu = 3;
           delay(200);
          lcd.clear();
        }
      } while (meniu == 2);
    
      lcd.print( " * SetData ");
      lcd.setCursor(2, 1); 
      lcd.print("  Exit");lcd.setCursor(1,0); lcd.blink(); delay(200);
      do
      {
     

        if (digitalRead(next) == LOW) {
          delay(200);
          lcd.noBlink();
          //timeDate();
          meniu = 4;
          lcd.clear();
        }
        delay(150);
        if (digitalRead(inc) == LOW) {
           delay(200);
          meniu = 4;
          lcd.clear();
         }
      } while (meniu == 3);
        
      lcd.print( " * EXIT");
      lcd.setCursor(0, 1); lcd.print("   MENIU ");lcd.setCursor(1,0); lcd.blink();
    
      do
    {
          if (digitalRead(next) == LOW)
        {
          lcd.noBlink();
          meniu = 1;
          delay(200);
          lcd.clear();
        }
        if (digitalRead(inc) == LOW) {
        delay(300);
          meniu = 5;
          lcd.clear();
     
      }
    } while (meniu == 4);
   
  } ;
 delay(200);
  
}

// activare releu vacuumCleaner
void vacuumCleaner(void){
  lcd.noBlink();
  if(coin == 0){
    lcd.print(" Credit 0");
    delay(1200);
    lcd.clear();
    return;
  }
   digitalWrite(outputRell,HIGH);
   totalCoin =  EEPROM.get(totalCoins,totalCoin);
  totalCoin = totalCoin+coin;
  EEPROM.put(totalCoins,totalCoin);
    digitalWrite(inhibitCoin,HIGH);        
  timeCoin = EEPROM.get(timeCoins,timeCoin);
   int intermediar=59;
  int totalTimeCoin = coin*timeCoin;
 byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  int hourStop,minutStop,secondStop;
 DateTime now = RTC.now();HOUR3=now.hour();MINUT3=now.minute();SECOND3=now.second(); 
 if((MINUT3+totalTimeCoin)>59){
  minutStop = totalTimeCoin-(60-MINUT3);
  secondStop = SECOND3;
 
 }
 if((MINUT3+totalTimeCoin)<59){
  minutStop=totalTimeCoin+MINUT3;
  secondStop=SECOND3;

 }
 if(coin>0){
   do{
 
  DateTime now = RTC.now();
  HOUR3=now.hour();MINUT3=now.minute();SECOND3=now.second(); 
  lcd.setCursor(0,0);
  lcd.print("TimpRamas : ");
  lcd.setCursor(0,1);
   lcd.print("Min :");
   if((totalTimeCoin-1)>=10){
     lcd.print(totalTimeCoin-1);
          }
          else{
             lcd.print("0");
            lcd.print(totalTimeCoin-1); 
          }      
   lcd.print(" :");
   if(intermediar>=10){
     lcd.print(intermediar);
   }
   else{
     lcd.print("0");
  lcd.print(intermediar);
   }
  lcd.print(" Sec");
  intermediar--;
    if(intermediar ==0){
    totalTimeCoin--;
   intermediar = 59;
   lcd.clear();
   }
    delay(1000);
     } while((totalTimeCoin>0)&&(intermediar>0));
 }
 lcd.clear();
    digitalWrite(inhibitCoin,LOW);
     digitalWrite(outputRell,LOW);
     coin = 0;
}

// afisare contabilitate
void displayConta(void){
  boolean isOk = true;
while(isOk){
  lcd.clear();
 lcd.print (" Total incasare :");
  lcd.setCursor(3,1);
 totalCoin =  EEPROM.get(totalCoins,totalCoin);
  lcd.print(totalCoin);
  lcd.print(" RON");
  delay(600);
  lcd.clear();
  lcd.print("Iesire =>Meniu");
  lcd.setCursor(0,1);
  lcd.print("Reset =>Next ");
  delay(600);
  if(digitalRead(next)==LOW){
    lcd.clear();
    lcd.print("Stergere conta....");
    EEPROM.put(totalCoins,0);
    }
    if(digitalRead(inc)==LOW){
      isOk = false;
      lcd.clear();
    }
}
}

// setare timp/coin
void coin_time(void){
  boolean isOk = true;
  
  timeCoin = EEPROM.get(timeCoins,timeCoin);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Timp/Ron");
  
  while(isOk){
    delay(250);
    if(digitalRead(inc)==LOW){
      delay(250);
      timeCoin ++;
      if(timeCoin > 5){
        timeCoin = 0;
      }
    }
    lcd.setCursor(2,1);
    lcd.print("1Ron = ");
    lcd.print(timeCoin);
    lcd.print(" minute");
    if(digitalRead(next)==LOW){
      EEPROM.put(timeCoins,timeCoin);
      isOk=false;
      lcd.clear();
    }
  }
}

/*Setare data si ora*/
 void timeDate(void){ 
  lcd.clear(); lcd.setCursor(0, 0);
  lcd.print("Set Time ");
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
 DateTime now = RTC.now();HOUR3=now.hour();MINUT3=now.minute();SECOND3=now.second();ziua3=now.day();luna=now.month();an=15;
  while (temp3 == 1)
  {delay (250);
    if (digitalRead(inc) == 0)
    {delay(100);
      HOUR3++;
      if (HOUR3 > 23)
      {
        HOUR3 = 0;
      }

    }

    lcd.setCursor(0, 0);
    lcd.print("Set Time ");
    //lcd.print(x);
    lcd.setCursor(0,1);
    if(HOUR3<10)
    {lcd.print("0");lcd.print(HOUR3);
      lcd.setCursor(2,1);
    lcd.print(":");
     lcd.setCursor(3,1);
    lcd.print(MINUT3);
    lcd.print(":");
     lcd.setCursor(6,1);
    lcd.print(SECOND3); lcd.setCursor(1,1);lcd.blink();     }
    
    else{
    lcd.print(HOUR3); 
    lcd.setCursor(2,1);
    lcd.print(":");
    lcd.setCursor(3,1);
    lcd.print(MINUT3);
    lcd.print(":");
    lcd.setCursor(6,1);
    lcd.print(SECOND3);
         lcd.setCursor(1,1);lcd.blink();                   }
    if (digitalRead(next) == 0)
    {lcd.noBlink();
      hour = HOUR3;
      temp3 = 2;
      while (digitalRead(next) == 0);
    }
  }

  while (temp3 == 2)
  {delay (200);
    if (digitalRead(inc) == 0)
    {delay(50);
      MINUT3 ++;
      if (MINUT3 > 59)
      {
        MINUT3 = 0;
      }
     
    }
 
    lcd.setCursor(0, 1);
    lcd.print(HOUR3);
    lcd.print(":");
     if(MINUT3<10)
     {lcd.print("0");lcd.print(MINUT3);
      lcd.setCursor(5, 1);lcd.print(":");
            lcd.print(SECOND3);lcd.setCursor(4,1);lcd.blink();}
     else{
    lcd.print(MINUT3);lcd.setCursor(3,1);
    lcd.setCursor(5, 1);
    lcd.print(":");
    lcd.print(SECOND3);lcd.setCursor(4,1);lcd.blink();
   }
    if (digitalRead(next) == 0)
    {
      lcd.noBlink();
      minute = MINUT3;
      lcd.clear();
      temp3 = 3;
      

    }
  }
  while (temp3 == 3)
  { delay (250);
    if (digitalRead(inc) == 0)
    {delay(250);
      ziua3++;
      if (ziua3 > 31)
      {
        ziua3 = 1;
      }
      
    }

    lcd.setCursor(0, 0);
    lcd.print("Set  Date ");
    //lcd.print(x);
    lcd.setCursor(0, 1);
    if(ziua3<10)
    {
      lcd.print("0");lcd.print(ziua3);
    lcd.setCursor(2,1);
    lcd.print(":");
                if(luna<10)
                      {lcd.print("0");lcd.print(luna);}
                              else{ lcd.print(luna);}
                                     lcd.print(":20");
                                       lcd.print(an);
                                                       }
    else{
    lcd.print(ziua3);
     lcd.setCursor(2,1);
    lcd.print(":");
            if(luna<10)
              {lcd.print("0");lcd.print(luna);}
                       else{  lcd.print(luna);}
                               lcd.print(":20");
                                 lcd.print(an);
                                                    }
                                                    lcd.setCursor(1,1);lcd.blink();
    if (digitalRead(next) == 0)
    {   
       lcd.noBlink();delay(200);
      dayOfMonth = ziua3;
      temp3 = 4;
      while (digitalRead(next) == 0);
    }
  }
  while (temp3 == 4)
  {delay (250);
    if (digitalRead(inc) == 0)
    {delay(50);
      luna++;
      if (luna > 12)
      {
        luna = 1;
      }
      
    }

    lcd.setCursor(0, 0);
    lcd.print("Set  Date ");
    lcd.setCursor(0, 1);
        if(ziua3<10)
              { 
                  lcd.print("0");lcd.print(ziua3); }
                         else{
                                lcd.print(ziua3);}
                                        lcd.print(":");
                          if(luna<10)
                                  {
                                     lcd.print("0");lcd.print(luna);  }
                                      else{ lcd.print(luna);}
                                                 lcd.setCursor(5,1);
                                                  lcd.print(":20");
                                                  lcd.print(an); 
                                                   lcd.setCursor(4,1);lcd.blink();
 
    if (digitalRead(next) == 0)
    {
      lcd.noBlink();
      month = luna;
      temp3 = 5;delay(200);
      
    }
  }
  an = 20;
  while (temp3 == 5)
  {delay (250);
    if (digitalRead(inc) == 0)
    { delay(50);
      an++;
      if (an >= 35)
      {
        an = 20;
      }
      
    }
    lcd.setCursor(0, 0);
    lcd.print("Set  Date ");
   
    lcd.setCursor(0, 1);
    if(ziua3<10)
              {lcd.print("0");lcd.print(ziua3);}
              else{
   lcd.print(ziua3);}
    lcd.print(":");
   if(luna<10)
              {lcd.print("0");lcd.print(luna);}
   else{ lcd.print(luna);}
    lcd.print(":20");
    lcd.print(an);lcd.setCursor(9,1);lcd.blink();
   
    if (digitalRead(next) == 0)
    {lcd.noBlink();delay(200);
      year = an;
      temp3 = 6;
    }
  

  } temp3 = 1;


  setDateDs1307(second, minute, hour, dayOfWeek, dayOfMonth,luna, an);
  if (!RTC.isrunning())
  {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  } lcd.clear();lcd.noBlink();
}


void loop() {
  
 
  //activare meniu reglaje
  if (digitalRead(inc) == LOW)
  {
   
    meniuprog();
  }
 
  //afisare contabilitate rapid
  if(digitalRead(next)==LOW){
      lcd.clear();
 lcd.print (" Total incasare :");
  lcd.setCursor(3,1);
 totalCoin =  EEPROM.get(totalCoins,totalCoin);
  lcd.print(totalCoin);
  lcd.print(" RON");
  delay(1200);
  lcd.clear();
  }

   
  if((digitalRead(coinPin) == HIGH)&&(coin==0)){
       sensors.requestTemperatures();
    DateTime now = RTC.now();
   // actualizare si afisare ora si data curenta
  int c = sensors.getTempCByIndex(0),
      SECOND2 = now.second(); HOUR = now.hour(); MINUT = now.minute();ZIUA=now.day();LUNA=now.month();
      lcd.setCursor(0, 0); 
 
      if (HOUR < 10)
  {
    lcd.print("0");
    lcd.print(HOUR);
  }
  else {
    lcd.print(HOUR);
  }
  lcd.print(":"); lcd.setCursor(3, 0);
  if (MINUT < 10)
  { lcd.print("0");
    lcd.print(MINUT);
  }
  else {
    lcd.print(MINUT);
  }
  lcd.print(":"); if ( SECOND2 < 10)
  {
    lcd.setCursor(6, 0);
    lcd.print("0");
    lcd.print(SECOND2);
  }
  else {
    lcd.print(SECOND2);
  }
  lcd.setCursor(8, 0); lcd.print (" T:");
  lcd.print(c); lcd.print((char)223); lcd.print("C");
lcd.write(byte(0));
  lcd.setCursor(0, 1);
   if(ZIUA<10)
    { lcd.print("0"); lcd.print(ZIUA);}
    else{
            lcd.print(ZIUA);}
  lcd.print("/");
  if(LUNA<10)
     { lcd.print("0"); lcd.print(LUNA);}
     else{
           lcd.print(LUNA);}
  lcd.print("/");
  lcd.print(now.year(), DEC);
 lcd.setCursor(15,0); lcd.blink();
     
}
  if( insert == true){
  lcd.clear();
  lcd.print("Credit = ");
  if(coin>=limitCoin){
    digitalWrite(inhibitCoin,HIGH);
        }
  else {
    digitalWrite(inhibitCoin,LOW);
  }
  lcd.print(coin);
  lcd.print(" Ron");
  lcd.setCursor(0,1);
  lcd.print("Timp: ");
  lcd.print(coin*timeCoin);
   lcd.print(" minute");
  insert = false;
     }
     //afisare credit
     if(coin>0){
      delay(25);
    lcd.setCursor(0,0);
  lcd.print("Credit = ");   
  lcd.print(coin);
  lcd.print(" Ron");
  lcd.setCursor(0,1);
  lcd.print("Timp: ");
  lcd.print(coin*timeCoin);
   lcd.print(" minute");
     }
      //activare program releu aspirator
  if(digitalRead(startProgram)==LOW){
    lcd.clear();
    vacuumCleaner();
  }
}
