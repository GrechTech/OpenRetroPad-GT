#include "Arduino.h"

#ifndef DEBUG
#define GAMEPAD_COUNT 1
#endif

#include "pins.h"

#include "SnesNes.cpp"
#include "N64Esp32.cpp"    
#include "SegaGenesis.cpp"
#include "Playstation.cpp"
//#include "SegaSaturn.cpp"
#include "WiiExtension.cpp" 

#if defined(ARDUINO_ARCH_ESP32)

const float Vin = 3.3;
const float steps = 4095.0;

#else

const float Vin = 5;
const float steps = 1024.0;

#endif

const float R1 = 1000; // 1K known resistor
const float tolerance = 0.22; // Max resistor tolerance of 20% with safety margin

void setup_sat(){} // Missing
void loop_sat(){} // Missing
void setup_sdc(){} // Missing
void loop_sdc(){} // Missing
void setup_ngc(){} // Missing
void loop_ngc(){} // Missing

void (*setups[])(void) = {setup_psx, setup_n64, setup_nes, setup_gen, setup_ngc, setup_sdc, setup_sat, setup_wii}; 
void (*loops[])(void) = {loop_psx, loop_n64, loop_nes, loop_gen, loop_ngc, loop_sdc, loop_sat, loop_wii}; 

void (*setup_run)(void); 
void (*loop_run)(void); 

enum SYSTEM_VALUE 
{
    S_UNDEFINED = -1,
    S_PSX,
    S_N64,
    S_NES,
    S_GEN,
    S_NGC,
    S_SDC,
    S_SAT,
    S_WII,
};

const uint16_t resistorValues[] =  
{
    100,  // R_PSX
    220,  // R_N64
    330,  // R_NES
    470,  // R_GEN
    680,  // R_NGC
    820,  // R_SDC
    1000, // R_SAT
    1500, // R_WII
};

SYSTEM_VALUE System = S_UNDEFINED;

inline SYSTEM_VALUE find_system(float R)
{
    for(int system = S_PSX; system < S_WII; system++)
    {
        if( (R < resistorValues[system] * (1.0f + tolerance) ) && (R > resistorValues[system] * (1.0f - tolerance) ) )
            return (SYSTEM_VALUE)system;
    }
    return S_UNDEFINED;
}

void setup() 
{
#ifdef DEBUG
	Serial.begin(115200);
    Serial.println("Begin universal setup");
#endif
    uint16_t raw = 0;
    float buffer, Vout, R2;

    while (System == S_UNDEFINED)
    {
        delay(100);
        raw = analogRead(OR_PIN_9);
        buffer = raw * Vin;
        Vout = (buffer) / steps;
        buffer = (Vin / Vout) - 1;
        R2 = R1 / buffer;
        System = find_system(R2);

#ifdef DEBUG
        Serial.print("raw: ");
        Serial.print(raw);
        Serial.print(" Vout: ");
        Serial.print(Vout);
        Serial.print(" R2: ");
        Serial.println(R2);

        if (System == S_UNDEFINED)
        {    
            delay(3000);
            Serial.println("Searching...");
        }
#endif
    }

    setup_run = setups[System];
    loop_run = loops[System];
#ifdef DEBUG
    Serial.print("Running setup for system: ");
    Serial.println(System);
#endif
    setup_run();
#ifdef DEBUG
    Serial.println("Setup done, run loop");
#endif
}

void loop() 
{
    loop_run();
}
