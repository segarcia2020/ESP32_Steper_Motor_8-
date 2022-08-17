/*
    
  Autor : Sergio E. Garcia
  Agosto-Septiembre-Octubre 2021
  Enero-Febrero- Marzo 2022
  Junio 2022
  Origin ESP32 Steper Motor _2
  Origin Matematica de traslado solar/ Matematica Stirling
  Pretende hacer un viaje entre un Elevacion y azimut
  12 valores
  Pagina
  https://www.sunearthtools.com/dp/tools/pos_sun.php?lang=es#top


  28/12/2021 Agregue desabilitar en la  inicializacion de los motores el ENABLE 
  para que no siga consumiendo, ahorro de energia
  Funcionamiento en fast OK Array cada min
   
  29/12/2021 Debieramos correguir el el Switch azimut "0"
  Enero 2022 uso de "preference" donde desarrollamos el encoder digital, usando la memoria de ESP
  https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/examples/StartCounter/StartCounter.ino   
  
  Descripcion de Codigos, que lo enviamos a la nube wegab !!!
  iAN0=60;                          // Culminacion de todo el ciclo
  iAN0=61;                          // Codigo que sabemos que viene del setup
  iAN1=hora_array_v[ind];           // Aprovecho informo cuando va a moverse
  iAN2=min_array_v[ind];            // Idem
  iAN3=ind_det;                     // Informo el ind_det
  iAN0=62;                          // Estamos en alive
  iAN0=63;                          // A desarrollar

  SIMULACION  "1" se programa RTC a 22/1/2022  - 8:16 hs u otra fecha
  
  Manejo de programas Matematico en internet
  Carpeta local
  C:\Users\sgarcia\Downloads\00000_web\ambaracs.com\calculo_solar
  
  Internet: https://ambaracs.com/Calculo_Solar/


ESP Version 8: esta version es autonoma, toma la fecha y la hora del RTC y calcula a partir de la matemateco desarralloda
el azimut y elevacion, ademas calcula la hora y minuto de cuando amanece y atardece

  Programas en php:
  calculo_solar_v2 y calculo_solar_v1 : Superados
  calculo_solar_v3.php tiene la base que sigue el programa ESP32
  
  Formularios:
  
  f.html ---> calculo_solar.php
  formulario.html  --> calculo_solar_v2.php
  s.html --> calculo_solar_v3.php  // Este deberia ser OK
  
//---------------------------------------------------------------------------------

Julio 2022 Armado de la pagina para visualizar donde esta el dispositivo
https://wegab.cl/s.php
Informacion grafica y de estado del dispositivo, encoder_digital y temperatura
Fecha y hora del servidor

//---------------------------------------------------------------------------------

  RECOMENDACIÖN!!!!!!
  COMPILACION!!!!!!!! sacar el cable naranja que conecta el pcb a el modulo RTC!!!!!

  Este modificacion fue relaizada en 21/6/2022 9:14 hs

  Link en Github : https://github.com/segarcia2020/ESP32_Steper_Motor_7.git

Las consideraciones de ubicacion del dispositivo son:

Latitude (+ to N)  -33,4377756
Longitude (+ to E)  -70,65045027
Time Zone (+ to E)  -3

SIM Serie 500 -  Nro. +5693339 2522 (Movistar)

*/

#include <Arduino.h>
#define SerialMon Serial
#include <Wire.h> // Manejo de dispositivos cableados
///-------------------------------------------------------------------------------------------------------
// Para usa el sensor de temperatura ds18b20
#include <DallasTemperature.h>
#include <OneWire.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 22; //32;   
float tempC;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);  
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

///-------------------------------------------------------------------------------------------------------
// Agregado para GPRS
///-------------------------------------------------------------------------------------------------------
// Variables del Temporizador
unsigned long ultimaConsulta = 0;
unsigned long tiempoConsulta = 60000; //4 min 240000 - cada 3 minutos 180000

