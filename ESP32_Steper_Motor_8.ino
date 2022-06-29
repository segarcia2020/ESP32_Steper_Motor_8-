/*
    
  Autor : Sergio Garcia
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

  SIMULACION  "1" se programa RTC a 22/1/2022  - 8:16 hs
  
  Manejo de programas Matematico en internet
  Carpeta local
  C:\Users\sgarcia\Downloads\00000_web\ambaracs.com\calculo_solar
  
  Internet: https://ambaracs.com/Calculo_Solar/

  Programas en php:
  calculo_solar_v2 y calculo_solar_v1 : Superados
  calculo_solar_v3.php tiene la base que sigue el programa ESP32
  
  Formularios:
  
  f.html ---> calculo_solar.php
  formulario.html  --> calculo_solar_v2.php
  s.html --> calculo_solar_v3.php  // Este deberia ser OK

  RECOMENDACIÖN!!!!!!
  COMPILACION!!!!!!!! sacar el cable naranja que conecta el pcb a el modulo RTC!!!!!

  Este modificacion fue relaizada en 21/6/2022 9:14 hs

  Link en Github : https://github.com/segarcia2020/ESP32_Steper_Motor_7.git
   
*/

#include <Arduino.h>
#define SerialMon Serial
#include <Wire.h> // Manejo de dispositivos cableados
///-------------------------------------------------------------------------------------------------------
// Para usa el sensor de temperatura ds18b20
#include <OneWire.h>
#include <DallasTemperature.h>
// GPIO where the DS18B20 is connected to
const int oneWireBus = 32;   
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
#define SMS_TARGET_2  "+56977945690"

///-------------------------------------------------------------------------------------------------------
//      SIMULACION y Version !!
//
#define Simulacion 1   // 0 sin simulacion, 1 con simulacion
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

//const int  port = 443;   
//char host[55];
// Nuestra pagina nuestra plataforma
//const char server[] = "ambaracs.com";
//const char resource[] = "https://ambaracs.com/bomba_stirling.php"; 
//const int  port = 443;  

const char server[] = "wegab.cl";
const char resource[] = "https://wegab.cl/bomba_stirling.php"; 
const int  port = 443; 
//String strhost = "ambaracs.com";
//String strurl = "https://ambaracs.com/stirling_1.php"; //bomba_stirling.php
//String strurlcon = "https://ambaracs.com/consulta_php.php";
//consulta_php1.php
//String strhost = "wegab.cl";
//String strurl = "https://wegab.cl/station_pm.php";
//String strurlcon = "https://wegab.cl/consulta_php.php";
//--------------------------------------------

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
//#define idd  29 // indice

//--------------------------------------------------------------------------------------------------------------
// Definicion de pines de conexion con RTC
// Libreria para manejar el RTC
#include <RTClib.h>
#define I2C_SDA            21 
#define I2C_SCL            22
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
double $lat_deg = -33.4377756;  //  
double $lon_deg = -70.65045027; //  radianes
int $utc=-3;  
//float $Sunrise_Time ; // Amanece
//float $Sunset_Time;   // Artardece
float amanece ; // Amanece
float atardece;   // Artardece
float duracion_dia;
//--------------------------------------------------------------------------------------------------------------
// Manejo de arreglos extraido de la pagina https://www.sunearthtools.com/dp/tools/pos_sun.php?lang=es#top
//
//
//--------------------------------------------------------------------------------------------------------------
//int const indice_max = 44;
/*
int Id[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14};
//float r_elevacion[] = {10,20,30,40,50,60,70,80,90,40,30,22,13 ,4}; //altura en grados
//float r_azimut[] = {170,140,120,110,90,60,50,30,20,0, 33.98,278.79,254.82,240.97};     //azimut en grados
String hora[14]={"4:25:00","4:26:00","4:26:30","4:27:00","4:27:30","4:28:00","4:28:30","4:29:00","4:30:00","4:30:30","4:31:00","4:31:30","4:32:00","4:32:30"};
float a_elevacion[14] = {10,20,30,40,50,60,70,80,85,40,30,22,15,5}; //altura en grados !!!!! 15 de Enero detectamos que cuando llega a 90 grados se queda alli el mismo problema que tuve con el azimuto
float a_azimut[14] = {115,110,100,98,90,60,50,30,20,0,338.98,278.79,254.82,240.97};     //azimut en grados
*/

