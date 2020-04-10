#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
//#include <SimpleTimer.h>

#define REPORTING_PERIOD_MS 1000

char auth[] = "I-K_sBE5xKH-KDMC6PdrobZFA-qaqls1";             // You should get Auth Token in the Blynk App.
char ssid[] = "Achyut";                                     // Your WiFi credentials.
char pass[] = "12345678";

// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0
PulseOximeter pox;

float BPM, SpO2;
float vref = 3.3;
float resolution = vref / 1023.0;
float temperature;

uint32_t tsLastReport = 0;
SimpleTimer timer;

void onBeatDetected()
{
    Serial.println("Beat Detected!");
}

void setup()
{
    Serial.begin(115200);
    pinMode(19, OUTPUT);
    Blynk.begin(auth, ssid, pass);
    Serial.print("Initializing Pulse Oximeter..");
    
    if (!pox.begin())
    {
         Serial.println("FAILED");
         for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }

    // The default current for the IR LED is 50mA and it could be changed by uncommenting the following line.
        pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
timer.setInterval(2000, sendUptime);
}
void sendUptime()
{
  float temperature = analogRead(A0);//lm35 is connected to pin A0 on NodeMcu
  temperature = (temperature * resolution);
  temperature = temperature * 100.0;
  Serial.print("LM35 temperature: ");//serial print the value 
  Serial.println(temperature);
  Blynk.virtualWrite(V0, temperature);//send the value to blynk application
}

void loop()
{
    pox.update();
    Blynk.run();
    timer.run();

    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");

        Blynk.virtualWrite(V3, BPM);
        Blynk.virtualWrite(V4, SpO2);

        tsLastReport = millis();
    }
}
