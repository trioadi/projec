/* Revisi Terakhir dibuat pada tanggal 30 September 2017 
 * pukul 09.15
 * Created by : Trio Adi Pamungkas
 */

#include <DS3231.h> //RTC Library
#include <Wire.h>  // i2C Conection Library
#include <LiquidCrystal_I2C.h>  //i2C LCD Library
#include <EEPROM.h> //EEPROM
#include <Servo.h> //Servo Library
#include "HX711.h" //loadcell Library
#define calibration_factor_1 150 //nilai berdasarkan kalibrasi
#define calibration_factor_2 130 //nilai berdasarkan kalibrasi
#define calibration_factor_3 130 //nilai berdasarkan kalibrasi
#define DOUT_1 A1
#define CLK_1 A0
#define DOUT_2 A3
#define CLK_2 A2
#define DOUT_3 A6
#define CLK_3 A5

const int pin_up_jam = 22; //pin push button jam atas lcd1
const int pin_down_jam = 23; //pin push button jam bawah lcd1
const int pin_up_berat = 24; //pin push button berat atas lcd1
const int pin_down_berat = 25; //pin push button berat bawah lcd1
const int pin_ok = 26; //pin push button OK lcd1

const int pin_up_jam2 = 27; //pin push button jam atas lcd2
const int pin_down_jam2 = 29; //pin push button jam bawah lcd2
const int pin_up_berat2 = 28; //pin push button berat atas lcd2
const int pin_down_berat2 = 31; //pin push button berat bawah lcd2
const int pin_ok2 = 32; //pin push button OK lcd2

const int pin_up_jam3 = 33; //pin push button jam atas lcd3
const int pin_down_jam3 =34; //pin push button jam bawah lcd3
const int pin_up_berat3 = 35; //pin push button berat atas lcd3
const int pin_down_berat3 = 36; //pin push button berat bawah lcd3
const int pin_ok3 = 37; //pin push button OK lcd3

const int pin_servo_ats = 2; // pin servo ats
const int pin_servo = 5 ; // pin servo
const int pin_motor_sp = 3; // pin motor spiral
const int pin_motor_pe = 6; // pin motor pelontar
const int pin_pot = A4; 

int almt_1 = 1; //alamat eeprom 1, waktu lcd 1
int almt_2 = 2; //alamat eeprom 2, berat lcd 1
int almt_3 = 3; //alamat eeprom 3, waktu lcd 2
int almt_4 = 4; //alamat eeprom 4, berat lcd 2
int almt_5 = 5; //alamat eeprom 5, waktu lcd 3
int almt_6 = 6; //alamat eeprom 6, berat lcd 3

int nilae_1;
int nilae_2;
int nilae_3;

int nilae_1_brt;
int nilae_2_brt;
int nilae_3_brt;

int nilae_1sc ; //tampil nilai waktu pada lcd 1
int nilae_2sc ; //tampil nilai waktu pada lcd 2
int nilae_3sc ; //tampil nilai waktu pada lcd 3

int nilae_1sc_brt ; //tampil nilai berat pada lcd 1
int nilae_2sc_brt ; //tampil nilai berat pada lcd 2
int nilae_3sc_brt ; //tampil nilai berat pada lcd 3

int nil_pb_ok;
int nil_pb_ok2;
int nil_pb_ok3;

int nil_pb_ok_brt;
int nil_pb_ok_brt2;
int nil_pb_ok_brt3;

int waktu_status_pakan = 0;
int waktu_status_pakan2 = 0;
int waktu_status_pakan3 = 0;

int berat_status_pakan = 0;
int berat_status_pakan2 = 0;
int berat_status_pakan3 = 0;

int stop_counter = 0;

int nil_ep = 0;
int count = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long OnTime = 0;
const long OffTime = 5000; //kalibrasi waktu pengaktifan motor
int state_m1 = HIGH;
int state_msp = HIGH;
int state_sv = 0;
int state_sv_ats = 0;
int nil_pwm;

//loadcell
float units_1; //berat loadcell Pertama
float units_2; //Berat laodcell Kedua
float units_3; //Berat laodcell Kedua
float ounces_1;
float ounces_2;
float ounces_3;
float avrg; //Berat rata-rata ketiga loadcell
float berat_total; //Berat total