///-------------------------------------------------------------------------------------------------------
// TTGO T-Call pins - Configure TinyGSM library
// Esta asignacion de pines limito las que venia utilizando en la
// planilla excell
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb
// TinyGsmClient............
#include <TinyGsmClient.h>
// incluida 12/2/21
#include <TinyGPS++.h>
// Set serial for GPS Module
#define Serialgps Serial2
// The TinyGPS++ object
TinyGPSPlus gps;
#define SMS_TARGET_1  "+56966897241"
#define SMS_TARGET_2  "+56962493724"

///-------------------------------------------------------------------------------------------------------
//      SIMULACION y Version !!
//
#define Simulacion 0   // 0 sin simulacion, 1 con simulacion
#define version_esp32 " - V: EPS32_Steper_Motor_8"


//
///-------------------------------------------------------------------------------------------------------

// Your GPRS credentials (leave empty, if not needed)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1
//#define Serialgps Serial2
#include <ArduinoHttpClient.h>
///-------------------------------------------------------------------------------------------------------
// Manejo de memoria flash
#include <Preferences.h>
Preferences preferences;
unsigned int encoder;
////------------------------------------------------------------------------------------------------------
/// Credenciales de Movistar

const char apn[]      = "wap.tmovil.cl"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = "wap"; // GPRS User
const char gprsPass[] = "wap"; // GPRS Password
// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "1234";
int contconexion = 0;  
char host[55];
int Serie=500;

const char server[] = "wegab.cl";
const char resource[] = "https://wegab.cl/bomba_stirling.php"; 
const int  port = 443; 

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif
// TinyGSM Client for Internet connection
// TinyGsmClient client(modem);
TinyGsmClientSecure client(modem);      // Creo que este aseguro la conexion htpps://
HttpClient http(client, server, port);  // Esto tambien


//--------------------------------------------------------------------------------------------------------------
// Entradas LDR y temp

#define AN0 0
#define AN1 13
#define AN2 39
#define AN3 34

//--------------------------------------------------------------------------------------------------------------
// Definicion de pines de conexion con RTC
// Libreria para manejar el RTC
#include <RTClib.h>
#define I2C_SDA            21 
#define I2C_SCL            32
RTC_DS3231 rtc;
String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };
int hora_rtc;
int min_rtc;
int sec_rtc;
int hora_array;
int min_array;
int seg_array;
int hora_array_max;
int min_array_max;
int seg_array_max;
int hora_array_min;
int min_array_min;
int seg_array_min;
int const indice_max = 43;
int hora_array_v[indice_max];
int min_array_v[indice_max];
// Inicializacion de ANO
int iAN0,iAN1, iAN2,iAN3;
int grado_positivo=10;
int ind=0;// colocar aqui el indica
int ind_det;
int flag;
int a;

///-------------------------------------------------------------------------------------------------------
//  Matematica definicion de variables globales
///-------------------------------------------------------------------------------------------------------
float $Solar_Elevation_Angle;
float $Solar_Elevation_corr_atm_refraction;
float $Solar_Azimuth_Angle_4;
float $Solar_Azimuth_Angle_5;
float elevacion_gral;
float azimut_gral;
float elevacion_gral_a_1;
float azimut_gral_a_1;
float elevacion_gral_a_2;
float azimut_gral_a_2;
//-------------------------------------------------------------------------------------------------------
// Datos donde esta ubicado el dispositivo
double $lat_deg = -33.4377756;  //  
double $lon_deg = -70.65045027; //  radianes
int $utc=-3;  
float version_esp=8.00;
//--------------------------------------------------------------------------------------------------------
float amanece ; // Amanece
float atardece;   // Artardece
float duracion_dia;

int hora_global;
int min_global;

int hora_amanece;
int min_amanece;

