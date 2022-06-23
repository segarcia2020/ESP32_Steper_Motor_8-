
//-------------------------------------------------------- Matematica  --------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------
// Funcion: void imprime_config(int $utc,int $hour, int $min, int $year,int $month,int $day, double  $hora_decimal_solar)
//
// Imprime configuracion 
// 

void imprime_config(int $utc,int $hour, int $min, int $year,int $month,int $day, double  $hora_decimal_solar){
  Serial.println( " -------------------------------------------------------------------------------------");
  Serial.print("Ubicacion Las Condes utc UTC: ");
  Serial.println($utc);
  Serial.print("Año: ");
  Serial.print($year);
  Serial.print(" mes: ");
  Serial.print($month);
  Serial.print(" dia: ");
  Serial.println($day);
  Serial.print("Hora: ");
  Serial.print($hour);
  Serial.print(" min: ");
  Serial.print($min);
  Serial.print(" hora decimal: ");
  Serial.println($hora_decimal_solar,5);
  Serial.print("lat_deg: ");
  Serial.println($lat_deg,5);
  Serial.print("lon_deg: ");
  Serial.println($lon_deg,5);
  Serial.println( " -------------------------------------------------------------------------------------"); 
}

//------------------------------------------------------------------------------------------------------------
// Conversion de grados a radianes y radianes a grados
// Funcion: void imprime_config(int $utc,int $hour, int $min, int $year,int $month,int $day, double  $hora_decimal_solar)
//
// 
double deg2rad(double angulo){
  return (angulo*PI)/180;
}
double rad2deg (double angulo){
  return (180* (angulo)/PI);
}

//------------------------------------------------------------------------------------------------------------
// Conversion de hora a  hora decimal
// Funcion: void hora_dec( float $hora_recib, String $texto)
// Recibe hora y string 
// 

void hora_dec( float $hora_recib, String $texto){
    float $minutos_tot=1400*$hora_recib;
    float $hora_decimal=$minutos_tot/60;
    int $hora_entera=abs($hora_decimal);
    float $min_dec=$hora_decimal-$hora_entera;
    int $min_entera=abs($min_dec*60);
    //echo " Hora recibida : ".$hora_recib." Hora decimal  ".$hora_decimal." Hora entera: ".$hora_entera." Minutos decimales ".$min_dec."<br>";
    Serial.print( $texto);
    Serial.print(" a las ");
    Serial.print($hora_entera);
    Serial.print(" : ");
    Serial.println($min_entera);
}
//------------------------------------------------------------------------------------------------------------
// Posicion
// Funcion: double posicion (double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour,int $min, int imp){
// Recibe:
// Latitud
// Longuitud
// UTC
// Año, Mes, dia
// Hora y minutos
// Flag de impresion 0 no imprime 1 si

double posicion (double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour,int $min, int imp){
Serial.print("Hora : ");
Serial.print($hour);
Serial.print(":");
Serial.println($min);

float $hora_decimal_solar =((60*$hour+$min));
$hora_decimal_solar= $hora_decimal_solar/1440;
float $d1 = 367 * $year -(7 *($year + 5001 + ($month-9)/7))/4 + (275 * $month)/9 + $day + 1729777; //era $d1
float $d = 367*$year - floor((7*($year + floor(($month+9)/12)))/4)+ floor((275*$month)/9) + $day - 730530; //era $d
float $J_century=($d1-2451545)/36525;
float $J_century_1=280.46646+$J_century*(36000.76983 + $J_century*0.0003032); //Geom Mean Long Sun (deg)

//Geom Mean Long Sun (deg) Columna I
float $residuo = fmod ($J_century_1 , 360);

// Geom Mean Anom Sun (deg) Columna J  
float $geom_Mean_A_sun=357.52911+$J_century*(35999.05029 - 0.0001537*$J_century); //Columna K

//Eccent Earth Orbit Columna K
float $EEO=0.016708634-($J_century*(0.000042037+0.0000001267*$J_century));

float $Sun_eq_of_ctr= sin(deg2rad($geom_Mean_A_sun))*(1.914602-$J_century*(0.004817+0.000014*$J_century))+sin(deg2rad(2*$geom_Mean_A_sun))*(0.019993-0.000101*$J_century)+sin(deg2rad(3*$geom_Mean_A_sun))*0.000289;
float $Sun_true_long= $Sun_eq_of_ctr+$residuo;

//Sun True Anom (deg) Columna N
float $Sun_true_anom=$Sun_eq_of_ctr+$geom_Mean_A_sun;

// Sun Rad Vector (AUs) Columna O
// float $Sun_rad_vec=$eeo;
float Sun_rad_vec = (1.000001018*(1-$EEO*$EEO))/(1+$EEO*cos(deg2rad($Sun_true_anom)));

//Sun App Long (deg) Columna P
float $Sun_app_long=$Sun_true_long-0.00569-0.00478*sin(deg2rad(125.04-1934.136*$J_century));

//Mean Obliq Ecliptic (deg) Columna Q
float $Mean_Obliq_Ecliptic=23+(26+((21.448-$J_century*(46.815+$J_century*(0.00059-$J_century*0.001813))))/60)/60;

//Obliq Corr (deg) Columna R
float $Obliq_Corr=$Mean_Obliq_Ecliptic+0.00256*cos(deg2rad(125.04-1934.136*$J_century));

//Sun Rt Ascen (deg) Columna S
//=GRADOS( ojoo!!!!!!!!!!!!!!! que el artang2 es alreves en excel que PHP atan2 (x,y) excell y atan2 (y,x) PHP
//$Sun_Rt_Ascen=atan2(cos(deg2rad($Sun_app_long)),(cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))));
// Arduino atan2 en Arduino atan2 ( y , X)
// PH atan2 (y , x)
// Excell atan2 (x,y)

float $Sun_Rt_Ascen=atan2((cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))),cos(deg2rad($Sun_app_long)));

float $Sun_Rt_Ascen_1=rad2deg($Sun_Rt_Ascen); // Verificado 19/2/2022

// Sun Declin (deg) Columna T
// $Sun_Declin =rad2deg(asin(sin(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))))
float $Sun_Declin =rad2deg(asin(sin(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))));

//var y Columna U
float $var_y=tan(deg2rad($Obliq_Corr/2))*tan(deg2rad($Obliq_Corr/2)); // Verificada OK 19/2/2022

//----------------------------------------------------------------------------------------------------------------
//Eq of Time (minutes) Columna V
float $Eq_of_time_1= $var_y*sin(2*deg2rad($residuo));
float $Eq_of_time_2=-2*$EEO*sin(deg2rad($geom_Mean_A_sun));
float a=sin(deg2rad($geom_Mean_A_sun));
float a1=cos(2*deg2rad($residuo));
float a2=a*a1;
float w = sin(deg2rad($geom_Mean_A_sun)) * cos(2*deg2rad($residuo));
float w1=$var_y * w;
float $Eq_of_time_3 = 4 * $EEO * $var_y * w1;
float $Eq_of_time_4=-0.5*$var_y*$var_y*sin(4*deg2rad($residuo));
float $Eq_of_time_5=-1.25*$EEO*$EEO*sin(2*deg2rad($geom_Mean_A_sun));
float $Eq_of_time= 4*rad2deg($Eq_of_time_1+ $Eq_of_time_2+ $Eq_of_time_3+ $Eq_of_time_4+ $Eq_of_time_5);
//----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
// HA Sunrise (deg) Columna W
// $Ha_1=GRADOS
// Verificacion
//$Sun_Declin=-15.5487; // Verificado OK HA = 101.64
float $Ha_1=cos(deg2rad(90.833))/(cos(deg2rad($lat_deg))*cos(deg2rad($Sun_Declin)));
float $Ha_2=-tan(deg2rad($lat_deg))*tan(deg2rad($Sun_Declin));
float $Ha=rad2deg(acos($Ha_1+$Ha_2)); // asi debiera ser
//----------------------------------------------------------------------------------------------------------------