LiquidCrystal_I2C lcd(0x3F, 16, 2); // jadwal pakan 1
LiquidCrystal_I2C lcd2(0x27, 16, 2); // jadwal pakan 2
LiquidCrystal_I2C lcd3(0x25, 16, 2); // jadwal pakan 3
LiquidCrystal_I2C lcd4(0x11, 16, 2); // Penunjuk Waktu
DS3231 rtc(SDA, SCL);
Servo servo;
Servo servo_ats;
HX711 scale_1(DOUT_1, CLK_1);// HX711.DOUT pin #A1, HX711.PD_SCK pin #A0
HX711 scale_2(DOUT_2, CLK_2);// HX711.DOUT pin #A3, HX711.PD_SCK pin #A2
HX711 scale_3(DOUT_3, CLK_3); //HX711.DOUT pin #A6, HX711.PD_SCK pin #A5

Time  t; //pencacah string time()
uint8_t hh = 0, mm = 0, ss = 0, dd = 0, bb = 0;

void tampilan_lcd() {
//Konfiguasi LCD 1
 lcd.setCursor(0,0);
 lcd.print("JAM");
 lcd.setCursor(5,0);
 lcd.print("BERAT");
 lcd.setCursor(14,0);
 lcd.print("OK");
  
 lcd.setCursor(0,1);
 lcd.print(nilae_1sc);
 lcd.setCursor(5,1);
 lcd.print(nilae_1sc_brt);
 lcd.setCursor(8,1);
 lcd.print("KG");
 
 //Konfiguasi LCD 2
 lcd2.setCursor(0,0);
 lcd2.print("JAM");
 lcd2.setCursor(5,0);
 lcd2.print("BERAT");
 lcd2.setCursor(14,0);
 lcd2.print("OK");
  
 lcd2.setCursor(0,1);
 lcd2.print(nilae_2sc);
 lcd2.setCursor(5,1);
 lcd2.print(nilae_2sc_brt);
 lcd2.setCursor(8,1);
 lcd2.print("KG");

 //Konfiguasi LCD 3
 lcd3.setCursor(0,0);
 lcd3.print("JAM");
 lcd3.setCursor(5,0);
 lcd3.print("BERAT");
 lcd3.setCursor(14,0);
 lcd3.print("OK");
  
 lcd3.setCursor(0,1);
 lcd3.print(nilae_3sc);
 lcd3.setCursor(5,1);
 lcd3.print(nilae_3sc_brt);
 lcd3.setCursor(8,1);
 lcd3.print("KG");

 //konfigurasi LCD 4
 lcd4.setCursor(1,0);
 lcd4.print(rtc.getTimeStr());
 
 lcd4.setCursor(11,0);
 lcd4.print(rtc.getTemp());

 lcd4.setCursor(0,1);
 lcd4.print(rtc.getDateStr());
 lcd4.print("  ");
 lcd4.print((char)223);
 lcd4.print("C");

}