int hora_atardece;
int min_atardece;
int flag_primera_vez_dia=0;
int flag_amanece=0;
int flag_atardece=0;
float dif_ele;
float dif_azi;
int min_noche=0;
int ret_min=3;
//--------------------------------------------------------------------------------------------------------------
// Manejo de arreglos extraido de la pagina https://www.sunearthtools.com/dp/tools/pos_sun.php?lang=es#top
//
//
//--------------------------------------------------------------------------------------------------------------
//int const indice_max = 44;
// 15 de Enero 2022

String hora_15[]={"6:48:32","7:00:00","7:20:00","7:40:00","8:00:00","8:20:00","8:40:00","9:00:00","9:20:00","9:40:00","10:00:00","10:20:00","10:40:00","11:00:00","11:20:00","11:40:00","12:00:00","12:20:00","12:40:00","13:00:00","13:20:00","13:40:00","14:00:00","14:20:00","14:40:00","15:00:00","15:20:00","15:40:00","16:00:00","16:20:00","16:40:00","17:00:00","17:20:00","17:40:00","18:00:00","18:20:00","18:40:00","19:00:00","19:20:00","19:40:00","20:00:00","20:20:00","20:40:00","20:55:18"};
float a_elevacion_15[46]={1,1.32,5.15,9.06,13.02,17.04,21.1,25.19,29.32,33.47,37.63,41.8,45.97,50.12,54.24,58.31,62.3,66.16,69.8,73.07,75.71,77.31,77.47,76.14,73.68,70.51,66.93,63.11,59.15,55.09,50.97,46.82,42.65,38.48,34.31,30.16,26.02,21.92,17.84,13.81,9.83,5.91,2.06,1,1};
float a_azimut_15[46]={116.16,114.58,111.92,109.34,106.84,104.38,101.95,99.53,97.08,94.58,91.99,89.27,86.36,83.18,79.61,75.51,70.61,64.56,56.73,46.23,31.85,12.96,351.48,331.77,316.46,305.25,296.96,290.6,285.5,281.26,277.6,274.35,271.39,268.64,266.03,263.51,261.06,258.63,256.21,253.76,251.27,248.71,246.07,243.98,242};
// Cada 20 minutos - 10 Diciembre 2021
// Lo extraemos cada 20 min de la pagina https://www.sunearthtools.com/dp/tools/pos_sun.php?lang=es#top
// LO bajamos a excell copiamos a otra hoja y trasponemos columnas en filas y luego exportamos en CSV
// Esta logica esta superada
//--------------------------------------------------------------------------------------------------------------
// Entradas LDR y temp
#define AN0 0
#define AN1 13
#define AN2 39
#define AN3 34
float an0[indice_max];
float an1[indice_max];
float an2[indice_max];
float an3[indice_max];

///------------------------------------------------------------------------------------------------------------
// Manejo de pines para driver A4988

// Driver motor elevacion
#define STEP_ME 33      // Donde estaba el led Verde - pin STEP de A4988 a pin 4
#define DIR_ME 25       // Donde estaba el led Rojo - pin DIR de A4988 a pin 5
#define E_ME 2          // estaba en 18 (19/10/2021) 23 estaba

// Driver motor azimut
#define STEP_MA 19      // Donde estaba el led Verde - pin STEP de A4988 a pin 4
#define DIR_MA 18       // Donde estaba el led Rojo - pin DIR de A4988 a pin 5
#define E_MA 15         // 23 estaba

///-----------------------------------------------------------------------------------------------------------------------
// Manejo de finales de carrera
// 
#define Final_elv_0 12
#define Final_elv_90 36 
#define Final_azim_0 14
#define Final_azim_180 10
bool flag_giro_elev;
bool flag_giro_azim;

///-----------------------------------------------------------------------------------------------------------------------
// Teoria para la relacion 1 a 120 diente Modulo de giro
// 200 pulsos son una vuelta completa 360 grados, relacion dientes 120 entonces 120x200=24000 pulsos para vuelta completa
// 100 pulsos son 180 grados 120x100=12000 
// 50  pulsos para 90 grados 120x50= 6000 pulsos

