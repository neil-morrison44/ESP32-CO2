#include <Adafruit_SCD30.h>
#include <Arduino_GFX_Library.h>
#include <math.h>

Adafruit_SCD30 scd30;
Arduino_DataBus *bus = new Arduino_HWSPI(15 /* DC */, 4 /* CS */);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, TFT_RST, 0 /* rotation */, false /* IPS */);

// const int CO2ValueCount = 50;
const int CO2ValueCount = 240;
float CO2Values[CO2ValueCount];
int readingPosition = 0;

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

  /***
   * The code below will report the current settings for each of the
   * settings that can be changed. To see how they work, uncomment the setting
   * code above a status message and adjust the value
   *
   * **Note:** Since Automatic self calibration and forcing recalibration with
   * a reference value overwrite each other, you should only set one or the other
  ***/

  /*** Adjust the rate at which measurements are taken, from 2-1800 seconds */
  // if (!scd30.setMeasurementInterval(5)) {
  //   Serial.println("Failed to set measurement interval");
  //   while(1){ delay(10);}
  // }
  Serial.print("Measurement interval: ");
  Serial.print(scd30.getMeasurementInterval());
  Serial.println(" seconds");

  /*** Restart continuous measurement with a pressure offset from 700 to 1400 millibar.
   * Giving no argument or setting the offset to 0 will disable offset correction
   */
  // if (!scd30.startContinuousMeasurement(15)){
  //   Serial.println("Failed to set ambient pressure offset");
  //   while(1){ delay(10);}
  // }
  Serial.print("Ambient pressure offset: ");
  Serial.print(scd30.getAmbientPressureOffset());
  Serial.println(" mBar");

  /*** Set an altitude offset in meters above sea level.
   * Offset value stored in non-volatile memory of SCD30.
   * Setting an altitude offset will override any pressure offset.
   */
  // if (!scd30.setAltitudeOffset(110)){
  //   Serial.println("Failed to set altitude offset");
  //   while(1){ delay(10);}
  // }
  Serial.print("Altitude offset: ");
  Serial.print(scd30.getAltitudeOffset());
  Serial.println(" meters");

  /*** Set a temperature offset in hundredths of a degree celcius.
   * Offset value stored in non-volatile memory of SCD30.
   */
  // if (!scd30.setTemperatureOffset(1984)){ // 19.84 degrees celcius
  //   Serial.println("Failed to set temperature offset");
  //   while(1){ delay(10);}
  // }
  Serial.print("Temperature offset: ");
  Serial.print((float)scd30.getTemperatureOffset() / 100.0);
  Serial.println(" degrees C");

  /*** Force the sensor to recalibrate with the given reference value
   * from 400-2000 ppm. Writing a recalibration reference will overwrite
   * any previous self calibration values.
   * Reference value stored in non-volatile memory of SCD30.
   */
  // if (!scd30.forceRecalibrationWithReference(400)){
  //   Serial.println("Failed to force recalibration with reference");
  //   while(1) { delay(10); }
  // }
  Serial.print("Forced Recalibration reference: ");
  Serial.print(scd30.getForcedCalibrationReference());
  Serial.println(" ppm");

  /*** Enable or disable automatic self calibration (ASC).
   * Parameter stored in non-volatile memory of SCD30.
   * Enabling self calibration will override any previously set
   * forced calibration value.
   * ASC needs continuous operation with at least 1 hour
   * 400ppm CO2 concentration daily.
   */
  // if (!scd30.selfCalibrationEnabled(true)){
  //   Serial.println("Failed to enable or disable self calibration");
  //   while(1) { delay(10); }
  // }
  if (scd30.selfCalibrationEnabled())
  {
    Serial.print("Self calibration enabled");
  }
  else
  {
    Serial.print("Self calibration disabled");
  }

  Serial.println("\n\n");
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
  if (scd30.dataReady())
  {

    if (!scd30.read())
    {
      Serial.println("Error reading sensor data");
      return;
    }

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

    int characterCount = (scd30.CO2 > 999 ? 4 : 3) + 4;
    gfx->setTextSize(3);
    gfx->setTextColor(WHITE);
    gfx->fillScreen(BLACK);
    gfx->setCursor((120 - ((3 * 18) / 2)), (120 - 14));
    gfx->println("CO2");
    if (scd30.CO2 > 2000)
    {
      gfx->setTextColor(RED);
    }
    else if (scd30.CO2 > 1000)
    {
      gfx->setTextColor(YELLOW);
    }

    gfx->setCursor((120 - ((characterCount * 18) / 2)), (120 + 14));
    gfx->println(String(String(scd30.CO2, 0) + " ppm"));

    addNewCO2Reading(scd30.CO2);
    //graph

    renderGraph(120, 190, 120, 50);
  }

  delay(100);
}