// 15 de Enero 2022
String hora[]={};
//int Id[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44};// son 44
//-------------------------------------------------------------------------------------------------------------------------------------------

// 15 de Enero 2022
String hora_15[]={"6:48:32","7:00:00","7:20:00","7:40:00","8:00:00","8:20:00","8:40:00","9:00:00","9:20:00","9:40:00","10:00:00","10:20:00","10:40:00","11:00:00","11:20:00","11:40:00","12:00:00","12:20:00","12:40:00","13:00:00","13:20:00","13:40:00","14:00:00","14:20:00","14:40:00","15:00:00","15:20:00","15:40:00","16:00:00","16:20:00","16:40:00","17:00:00","17:20:00","17:40:00","18:00:00","18:20:00","18:40:00","19:00:00","19:20:00","19:40:00","20:00:00","20:20:00","20:40:00","20:55:18"};
float a_elevacion_15[46]={1,1.32,5.15,9.06,13.02,17.04,21.1,25.19,29.32,33.47,37.63,41.8,45.97,50.12,54.24,58.31,62.3,66.16,69.8,73.07,75.71,77.31,77.47,76.14,73.68,70.51,66.93,63.11,59.15,55.09,50.97,46.82,42.65,38.48,34.31,30.16,26.02,21.92,17.84,13.81,9.83,5.91,2.06,1,1};
float a_azimut_15[46]={116.16,114.58,111.92,109.34,106.84,104.38,101.95,99.53,97.08,94.58,91.99,89.27,86.36,83.18,79.61,75.51,70.61,64.56,56.73,46.23,31.85,12.96,351.48,331.77,316.46,305.25,296.96,290.6,285.5,281.26,277.6,274.35,271.39,268.64,266.03,263.51,261.06,258.63,256.21,253.76,251.27,248.71,246.07,243.98,242};

// 22/6/22
//String hora_15[]={"9:40:43","10:00:00","10:20:00","10:40:00","11:00:00","11:20:00","11:40:00","12:00:00","12:20:00","12:40:00","13:00:00","13:20:00","13:40:00","14:00:00","14:20:00","14:40:00","15:00:00","15:20:00","15:40:00","16:00:00","16:20:00","16:40:00","17:00:00","17:20:00","17:40:00","18:00:00","18:20:00","18:40:00","19:00:00","19:20:00","19:40:00","19:57:17"};
//float a_elevacion_15[32]={-0.833,2.8,6.49,10.07,13.53,16.84,19.99,22.96,25.7,28.21,30.44,32.36,33.94,35.15,35.97,36.37,36.35,35.9,35.05,33.8,32.18,30.23,27.97,25.44,22.67,19.69,16.52,13.19,9.72,6.13,2.43,-0.833};
//float a_azimut_15[32]={66.15,63.46,60.55,57.49,54.24,50.8,47.14,43.23,39.05,34.6,29.86,24.84,19.57,14.06,8.38,2.59,356.76,350.97,345.31,339.82,334.57,329.58,324.87,320.44,316.29,312.4,308.76,305.34,302.11,299.06,296.15,293.75};


// estaba en 46


//------------------------------------30 de Octubre -------------------------
//int Id[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
//String hora[]={"5:44:25","6:00:00","6:30:00","7:00:00","7:30:00","8:00:00","8:30:00","9:00:00","9:30:00","10:00:00","10:30:00","11:00:00","11:30:00","12:00:00","12:30:00","13:00:00","13:30:00","14:00:00","14:30:00","15:00:00","15:30:00","16:00:00","16:30:00","17:00:00","17:30:00","18:00:00","18:30:00","19:00:00","19:08:37"};
//float a_elevacion[29] = {1,2.29,8.38,14.56,20.8,27.06,33.31,39.52,45.63,51.58,57.23,62.4,66.73,69.67,70.55,69.12,65.78,61.21,55.9,50.16,44.17,38.03,31.81,25.56,19.31,13.1,6.94,0.88,1};
//float a_azimut[29] = {107.33,105.2,101.2,97.27,93.32,89.26,84.94,80.22,74.84,68.48,60.59,50.35,36.66,18.62,357.28,336.47,319.5,306.76,297.19,289.73,283.65,278.45,273.84,269.6,265.56,261.62,257.68,253.64,252.45};


