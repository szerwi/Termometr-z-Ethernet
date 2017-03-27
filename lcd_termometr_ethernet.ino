#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <EtherCard.h>

#define czujnik A0 //termometr
#define czujruchu 2
#define niebieska 6 //
#define zielona 7   //diody
#define czerwona 5  //
#define diodaczuj 4 //
#define buttonczuj 15
#define histbutton 17
#define maxbutton 16
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,1,110 };                     //definiowanie ethernet
static byte gwip[] = { 192,168,1,1 };
byte Ethernet::buffer[500];
BufferFiller bfill;

int odczyt = 0;
float temperatura=0;
float wilgotnosc=0;
float tostatni = 0;
float tdwatemu = 0;
float ttrzytemu = 0;
float wostatni = 0;
float wdwatemu = 0;
float wtrzytemu = 0;
float maxtemp = 0;
float maxwilg = 0;
float mintemp = 0;
float minwilg = 0;
int i = 0;
int a = 0; //zmienna do petli
int czas = 0;
int stan = 1;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //definiowanie lcd

void maks() //przycisk maksymalne odczyty
{
  if(digitalRead(maxbutton) == LOW)
  {
    a=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Najwiekszy odczyt");
    lcd.setCursor(0,1);
    lcd.print(maxtemp);
    lcd.setCursor(6,1);
    lcd.print("C");
    lcd.setCursor (9,1);
    lcd.print(maxwilg);
    lcd.setCursor(15,1);
    lcd.print("%");
    for(a>0; a<=20; a++)
    {
      delay(250);
      if(digitalRead(maxbutton) == LOW)
      {
      a=0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Najmniejszy odczyt:");
      lcd.setCursor(0,1);
      lcd.print(mintemp);
      lcd.setCursor(6,1);
      lcd.print("C");
      lcd.setCursor (9,1);
      lcd.print(minwilg);
      lcd.setCursor(15,1);
      lcd.print("%");
      for(a>0; a<=20; a++)
      {
        delay(250);
      }
      }
    }
  }
}

 
void historia() //przycisk historia
  if(digitalRead(histbutton) == LOW)
  {
    a=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ostatni pomiar: ");
    lcd.setCursor(0,1);
    lcd.print(tostatni);
    lcd.setCursor(6,1);
    lcd.print("C");
    lcd.setCursor (9,1);
    lcd.print(wostatni);
    lcd.setCursor(15,1);
    lcd.print("%");
    for(a>0; a<=20; a++)
    {
      delay(250);
      if(digitalRead(histbutton) == LOW)
      {
        a=0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("2 pom. wczesniej: ");
        lcd.setCursor(0,1);
        lcd.print(tdwatemu);
        lcd.setCursor(6,1);
        lcd.print("C");
        lcd.setCursor (9,1);
        lcd.print(wdwatemu);
        lcd.setCursor(15,1);
        lcd.print("%");
        for(a>0; a<=20; a++)
          {
            delay(250);
    
          if(digitalRead(histbutton) == LOW)
          {
            a=0;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("3 pom. wczesniej: ");
            lcd.setCursor(0,1);
            lcd.print(ttrzytemu);
            lcd.setCursor(6,1);
            lcd.print("C");
            lcd.setCursor (9,1);
            lcd.print(wtrzytemu);
            lcd.setCursor(15,1);
            lcd.print("%");
            for(a>0; a<=20; a++)
            {
              delay(250);
            }
           }
          }
      }
    }
  }
}