// Estos hay que llevarlos a formato hh:mm:ss
// Solar Noon (LST) Columna X
float $Solar_noon_LST=(720-4*$lon_deg-$Eq_of_time+$utc*60)/1440  ;

// Sunrise Time (LST) Columna Y
float $Sunrise_Time=$Solar_noon_LST-$Ha*4/1440;
// $hour_s =date("h",$Sunrise_Time);
// $min_s =date("i",$Sunrise_Time);
hora_dec($Sunrise_Time,"Amanece  ");
// Sunset Time (LST) Columna z
// =X81+W81*4/1440 
float $Sunset_Time=$Solar_noon_LST+$Ha*4/1440;
hora_dec( $Sunset_Time, "Artardece  ");
//Sunlight Duration (minutes)  en miuntos AA -  8*W81
float  $Sunlight_duration=8*$Ha;

//---------------------------------------------------------------
// True Solar Time (min) Columna AB
float   $True_Solar_Time_1=$hour*1440+$Eq_of_time+4*$lon_deg-60*$utc;
float   $True_Solar_Time_2=$hora_decimal_solar;//$hour*1440;
float   $True_Solar_Time_5=$hora_decimal_solar*1440;
float   $True_Solar_Time_3=$Eq_of_time; //OK
float   $True_Solar_Time_4=4*$lon_deg-60*$utc; //oK
//print (" TST_1 ").$True_Solar_Time_1." TST_2 ".$True_Solar_Time_2." TST_3 ".$True_Solar_Time_3." TST_4".$True_Solar_Time_4."  tst_5 ".$True_Solar_Time_5."<br>";
$True_Solar_Time_1=$hora_decimal_solar*1440+$Eq_of_time+4*$lon_deg-60*$utc;
// ojo!!!!!!!!!!!!!! evaluar el fmod!!!
float  $True_Solar_Time=fmod($True_Solar_Time_1,1440);

//---------------------------------------------------------------
// Hour Angle (deg) Columna AC
// =SI(AB81/4<0;AB81/4+180;AB81/4-180)
float $Hour_Angle_deg;
  
  if (($True_Solar_Time/4)<0){
        $Hour_Angle_deg= ($True_Solar_Time/4)+180;   
  }else{
       $Hour_Angle_deg= ($True_Solar_Time/4)-180;   
  }
  // print(" Hour angle :  ");
  // println($Hour_Angle_deg);

//---------------------------------------------------------------
// Solar Zenith Angle (deg) Columna AD
// $Solar_Zenith_Angle=GRADOS(ACOS(.....)

float $Solar_Zenith_Angle_1 =sin(deg2rad($lat_deg))*sin(deg2rad($Sun_Declin));
float $Solar_Zenith_Angle_2 =cos(deg2rad($lat_deg))*cos(deg2rad($Sun_Declin))*cos(deg2rad($Hour_Angle_deg));
float $Solar_Zenith_Angle=rad2deg(acos($Solar_Zenith_Angle_1+$Solar_Zenith_Angle_2));

 //Solar Elevation Angle (deg)  Columna AE
float $Solar_Elevation_Angle=90-$Solar_Zenith_Angle;
// Solar Elevation corrected for atm refraction (deg)
float $Approx_Atmospheric_Refraction=0.09;
//Approx Atmospheric Refraction (deg) Columna AF
// Solar Elevation corrected for atm refraction (deg)
float $Solar_Elevation_corr_atm_refraction=$Solar_Elevation_Angle+$Approx_Atmospheric_Refraction;
elevacion_gral=$Solar_Elevation_Angle;
//---------------------------------------------------------------
//Solar Azimuth Angle (deg cw from N) - Angulo desde el Norte

    if ($Hour_Angle_deg>0){
        Serial.println( " Hour_Angle_deg>0  ");
        float $Solar_Azimuth_Angle=(((sin(deg2rad($lat_deg))*cos(deg2rad($Solar_Zenith_Angle)))-sin(deg2rad($Sun_Declin)))/(cos(deg2rad($lat_deg))*sin(deg2rad($Solar_Zenith_Angle)))); //+180
        //echo " Solar_Azimuth_Angle 1: ".$Solar_Azimuth_Angle."<br>";
        float $Solar_Azimuth_Angle_2=rad2deg(acos($Solar_Azimuth_Angle));
        float $Solar_Azimuth_Angle_3=$Solar_Azimuth_Angle_2+180;
        //echo " Solar_Azimuth_Angle 2: ".$Solar_Azimuth_Angle_2."<br>";
        //echo " Solar_Azimuth_Angle 3 +180: ".$Solar_Azimuth_Angle_3."<br>";
        float $Solar_Azimuth_Angle_4=fmod($Solar_Azimuth_Angle_3,360);
        azimut_gral=$Solar_Azimuth_Angle_4;
        
        Serial.println( " -------------------------------------------------------------------------------------");
        Serial.print ( " Solar_Elevation_Angle corregido por refraccion:  ");
        Serial.println($Solar_Elevation_corr_atm_refraction);
        Serial.print( " Solar_Azimuth_Angle 4:  ");
        Serial.println($Solar_Azimuth_Angle_4);
        Serial.println( " -------------------------------------------------------------------------------------");

    }else{
        Serial.println(" Hour_Angle_deg<0  ");
        float  $Solar_Azimuth_Angle_5=(((sin(deg2rad($lat_deg))*cos(deg2rad($Solar_Zenith_Angle)))-sin(deg2rad($Sun_Declin)))/(cos(deg2rad($lat_deg))*sin(deg2rad($Solar_Zenith_Angle))));
        $Solar_Azimuth_Angle_5=rad2deg(acos($Solar_Azimuth_Angle_5));
        $Solar_Azimuth_Angle_5=540-$Solar_Azimuth_Angle_5;
        $Solar_Azimuth_Angle_5=fmod($Solar_Azimuth_Angle_5,360);
        azimut_gral=$Solar_Azimuth_Angle_5;
        
        Serial.println( " -------------------------------------------------------------------------------------");
        Serial.print ( " Solar_Elevation_Angle corregido por refraccion:  ");
        Serial.println($Solar_Elevation_corr_atm_refraction);
        Serial.print (" Solar_Azimuth_Angle 5:  ");
        Serial.println($Solar_Azimuth_Angle_5);
        Serial.println( " -------------------------------------------------------------------------------------");

    }  