void status_tombol(){
  
//Perintah Push Button Untuk LCD 1=====================================
//Pengaturan tombol atas waktu pemberian pakan
int stat_pb_up = digitalRead(pin_up_jam);
int stat_pb_down = digitalRead(pin_down_jam);
nilae_1sc = waktu_status_pakan+nilae_1;
  if(stat_pb_up == HIGH){
    if(nilae_1sc == 24){
      nilae_1sc = 1;
      waktu_status_pakan = 0;
      nilae_1 = 0;
      lcd.clear();
    }
    waktu_status_pakan++;
  }
  
  //Pengaturan tombol bawah waktu pemberian pakan
  if(stat_pb_down == HIGH){
    if(nilae_1sc==1){
      nilae_1sc=24;
      nilae_1=0;
      waktu_status_pakan=25;
    }else if (nilae_1sc==10){
      nilae_1sc=9;
      lcd.clear();
    }
    waktu_status_pakan--;
  }

//Pengaturan tombol atas pemilihan berat pakan   
int stat_pb_up_brt = digitalRead(pin_up_berat);
int stat_pb_down_brt = digitalRead(pin_down_berat);
nilae_1sc_brt = berat_status_pakan+nilae_1_brt;
  if(stat_pb_up_brt == HIGH){
    if(nilae_1sc_brt == 20){
      nilae_1sc_brt = 0;
      berat_status_pakan = 0;
      nilae_1_brt = 0;
      lcd.clear();
    }
    berat_status_pakan++;
  }
  
  //Pengaturan tombol bawah pemilihan berat pakan
  if(stat_pb_down_brt == HIGH){
    if(nilae_1sc_brt == 0){
      nilae_1sc_brt = 20;
      nilae_1_brt = 0;
      berat_status_pakan = 21;
    }else if(nilae_1sc_brt ==10){
      nilae_1sc_brt = 9;
      lcd.clear();
    }
    berat_status_pakan--;
  }

//Pengaturan tombol OK LCD 1
int stat_pb_ok = digitalRead(pin_ok);
  if( stat_pb_ok == HIGH){
    nil_pb_ok = nilae_1sc ;
    nil_pb_ok_brt = nilae_1sc_brt;
    simpan_eeprom();
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Data Tersimpan");
    delay(1000);
    lcd.clear();
  }
  
//Perintah Push Button Untuk LCD 2=======================================
//Pengaturan tombol atas waktu pemberian pakan
int stat_pb_up2 = digitalRead(pin_up_jam2);
int stat_pb_down2 = digitalRead(pin_down_jam2);
nilae_2sc = waktu_status_pakan2+nilae_2;
  if((stat_pb_up2 == HIGH)&&(stop_counter == true)){
    stop_counter = false;
    if(nilae_2sc == 24){
      nilae_2sc = 1;
      waktu_status_pakan2 = 0;
      nilae_2 = 0;
      lcd2.clear();
    }
   waktu_status_pakan2 ++;
  }else if((stat_pb_up == LOW)&&(stop_counter == false)){
    stop_counter = true;
  }

  //Pengaturan tombol bawah waktu pemberian pakan
  if(stat_pb_down2 == HIGH){
    if(nilae_2sc==1){
      nilae_2sc=24;
      nilae_2=0;
      waktu_status_pakan2=25;
    }else if (nilae_2sc==10){
      nilae_2sc=9;
      lcd2.clear();
    }
    waktu_status_pakan2--;
  }

//Pengaturan tombol atas pemilihan berat pakan   
int stat_pb_up_brt2 = digitalRead(pin_up_berat2);
int stat_pb_down_brt2 = digitalRead(pin_down_berat2);
nilae_2sc_brt = berat_status_pakan2 + nilae_2_brt;
  if(stat_pb_up_brt2 == HIGH){
    if(nilae_2sc_brt == 20){
      nilae_2sc_brt = 0;
      berat_status_pakan2 = 0;
      nilae_2_brt = 0;
      lcd2.clear();
    }
    berat_status_pakan2 ++;
  }
  
  //Pengaturan tombol bawah pemilihan berat pakan
  if(stat_pb_down_brt2 == HIGH){
    if(nilae_2sc_brt == 0){
      nilae_2sc_brt = 20;
      nilae_2_brt = 0;
      berat_status_pakan2 = 21;
    }else if(nilae_2sc_brt ==10){
      nilae_2sc_brt = 9;
      lcd2.clear();
    }
    berat_status_pakan2 --;
  }

//Pengaturan tombol OK LCD 2
int stat_pb_ok2 = digitalRead(pin_ok2);
  if( stat_pb_ok2 == HIGH){
    nil_pb_ok2 = nilae_2sc ;
    nil_pb_ok_brt2 = nilae_2sc_brt;
    simpan_eeprom2();
    lcd2.clear();
    lcd2.setCursor(1,0);
    lcd2.print("Data Tersimpan");
    delay(1000);
    lcd2.clear();
  }
//Perintah Push Button Untuk LCD 3=====================================
//Pengaturan tombol atas waktu pemberian pakan
int stat_pb_up3 = digitalRead(pin_up_jam3);
int stat_pb_down3 = digitalRead(pin_down_jam3);
nilae_3sc = waktu_status_pakan3+nilae_3;
  if(stat_pb_up3 == HIGH){
    if(nilae_3sc == 24){
      nilae_3sc = 1;
      waktu_status_pakan3 = 0;
      nilae_3 = 0;
      lcd3.clear();
    }
   waktu_status_pakan3 ++;
  }

  //Pengaturan tombol bawah waktu pemberian pakan
  if(stat_pb_down3 == HIGH){
    if(nilae_3sc==1){
      nilae_3sc=24;
      nilae_3=0;
      waktu_status_pakan3=25;
    }else if (nilae_3sc==10){
      nilae_3sc=9;
      lcd3.clear();
    }
    waktu_status_pakan3--;
  }

//Pengaturan tombol atas pemilihan berat pakan   
int stat_pb_up_brt3 = digitalRead(pin_up_berat3);
int stat_pb_down_brt3 = digitalRead(pin_down_berat3);
nilae_3sc_brt = berat_status_pakan3 + nilae_3_brt;
  if(stat_pb_up_brt3 == HIGH){
    if(nilae_3sc_brt == 20){
      nilae_3sc_brt = 0;
      berat_status_pakan3 = 0;
      nilae_3_brt = 0;
      lcd3.clear();
    }
    berat_status_pakan3 ++;
  }
  
  //Pengaturan tombol bawah pemilihan berat pakan
  if(stat_pb_down_brt3 == HIGH){
    if(nilae_3sc_brt == 0){
      nilae_3sc_brt = 20;
      nilae_3_brt = 0;
      berat_status_pakan3 = 21;
    }else if(nilae_3sc_brt ==10){
      nilae_3sc_brt = 9;
      lcd3.clear();
    }
    berat_status_pakan3 --;
  }

//Pengaturan tombol OK LCD 3
int stat_pb_ok3 = digitalRead(pin_ok3);
  if( stat_pb_ok3 == HIGH){
    nil_pb_ok3 = nilae_3sc ;
    nil_pb_ok_brt3 = nilae_3sc_brt;
    simpan_eeprom3();
    lcd3.clear();
    lcd3.setCursor(1,0);
    lcd3.print("Data Tersimpan");
    delay(1000);
    lcd3.clear();
  }
 delay(50);
}