// Cada 20 minutos - 10 Diciembre 2021
// Lo extraemos cada 20 min de la pagina https://www.sunearthtools.com/dp/tools/pos_sun.php?lang=es#top
// LO bajamos a excell copiamos a otra hoja y trasponemos columnas en filas y luego exportamos en CSV


//int Id[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45};
//String hora[45]={"4:25:59","4:40:00","5:00:00","5:20:00","5:40:00","6:00:00","6:20:00","6:40:00","7:00:00","7:20:00","7:40:00","8:00:00","8:20:00","8:40:00","9:00:00","9:20:00","9:40:00","10:00:00","10:09:00","10:40:00","11:00:00","11:20:00","11:40:00","12:00:00","12:20:00","12:40:00","13:00:00","13:20:00","13:40:00","14:00:00","14:20:00","14:40:00","15:00:00","15:20:00","15:40:00","16:00:00","16:20:00","16:40:00","17:00:00","17:20:00","17:40:00","18:00:00","18:20:00","18:40:00","18:45:21"};
//String hora[45]={"4:25:59","4:26:00","4:27:00","4:28:00","4:29:00","4:30:00","4:31:00","4:32:00","4:33:00","4:34:00","4:35:00","4:36:00","4:37:00","4:38:00","4:39:00","4:40:00","4:41:00","4:42:00","4:43:00","4:44:00","4:45:00","4:46:00","4:47:00","4:48:00","4:49:00","4:50:00","4:51:00","4:52:00","4:53:00","4:54:00","4:55:00","4:55:00","4:56:00","4:57:00","4:58:00","4:59:00","5:00:00","5:01:00","5:02:00","5:03:00","5:04:00","5:05:00","5:06:21","5:07:21","5:08:21"};
//float a_elevacion[45]={1, 1.76, 5.53, 9.38, 13.3, 17.27, 21.29, 25.36, 29.46, 33.59, 37.74, 41.9, 46.07, 50.24, 54.39, 58.52, 62.59, 66.56, 70.38, 73.92, 76.93, 78.98, 79.49, 78.24, 75.7, 72.41, 68.73, 64.83, 60.81, 56.71, 52.58, 48.41, 44.24, 40.08, 35.92, 31.78, 27.66, 23.58, 19.53, 15.53, 11.59, 7.7, 3.88, 1,1};
//float a_azimut[45]={ 118.48, 116.56, 113.92, 111.37, 108.91, 106.5, 104.13, 101.78, 99.42, 97.04, 94.59, 92.03, 89.33, 86.4, 83.15, 79.45, 75.06, 69.65, 62.61, 52.95, 39.03, 19.12, 354.44, 331.31, 314.17, 302.36, 294.01, 287.78, 282.88, 278.82, 275.34, 272.25, 269.43, 266.8, 264.29, 261.87, 259.5, 257.15, 254.79, 252.41, 249.98, 247.48, 244.89, 242.2, 241.46};


//-------------- 17 de Diciembre
//int Id[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45};
//Cada 1 minuto

//String hora[45]={"4:25:00","4:26:00","4:26:30","4:27:00","4:27:30","4:30:00","4:31:00","4:32:00","4:33:00","4:34:00","4:35:00","4:36:00","4:37:00","4:38:00","4:39:00","4:40:00","4:41:00","4:42:00","4:43:00","4:44:00","4:45:00","4:46:00","4:47:00","4:48:00","4:49:00","4:50:00","4:51:00","4:52:00","4:53:00","4:54:00","4:55:00","4:55:00","4:56:00","4:57:00","4:58:00","4:59:00","5:00:00","5:01:00","5:02:00","5:03:00","5:04:00","5:05:00","5:06:21","5:07:21","5:08:21"};
//mas rapido
//String hora[45]={"4:25:00","4:26:00","4:26:30","4:27:00","4:27:30","4:28:00","4:28:30","4:29:00","4:30:00","4:30:30","4:31:00","4:31:30","4:32:00","4:32:30","4:33:00","4:33:30","4:34:00","4:34:30","4:35:00","4:35:30","4:36:00","4:36:30","4:37:00","4:37:30","4:38:00","4:38:30","4:39:00","4:39:30","4:40:00","4:40:30","4:41:00","4:41:30","4:42:00","4:42:30","4:43:00","4:43:30","4:44:00","4:44:30","4:45:00","4:45:30","4:46:00","4:46:30","4:47:00","4:47:30","4:48:00"};

