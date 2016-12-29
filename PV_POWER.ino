#include <U8glib.h>

#define Prad A2
//#define NapiecieKol A0
#define NapiecieBat A1

#define OLED_SCL A5
#define OLED_SDA A4

float moc;
float energia;
float IBat;
//float VKol;
float VBat;

float InIBat;
//float InVKol;
float InVBat;

float tmpenergia;
int tmpInIBat;
int tmpInVBat;
//int tmpInVKol;

//float czasPracyWMin; // czas pracy wyrażony w minutach
// MAX wartość czasu 49dni 17h
float czasPracyMin;  //czas pracy sterownika część: minut
float czasPracyGodz; //czas pracy sterownika część: godzin
float czasPracyDni;   //czas pracy sterownika część: dni
float czasPracySec; //czas pracy sterownika część: sekundy
int czas;
int i=0;

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);

pinMode (Prad, INPUT);
pinMode (NapiecieBat, INPUT);
//pinMode (NapiecieKol, INPUT);
pinMode(13,OUTPUT);
pinMode(OLED_SCL, OUTPUT);
pinMode(OLED_SDA, OUTPUT);

if (u8g.getMode()==U8G_MODE_R3G3B2){u8g.setColorIndex(255);}
else if (u8g.getMode()== U8G_MODE_GRAY2BIT) { u8g.setColorIndex(3);}
else if (u8g.getMode() == U8G_MODE_BW) { u8g.setColorIndex(1);}
else if (u8g.getMode() == U8G_MODE_HICOLOR) { u8g.setHiColorByRGB(255,255,255);}
u8g.setFont(u8g_font_6x12);


}

void drawOLED ()
{
  u8g.drawStr(0,10, " PV ");

  //dni
  u8g.setPrintPos(26,10);
  u8g.print(czasPracyDni,0);
  u8g.drawStr(38,10,"d ");

  //godziny
  u8g.setPrintPos(50,10);
  u8g.print(czasPracyGodz,0);
  u8g.drawStr(62,10,"g ");

  //minuty
  u8g.setPrintPos(74, 10);
  u8g.print(czasPracyMin,0);
  u8g.drawStr(86,10, "m ");

  //sekundy
  u8g.setPrintPos(98,10);
  u8g.print(czasPracySec,0);
  u8g.drawStr(110,10,"s ");
  
  
  u8g.drawStr(0,25, " V Aku [V] "); //11
  u8g.setPrintPos(90,25);
  u8g.print(VBat);
 
  u8g.drawStr(0,38, " I Aku [A] "); //11
  u8g.setPrintPos(90,38);
  u8g.print(IBat);
  
  u8g.drawStr(0,50, " MOC [W] "); // 9 
  u8g.setPrintPos(90,50);
  u8g.print(moc);
  
  u8g.drawStr(0,62, " E [kWh] ");  //9
  u8g.setPrintPos(75,62);
  u8g.print(energia,5);
}

void timer ()
{
//czasPracyWMin = millis()/60000;
czasPracyMin = millis()/60000%60; // ms ->s 1000 // s -> min 60
czasPracyGodz = millis()/3600000%24; //ms ->s 1000 //s->min 60 // min ->godz 60
czasPracyDni = millis()/86400000; // ms->s 1000 //s->min 60 // min->godz 60 // godz->dni 24
czasPracySec = millis()/1000%60; //ms->s 
}

void loop() {
  // put your main code here, to run repeatedly:


if (i<1000)
{ i++;
tmpInIBat = analogRead(Prad);
InIBat += tmpInIBat;

tmpInVBat = analogRead(NapiecieBat);
InVBat += tmpInVBat;

//tmpInVKol = analogRead (NapiecieKol);
//InVKol += tmpInVKol;

//u8g.firstPage();
//do 
//{u8g.drawStr (10,10, " LICZE");
//}
//while (u8g.nextPage());


}
else {
  i=0;
  czas = millis() - czas;
  InIBat = InIBat/1000; // i = max (100);
  InVBat = InVBat/1000;
//  InVKol = InVKol/100;
//    InIBat = tmpInIBat/100;
//    InVBat = tmpInVBat/100;
//    InVKol = tmpInVKol/100;


  
IBat = (0.048828125*InIBat) - 25+0.07; //5/1024/0.1 Sensivity = 0.1V/A 2.5/0.1 = 25 
                                          //0.07 błąd pomiaru!
VBat = InVBat * 0.0146484375; //(5/1024)*(990Ohm/330Ohm);
//VKol = InVKol * 0.0146484375;//(5/1024)*(990Ohm/330Ohm); 990 Ohm = 330+330+330
//VKol = InVKol *0.0048828125 ;
moc = VBat * IBat;
energia =(energia+ ((moc*czas)/3600000000)); //3600 - sek na h, 1000 bo kilo(waty), 1000 ms to s

timer();

u8g.firstPage();
do 
{drawOLED();
}
while (u8g.nextPage());

InIBat = 0;
  InVBat = 0;
 // InVKol = 0;

  
  
 // Serial.println(czas);
  

  Serial.println(energia,10);
  Serial.println(moc);
  Serial.println(czas);
  Serial.println(czasPracyMin);
    Serial.println (" ");

  czas = millis();
  if (!digitalRead(13)){digitalWrite(13,1); }
  else if (digitalRead(13)){digitalWrite(13,0);}
  }
//  Serial.println(IBat);
}

