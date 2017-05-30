#include <U8glib.h>

#define Prad A2
//#define NapiecieKol A0
#define NapiecieBat A1

#define OLED_SCL A5
#define OLED_SDA A4

#define alertPin 10  //has PWM!

float moc;
float energia;
float IBat;
////float VKol;
float VBat;

float InIBat;
//float InVKol;
float InVBat;

//float tmpenergia;
int tmpInIBat;
int tmpInVBat;
//int tmpInVKol;

//float czasPracyWMin; // czas pracy wyrażony w minutach
// MAX wartość czasu 49dni 17h
byte czasPracyMin;  //czas pracy sterownika część: minut
byte czasPracyGodz; //czas pracy sterownika część: godzin
int czasPracyDni;   //czas pracy sterownika część: dni
byte czasPracySec; //czas pracy sterownika część: sekundy

unsigned long czas;
unsigned long previousCzas;

int i = 0;
byte timerRow = 10;
byte timerCol = 26;

int numCheck = 1000;
unsigned long uCtime;
byte previous = 0;

float switchVoltage = 12; //if battery voltage drop below this value it starts blinking diode! 
float minVoltage = 10.5; //min battery voltage from safety reason, this value must be lower than switchVoltage!
byte val; //temporary alert value for analogwrite
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

void setup() {
  // put your setup code here, to run once:

//  Serial.begin(115200);

  pinMode (Prad, INPUT);
  pinMode (NapiecieBat, INPUT);
  //pinMode (NapiecieKol, INPUT);
  pinMode(13, OUTPUT);
  pinMode(OLED_SCL, OUTPUT);
  pinMode(OLED_SDA, OUTPUT);

  pinMode(alertPin, OUTPUT);

  //if (u8g.getMode()==U8G_MODE_R3G3B2){u8g.setColorIndex(255);}
  //else if (u8g.getMode()== U8G_MODE_GRAY2BIT) { u8g.setColorIndex(3);}
  //else if (u8g.getMode() == U8G_MODE_BW) { u8g.setColorIndex(1);}
  //else if (u8g.getMode() == U8G_MODE_HICOLOR) { u8g.setHiColorByRGB(255,255,255);}
  //u8g.setFont(u8g_font_6x12);


}

void drawOLED ()
{
  u8g.setFont(u8g_font_6x12);
  //u8g.setFont(u8g_font_7x14);
  u8g.drawStr(0, 10, " PV ");

  //  u8g.setPrintPos(30,10);
  //  u8g.print(czas);
  //
  //  u8g.setPrintPos(6,30);
  //  u8g.print(millis());
  //
  //u8g.setFont(u8g_font_6x12);
  //dni
  u8g.setPrintPos(timerCol + 6, timerRow);
  u8g.print(czasPracyDni);
  u8g.drawStr(timerCol + 18, timerRow, "d ");

  //godziny
  u8g.setPrintPos(timerCol + 30, timerRow);
  u8g.print(czasPracyGodz);
  u8g.drawStr(timerCol + 42, timerRow, "g ");

  //minuty
  u8g.setPrintPos(timerCol + 54, timerRow);
  u8g.print(czasPracyMin);
  u8g.drawStr(timerCol + 66, timerRow, "m ");

  //sekundy
  //u8g.setFont(u8g_font_10x20);
  // u8g.setFont(u8g_font_osb18);

  u8g.setPrintPos(timerCol + 78, timerRow);
  u8g.print(czasPracySec);
  //   u8g.setFont(u8g_font_10x20);
  u8g.drawStr(timerCol + 90, timerRow, "s");


  u8g.drawStr(0, 25, " V Aku [V] "); //11
  u8g.setPrintPos(90, 25);
  u8g.print(VBat);

  u8g.drawStr(0, 38, " I Aku [A] "); //11
  u8g.setPrintPos(90, 38);
  u8g.print(IBat);

  u8g.drawStr(0, 50, " MOC [W] "); // 9
  u8g.setPrintPos(90, 50);
  u8g.print(moc);

  u8g.drawStr(0, 62, " E [kWh] "); //9
  u8g.setPrintPos(75, 62);
  u8g.print(energia, 5);
}