/*membaca jam pada eeprom dan pada rtc, jika hasil pembacaan sama
maka motor akan bekerja*/
void alarm(){
int nilae_talm1 = EEPROM.read(almt_1); 
int nilae_talm2 = EEPROM.read(almt_3);
int nilae_talm3 = EEPROM.read(almt_5);
  t = rtc.getTime();
  hh = t.hour,DEC;
   if((hh == nilae_talm1) && (nilae_1_brt !=0)){
    nil_ep = EEPROM.read(almt_2);
    motor();
  }else if((hh == nilae_talm2)&&(nilae_2_brt !=0)){
    nil_ep = EEPROM.read(almt_4);
    motor();
  }else if((hh == nilae_talm3)&&(nilae_3_brt !=0)){
    nil_ep = EEPROM.read(almt_6);
    motor();
  }else{
    servo_ats.write(0);
    servo.write(0);
    digitalWrite(pin_motor_sp, HIGH);
    analogWrite(pin_motor_pe, 0);  
  }
}

void loadcell(){
  Serial.print("Reading 1: ");
  units_1 = scale_1.get_units(), 10;
  if (units_1 < 0)
  {
    units_1 = 0.00;
  }
  ounces_1 = units_1 * 0.035274;
  Serial.print(units_1);
  Serial.print(" grams"); 
  Serial.print(" calibration factor 1: ");
  Serial.print(calibration_factor_1);
  Serial.println();
  
  //loadcell kedua
  Serial.print("Reading 2: ");
  units_2 = scale_2.get_units(), 10;
  if (units_2 < 0)
  {
    units_2 = 0.00;
  }
  ounces_2 = units_2 * 0.035274;
  Serial.print(units_2);
  Serial.print(" grams"); 
  Serial.print(" calibration factor 2: ");
  Serial.print(calibration_factor_2);
  Serial.println();

  //loadcell ketiga
  Serial.print("Reading 3: ");
  units_3 = scale_3.get_units(), 10;
  if (units_3 < 0)
  {
    units_3 = 0.00;
  }
  ounces_3 = units_3 * 0.035274;
  Serial.print(units_3);
  Serial.print(" grams"); 
  Serial.print(" calibration factor 3: ");
  Serial.print(calibration_factor_3);
  Serial.println();

  //Jumlah rata-rata
  avrg = ((units_1+units_2+units_3)/3);
  berat_total = (units_1+units_2+units_3);
  Serial.print("Jumlah rata-rata: ");
  Serial.print(avrg);
  Serial.print(" grams");
  Serial.print("Jumlah total: ");
  Serial.print(berat_total);
  Serial.print(" grams");
  Serial.println();
}