//  Rutina de impresion  
//  Si imp=0 no imprime
//  Si imp=1 imprime


    if (imp==1){

          Serial.print("Dia Juliano: ");
          Serial.print($d);
          Serial.print("  JN:  ");
          Serial.print($d1);
          Serial.print("  Julian Cenruty:  ");
          Serial.println($J_century);
          
          Serial.print("Geom Mean Long SUn (deg):  ");
          Serial.print($residuo);
          Serial.print("  Geom_Mean Anom sun:  ");
          Serial.println($geom_Mean_A_sun);
          Serial.print("  $Sun_Declin:  ");
          Serial.println($Sun_Declin);
          Serial.print(" EEO:  ");
          Serial.print($EEO,5);

          //Sun rad vector: 0.9997221507465 Sun_app_long: 341.48982184728
          Serial.print("$Sun rad vector:  ");
          Serial.println(Sun_rad_vec);
          Serial.print("Sun_app_long:  ");
          Serial.println($Sun_app_long);
          //Sun_Rt_Ascen: 0.043029001994514 Eq of Time(minutos): -12.201642904063
          Serial.print("Sun_Rt_Ascen:  ");
          Serial.print($Sun_Rt_Ascen_1);
          Serial.print(" var_y  ");
          Serial.println($var_y,4); // OK verificado
          Serial.print("  Eq of Time(minutos):  ");
          Serial.println($Eq_of_time);
          // debug Eq of time........
          Serial.print("  a:  ");
          Serial.print(a,7);
          Serial.print("  a1:  ");
          Serial.print(a1,7);
          Serial.print("  a2:  ");
          Serial.println(a2,7);

          Serial.print("  w:  ");
          Serial.print(w,7);
          Serial.print("  w1:  ");
          Serial.println(w1,7);
          
          Serial.print("  Eq1:  ");
          Serial.print($Eq_of_time_1,7);
          Serial.print("  Eq2:  ");
          Serial.print($Eq_of_time_2,7);
          Serial.print("  Eq3:  ");
          Serial.print($Eq_of_time_3,7);
          Serial.print("  Eq4:  ");
          Serial.print($Eq_of_time_4,7);
          Serial.print("  Eq5:  ");
          Serial.println($Eq_of_time_5,7); // muestra 5 digitos
          
          Serial.print("HA:  ");
          Serial.print($Ha);
          Serial.print("  h1:  ");
          Serial.print($Ha_1);
          Serial.print("   h2 :  ");
          Serial.println($Ha_2);
          Serial.print("Sun_noon_LST:  ");
          Serial.println ($Solar_noon_LST);  
          Serial.print ("Sunrise time :  ");
          Serial.println($Sunrise_Time);  
          hora_dec( $Sunrise_Time,"Amanece  ");
          Serial.print( "Sunset time :  ");
          Serial.println($Sunset_Time);
          hora_dec( $Sunset_Time,"Atardece  ");
          Serial.print( " TST_1 ");
          Serial.print($True_Solar_Time_1);
          Serial.print(" TST_2 ");
          Serial.print($True_Solar_Time_2);
          Serial.print(" TST_3 ");
          Serial.print($True_Solar_Time_3);
          Serial.print(" TST_4");
          Serial.print($True_Solar_Time_4);
          Serial.print(" tst_5 ");
          Serial.println($True_Solar_Time_5);
          Serial.print("Sun light duration :  ");
          Serial.println($Sunlight_duration);
          // True Solar Time : 1025.1965560159
          Serial.print( "True Solar Time :  ");
          Serial.println($True_Solar_Time);
          Serial.print( "Solar_Zenith_Angle:  ");
          Serial.print($Solar_Zenith_Angle);
          //Solar_Elevation_Angle: 15.405573438375
          Serial.print( "Solar_Elevation_Angle:   ");
          Serial.println($Solar_Elevation_Angle);
          //Solar_Elevation_Angle corregido por refraccion: 15.495573438375
          Serial.print("Solar_Elevation_Angle corregido por refraccion:  ");
          Serial.println($Solar_Elevation_corr_atm_refraction);   

    }
///------------------------
}

//------------------------------------------------------------------------------------------------------------
// Posicion
// Funcion: double posicion (double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour,int $min, int imp){
// Recibe:
// Latitud
// Longuitud
// UTC
// Año, Mes, dia
// Hora y minutos
// Flag de impresion 0 no imprime 1 si

double posicion_ab (double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour,int $min, int imp){
//Serial.print("Hora : ");
//Serial.print($hour);
//Serial.print(":");
//Serial.println($min);

float $hora_decimal_solar =((60*$hour+$min));
$hora_decimal_solar= $hora_decimal_solar/1440;
float $d1 = 367 * $year -(7 *($year + 5001 + ($month-9)/7))/4 + (275 * $month)/9 + $day + 1729777; //era $d1
float $d = 367*$year - floor((7*($year + floor(($month+9)/12)))/4)+ floor((275*$month)/9) + $day - 730530; //era $d
float $J_century=($d1-2451545)/36525;
float $J_century_1=280.46646+$J_century*(36000.76983 + $J_century*0.0003032); //Geom Mean Long Sun (deg)

//Geom Mean Long Sun (deg) Columna I
float $residuo = fmod ($J_century_1 , 360);

// Geom Mean Anom Sun (deg) Columna J  
float $geom_Mean_A_sun=357.52911+$J_century*(35999.05029 - 0.0001537*$J_century); //Columna K

//Eccent Earth Orbit Columna K
float $EEO=0.016708634-($J_century*(0.000042037+0.0000001267*$J_century));

float $Sun_eq_of_ctr= sin(deg2rad($geom_Mean_A_sun))*(1.914602-$J_century*(0.004817+0.000014*$J_century))+sin(deg2rad(2*$geom_Mean_A_sun))*(0.019993-0.000101*$J_century)+sin(deg2rad(3*$geom_Mean_A_sun))*0.000289;
float $Sun_true_long= $Sun_eq_of_ctr+$residuo;

//Sun True Anom (deg) Columna N
float $Sun_true_anom=$Sun_eq_of_ctr+$geom_Mean_A_sun;

// Sun Rad Vector (AUs) Columna O
// float $Sun_rad_vec=$eeo;
float Sun_rad_vec = (1.000001018*(1-$EEO*$EEO))/(1+$EEO*cos(deg2rad($Sun_true_anom)));

//Sun App Long (deg) Columna P
float $Sun_app_long=$Sun_true_long-0.00569-0.00478*sin(deg2rad(125.04-1934.136*$J_century));

//Mean Obliq Ecliptic (deg) Columna Q
float $Mean_Obliq_Ecliptic=23+(26+((21.448-$J_century*(46.815+$J_century*(0.00059-$J_century*0.001813))))/60)/60;

//Obliq Corr (deg) Columna R
float $Obliq_Corr=$Mean_Obliq_Ecliptic+0.00256*cos(deg2rad(125.04-1934.136*$J_century));

//Sun Rt Ascen (deg) Columna S
//=GRADOS( ojoo!!!!!!!!!!!!!!! que el artang2 es alreves en excel que PHP atan2 (x,y) excell y atan2 (y,x) PHP
//$Sun_Rt_Ascen=atan2(cos(deg2rad($Sun_app_long)),(cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))));
// Arduino atan2 en Arduino atan2 ( y , X)
// PH atan2 (y , x)
// Excell atan2 (x,y)

float $Sun_Rt_Ascen=atan2((cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))),cos(deg2rad($Sun_app_long)));

float $Sun_Rt_Ascen_1=rad2deg($Sun_Rt_Ascen); // Verificado 19/2/2022

// Sun Declin (deg) Columna T
// $Sun_Declin =rad2deg(asin(sin(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))))
float $Sun_Declin =rad2deg(asin(sin(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))));

//var y Columna U
float $var_y=tan(deg2rad($Obliq_Corr/2))*tan(deg2rad($Obliq_Corr/2)); // Verificada OK 19/2/2022