//String hora[45]={"6:27:54","6:40:00","7:00:00","7:20:00","7:40:00","8:00:00","8:20:00","8:40:00","9:00:00","9:20:00","9:40:00","10:00:00","10:09:00","10:40:00","11:00:00","11:20:00","11:40:00","12:00:00","12:20:00","12:40:00","13:00:00","13:20:00","13:40:00","14:00:00","14:20:00","14:40:00","15:00:00","15:20:00","15:40:00","16:00:00","16:20:00","16:40:00","17:00:00","17:20:00","17:40:00","18:00:00","18:20:00","18:40:00","19:00:00","19:20:00","19:40:00","20:00:00","20:20:00","20:50:00"};


///-------------------------------------------------------------------------------------------------------------

float a_elevacion[43]={1.39,5.14,8.96,12.86,16.82,20.82,24.87,28.96,33.08,37.22,41.38,45.55,49.72,53.88,58.02,62.11,66.12,70,73.63,76.81,79.12,79.94,78.93,76.5,73.26,69.59,65.7,61.68,57.58,53.44,49.28,45.11,40.94,36.78,32.64,28.53,24.44,20.4,16.4,12.45,8.56,4.74,1};
float a_azimut[43]={119.01,117.35,114.7,112.15,109.68,107.28,104.92,102.58,100.24,97.88,95.47,92.97,90.32,87.48,84.35,80.8,76.62,71.51,64.91,55.88,42.81,23.63,358.59,333.98,315.53,302.99,294.28,287.87,282.88,278.79,275.29,272.19,269.37,266.75,264.26,261.85,259.49,257.16,254.82,252.45,250.04,247.57,245.01  };


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
// esta estamos usando
// 17/10/2021
// 19/10/2021


//elevacion ((abs(dif_elevacion))*pasos_elevacion_1, flag_giro_elev);
//--------------------------------------------------------------
// Modificacion de funcion de azimut
// 23/10/2021 esta estamos usando
// 17/10/2021
// 19/10/2021


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
  SerialMon.begin(115200);
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
///-----------------------------------------------------------------------------------------------------------------------
  preferences.begin("encoder", false);
// Remove all preferences under the opened namespace
// preferences.clear();
// Get the counter value, if the key does not exist, return a default value of 0
// Note: Key name is limited to 15 chars.
// encoder = preferences.getUInt("encoder", 0);
  
///-----------------------------------------------------------------------------------------------------------------------
  sensors.begin(); // Inicializa el sensor de temperatura DS18B20
  if (Simulacion==1){
    inicializacion_vector();
  }else{
    inicializacion_vector_1();
  }

