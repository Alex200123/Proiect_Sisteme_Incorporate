

#include "LiquidCrystal_I2C.h"
#include "DHT.h"

#define DHTPIN 3     
#define DHTTYPE DHT11

int buton = 2;//pin ptr intrerupere
int led_rosu = 0;

LiquidCrystal_I2C lcd(0x27,  16, 2);// SDA trebuie la A4, SCL la A5



int delay_max = 25;
int nr_citire = 15;
int interval_max = 1000;
int cal_citire = 2000/(float)nr_citire;
int prag_de_calculat_media = nr_citire*cal_citire;
float temp_max = 0;
float medie_temperatura = 0;
float medie_umiditate = 0;
float medie_temperatura_resimtita = 0;
float suma = 0;
float suma_umiditate = 0;
float suma_temp_resimtita = 0;
int afiseaza_temperatura = 0;



DHT dht(DHTPIN, DHTTYPE);

int citeste_date_senzor = 1;
int volatile aprins_stins  = 1; //SĂ MAI TESTEZ SĂ FIU SIGUR CĂ MERGE
int volatile contor = 0;
int increment_citire = 1;
int nan_detectat = 0;

void aprinde_stinge()
{
  aprins_stins  = !aprins_stins ; 
  contor = 0;
  digitalWrite(led_rosu, 0);
}

void calcul_temperatura()
{
  int calcul_incremental_pentru_citire = increment_citire*cal_citire;

  if(((contor) >= (calcul_incremental_pentru_citire) ) && contor != 0 && increment_citire <= nr_citire)
  {
    increment_citire++;
    float t = dht.readTemperature();
    float umid = dht.readHumidity();
    float temp_resimtita = dht.computeHeatIndex(t, umid, false);

    suma=suma +t;
    suma_umiditate=suma_umiditate + umid;
    suma_temp_resimtita=suma_temp_resimtita + temp_resimtita;
  }
  if(((contor) > (prag_de_calculat_media)) && (contor != 0))
  {
  
    medie_temperatura = suma/((float)nr_citire);
    medie_umiditate = suma_umiditate/((float)nr_citire);
    medie_temperatura_resimtita = suma_temp_resimtita/((float)nr_citire);

    if(medie_temperatura == medie_temperatura)//verifica daca avem NaN
    {
      nan_detectat = 0;
    }
    else
    {
      nan_detectat = 1;
    }

    
    if(temp_max < medie_temperatura)
    {
      temp_max = medie_temperatura;
    }
    
    digitalWrite(led_rosu, medie_temperatura > 50);
    suma = 0;
    suma_umiditate = 0;
    suma_temp_resimtita = 0;

    afiseaza_temperatura = 1;
    increment_citire = 1;
    contor = 0;
  }
}

void setup() {
  pinMode(led_rosu, OUTPUT);
  pinMode(buton, INPUT);
  attachInterrupt(digitalPinToInterrupt(buton), aprinde_stinge, RISING);
  lcd.init();
  dht.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Asteptati:");
}

void loop() {
  
  if(aprins_stins  == 1)
  {
    lcd.backlight();
    contor = contor + delay_max;
    calcul_temperatura();

    if(afiseaza_temperatura == 1)
    {
      lcd.clear();
      
      if(nan_detectat == 0)
      {
        lcd.setCursor(0,0);
        lcd.print("Max=");
        lcd.print(temp_max);
        lcd.print("U=");
        lcd.print(medie_umiditate);
        lcd.setCursor(0,1);
        lcd.print("Med=");
        lcd.print(medie_temperatura);
        lcd.print("R=");
        lcd.print(medie_temperatura_resimtita);
      }
      else
      {
         lcd.print("Valoare NAN");
      }

      afiseaza_temperatura = 0;
    }
  }
  else
  {
    lcd.noBacklight();
  }
    delay(delay_max);
}