//----------------------------------------------------------------------------------------------------------------
//Eq of Time (minutes) Columna V
float $Eq_of_time_1= $var_y*sin(2*deg2rad($residuo));
float $Eq_of_time_2=-2*$EEO*sin(deg2rad($geom_Mean_A_sun));
float a=sin(deg2rad($geom_Mean_A_sun));
float a1=cos(2*deg2rad($residuo));
float a2=a*a1;
float w = sin(deg2rad($geom_Mean_A_sun)) * cos(2*deg2rad($residuo));
float w1=$var_y * w;
float $Eq_of_time_3 = 4 * $EEO * $var_y * w1;
float $Eq_of_time_4=-0.5*$var_y*$var_y*sin(4*deg2rad($residuo));
float $Eq_of_time_5=-1.25*$EEO*$EEO*sin(2*deg2rad($geom_Mean_A_sun));
float $Eq_of_time= 4*rad2deg($Eq_of_time_1+ $Eq_of_time_2+ $Eq_of_time_3+ $Eq_of_time_4+ $Eq_of_time_5);
//----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
// HA Sunrise (deg) Columna W
// $Ha_1=GRADOS
// Verificacion
//$Sun_Declin=-15.5487; // Verificado OK HA = 101.64
float $Ha_1=cos(deg2rad(90.833))/(cos(deg2rad($lat_deg))*cos(deg2rad($Sun_Declin)));
float $Ha_2=-tan(deg2rad($lat_deg))*tan(deg2rad($Sun_Declin));
float $Ha=rad2deg(acos($Ha_1+$Ha_2)); // asi debiera ser
//----------------------------------------------------------------------------------------------------------------

// Estos hay que llevarlos a formato hh:mm:ss
// Solar Noon (LST) Columna X
float $Solar_noon_LST=(720-4*$lon_deg-$Eq_of_time+$utc*60)/1440  ;

// Sunrise Time (LST) Columna Y
float $Sunrise_Time=$Solar_noon_LST-$Ha*4/1440;
// $hour_s =date("h",$Sunrise_Time);
// $min_s =date("i",$Sunrise_Time);
//---------------------------------------------------------------------------------------------------------------------sacado 22/6/22
//hora_dec($Sunrise_Time,"Amanece  ");
// Sunset Time (LST) Columna z
// =X81+W81*4/1440 
float $Sunset_Time=$Solar_noon_LST+$Ha*4/1440;
//---------------------------------------------------------------------------------------------------------------------sacado 22/6/22
//hora_dec( $Sunset_Time, "Artardece  ");
//Sunlight Duration (minutes)  en miuntos AA -  8*W81
float  $Sunlight_duration=8*$Ha;

//---------------------------------------------------------------
// True Solar Time (min) Columna AB
float   $True_Solar_Time_1=$hour*1440+$Eq_of_time+4*$lon_deg-60*$utc;
float   $True_Solar_Time_2=$hora_decimal_solar;//$hour*1440;
float   $True_Solar_Time_5=$hora_decimal_solar*1440;
float   $True_Solar_Time_3=$Eq_of_time; //OK
float   $True_Solar_Time_4=4*$lon_deg-60*$utc; //oK
//print (" TST_1 ").$True_Solar_Time_1." TST_2 ".$True_Solar_Time_2." TST_3 ".$True_Solar_Time_3." TST_4".$True_Solar_Time_4."  tst_5 ".$True_Solar_Time_5."<br>";
$True_Solar_Time_1=$hora_decimal_solar*1440+$Eq_of_time+4*$lon_deg-60*$utc;
// ojo!!!!!!!!!!!!!! evaluar el fmod!!!
float  $True_Solar_Time=fmod($True_Solar_Time_1,1440);

//---------------------------------------------------------------
// Hour Angle (deg) Columna AC
// =SI(AB81/4<0;AB81/4+180;AB81/4-180)
float $Hour_Angle_deg;
  
  if (($True_Solar_Time/4)<0){
        $Hour_Angle_deg= ($True_Solar_Time/4)+180;   
  }else{
       $Hour_Angle_deg= ($True_Solar_Time/4)-180;   
  }
  // print(" Hour angle :  ");
  // println($Hour_Angle_deg);

//---------------------------------------------------------------
// Solar Zenith Angle (deg) Columna AD
// $Solar_Zenith_Angle=GRADOS(ACOS(.....)

float $Solar_Zenith_Angle_1 =sin(deg2rad($lat_deg))*sin(deg2rad($Sun_Declin));
float $Solar_Zenith_Angle_2 =cos(deg2rad($lat_deg))*cos(deg2rad($Sun_Declin))*cos(deg2rad($Hour_Angle_deg));
float $Solar_Zenith_Angle=rad2deg(acos($Solar_Zenith_Angle_1+$Solar_Zenith_Angle_2));

 //Solar Elevation Angle (deg)  Columna AE
float $Solar_Elevation_Angle=90-$Solar_Zenith_Angle;
// Solar Elevation corrected for atm refraction (deg)
float $Approx_Atmospheric_Refraction=0.09;
//Approx Atmospheric Refraction (deg) Columna AF
// Solar Elevation corrected for atm refraction (deg)
float $Solar_Elevation_corr_atm_refraction=$Solar_Elevation_Angle+$Approx_Atmospheric_Refraction;
elevacion_gral=$Solar_Elevation_Angle;
//---------------------------------------------------------------
//Solar Azimuth Angle (deg cw from N) - Angulo desde el Norte

    if ($Hour_Angle_deg>0){
        //Serial.println( " Hour_Angle_deg>0  ");
        float $Solar_Azimuth_Angle=(((sin(deg2rad($lat_deg))*cos(deg2rad($Solar_Zenith_Angle)))-sin(deg2rad($Sun_Declin)))/(cos(deg2rad($lat_deg))*sin(deg2rad($Solar_Zenith_Angle)))); //+180
        //echo " Solar_Azimuth_Angle 1: ".$Solar_Azimuth_Angle."<br>";
        float $Solar_Azimuth_Angle_2=rad2deg(acos($Solar_Azimuth_Angle));
        float $Solar_Azimuth_Angle_3=$Solar_Azimuth_Angle_2+180;
        //echo " Solar_Azimuth_Angle 2: ".$Solar_Azimuth_Angle_2."<br>";
        //echo " Solar_Azimuth_Angle 3 +180: ".$Solar_Azimuth_Angle_3."<br>";
        float $Solar_Azimuth_Angle_4=fmod($Solar_Azimuth_Angle_3,360);
        azimut_gral=$Solar_Azimuth_Angle_4;
        
        //Serial.println( " -------------------------------------------------------------------------------------");
        //Serial.print ( " Solar_Elevation_Angle corregido por refraccion:  ");
        //Serial.println($Solar_Elevation_corr_atm_refraction);
        //Serial.print( " Solar_Azimuth_Angle 4:  ");
        //Serial.println($Solar_Azimuth_Angle_4);
        //Serial.println( " -------------------------------------------------------------------------------------");

    }else{
        //Serial.println(" Hour_Angle_deg<0  ");
        float  $Solar_Azimuth_Angle_5=(((sin(deg2rad($lat_deg))*cos(deg2rad($Solar_Zenith_Angle)))-sin(deg2rad($Sun_Declin)))/(cos(deg2rad($lat_deg))*sin(deg2rad($Solar_Zenith_Angle))));
        $Solar_Azimuth_Angle_5=rad2deg(acos($Solar_Azimuth_Angle_5));
        $Solar_Azimuth_Angle_5=540-$Solar_Azimuth_Angle_5;
        $Solar_Azimuth_Angle_5=fmod($Solar_Azimuth_Angle_5,360);
        azimut_gral=$Solar_Azimuth_Angle_5;
        
        //Serial.println( " -------------------------------------------------------------------------------------");
        //Serial.print ( " Solar_Elevation_Angle corregido por refraccion:  ");
        //Serial.println($Solar_Elevation_corr_atm_refraction);
        //Serial.print (" Solar_Azimuth_Angle 5:  ");
        //Serial.println($Solar_Azimuth_Angle_5);
        //Serial.println( " -------------------------------------------------------------------------------------");

    }  

