#include <Adafruit_SCD30.h>
#include <Arduino_GFX_Library.h>
#include <math.h>
#include <Adafruit_SGP30.h>
#include "./homekit.h"
#include "./aqi.h"

Adafruit_SGP30 sgp;
Adafruit_SCD30 scd30;
Arduino_DataBus *bus = new Arduino_HWSPI(15 /* DC */, 4 /* CS */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, false /* IPS */);

// const int CO2ValueCount = 50;
const int CO2ValueCount = 240;
float CO2Values[CO2ValueCount];
int readingPosition = 0;

float lastCO2Reading = 400.0;
int lastTVOCReading = 0;
int lastIAQValue = 0;
float lastHumidityReading = 0;

bool loopSerialOutput = false;

#define CO2_WARNING_LEVEL 1250

void addNewCO2Reading(float reading)
{
  if (readingPosition < CO2ValueCount)
  {
    CO2Values[readingPosition] = reading;
    readingPosition++;
  }
  else
  {
    for (int i = 0; i < CO2ValueCount - 1; i++)
    {
      CO2Values[i] = CO2Values[i + 1];
    }
    CO2Values[CO2ValueCount - 1] = reading;
  }
}

uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
  return absoluteHumidityScaled;
}

void setup(void)
{
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  gfx->begin();
  gfx->fillScreen(BLACK);

#ifdef TFT_BL
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
#endif
  gfx->invertDisplay(true);
  Serial.println("Adafruit SCD30 Sensor adjustment test!");

  // Try to initialize!
  if (!scd30.begin())
  {
    gfx->setCursor(0, 120 - 6);
    gfx->setTextColor(RED);
    gfx->println("Failed to find SCD30 chip");
    Serial.println("Failed to find SCD30 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("SCD30 Found!");

  if (!scd30.setTemperatureOffset(400))
  {
    Serial.println("Failed to set temperature offset");
    while (1)
    {
      delay(10);
    }
  }

  if (!sgp.begin())
  {
    gfx->setCursor(0, 120 - 6);
    gfx->setTextColor(RED);
    gfx->println("Failed to find SGP30 chip");
    Serial.println("Failed to find SGP30 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("SGP30 Found!");

  setupHomeKit(&lastCO2Reading, &lastTVOCReading, &lastIAQValue, &lastHumidityReading);
}

void renderGraph(int x, int y, int width, int height)
{

  int startX = x - (width / 2);
  int startY = y - (height / 2);

  gfx->drawFastHLine(startX, startY + height, width, WHITE);
  gfx->drawFastVLine(startX, startY, height, WHITE);

  float step = (width * 1.0) / (CO2ValueCount - 1);
  int min = 400;
  int max = 10000;

  int divider = 400;
  float logMax = log10(max / divider);
  float logMin = log10(min / divider);
  // printf("\nstep: %f\n", step);
  // printf("\nlogMin: %f\n", logMin);
  // printf("\nlogMax: %f\n", logMax);

  for (int i = 1; i < CO2ValueCount; i++)
  {
    if (CO2Values[i] > min && CO2Values[i] < max)
    {
      int lastY = map(log10(CO2Values[i - 1] / divider) * 1000, logMin, logMax * 1000, startY + height, startY);
      int currentY = map(log10(CO2Values[i] / divider) * 1000, logMin, logMax * 1000, startY + height, startY);
      uint16_t colour = GREEN;
      if (CO2Values[i] > 2000)
      {
        colour = RED;
      }
      else if (CO2Values[i] > 1000)
      {
        colour = YELLOW;
      }

      gfx->drawLine(startX + ((i - 1) * step), lastY, startX + (i * step), currentY, colour);
    }
  }
}

void loop()
{
  loopHomeKit();

  if (scd30.dataReady())
  {

    if (!scd30.read())
    {
      Serial.println("SCD30: Error reading sensor data");
      return;
    }

    sgp.setHumidity(getAbsoluteHumidity(scd30.temperature, scd30.relative_humidity));

    if (!sgp.IAQmeasure())
    {
      Serial.println("SGP30: Measurement failed");
      return;
    }

    if (loopSerialOutput)
    {

      Serial.print("TVOC ");
      Serial.print(sgp.TVOC);
      Serial.print(" ppb\t");
      Serial.print("eCO2 ");
      Serial.print(sgp.eCO2);
      Serial.println(" ppm");

      if (!sgp.IAQmeasureRaw())
      {
        Serial.println("Raw Measurement failed");
        return;
      }
      Serial.print("Raw H2 ");
      Serial.print(sgp.rawH2);
      Serial.print(" \t");
      Serial.print("Raw Ethanol ");
      Serial.print(sgp.rawEthanol);
      Serial.println("");

      Serial.print("Temperature: ");
      Serial.print(scd30.temperature);
      Serial.println(" degrees C");

      Serial.print("Relative Humidity: ");
      Serial.print(scd30.relative_humidity);
      Serial.println(" %");

      Serial.print("CO2: ");
      Serial.print(scd30.CO2, 3);
      Serial.println(" ppm");
      Serial.println("");

      Serial.print("Temperature offset: ");
      Serial.print((float)scd30.getTemperatureOffset() / 100.0);
      Serial.println(" degrees C");

      printf("\nIAQ: %d \n ", calculateAQI(scd30.CO2, sgp.TVOC, scd30.relative_humidity, scd30.temperature));
    }

    lastCO2Reading = scd30.CO2;
    lastTVOCReading = sgp.TVOC;
    lastHumidityReading = scd30.relative_humidity;
    lastIAQValue = calculateAQI(scd30.CO2, sgp.TVOC, scd30.relative_humidity, scd30.temperature);

    String co2Text = String(String(lastCO2Reading, 0) + " ppm");
    String TVOCText = String(String(lastTVOCReading) + " ppb");

    addNewCO2Reading(lastCO2Reading);
    gfx->setTextSize(3);
    gfx->setTextColor(WHITE);
    gfx->fillScreen(BLACK);

    gfx->setCursor((120 - ((4 * 18) / 2)), 30);
    gfx->println("TVOC");
    gfx->setCursor((120 - ((TVOCText.length() * 18) / 2)), 30 + 28);

    if (lastTVOCReading > 500)
    {
      gfx->setTextColor(RED);
    }
    else if (lastTVOCReading > 250)
    {
      gfx->setTextColor(YELLOW);
    }

    gfx->println(TVOCText);

    gfx->setTextColor(WHITE);

    gfx->setCursor((120 - ((3 * 18) / 2)), (120 - 14));
    gfx->println("CO2");
    if (lastCO2Reading > 2000)
    {
      gfx->setTextColor(RED);
    }
    else if (lastCO2Reading > 1000)
    {
      gfx->setTextColor(YELLOW);
    }

    gfx->setCursor((120 - ((co2Text.length() * 18) / 2)), (120 + 14));
    gfx->println(co2Text);

    //graph

    renderGraph(120, 190, 120, 50);
  }

  delay(100);
}
