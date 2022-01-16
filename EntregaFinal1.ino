//Librerias
#include "max6675.h"              
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>    

//Entradas y salidas
int SSR = 3;
int thermoDO = 4;                 //pin para MAX6675
int thermoCS = 5;                 //CS pin para MAX6675
int thermoCLK = 6;                //Clock pin para MAX6675
int but_1 = 11;                   //Boton 1 entrada
int but_2 = 10;                   //Boton 2 entrada



MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);  // MAX6675 SPI comunicacion 
LiquidCrystal_I2C lcd(0x27,20,4);                     
   
//Variables
unsigned int millis_before, millis_before_2;
unsigned int millis_now = 0;
int refresh_rate = 1000;
int pid_refresh_rate = 300;
unsigned int seconds = 0;
int running_mode = 0;
int selected_mode = 0;
int max_modes = 1;
bool but_1_state = true;
bool but_2_state  =true;
float pwm_value = 5;
int max_pwm_value = 200;
int min_pwm_value = 2;
float temp_setpoint = 0;
float temperature = 0;

void setup() {
  //Definicion de salidas
  pinMode(SSR, OUTPUT);       //SSR apagado
  digitalWrite(SSR, LOW);
  pinMode(but_1, INPUT_PULLUP);
  pinMode(but_2, INPUT_PULLUP);
  
  Serial.begin(115200);
  lcd.init();                 //Iniciar la pantalla LCD
  lcd.setCursor(0,0); 
  lcd.print("Bienvenidos ");
  lcd.setCursor(0,1); 
  lcd.print("Grupo3");
  lcd.setCursor(0,2); 
  lcd.print("Montoya");
  delay(3000);
  lcd.backlight();            //Activa luz      

  millis_before = millis();
  millis_now = millis();
  
}

void loop() { 
  millis_now = millis();
  
  if(millis_now - millis_before_2 > pid_refresh_rate){    
    millis_before_2 = millis();      
    temperature = thermocouple.readCelsius();
    if(running_mode == 1){       
      if(temperature < 150){
        temp_setpoint = seconds;
        if(temp_setpoint > temperature){
          pwm_value = pwm_value + 0.3;
          if(pwm_value > max_pwm_value){
            pwm_value = max_pwm_value;
          }
        }
        else{
          pwm_value--;
          if(pwm_value < min_pwm_value){
            pwm_value = min_pwm_value;
          }
        }
      }
      else if(temperature > 140){
        max_pwm_value = 255;
        temp_setpoint = 200;
        if(temp_setpoint > temperature){
          pwm_value = pwm_value + 0.3;
          if(pwm_value > max_pwm_value){
            pwm_value = max_pwm_value;
          }
        }
        else{
          pwm_value--;
          if(pwm_value < min_pwm_value){
            pwm_value = min_pwm_value;
          }
        }
      } 
               
      analogWrite(SSR, pwm_value);  //Cambio del ciclo de trabajo
        
      if(temperature > 190){
        running_mode = 10;  
        analogWrite(SSR, LOW);     
      }
    }//end of running_mode == 1
    
    if(running_mode == 10){
      lcd.clear();
      lcd.setCursor(0,1);     
      lcd.print("      COMPLETO     ");
        
      seconds = 0;              //Reseteo de tiempo
      running_mode = 11;
      delay(2000);
    }  
  }//fin de reseteo

  
  
  if(millis_now - millis_before > refresh_rate){
    millis_before = millis();   
    seconds = seconds + (refresh_rate/1000);      //We count time
    Serial.println(temperature);
    
    if(running_mode == 0){    
      digitalWrite(SSR, LOW);   
      lcd.clear();
      lcd.setCursor(0,0);     
      lcd.print("Temp: ");
      lcd.print(temperature,1);
      lcd.setCursor(0,1);     
      lcd.print("SSR: OFF");  
      lcd.setCursor(0,2);     
      lcd.print("MODO SELECCIONADO: ");  
      lcd.print(selected_mode);  
      lcd.setCursor(0,3);     
      lcd.print("DETENIDO");     
    }//fin de running_mode == 0

    else if(running_mode == 11){ 
      if(temperature < 40){
        running_mode = 0; 
          
      }
      digitalWrite(SSR, LOW);   
      lcd.clear();
      lcd.setCursor(0,0);     
      lcd.print("Temp: ");
      lcd.print(temperature,1);
      lcd.setCursor(0,1);     
      lcd.print("SSR: OFF");  
      lcd.setCursor(0,2);     
      lcd.print("MODO SELECCIONADO: ");  
      lcd.print(selected_mode);  
      lcd.setCursor(0,3);     
      lcd.print("ENFRIAR");     
    }//Fin de running_mode == 0
  
      
    else if(running_mode == 1){            
      lcd.clear();
      lcd.setCursor(0,0);     
      lcd.print("Temp: ");
      lcd.print(temperature,1);
      lcd.setCursor(0,1);     
      lcd.print("SSR: ON ");  lcd.print("TIEMPO: "); lcd.print(seconds); 
      lcd.setCursor(0,2);     
      lcd.print("MODO SELECCIONADO: ");  
      lcd.print(selected_mode);  
      lcd.setCursor(0,3);     
      lcd.print("CORRIENDO | PWM: ");  lcd.print(pwm_value,1);
    }//Fin de running_mode == 1
  }//FIn de millis_now - millis_before > refresh_rate




  /////////////////////////////////////////////////////////////////// Control por medio de botones
  /////////////////////////////////////////////////////////////////// 
  if(!digitalRead(but_1) && but_1_state){
    but_1_state = false;
    selected_mode ++;
    millis_before = millis_before-1000;
    if(selected_mode > max_modes){
      selected_mode = 0;
    }
  }
  else if(digitalRead(but_1) && !but_1_state){
    but_1_state = true;
  }
  ///////////////////////////////////////////////////////////////////
  if(!digitalRead(but_2) && but_2_state){
    if(running_mode == 1){
      digitalWrite(SSR, LOW);
      running_mode = 0;
      selected_mode = 0; 
    }
    
    but_2_state = false;
    if(selected_mode == 0){
      running_mode = 0;
    }
    else if(selected_mode == 1){
      running_mode = 1;
      seconds = 0;              //Reset del tiempo
    }
  }
  else if(digitalRead(but_2) && !but_2_state){
    but_2_state = true;
  }
  ///////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////
  
}//fin de void loop