//  Rutina de impresion  
//  Si imp=0 no imprime
//  Si imp=1 imprime


    if (imp==1){

          Serial.print("Dia Juliano: ");
          Serial.print($d);
          Serial.print("  JN:  ");
          Serial.print($d1);
          Serial.print("  Julian Cenruty:  ");
          Serial.println($J_century);
          
          Serial.print("Geom Mean Long SUn (deg):  ");
          Serial.print($residuo);
          Serial.print("  Geom_Mean Anom sun:  ");
          Serial.println($geom_Mean_A_sun);
          Serial.print("  $Sun_Declin:  ");
          Serial.println($Sun_Declin);
          Serial.print(" EEO:  ");
          Serial.print($EEO,5);

          //Sun rad vector: 0.9997221507465 Sun_app_long: 341.48982184728
          Serial.print("$Sun rad vector:  ");
          Serial.println(Sun_rad_vec);
          Serial.print("Sun_app_long:  ");
          Serial.println($Sun_app_long);
          //Sun_Rt_Ascen: 0.043029001994514 Eq of Time(minutos): -12.201642904063
          Serial.print("Sun_Rt_Ascen:  ");
          Serial.print($Sun_Rt_Ascen_1);
          Serial.print(" var_y  ");
          Serial.println($var_y,4); // OK verificado
          Serial.print("  Eq of Time(minutos):  ");
          Serial.println($Eq_of_time);
          // debug Eq of time........
          Serial.print("  a:  ");
          Serial.print(a,7);
          Serial.print("  a1:  ");
          Serial.print(a1,7);
          Serial.print("  a2:  ");
          Serial.println(a2,7);

          Serial.print("  w:  ");
          Serial.print(w,7);
          Serial.print("  w1:  ");
          Serial.println(w1,7);
          
          Serial.print("  Eq1:  ");
          Serial.print($Eq_of_time_1,7);
          Serial.print("  Eq2:  ");
          Serial.print($Eq_of_time_2,7);
          Serial.print("  Eq3:  ");
          Serial.print($Eq_of_time_3,7);
          Serial.print("  Eq4:  ");
          Serial.print($Eq_of_time_4,7);
          Serial.print("  Eq5:  ");
          Serial.println($Eq_of_time_5,7); // muestra 5 digitos
          
          Serial.print("HA:  ");
          Serial.print($Ha);
          Serial.print("  h1:  ");
          Serial.print($Ha_1);
          Serial.print("   h2 :  ");
          Serial.println($Ha_2);
          Serial.print("Sun_noon_LST:  ");
          Serial.println ($Solar_noon_LST);  
          Serial.print ("Sunrise time :  ");
          Serial.println($Sunrise_Time);  
          hora_dec( $Sunrise_Time,"Amanece  ");
          Serial.print( "Sunset time :  ");
          Serial.println($Sunset_Time);
          hora_dec( $Sunset_Time,"Atardece  ");
          Serial.print( " TST_1 ");
          Serial.print($True_Solar_Time_1);
          Serial.print(" TST_2 ");
          Serial.print($True_Solar_Time_2);
          Serial.print(" TST_3 ");
          Serial.print($True_Solar_Time_3);
          Serial.print(" TST_4");
          Serial.print($True_Solar_Time_4);
          Serial.print(" tst_5 ");
          Serial.println($True_Solar_Time_5);
          Serial.print("Sun light duration :  ");
          Serial.println($Sunlight_duration);
          // True Solar Time : 1025.1965560159
          Serial.print( "True Solar Time :  ");
          Serial.println($True_Solar_Time);
          Serial.print( "Solar_Zenith_Angle:  ");
          Serial.print($Solar_Zenith_Angle);
          //Solar_Elevation_Angle: 15.405573438375
          Serial.print( "Solar_Elevation_Angle:   ");
          Serial.println($Solar_Elevation_Angle);
          //Solar_Elevation_Angle corregido por refraccion: 15.495573438375
          Serial.print("Solar_Elevation_Angle corregido por refraccion:  ");
          Serial.println($Solar_Elevation_corr_atm_refraction);   

    }
///------------------------
}



void comparacion(){
  //double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour
  int $utc=-3;
  int $year=2022;
  int $month=6;
  int $day=22;
  //int hora_array;   // Variable global
  //int min_array;    // Variable global
  //int seg_array;    // Variable global
  //elevacion_gral;   // Variable global
  //azimut_gral;      // Variable global
  for(int i = 0; i < 31; i++){
    // Array perteneciente al 15 de enero 2022
    hora_15[i];
    hora_a(hora_15[i]);
    a_elevacion_15[i];
    a_azimut_15[i];
    
    posicion_ab ($lat_deg,$lon_deg,$utc,$year,$month,$day,hora_array,min_array, 0);
    //Serial.print("---------------------------------------------------------------------------------------------------------------------------------- ");
    /*
    Serial.println();
    Serial.print("indice: ");
    Serial.print(i);
    Serial.print(" hora: ");
    Serial.print(hora_array);
    Serial.print(" Min: ");
    Serial.print(min_array);
    Serial.print(" Elevacion array: ");
    Serial.print(a_elevacion_15[i]); 
    Serial.print(" Elevacion calculado: ");
    Serial.print(elevacion_gral); 
    Serial.print(" Azimut array: ");
    Serial.print(a_azimut_15[i]); 
    Serial.print(" Azimut calculado: ");
    Serial.println(azimut_gral); 
*/
    //Serial.println();
    Serial.print("indice: ");
    Serial.print(i);
    Serial.print(",");
    Serial.print(hora_array);
    Serial.print(":");
    Serial.print(min_array);
    Serial.print(",");
    Serial.print(a_elevacion_15[i]); 
    Serial.print(",");
    Serial.print(elevacion_gral); 
    Serial.print(",");
    Serial.print(a_azimut_15[i]); 
    Serial.print(",");
    Serial.println(azimut_gral); 
    
  }
}