#define periodo 1  
#define pasos_azimut 24000 // 120x200=360 grados
#define pasos_azimut_1 66
#define pasos_azimut_10 660

#define pasos_elevacion 24000 //6000 // 90 grados
#define pasos_elevacion_1 66
#define pasos_elevacion_10 660
#define pasos_mot 200

#define Azimut_min_verano 120
#define Azimut_max_verano 240

#define Azimut_min_invierno 60
#define Azimut_max_invierno 300

///-----------------------------------------------------------------------------------------------------------------------
// Se incluye libreria con todas las funciones
// 
#include "Manejo_posicion.h"
#include "posicion_elev_azi.h"
//
//  
///-----------------------------------------------------------------------------------------------------------------------


///-----------------------------------------------------------------------------------------------------------------------
// setup()
//
///-----------------------------------------------------------------------------------------------------------------------

void setup() 
{
///-----------------------------------------------------------------------------------------------------------------------  
  SerialMon.begin(115200);
  
  pinMode( oneWireBus, OUTPUT);
  digitalWrite( oneWireBus, HIGH);
  sensors.begin(); // Inicializa el sensor de temperatura DS18B20
  
// Inicializar el RTC ///-------------------------------------------------------------------------------------------------
    Wire.begin(I2C_SDA, I2C_SCL); 
    if (!rtc.begin()) {
        Serial.println(F("Couldn't find RTC"));
       //while (1);
    }

///-----------------------------------------------------------------------------------------------------------------------
// Set GPRS
// Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);
  SerialMon.println("Initializing modem...");
  modem.restart();
 // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }
   configuracion_entrada_salida(); // Configuracion de entradas - salidas
///-----------------------------------------------------------------------------------------------------------------------
  preferences.begin("encoder", false);
// Remove all preferences under the opened namespace
// preferences.clear();
// Get the counter value, if the key does not exist, return a default value of 0
// Note: Key name is limited to 15 chars.
// encoder = preferences.getUInt("encoder", 0);
  

///-----------------------------------------------------------------------------------------------------------------------
//  rtc.adjust(DateTime(__DATE__,__TIME__)); // funcion que permite establecer fecha y horario
//  ............... Formato.........................................
// Aqui algunos ejemplos por si quieren ajustar a una determinada fecha como por ejemplo simular extremos Verano e Invierno
//  rtc.adjust(DateTime(2021, mes, dia, hora, min, seg));
//  if (Simulacion==1) rtc.adjust(DateTime(2022,1,15,6,30, 0));
//  if (Simulacion==1) rtc.adjust(DateTime(2022,1,22,20,5, 0));
if (Simulacion==1) rtc.adjust(DateTime(2022,8,14,10,30, 0));
if (Simulacion==0) rtc.adjust(DateTime(__DATE__,__TIME__));
   //rtc.adjust(DateTime(2022,7,3,18,00, 0));
   DateTime fecha = rtc.now();
   Serial.print("RTC dentro del setup  ");
   Serial.print(fecha.day());
   Serial.print('/');
   Serial.print(fecha.month());
   Serial.print('/');
   Serial.print(fecha.year());
   Serial.print(" ");
   Serial.print(fecha.hour());
   Serial.print(':');
   hora_rtc=fecha.hour();
   Serial.print(fecha.minute());
   Serial.print(':');
   min_rtc=fecha.minute();
   Serial.print(fecha.second());
   sec_rtc=fecha.second();
   Serial.println(" -->");

//--------------------------------------------------------------------------------------------------------------------------
// Esto es para salvar la situacion cuando esta en algun posiscion que queremos sacar Ajustamos forzamos el encoder digital a
// otra posicion.
// Ejemplo: 
// 1.En campo vemos que el azimut esta en 250 y nos estamos seguro cual fue el ultimo encoder almacenado, forzamos 
// la variable encoder= 250 y subimos el programa.
// 2. una vez que empieza a funcionar comentamos esta rutina entre /*5 y subimos denuevo el programa
// 3. POdemos saber el valor del encoder en la pagina https://wegab.cl/s.php

/*5
          encoder =270;
          preferences.putUInt("encoder", encoder);
          delay(500);
          Serial.print("Encoder, en primera vez: ");
          Serial.println(encoder);
  */      
//--------------------------------------------------------------------------------------------------------------------------   
   
   posicion ($lat_deg,$lon_deg,$utc,fecha.year() ,fecha.month(),fecha.day(),hora_rtc,min_rtc,0);
   posicionar_motores();

//--------------------------------------------------------------------------------------------------------------------------   
// Leo temperatura en grados centigrados DS18B20
  sensors.requestTemperatures(); 
  tempC = sensors.getTempCByIndex(0);
  delay(1000);
  envia_mensaje_sms(SMS_TARGET_2, tempC,fecha.day(),fecha.month(),fecha.year(),hora_rtc,min_rtc);
  Serial.print("Mensaje sms enviado");
  Serial.println();
} // setup