void ruch() //wykrywanie ruchu i zapalanie wyswietlacza
{
  if(digitalRead(buttonczuj) == LOW) //wlacz / wylacz czujnik ruchu
  {
    lcd.backlight();
    if(stan==0)
    {
      delay(200);
      stan=1;
      digitalWrite(diodaczuj, HIGH);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" CZUJNIK RUCHU  ");
      lcd.setCursor(0,1);
      lcd.print("    WLACZONY   ");
      delay(1000);
    }
    else if(stan==1)
    {
      delay(200);
      stan=0;
      digitalWrite(diodaczuj, LOW);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" CZUJNIK RUCHU  ");
      lcd.setCursor(0,1);
      lcd.print("   WYLACZONY   ");
      delay(1000);
    }
  }
  if(stan==0)
  {
    lcd.backlight();
  }
  else if(stan==1)
  {
  if(digitalRead(czujruchu) == HIGH) //sprawdzanie stanu czujnika ruchu
  {
    czas=0;
    lcd.backlight();
  }
  if(czas>=2)
  {
    lcd.noBacklight();
    czas=0;
  }
  }
  
}


void wysylaniedanych()
{
    if (ether.packetLoop(ether.packetReceive())) 
    {
      
      word len = ether.packetReceive();
      word pos = ether.packetLoop(len);
      memcpy_P(ether.tcpOffset(), homePage(), sizeof homePage);
      ether.httpServerReply(homePage()); // send web page data
      delay(200);
    }
}


// Setup - bring Ethernet, LCD and DHT11 live
void setup () {
  Serial.begin(57600);
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
  Serial.println(F("Failed to access Ethernet controller"));
  ether.staticSetup(myip, gwip);
  lcd.begin(16,2);
  dht.begin();
  lcd.noBacklight();
  pinMode(niebieska, OUTPUT);
  pinMode(zielona, OUTPUT);
  pinMode(czerwona, OUTPUT);
  pinMode(diodaczuj, OUTPUT);
  pinMode(buttonczuj, INPUT_PULLUP);
  pinMode(histbutton, INPUT_PULLUP);
  pinMode(maxbutton, INPUT_PULLUP);
  digitalWrite(diodaczuj, HIGH);
  odczyt = analogRead(czujnik);
  temperatura = ((odczyt * 5.0) / 1024.0) * 100;
  wilgotnosc  = dht.readHumidity();
  minwilg = wilgotnosc;
  mintemp = temperatura;
  tostatni = temperatura;
  tdwatemu = temperatura;                   //ustawienie zmiennych na odczyt z czujnika
  ttrzytemu = temperatura;
  wostatni = wilgotnosc;
  wdwatemu = wilgotnosc;
  wtrzytemu = wilgotnosc;
}

//  Here we build a web page and pass the t and h values into it
static word homePage() //tworzenie strony internetowej
{
  int t = temperatura;
  int w = wilgotnosc;
  int treszta = (temperatura - t) * 10 + 1;
  
  int maxt = maxtemp;
  int mint = mintemp;
  int maxw = maxwilg;
  int minw = minwilg;
  int maxtreszta = (maxtemp - maxt) * 10 + 1;   //ulamki temperatur
  int mintreszta = (mintemp - mint) * 10 + 1;
  int tost = tostatni;
  int tdwa = tdwatemu;
  int ttrzy = ttrzytemu;                                    //zmiana float na int
  int wost = wostatni;
  int wdwa = wdwatemu;
  int wtrzy = wtrzytemu;
  int tostreszta = (tostatni - tost) * 10 + 1;
  int tdwareszta = (tdwatemu - tdwa) * 10 + 1;            //ulamki temperatur
  int ttrzyreszta = (ttrzytemu - ttrzy) * 10 + 1;

  if(treszta>=9){t = t + 1; treszta=0;}
  if(maxtreszta>=9){maxt = maxt + 1; maxtreszta=0;}
  if(mintreszta>=9){mint = mint + 1; mintreszta=0;}
  if(tostreszta>=9){tost = tost + 1; tostreszta=0;}       //ulamki temperatur    
  if(tdwareszta>=9){tdwa = tdwa + 1; tdwareszta=0;}
  if(ttrzyreszta>=9){ttrzy = ttrzy + 1; ttrzyreszta=0;}

  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
  "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='10'/>"
    "<title>Arduino termometr</title>" 
    "<body bgcolor=lightblue><center><h1>TERMOMETR</h1><font size=6><br>Aktualny pomiar: <br>Temperatura: $D.$D °C <br>Wilgotnosc: $D %</font>" //1, 2, 3
    "<br> <br>Najwiekszy pomiar temperatury: $D.$D °C <br>Najwiekszy pomiar wilgotnosci: $D % <br>" //4, 5, 6
    "<br>Najmniejszy pomiar temperatury: $D.$D °C <br>Najmniejszy pomiar wilgotnosci: $D %" // 7, 8, 9
    "<br> <br>Ostatni pomiar: $D.$D °C $D % <br>2 pomiary wczesniej: $D.$D °C $D % <br>3 pomiary wczesniej: $D.$D °C $D % <br> </center></body>"), // 10, 11, 12, 13, 14, 15, 16, 17, 18
    t, treszta, w, maxt, maxtreszta, maxw, mint, mintreszta, minw, tost, tostreszta, wost, tdwa, tdwareszta, wdwa, ttrzy, ttrzyreszta, wtrzy);
  return bfill.position();
}