void motor(){
int y = nil_ep;
while (count <y){     
 if((avrg <= 100)&&(currentMillis - previousMillis >= OffTime)){
     if(state_sv != 0){
      count++;
     }
      state_sv_ats = 0;
      state_sv = 0;
      state_msp = HIGH;
      nil_pwm = 0;
 }else if((avrg >= 1000)&&(currentMillis - previousMillis >= OnTime)){
  previousMillis = currentMillis;
        state_sv_ats = 90;
        state_sv = 90;
        state_msp = LOW;
        nil_pwm = analogRead(pin_pot)/4;
        }
     break;
     }if(count==nil_ep){
      state_sv_ats = 0;
      state_sv = 0;
      state_msp = HIGH;
      nil_pwm = 0;
      loop();
     }
  servo_ats.write(state_sv_ats);
  digitalWrite(pin_motor_sp, state_msp);
  servo.write(state_sv);
  analogWrite(pin_motor_pe, nil_pwm);  
}

//baris program penyimpanan eeprom====================
void simpan_eeprom(){
  int val = nil_pb_ok;
  int val_brt = nil_pb_ok_brt;
  EEPROM.write(almt_1, (byte)val); 
  EEPROM.write(almt_2, (byte)val_brt);
}

void simpan_eeprom2(){
  int val = nil_pb_ok2;
  int val_brt = nil_pb_ok_brt2;
  EEPROM.write(almt_3, (byte)val); 
  EEPROM.write(almt_4, (byte)val_brt);
}

void simpan_eeprom3(){
  int val = nil_pb_ok3;
  int val_brt = nil_pb_ok_brt3;
  EEPROM.write(almt_5, (byte)val); 
  EEPROM.write(almt_6, (byte)val_brt);
}

//baris program pembacaan eeprom
void baca_eeprom(){
nilae_1 = EEPROM.read(almt_1);
nilae_1_brt = EEPROM.read(almt_2); 

nilae_2 = EEPROM.read(almt_3);
nilae_2_brt = EEPROM.read(almt_4);

nilae_3 = EEPROM.read(almt_5);
nilae_3_brt = EEPROM.read(almt_6);
}

void setup() {
Serial.begin(9600);
pinMode (pin_up_jam, INPUT);
pinMode (pin_down_jam, INPUT);
pinMode (pin_up_berat, INPUT);
pinMode (pin_down_berat, INPUT);
pinMode (pin_ok, INPUT);
pinMode (pin_up_jam2, INPUT);
pinMode (pin_down_jam2, INPUT);
pinMode (pin_up_berat2, INPUT);
pinMode (pin_down_berat2, INPUT);
pinMode (pin_ok2, INPUT);
pinMode (pin_up_jam3, INPUT);
pinMode (pin_down_jam3, INPUT);
pinMode (pin_up_berat3, INPUT);
pinMode (pin_down_berat3, INPUT);
pinMode (pin_ok3, INPUT);
pinMode (pin_motor_sp, OUTPUT);
pinMode (pin_motor_pe, OUTPUT);
servo.attach(pin_servo);
servo.write(0);
servo_ats.attach(pin_servo_ats);
servo_ats.write(0);

lcd.begin();
lcd2.begin();
lcd3.begin();
lcd4.begin();
rtc.begin();
baca_eeprom();

scale_1.set_scale(calibration_factor_1); // this value is obtained by calibrating the scale with known weights; see the README for details
scale_1.tare();    // reset the scale to 0
scale_2.set_scale(calibration_factor_2); // this value is obtained by calibrating the scale with known weights; see the README for details
scale_2.tare();    // reset the scale to 0
scale_3.set_scale(calibration_factor_3); // this value is obtained by calibrating the scale with known weights; see the README for details
scale_3.tare();    // reset the scale to 0
}

void loop() {
 currentMillis = millis();
 loadcell();
 alarm();
 tampilan_lcd();
 status_tombol();
}
