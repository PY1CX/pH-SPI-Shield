/*
 *  This is a test file for the pH-SPI-Shield
 *  You must use the following library: https://github.com/jrleeman/ADS1220_Arduino 
 *  There is some uncommented pointer arithmetic in the following lines that must be   
 *  changed with care.
 *  
 *  Attention:
 *  There isn't any test files for the K-Type reading, I only implemented them on the 
 *  Cortex-M3 version of the YAIoT which isn't open-sourced at the moment.
 */

#include "ADS1220.h"

ADS1220 adc;

long adc_reading;

#define PGA 1                 // Programmable Gain = 1
#define VREF 2.048            // External Reference 2.048V
#define VFSR VREF/PGA             
#define FSR (((long int)1<<23)-1)  

#define difganho 1

void setup() {
  Serial.begin(115200);

  adc.begin(8,7);
  adc.setGain(1);

  adc.setVoltageRef(0x01); // Vref REFP0 e 1
  adc.setFIR(0x01); // Reject 60hz
  adc.setDataRate(0x00);
  adc.setOpMode(0x00);
  
  adc.setConversionMode(0x01); //Single Conversion mode
  adc.setPGAbypass(0x00);

  adc.setMultiplexer(0x00);
}

/* 
 * Notes from LMP91200 Datasheet:
 * LMP91200
 * pH = 7 + (VOUT - VOCM)/alpha
 * alpha = -59.16mV/pH @ 25°C
 */

void loop() {
    //Get pH data
    adc.setMultiplexer(0x00);
    adc.setGain(1);
    delay(10);
    byte * point;
    point = adc.readADC_SingleArray();


    #ifdef DEBUG
    for( int x = 0 ; x < 3; x++ )
    {
      Serial.println(*(point + x));
    }
    #endif
    
    Serial.println("------------------");

    long adcVal = * point;
    adcVal = (adcVal << 8) | *(point + 1);
    adcVal = (adcVal << 8) | *(point + 2);
#ifdef DEBUG  
    adcVal = ( adcVal << 8 );
    adcVal = ( adcVal >> 8 );
#endif    
    float Vout = (float)((adcVal*VFSR*1000)/FSR);     //In  mV
#ifdef DEBUG
    Serial.println(Vout);
#endif    
    float pH   = 7 + (Vout / -59.16);
    Serial.println(pH);


    adc.setMultiplexer(0x07);
    adc.setGain(1);
    delay(150);
    point = adc.readADC_SingleArray();
    for( int x = 0 ; x < 3; x++ )
    {
      Serial.print(*(point + x),HEX); Serial.print(" ");
    }
    Serial.println("");
    adcVal = * point;
    adcVal = (adcVal << 8) | *(point + 1);
    adcVal = (adcVal << 8) | *(point + 2);
    Serial.println(adcVal);
    float Vout2 = (float)((adcVal*(2.048/1)*1000)/FSR);     //In  mV
    Serial.println(Vout2,4);
    delay(500);  
}