void loop () {
historia(); ruch(); maks();
  i = 0;
  czas++;
  
  ttrzytemu = tdwatemu;   tdwatemu = tostatni;    tostatni = temperatura;
                                                                                //odswiezenie historii pomiarow
  wtrzytemu = wdwatemu;   wdwatemu = wostatni;    wostatni = wilgotnosc;

  if(temperatura>maxtemp) {maxtemp = temperatura;}
  if(temperatura<mintemp) {mintemp = temperatura;}
                                                                              //ustawienie maksymalnych i minimalnych odczytow
  if(wilgotnosc>maxwilg) {maxwilg = wilgotnosc;}
  if(wilgotnosc<minwilg) {minwilg = wilgotnosc;}
  
  odczyt = analogRead(czujnik);   temperatura = ((odczyt * 5.0) / 1064.0) * 100; //Zmiana przeskalowania (1024.0)
  wilgotnosc  = dht.readHumidity();                               //odczyt z czujnikow
  historia(); ruch(); maks(); wysylaniedanych();                  //sprawdzenie przyciskow, wysylanie danych na strone

  for(i>0; i<2; i++)
  {
  lcd.setCursor(0,0);   lcd.write("Temp.:            ");
  lcd.setCursor(7,0);   lcd.print(temperatura);
  lcd.setCursor(13,0);  lcd.print("C");                                 //wyswietlenie temperatury
  lcd.setCursor(0,1);   lcd.print ("Wilg.:           ");
  lcd.setCursor(7,1);   lcd.print (wilgotnosc);
  lcd.setCursor(13,1);  lcd.print("%");
  if(temperatura==tostatni)
  {
    digitalWrite(niebieska, LOW);
    digitalWrite(zielona, HIGH);
    digitalWrite(czerwona, LOW);
  }
  else if(temperatura>tostatni)
  {
    digitalWrite(niebieska, LOW);
    digitalWrite(zielona, LOW);                                     //obsluga diod
    digitalWrite(czerwona, HIGH);
  }
  else if(temperatura<tostatni)
  {
    digitalWrite(niebieska, HIGH);
    digitalWrite(zielona, LOW);
    digitalWrite(czerwona, LOW);
  }
  a=0;
  for(a>0; a<50; a++)
  {
  historia(); maks(); ruch(); wysylaniedanych(); delay(100);                //przerwa, sprawdzenie stanow przyciskow
  }
  
  lcd.setCursor(0,0);
  if(temperatura>tostatni) {lcd.print("     WZRASTA     ");}
  else if(temperatura<tostatni) {lcd.print("      SPADA     ");}
                                                                            //wyswietlanie informacji o stanie temperatury (wzrasta/spada)
  lcd.setCursor(0,1);  
  if(wilgotnosc>wostatni) {lcd.print("     WZRASTA     ");}
  else if(wilgotnosc<wostatni) {lcd.print("      SPADA     ");}

  a=0;
  for(a>0; a<10; a++)
  {
  historia(); ruch(); maks(); wysylaniedanych();                            //przerwa, sprawdzanie stanow przyciskow
  delay(50);
  }
  }
}