void timer ()
{
  // sprawdzić czy może nie dać czasów w unsigned long + rzutowanie po obliczeniu dzielenia na intiger
  //czasPracyWMin = millis()/60000;
//  uCtime = millis();

  uCtime = ((unsigned long)(millis() / 1000)); //ms->s
//  Serial.print("uCtime: ");
//  Serial.println(uCtime);
  uCtime = uCtime%2;
//  Serial.print("uCtime 2: ");
//  Serial.println(uCtime);
  if (previous!=uCtime) {
    previous = uCtime;

    czasPracySec = (czasPracySec + 1) % 60;
    if (!czasPracySec) {
      alert();
      //    czasPracyMin = (int)((uCtime / 60000) % 60); // ms ->s 1000 // s -> min 60
      czasPracyMin = (czasPracyMin + 1) % 60;
      if (!czasPracyMin) { 
        //      czasPracyGodz = (int)(uCtime / 3600000) % 24; //ms ->s 1000 //s->min 60 // min ->godz 60
        czasPracyGodz =(czasPracyGodz+1)%24;
        if (!czasPracyGodz) {
          //        czasPracyDni = ((int)(uCtime / 86400000)); // ms->s 1000 //s->min 60 // min->godz 60 // godz->dni 24
          czasPracyDni++;
        }
      }
    }
  } 
  
//  Serial.print("previous: ");
//  Serial.print(previous);
//  Serial.print("   uCtime: ");
//  Serial.print(uCtime);
//  Serial.print("   czasPracySec: ");
//  Serial.println(czasPracySec);

}

//if voltage is too low -> uC gives signals!
void alert(){
  if(VBat<minVoltage){
      val=255;
    }
  else if(VBat<switchVoltage){
    val = (abs(VBat - switchVoltage)/(switchVoltage-minVoltage))*255;
  }
  else{
    val=0;
  }
  analogWrite(alertPin, val);
}

void loop() {
  if (i < numCheck)
  { i++;
    tmpInIBat = analogRead(Prad);
    InIBat += tmpInIBat;

    tmpInVBat = analogRead(NapiecieBat);
    InVBat += tmpInVBat;

    //tmpInVKol = analogRead (NapiecieKol);
    //InVKol += tmpInVKol;

  }
  else {
    i = 0;

    InIBat = InIBat / numCheck; // i = max (100);
    InVBat = InVBat / numCheck;
    //  InVKol = InVKol/100;

    IBat = (0.048828125 * InIBat) - 25 + 0.07; //5/1024/0.1 Sensivity = 0.1V/A 2.5/0.1 = 25
    //                                          //0.07 błąd pomiaru!
    VBat = InVBat * 0.0146484375; //(5/1024)*(990Ohm/330Ohm);
    ////VKol = InVKol * 0.0146484375;//(5/1024)*(990Ohm/330Ohm); 990 Ohm = 330+330+330
    ////VKol = InVKol *0.0048828125 ;
    moc = VBat * IBat;
    //    Serial.print("moc: ");
    //    Serial.println(moc);
    czas = millis();
    if(czas<previousCzas){
      previousCzas=0;
    }

    previousCzas = czas-previousCzas;
    //    Serial.print("czas: ");
    //    Serial.println(czas);
    energia += ((moc * previousCzas) / 3600000000); //3600 - sek na h, 1000 bo kilo(waty), 1000 ms to s
    previousCzas = czas;
    //    Serial.print("energia: ");
    //    Serial.println(energia,8);
    //    Serial.println(czasPracySec);


    timer();
    
    u8g.firstPage();

    do
    { drawOLED();

    }
    while (u8g.nextPage());


    InIBat = 0;
    InVBat = 0;
    //  InVKol = 0;

    // Serial.println(czas);
    //  Serial.println(energia,10);
    //  Serial.println(moc);
    //  Serial.println(czas);
    //  Serial.println(czasPracyMin);
    //    Serial.println (" ");


    if (!digitalRead(13)) {
      digitalWrite(13, 1);
    }
    else if (digitalRead(13)) {
      digitalWrite(13, 0);
    }
  }
  //  Serial.println(IBat);
}

