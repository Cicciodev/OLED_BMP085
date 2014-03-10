/*********************************************************************
This combines the use fo Adafruit's Monochrome OLEDs based on SSD1306 drivers
with a Bosch BMP085/BMP180 sensor to display temperature, barometric pressure
and altitude a 128x64 OLED display using an SS1306 display driver.

I purchased everything from Limor's excellent Adafruit shop!
  ------> http://www.adafruit.com/category/63_98

Required hardware:
  -Arduino (I used Uno Rev3)
  -Bosch BMP085/BMP180 temp/baro sensor
  -SSD1306 128x64 OLED display

   History
   =======
   2013/JUN/17  - Updated altitude calculations (KTOWN)
   2013/FEB/13  - First version (KTOWN)
   2014/JAN/18  - Unique ID= 10085 (brycej)
  
*********************************************************************/
#include <Wire.h>                
#include <Adafruit_GFX.h>        /* OLED/LCD Gisplay grapshics libraey */
#include <Adafruit_SSD1306.h>    /* OLED Graphics dipslay              */
#include <Adafruit_Sensor.h>     /* Adafruit's Sensor incl. BMP085/180 */ 
#include <Adafruit_BMP085_U.h>   /* Adafruit's Sensor incl. BMP085/180 */

/* The BMP085/BMP180 uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you need to download the Adafruit_Sensor
   library and include it in your libraries folder. Reference
   Adafruit's tutorial on how to install libraries if you are not familiar.
   
   If you have multiple I2C sensors you should also assign a unique ID(I2C)
   to this sensor for use with the Adafruit Sensor API so that you can identify 
   this particular sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (10085
   is used in this example).
   
   Connections for the Bosch BMP085/ BMP180 Sensor
   ===========
   Connect Sensor_SDA to analog 4
   Connect Sensor_SCL to analog 5
   Connect Sensor_VDD to 3.3V DC (5V tolerant)
   Connect Sensor_GROUND to common ground
*/
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

/* 
   To use this OLED driver you will need to download the Adafruit
   OLED library and include it in your libraries folder. I used Adafruit's
   128x64 OLED. It uses the SSD1306 driver and this sketch uses an SPI
   interface to the OLED for a speedy refresh!
   
   Connections for the Adafruit 128x64 SPI OLED
   ===========
   Connect OLED_MOSI(DATA) 9
   Connect OLED_CLS(Clock) 10
   Connect OLED_DC(SAO) 11
   Connect OLED_RESET 13
   Connect OLED_CS 12
   Connect OLED_VDD(Vin) to 3.3V DC (5V tolerant)
   Connect OLED_GROUND to common ground


*/

#define OLED_DC 11
#define OLED_CS 12
#define OLED_CLK 10
#define OLED_MOSI 9
#define OLED_RESET 13  
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Check to make sure the OLED display is 128x64 in size, if not stop!  */
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("--------------------");
  display.print  ("Sensor: "); display.println(sensor.name);
  display.print  ("Drv. Ver: "); display.println(sensor.version);
  display.print  ("Unique ID: "); display.println(sensor.sensor_id);
  display.print  ("Max Val.:"); display.print(sensor.max_value); display.println(" hPa");
  display.print  ("Min Val.:"); display.print(sensor.min_value); display.println(" hPa");
  display.print  ("Res.: "); display.print(sensor.resolution); display.println(" hPa");  
  display.println("--------------------");
  display.println("");
  display.display();
  delay(3000);
}


/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)  
{
  Serial.begin(9600);
 
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  /* Display some basic information on this temp/barometer sensor */
  displaySensorDetails();

  /* Initialise the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    display.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }  
    /* text display text. */           
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(" ");
  display.println("Setting Up Bosch");
  display.println("BMP085/BMP180 Sensor");
  display.println("  ....brycej");
  display.display();
  delay(3000);


} 

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)  
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(WHITE);

  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure)
  {

     
    // Now send the same data to the display

    display.setCursor(0,0);
    display.print("BARO: ");
    display.print(event.pressure);
    display.println(" hPa");
    //display.display();
 
     /* Display atmospheric pressue in inHg */
    float PRESSURE_HG;
    /* Convert HPa to inches Mercury       */
    PRESSURE_HG= (event.pressure * 0.02952998751);
    display.print("BARO: ");
    display.print(PRESSURE_HG);
    display.print(" inHg ");
    /* Decimal 94 is an up arrown          */
    display.write(24);
    /* Decimal 118 is a down arrow         */
    // Serial.write(118);
    display.println();
    //display.display();
    
    // Display temperature in Celsius
    // bmp.getTemperature(&temperature);

    /* First we get the current temperature from the BMP085 */    
    // Print Temperature in Celsius 
    float temperature;
    bmp.getTemperature(&temperature);    
    display.print("TEMP: ");            // Printing temp in Celsius
    display.print(temperature);
    display.println(" C");
    //display.display();
 
   /* Now lets convert to Farenheit and Dispaly */
    // Print Temperature in Farenheit 
    // Serial.print("Temperature: ");
    // Serial.print(temperature);
    // Serial.println(" C");
    
    /* Display temperature in Farenheit */
    float farenheit;
    farenheit = (temperature * 1.8) +32;
    display.print("TEMP: ");
    display.print(farenheit);
    display.println(" F"); 
    //display.display();
    
    /* Then convert the atmospheric pressure, SLP and temp to altitude    */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    display.print("ALTD: "); 
    display.print(bmp.pressureToAltitude(seaLevelPressure,
                                        event.pressure,
                                        temperature)); 
    display.println(" m");
    display.display();    

    // display.clearDisplay(); 
  }
  else
  {
    display.println("Sensor error");
  }
  delay(1000);
}