// Aqui esta la Matematica original
/*
//------------------------------------------------------------------------------------------------------------
// Posicion
// Funcion: double posicion (double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour,int $min, int imp){
// Recibe:
// Latitud
// Longuitud
// UTC
// Año, Mes, dia
// Hora y minutos
// Flag de impresion 0 no imprime 1 si

double posicion (double $lat_deg,double $lon_deg,int $utc,int $year,int $month,int $day,int $hour,int $min, int imp){
Serial.print("Hora : ");
Serial.print($hour);
Serial.print(":");
Serial.println($min);

float $hora_decimal_solar =((60*$hour+$min));
$hora_decimal_solar= $hora_decimal_solar/1440;
//$hora_decimal_solar = 0.45;
float $d1 = 367 * $year -(7 *($year + 5001 + ($month-9)/7))/4 + (275 * $month)/9 + $day + 1729777; //era $d1
float $d = 367*$year - floor((7*($year + floor(($month+9)/12)))/4)+ floor((275*$month)/9) + $day - 730530; //era $d
float $J_century=($d1-2451545)/36525;
float $J_century_1=280.46646+$J_century*(36000.76983 + $J_century*0.0003032); //Geom Mean Long Sun (deg)
//Geom Mean Long Sun (deg) Columna I
//$residuo = fmod($J_century_1, 360);//Geom Mean Long Sun (deg)
float $residuo = fmod ($J_century_1 , 360);
// Geom Mean Anom Sun (deg) Columna J  
float $geom_Mean_A_sun=357.52911+$J_century*(35999.05029 - 0.0001537*$J_century); //Columna K
//Eccent Earth Orbit Columna K
// $eeo=0.016708634-$J_century*(0.000042037+0.0000001267*$J_century);
float $EEO=0.016708634-($J_century*(0.000042037+0.0000001267*$J_century));

//$a=.016708634-$a;
//int_eeo =.016708634-($J_century*(0.000042037+0.0000001267*$J_century));
float $Sun_eq_of_ctr= sin(deg2rad($geom_Mean_A_sun))*(1.914602-$J_century*(0.004817+0.000014*$J_century))+sin(deg2rad(2*$geom_Mean_A_sun))*(0.019993-0.000101*$J_century)+sin(deg2rad(3*$geom_Mean_A_sun))*0.000289;
float $Sun_true_long= $Sun_eq_of_ctr+$residuo;
//Sun True Anom (deg) Columna N
float $Sun_true_anom=$Sun_eq_of_ctr+$geom_Mean_A_sun;
// Sun Rad Vector (AUs) Columna O
//float $Sun_rad_vec=$eeo;
float Sun_rad_vec = (1.000001018*(1-$EEO*$EEO))/(1+$EEO*cos(deg2rad($Sun_true_anom)));
//Sun App Long (deg) Columna P
float $Sun_app_long=$Sun_true_long-0.00569-0.00478*sin(deg2rad(125.04-1934.136*$J_century));
//Mean Obliq Ecliptic (deg) Columna Q
float $Mean_Obliq_Ecliptic=23+(26+((21.448-$J_century*(46.815+$J_century*(0.00059-$J_century*0.001813))))/60)/60;
//Obliq Corr (deg) Columna R
float $Obliq_Corr=$Mean_Obliq_Ecliptic+0.00256*cos(deg2rad(125.04-1934.136*$J_century));
//Sun Rt Ascen (deg) Columna S
//=GRADOS( ojoo!!!!!!!!!!!!!!! que el artang2 es alreves en excel que PHP atan2 (x,y) excell y atan2 (y,x) PHP
//$Sun_Rt_Ascen=atan2(cos(deg2rad($Sun_app_long)),(cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))));
// Arduino atan2 en Arduino atan2 ( y , X)
// PH atan2 (y , x)
// Excell atan2 (x,y)

float $Sun_Rt_Ascen=atan2((cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))),cos(deg2rad($Sun_app_long)));
// No..............float $Sun_Rt_Ascen_x=atan2(cos(deg2rad($Sun_app_long)),(cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long)))); // Asi esta en excell
//$a=cos(deg2rad($Sun_app_long));
//$b=cos(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long));
//$c=atan2($b,$a);
//float c= rad2deg($Sun_Rt_Ascen);

float $Sun_Rt_Ascen_1=rad2deg($Sun_Rt_Ascen); // Verificado 19/2/2022

//  Sun Declin (deg) Columna T
// $Sun_Declin =rad2deg(asin(sin(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))))
float $Sun_Declin =rad2deg(asin(sin(deg2rad($Obliq_Corr))*sin(deg2rad($Sun_app_long))));
    //var y Columna U
float $var_y=tan(deg2rad($Obliq_Corr/2))*tan(deg2rad($Obliq_Corr/2)); // Verificada OK 19/2/2022
//Eq of Time (minutes) Columna V
//$Eq_of_time_1=4*GRADOS(U81*SENO(2*RADIANES(I81))-2*K81*SENO(RADIANES(J81))+4*K81*U81*SENO(RADIANES(J81))*COS(2*RADIANES(I81))-0,5*U81*U81*SENO(4*RADIANES(I81))-1,25*K81*K81*SENO(2*RADIANES(J81)))
float $Eq_of_time_1= $var_y*sin(2*deg2rad($residuo));
float $Eq_of_time_2=-2*$EEO*sin(deg2rad($geom_Mean_A_sun));
float a=sin(deg2rad($geom_Mean_A_sun));
float a1=cos(2*deg2rad($residuo));
float a2=a*a1;
float w = sin(deg2rad($geom_Mean_A_sun)) * cos(2*deg2rad($residuo));
float w1=$var_y * w;
float $Eq_of_time_3 = 4 * $EEO * $var_y * w1;
float $Eq_of_time_4=-0.5*$var_y*$var_y*sin(4*deg2rad($residuo));
float $Eq_of_time_5=-1.25*$EEO*$EEO*sin(2*deg2rad($geom_Mean_A_sun));
float $Eq_of_time= 4*rad2deg($Eq_of_time_1+ $Eq_of_time_2+ $Eq_of_time_3+ $Eq_of_time_4+ $Eq_of_time_5);

// HA Sunrise (deg) Columna W
// $Ha_1=GRADOS
// Verificacion
//$Sun_Declin=-15.5487; // Verificado OK HA = 101.64
float $Ha_1=cos(deg2rad(90.833))/(cos(deg2rad($lat_deg))*cos(deg2rad($Sun_Declin)));
float $Ha_2=-tan(deg2rad($lat_deg))*tan(deg2rad($Sun_Declin));
float $Ha=rad2deg(acos($Ha_1+$Ha_2)); // asi debiera ser


// Estos hay que llevarlos a formato hh:mm:ss
// Solar Noon (LST) Columna X
float $Solar_noon_LST=(720-4*$lon_deg-$Eq_of_time+$utc*60)/1440  ;
// Sunrise Time (LST) Columna Y
float $Sunrise_Time=$Solar_noon_LST-$Ha*4/1440;
// $hour_s =date("h",$Sunrise_Time);
// $min_s =date("i",$Sunrise_Time);
hora_dec($Sunrise_Time,"Amanece  ");
// Sunset Time (LST) Columna z
// =X81+W81*4/1440 
float $Sunset_Time=$Solar_noon_LST+$Ha*4/1440;
hora_dec( $Sunset_Time, "Artardece  ");
//Sunlight Duration (minutes)  en miuntos AA 
// 8*W81
float  $Sunlight_duration=8*$Ha;

//---------------------------------------------------------------
// True Solar Time (min) Columna AB
float  $True_Solar_Time_1=$hour*1440+$Eq_of_time+4*$lon_deg-60*$utc;
float   $True_Solar_Time_2=$hora_decimal_solar;//$hour*1440;
float  $True_Solar_Time_5=$hora_decimal_solar*1440;
float  $True_Solar_Time_3=$Eq_of_time; //OK
float  $True_Solar_Time_4=4*$lon_deg-60*$utc; //oK
//print (" TST_1 ").$True_Solar_Time_1." TST_2 ".$True_Solar_Time_2." TST_3 ".$True_Solar_Time_3." TST_4".$True_Solar_Time_4."  tst_5 ".$True_Solar_Time_5."<br>";
$True_Solar_Time_1=$hora_decimal_solar*1440+$Eq_of_time+4*$lon_deg-60*$utc;
// ojo!!!!!!!!!!!!!! evaluar el fmod!!!
float  $True_Solar_Time=fmod($True_Solar_Time_1,1440);
//---------------------------------------------------------------
//Hour Angle (deg) Columna AC
//=SI(AB81/4<0;AB81/4+180;AB81/4-180)
float      $Hour_Angle_deg;
  if (($True_Solar_Time/4)<0){
        $Hour_Angle_deg= ($True_Solar_Time/4)+180;   
  }else{
       $Hour_Angle_deg= ($True_Solar_Time/4)-180;   
  }
  //print(" Hour angle :  ");
//println($Hour_Angle_deg);

    //---------------------------------------------------------------
    //Solar Zenith Angle (deg) Columna AD
    //$Solar_Zenith_Angle=
    //=GRADOS(ACOS(
float $Solar_Zenith_Angle_1 =sin(deg2rad($lat_deg))*sin(deg2rad($Sun_Declin));
float $Solar_Zenith_Angle_2 =cos(deg2rad($lat_deg))*cos(deg2rad($Sun_Declin))*cos(deg2rad($Hour_Angle_deg));
float $Solar_Zenith_Angle=rad2deg(acos($Solar_Zenith_Angle_1+$Solar_Zenith_Angle_2));
 //Solar Elevation Angle (deg)  Columna AE
float $Solar_Elevation_Angle=90-$Solar_Zenith_Angle;
// Solar Elevation corrected for atm refraction (deg)
float $Approx_Atmospheric_Refraction=0.09;
//Approx Atmospheric Refraction (deg) Columna AF
// Solar Elevation corrected for atm refraction (deg)
float $Solar_Elevation_corr_atm_refraction=$Solar_Elevation_Angle+$Approx_Atmospheric_Refraction;

//---------------------------------------------------------------
//Solar Azimuth Angle (deg cw from N)

    if ($Hour_Angle_deg>0){
        Serial.println( " Hour_Angle_deg>0  ");
        float $Solar_Azimuth_Angle=(((sin(deg2rad($lat_deg))*cos(deg2rad($Solar_Zenith_Angle)))-sin(deg2rad($Sun_Declin)))/(cos(deg2rad($lat_deg))*sin(deg2rad($Solar_Zenith_Angle)))); //+180
        //echo " Solar_Azimuth_Angle 1: ".$Solar_Azimuth_Angle."<br>";
        float $Solar_Azimuth_Angle_2=rad2deg(acos($Solar_Azimuth_Angle));
        float $Solar_Azimuth_Angle_3=$Solar_Azimuth_Angle_2+180;
        //echo " Solar_Azimuth_Angle 2: ".$Solar_Azimuth_Angle_2."<br>";
        //echo " Solar_Azimuth_Angle 3 +180: ".$Solar_Azimuth_Angle_3."<br>";
        float $Solar_Azimuth_Angle_4=fmod($Solar_Azimuth_Angle_3,360);
        
        Serial.println( " -------------------------------------------------------------------------------------");
        Serial.print ( " Solar_Elevation_Angle corregido por refraccion:  ");
        Serial.println($Solar_Elevation_corr_atm_refraction);
        Serial.print( " Solar_Azimuth_Angle 4:  ");
        Serial.println($Solar_Azimuth_Angle_4);
        Serial.println( " -------------------------------------------------------------------------------------");

    }else{      
      Serial.println(" Hour_Angle_deg<0  ");
      float  $Solar_Azimuth_Angle_5=(((sin(deg2rad($lat_deg))*cos(deg2rad($Solar_Zenith_Angle)))-sin(deg2rad($Sun_Declin)))/(cos(deg2rad($lat_deg))*sin(deg2rad($Solar_Zenith_Angle))));
  //      echo " Solar_Azimuth_Angle 1: ".$Solar_Azimuth_Angle_5."<br>";
        $Solar_Azimuth_Angle_5=rad2deg(acos($Solar_Azimuth_Angle_5));
  //      echo " Solar_Azimuth_Angle 2: ".$Solar_Azimuth_Angle_5."<br>";
        $Solar_Azimuth_Angle_5=540-$Solar_Azimuth_Angle_5;
  //      echo " Solar_Azimuth_Angle 3: ".$Solar_Azimuth_Angle_5."<br>";
        $Solar_Azimuth_Angle_5=fmod($Solar_Azimuth_Angle_5,360);
        
        Serial.println( " -------------------------------------------------------------------------------------");
        Serial.print ( " Solar_Elevation_Angle corregido por refraccion:  ");
        Serial.println($Solar_Elevation_corr_atm_refraction);
        Serial.print (" Solar_Azimuth_Angle 4:  ");
        Serial.println($Solar_Azimuth_Angle_5);
        Serial.println( " -------------------------------------------------------------------------------------");

    }  
// Rutina de impresion  
/// Si imp=0 no imprime
//  Si imp=1 imprime


    if (imp==1){

          Serial.print("Dia Juliano: ");
          Serial.print($d);
          Serial.print("  JN:  ");
          Serial.print($d1);
          Serial.print("  Julian Cenruty:  ");
          Serial.println($J_century);
          
          Serial.print("Geom Mean Long SUn (deg):  ");
          Serial.print($residuo);
          Serial.print("  Geom_Mean Anom sun:  ");
          Serial.println($geom_Mean_A_sun);
          Serial.print("  $Sun_Declin:  ");
          Serial.println($Sun_Declin);
          Serial.print(" EEO:  ");
          Serial.print($EEO,5);

          //Sun rad vector: 0.9997221507465 Sun_app_long: 341.48982184728
          Serial.print("$Sun rad vector:  ");
          Serial.println(Sun_rad_vec);
          Serial.print("Sun_app_long:  ");
          Serial.println($Sun_app_long);
          //Sun_Rt_Ascen: 0.043029001994514 Eq of Time(minutos): -12.201642904063
          Serial.print("Sun_Rt_Ascen:  ");
          Serial.print($Sun_Rt_Ascen_1);
          Serial.print(" var_y  ");
          Serial.println($var_y,4); // OK verificado
          Serial.print("  Eq of Time(minutos):  ");
          Serial.println($Eq_of_time);
          // debug Eq of time........
          Serial.print("  a:  ");
          Serial.print(a,7);
          Serial.print("  a1:  ");
          Serial.print(a1,7);
          Serial.print("  a2:  ");
          Serial.println(a2,7);

          Serial.print("  w:  ");
          Serial.print(w,7);
          Serial.print("  w1:  ");
          Serial.println(w1,7);
          
          Serial.print("  Eq1:  ");
          Serial.print($Eq_of_time_1,7);
          Serial.print("  Eq2:  ");
          Serial.print($Eq_of_time_2,7);
          Serial.print("  Eq3:  ");
          Serial.print($Eq_of_time_3,7);
          Serial.print("  Eq4:  ");
          Serial.print($Eq_of_time_4,7);
          Serial.print("  Eq5:  ");
          Serial.println($Eq_of_time_5,7); // muestra 5 digitos
          

          //float $Eq_of_time= 4*rad2deg($Eq_of_time_1+ $Eq_of_time_2+ $Eq_of_time_3+ $Eq_of_time_4+ $Eq_of_time_5);
          //HA: 95.832368454416 h1: -0.017562208388791 h2 : -0.084056116668994
          Serial.print("HA:  ");
          Serial.print($Ha);
          Serial.print("  h1:  ");
          Serial.print($Ha_1);
          Serial.print("   h2 :  ");
          Serial.println($Ha_2);
          //Sun_noon_LST: 0.57972461387782
          Serial.print("Sun_noon_LST:  ");
          Serial.println ($Solar_noon_LST);  
          //Sunrise time : 0.31352359039333
          Serial.print ("Sunrise time :  ");
          Serial.println($Sunrise_Time);  
          hora_dec( $Sunrise_Time,"Amanece  ");
          Serial.print( "Sunset time :  ");
          Serial.println($Sunset_Time);
          hora_dec( $Sunset_Time,"Atardece  ");
          // TST_1 9965.1965560159 TST_2 0.79166666666667 TST_3 -12.201642904063 TST_4-102.60180108 tst_5 1140
          Serial.print( " TST_1 ");
          Serial.print($True_Solar_Time_1);
          Serial.print(" TST_2 ");
          Serial.print($True_Solar_Time_2);
          Serial.print(" TST_3 ");
          Serial.print($True_Solar_Time_3);
          Serial.print(" TST_4");
          Serial.print($True_Solar_Time_4);
          Serial.print(" tst_5 ");
          Serial.println($True_Solar_Time_5);
          

          // TST_1 9965.1965560159 TST_2 0.79166666666667 TST_3 -12.201642904063 TST_4-102.60180108 tst_5 1140
          //Sun light duration : 766.65894763533
          Serial.print("Sun light duration :  ");
          Serial.println($Sunlight_duration);
          // True Solar Time : 1025.1965560159
          Serial.print( "True Solar Time :  ");
          Serial.println($True_Solar_Time);
          //Hour angle : 76.299139003984
          //Solar_Zenith_Angle: 74.594426561625
          Serial.print( "Solar_Zenith_Angle:  ");
          Serial.print($Solar_Zenith_Angle);
          //Solar_Elevation_Angle: 15.405573438375
          Serial.print( "Solar_Elevation_Angle:   ");
          Serial.println($Solar_Elevation_Angle);
          //Solar_Elevation_Angle corregido por refraccion: 15.495573438375
          Serial.print("Solar_Elevation_Angle corregido por refraccion:  ");
          Serial.println($Solar_Elevation_corr_atm_refraction);   

    }
///------------------------
}
*/
/*

float dif_elevacion;
float dif_azimut;

//-------------------------------------------------------------------------------------------------  
// Probamos hacer una union de ambas rutinas 25/10/2021 

  if (ind!=ind_det){ // 
    dif_elevacion=(a_elevacion[ind]-a_elevacion[ind-1]);
    dif_azimut=(a_azimut[ind]-a_azimut[ind-1]);
    if (dif_azimut>=30){
      dif_azimut=360-dif_azimut;
    }
    
    if (dif_azimut>=200){
      dif_azimut=(a_azimut[ind-1]-a_azimut[ind]);
    }
    if (dif_elevacion<=0)flag_giro_elev=0;
    if (dif_elevacion>=0)flag_giro_elev=1;
    if (dif_azimut<=0)flag_giro_azim=0;
    //if (dif_azimut>=0)flag_giro_azim=1;
    
  }else{ // aqui entra si empieza la secuencia o si empieza en algun momento es pecial
  
  //
  // Cuando el equipo esta entre 0 y 180 el flag de giro para azimut es "0"
  // Cuando el equipo esta entre 360 y 270 el flag de giro debe ser 1
  // el tema es como saber donde estaba?? puede ser a partir de ind??
  
    dif_elevacion=a_elevacion[ind];
    dif_azimut=a_azimut[ind];
    
    if (dif_elevacion<=0)flag_giro_elev=0;
    if (dif_elevacion>=0)flag_giro_elev=1;
    //if encoder>=270{}
    if (ind>=12){
      flag_giro_azim=0;
      dif_azimut=360-dif_azimut;
    }else{
      if (dif_azimut<=0)flag_giro_azim=0;
      if (dif_azimut>=0)flag_giro_azim=1;
    }
  }
  
  
//imprimir (hora[ind],a_elevacion[ind],abs(dif_elevacion),flag_giro_elev,a_azimut[ind],abs(dif_azimut),(flag_giro_azim));
//elevacion ((abs(dif_elevacion))*pasos_elevacion_1, flag_giro_elev);
//-------------------------------------------------------------------------
// Bugs a resolver - Me da 1 el flag de giro y se vuelve - Va cero cero y por ahi calcula un 1
//  if ((flag_giro_azim==1) && (ind!=0) ){
//    flag_giro_azim=0;
//  }

//-----------------------------------------------------------------------------------------------------------------------
// Envia Mensaje al telefono configurado
// Aqui hay dos telefonos
// 
 /* Funciona OK !!!!
 //if (ind == 1){
 //  envia_mensaje_sms(SMS_TARGET_1,tempC);
 //  envia_mensaje_sms(SMS_TARGET_2,tempC);
 //}
 
 
//----------------------------------------------------------------------------------------------------------------------
// Envio datos mientras esta en stand_by pasando la noche

  is_alive();  

//---------------------------------------------------------------------------------------------------------------------
// Aqui encontramos coincidencia y movemos
// Vamos a sacar esta logica 3/1/22

if (hora_rtc==hora_array_v[ind]) {
  if (min_rtc==min_array_v[ind]){
    Serial.print("- LLegamos a una hora y min coincidente!!!,  en el arreglo: ");
    Serial.println(ind);
  
  // primer movimiento
  imprimir (hora_array_v[ind],a_elevacion[ind],abs(dif_elevacion),flag_giro_elev,a_azimut[ind],abs(dif_azimut),(flag_giro_azim));
  elevacion ((abs(dif_elevacion))*pasos_elevacion_1, flag_giro_elev);
  azimut ((abs(dif_azimut))*pasos_azimut_1,flag_giro_azim); // Poner en cero
  
  Serial.print("Aqui hacemos todo!!!  ");
  //delay(15000);
  delay(7000); // Espera 10 segundos para volver
  
  //---------------------------------------------------------------------------------------------------------------------
  // Almaceno el azimut en el encoder
  encoder = a_azimut[ind];
  preferences.putUInt("encoder", encoder);
  delay(500);
  Serial.print("Encoder: ");
  Serial.println(encoder);
  //---------------------------------------------------------------------------------------------------------------------
  envia(); // Envia los datos a la nube....
  grado_positivo=grado_positivo+10;
  ind=ind+1;   
  }
}else{
    Serial.print("esperando...");
    Serial.println(ind);
    encoder = preferences.getUInt("encoder", 0);
    Serial.print("Encoder: ");
    Serial.println(encoder);
}
     
//--------------------------------------------------------------------------------------------------------------------------
// Aqui determinamos si llego al ultimo valor..........    
//--------------------------------------------------------------------------------------------------------------------------
      if (ind>=indice_max){
        Serial.print("- Reset de arreglo: ");
        Serial.println(ind);
        Serial.print("Ajustando a cero elevacion....");
        buscar_cero_elevacion();
        Serial.print("Ajustando a cero azimut....");
        buscar_cero_azimut(300); // depende del angulo es el giro horario o antihorario
        delay (5000);
        // Preparo los codigos para enviar
        // Podria enviar alguos codigos de culminacion OK
        float a=promedio (ind);
        iAN0 =60;     //Codigo de culminacion OK
        iAN1= a;      //promedio
        iAN2= ind;    //ind
        envia();
        Serial.print("Promedio: ");
        Serial.println(a);
        ind=0;
    }
//--------------------------------------------------------------------------------------------------------------------------
*/