///-----------------------------------------------------------------------------------------------------------------------
//  rtc.adjust(DateTime(__DATE__,__TIME__)); // funcion que permite establecer fecha y horario
//  ............... Formato.........................................
//  rtc.adjust(DateTime(2021, mes, dia, hora, min, seg));
    //if (Simulacion==1) rtc.adjust(DateTime(2022,1,15,6,30, 0));
   if (Simulacion==1) rtc.adjust(DateTime(2022,1,22,8,16, 0));
   if (Simulacion==0) rtc.adjust(DateTime(__DATE__,__TIME__));

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
   
   configuracion_entrada_salida(); // Configuracion de entradas - salidas
   posicion ($lat_deg,$lon_deg,$utc,fecha.year() ,fecha.month(),fecha.day(),hora_rtc,min_rtc,0);
  
    encoder = preferences.getUInt("encoder", 0);
    Serial.print("Buscando 90 elevacion....");
    buscar_noventa_elevacion();
    Serial.print("Buscando 0 elevacion....");
    buscar_cero_elevacion();
    //Serial.print("Buscando 180 azimut....");
    //buscar_180_azimut(20);
    Serial.print("Buscando cero azimut....");
    Serial.println(encoder);
     
    if ((encoder>=0) && (encoder <=180)){
      Serial.print("Buscar_cero 20 grados....");
      buscar_cero_azimut(20);
    }
    if ((encoder<=360) && (encoder >=250)){
      Serial.print("Buscar_cero 250 grados....");
      buscar_cero_azimut(250);
    }
    
   //motores_begin(); 
   hora_max(43,1);
   if (Simulacion==0){
    analisis_posicion();
   }
   if (Simulacion==1){
    ind=0;
    ind_det=0;
   }
   //almacenamiento_vector(1);
   //delay (6000);
   
     if (Simulacion==1){
        hora_array_v[0]=8;
        min_array_v[0]=17;
     }else{
        hora_array_v[0]=6;
        min_array_v[0]=48;
     }
     
   Serial.println("Dentro del setup - logica deberia empezar en ind:  ");
   Serial.print(ind);
   Serial.print("ind_det:  ");
   Serial.println(ind_det);
   
     if (Simulacion==1){
        hora_array_v[0]=8;
        min_array_v[0]=17;
     }else{
        hora_array_v[0]=6;
        min_array_v[0]=48;
     }
     
   Serial.print(hora_array_v[ind]);
   Serial.print(":");
   Serial.println (min_array_v[ind]);
   delay (2000);
   
   //delay(25000);
   iAN0=61;                        // Codigo que sabemos que viene del setup
   iAN1=hora_array_v[ind];         // Aprovecho informo cuando va a moverse
   iAN2=min_array_v[ind];          // Idem
   iAN3=ind_det;                   // Informo el ind_det
   if (Simulacion==0)envia(); // Envia a la hora que va arrancar 
}


///-----------------------------------------------------------------------------------------------------------------------
// Loop()
//
///-----------------------------------------------------------------------------------------------------------------------

void loop() 
{

///-----------------------------------------------------------------------------------------------------------------------
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
   Serial.println(" -->");
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
// Leo temperatura en grados centigrados

  sensors.requestTemperatures(); 
  tempC = sensors.getTempCByIndex(0);
  delay(350);
 
// -----------------------------------------------------------------
// lamo funcion de impresion de AN's

  print_AN();
  Serial.print("ind:  ");
  Serial.println(ind);
  Serial.print("ind_det:  ");
  Serial.println(ind_det); 
   
// debo hacer la diferencia del estado anterior
// Aqui saco la diferencia si el puntero i>=0 comienzo y voy teniendo en cuenta el valor anterior
//
//-----------------------------------------------------------------------------------------------
// Tomo la hora actual
// Paso hora y min lat y long y saco la hora que amanece y atardece
int $utc=-3;
int $year=2022;
int $month=1;
int $day=22;
//hora_array=8;
//min_array=;
//hora_rtc
  //int hora_array;   // Variable global
  //int min_array;    // Variable global
  //int seg_array;    // Variable global
  //elevacion_gral;   // Variable global
  //azimut_gral;      // Variable global
  Serial.print("Hora:  ");
  Serial.println(hora_rtc);
  Serial.print("Min:  ");
  Serial.println(min_rtc);
  
  posicion_ab ($lat_deg,$lon_deg,$utc,fecha.year() ,fecha.month(),fecha.day(),hora_rtc,min_rtc);
  Serial.print("Amanece antes de la funcion  ");
  Serial.println(amanece);
  Serial.print("Atardece antes de la funcion  ");
  Serial.println(atardece);
  hora_dec(amanece,"Amanece  ");
  hora_dec(atardece,"Atardece ");
  Serial.print("Duracion dia  ");
  Serial.println(duracion_dia);

// duracion_dia=$Sunlight_duration;


//-------------------------------------------------
// Aqui comparo y saco los deltas
//  comparacion(); // Funciona OK
//-------------------------------------------------  
  delay(60000);
}