///-----------------------------------------------------------------------------------------------------------------------
// Loop()
//
///-----------------------------------------------------------------------------------------------------------------------

void loop() 
{
//-----------------------------------------------------------------------------------------------------------------------
// antes de amanecer formula de salida de sol y puesta de sol se hace calibración
// 1_ Hace cero de elevacion 
// 2_ Hace 90 grados de elevacion
// 3_busca norte y se mueve 90 grados
// Recibe informacion de ubicacion
// Elevacion EE
// Azimut    AA
// Posiciona 
// Ajuste fino Ldr!!!

    DateTime fecha = rtc.now();
    Serial.print("RTC   ");
    Serial.print(fecha.day());
    Serial.print('/');
    Serial.print(fecha.month());
    Serial.print('/');
    Serial.print(fecha.year());
    Serial.print(" ");
    Serial.print(fecha.hour());
    Serial.print(':');
    hora_rtc=fecha.hour();
    Serial.print(fecha.minute());
    Serial.print(':');
    min_rtc=fecha.minute();
    Serial.print(fecha.second());
    sec_rtc=fecha.second();
    Serial.print(" -->");
    encoder = preferences.getUInt("encoder", 0);
    Serial.print("Encoder: ");
    Serial.print(encoder);
    Serial.println(version_esp32);
// -----------------------------------------------------------------
// Tomo las lecturas de la LDr y la asigno al vector

  iAN0=analogRead(AN0);
  an0[ind]=iAN0;
  iAN1=analogRead(AN1);
  an1[ind]=iAN1;
  iAN2=analogRead(AN2);
  an2[ind]=iAN2;
  iAN3=analogRead(AN3);
  an3[ind]=iAN3;
// -----------------------------------------------------------------
// Leo temperatura en grados centigrados DS18B20

  sensors.requestTemperatures(); 
  tempC = sensors.getTempCByIndex(0);
  delay(1000);
  Serial.print(" D18B20: ");
  Serial.println(tempC);
// -----------------------------------------------------------------
// lamo funcion de impresion de AN's
   print_AN();
//-----------------------------------------------------------------------------------------------
int $utc=-3;

//-------------------------------------------------------------------------------------------------------
// Analisis de fecha

Serial.print("Hora/Min rtc (Actual):  ");
Serial.print(hora_rtc);
Serial.print(" : ");
Serial.println(min_rtc);

// Llamada a la funcion traigo la hora y minutos de amanece y atardece
posicion_ab ($lat_deg,$lon_deg,$utc,fecha.year() ,fecha.month(),fecha.day(),hora_rtc,min_rtc);

hora_dec(amanece,"Amanece  ",1);
hora_amanece=hora_global;
min_amanece=min_global;
hora_dec(atardece,"Atardece ",1);
hora_atardece=hora_global;
min_atardece=min_global;

//-------------------------------------------------------------------------------------------------------
// Estamos en el dia ????

    if (hora_rtc>=hora_amanece){
        if (min_rtc>=min_amanece){
           Serial.print("hs amanece min_rtc > min_amanece  ");
           Serial.println();
           flag_amanece=1; 
          }else{
           Serial.print("hs amanece min_rtc < min_amanece ");
           Serial.println();
           flag_amanece=1;
          } 
      }else{
          flag_amanece=0;
      }
        
    
      if (hora_rtc<=hora_atardece){
       if (min_rtc<=min_atardece){
        //Serial.print("Por debajo de la hora de atardecer  ");
        //Serial.println();
          flag_atardece=1;
        }else{
          flag_atardece=1;
        }
      
      }else{
          flag_atardece=0;
      }
  
  
// -------------------------------------------------------------------------------------------------------------------
// Estamos en el dia...................................................
 if ((flag_amanece==1)&&(flag_atardece==1)){
      Serial.print("Estoy en el dia  ");
      
      //posicionar_motores(); Estaba aca!!! y no funciona bien
      // Aqui debo calibrar el sistema
      // config_movimiento();
      
      Serial.println();
      //--------------------------------------------------------------------------------------------------------------
      // Si es la primera vez-----------------------------------------------------------------------------------------
      if (flag_primera_vez_dia==0){
          posicionar_motores();
         flag_giro_elev=1;
         min_noche=0;
         Serial.print("es la primera vez  ");
         Serial.println();
         posicion_ab ($lat_deg,$lon_deg,$utc,fecha.year() ,fecha.month(),fecha.day(),hora_rtc,min_rtc);
         elevacion_gral_a_1=elevacion_gral;
         azimut_gral_a_1=azimut_gral;
         
         Serial.print(" Azimut gral 1 vez  ");
         Serial.print(azimut_gral);
         Serial.print(" Azimut_gral_a_1 1 vez: ");
         Serial.println(azimut_gral_a_1);
     
         if (azimut_gral>=0 && azimut_gral<=180){
          dif_azi=azimut_gral_a_1;
          flag_giro_azim=1;
         }
     
         
         if (azimut_gral<=360 && azimut_gral>=240){
          azimut_gral_a_1=azimut_gral;
          dif_azi=360-azimut_gral_a_1;
          flag_giro_azim=0;
         }
        
         Serial.print("Elevacion  ");
         Serial.print(elevacion_gral_a_1);
     
         Serial.print("  - Azimut  :");
         Serial.print(azimut_gral_a_1);
         Serial.print("  -  Flag giro elev:  ");
         Serial.print(flag_giro_elev);
         Serial.print("  - Flag giro azimut: ");
         Serial.print(flag_giro_azim);
         Serial.print("  - dif azimut: ");
         Serial.println(dif_azi);       
         flag_primera_vez_dia=flag_primera_vez_dia+1;
         Serial.print("  - Count_1 ");
         Serial.print(flag_primera_vez_dia);
        
        // Almaceno el valor del azimut
          encoder = azimut_gral_a_1;
          preferences.putUInt("encoder", encoder);
          delay(500);
          Serial.print(" -Encoder, 1 vez: ");
          Serial.println(encoder);
          Serial.println(" -------------------------------------------------------------------------------------------------------------");
        // Movimiento de motres............................................................... 
          elevacion (elevacion_gral_a_1*pasos_elevacion_1, flag_giro_elev);
          azimut ((abs(dif_azi))*pasos_azimut_1,flag_giro_azim); // Poner en cero
          Serial.println("Retardo en min ");
          envia(); 
          retardo_en_min(ret_min);
         
      }else{ // next time 
         // Aqui va estra siempre en general............
          Serial.print("next...  ");
          Serial.println();
        // Aqui estamos incremando .....
          posicion_ab ($lat_deg,$lon_deg,$utc,fecha.year() ,fecha.month(),fecha.day(),hora_rtc,min_rtc);
          azimut_gral_a_2=azimut_gral;
          elevacion_gral_a_2=elevacion_gral;
         // alamaceno en el encoder
          encoder = azimut_gral;
          preferences.putUInt("encoder", encoder);
          delay(500);
          Serial.print(" Encoder,next: ");
          Serial.print(encoder);   
         
    //---------------------------------------------------------------------------------------------------------
            
          dif_ele=(elevacion_gral_a_2-elevacion_gral_a_1);
          dif_azi=(azimut_gral_a_2-azimut_gral_a_1);
          
            if (dif_azi>=300){
              dif_azi=360-dif_azi;
            }
    /*
            if (dif_azi>=200){
              dif_azi=(azimut_gral_a_1-azimut_gral_a_2);
            }
      */      
            if (dif_ele<=0)flag_giro_elev=0;
            if (dif_ele>=0)flag_giro_elev=1;
            if (dif_azi<=0)flag_giro_azim=0;
         
          Serial.print(" Elevacion..  ");
          Serial.print(elevacion_gral_a_2);
          Serial.print("  - Azimut..  ");
          Serial.print(azimut_gral_a_2);
          
          //dif_ele= elevacion_gral_a_2-elevacion_gral_a_1;
          //dif_azi=azimut_gral_a_2-azimut_gral_a_1;
          flag_primera_vez_dia=flag_primera_vez_dia+1;
        
          Serial.print(" - Dif Elev..  ");
          Serial.print(dif_ele);
          Serial.print(" - Flag gi ele:  ");
          Serial.print(flag_giro_elev);
        
          Serial.print("  - Dif Azi..  ");
          Serial.print(dif_azi);
          Serial.print(" - Flag gi azi: ");
          Serial.print(flag_giro_azim);
          Serial.print(" Count:  ");
          Serial.print(flag_primera_vez_dia);
          Serial.print(" Amanece flag:  ");
          Serial.print(flag_amanece);
          Serial.print(" Atardece flag:  ");
          Serial.println(flag_atardece);

        //ANtes de mover puedo hacer una validadcion de elevacion y azimut a partir de la  historia
        // Movimiento de motres...............................................................
          elevacion ((abs(dif_ele))*pasos_elevacion_1, flag_giro_elev);
          azimut ((abs(dif_azi))*pasos_azimut_1,flag_giro_azim); // Poner en cero
        
          elevacion_gral_a_1=elevacion_gral_a_2;
          azimut_gral_a_1=azimut_gral_a_2;
          Serial.println("Retardo en min ");
          Serial.println(" -------------------------------------------------------------------------------------------------------------");
          Serial.println(" ");
          Serial.println(" ");
          //Serial.println(flag_primera_vez_dia);
          envia();
          retardo_en_min(ret_min);
      } // next time
   
 }else{  //if ((flag_amanece==1)&&(flag_atardece==1)){
    flag_primera_vez_dia=0; // reseteo el flag
    // Aqui deberia haber una funcion que reporte cada una hora que esta en modo Noche
    // Contador
    Serial.print(" Contador de minutos nocturnos ");
    Serial.println(min_noche);
    delay (60000); // espera 1 minuto
    min_noche=min_noche+1;
    if (min_noche==2){
        iAN0 =62; //Codigo dentro de alive
        envia();
    }
    if (min_noche==60){
        Serial.println(" -------------------------------------------------------------------------------------------------------------");
        Serial.println("  Estamos de noche.... envio señales que estoy vivo!! ");
        Serial.println(" -------------------------------------------------------------------------------------------------------------");
        iAN0 =62; //Codigo dentro de alive
        envia();
        min_noche=0;
    }
 }// if ((flag_amanece==1)&&(flag_atardece==1)){
 
} // Loop
