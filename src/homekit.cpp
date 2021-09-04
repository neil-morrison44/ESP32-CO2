#include <HomeSpan.h>

struct CO2Sensor : Service::CarbonDioxideSensor
{
  SpanCharacteristic *co2;
  SpanCharacteristic *co2Detected;
  float *lastReadingPointer;

  CO2Sensor(float *lastReading) : Service::CarbonDioxideSensor()
  {

    co2 = new Characteristic::CarbonDioxideLevel(400);
    co2->setRange(200, 10000);
    lastReadingPointer = lastReading;

    co2Detected = new Characteristic::CarbonDioxideDetected(false);
  }

  void loop()
  {
    if (co2->timeVal() > 5000)
    {
      co2->setVal(*lastReadingPointer);
      co2Detected->setVal((*lastReadingPointer > 1250));

      // printf("\n setting value to %f\n", *lastReadingPointer);
      // printf("\n setting detected to %d\n", (*lastReadingPointer > 1250));
    }
  }
};

struct AirQualitySensor : Service::AirQualitySensor
{
  SpanCharacteristic *tvoc;
  SpanCharacteristic *aqi;
  int *lastTVOCPointer;
  int *lastAQIValuePointer;

  AirQualitySensor(int *lastTVOCReading, int *lastAQIValue) : Service::AirQualitySensor()
  {
    tvoc = new Characteristic::VOCDensity(0);
    tvoc->setRange(0, 1000);
    lastTVOCPointer = lastTVOCReading;
    lastAQIValuePointer = lastAQIValue;
    aqi = new Characteristic::AirQuality();
  }

  void loop()
  {
    if (tvoc->timeVal() > 5000)
    {
      // ppb to mg/m^3
      double density = *lastTVOCPointer * 0.0045;
      tvoc->setVal(density);
      aqi->setVal(*lastAQIValuePointer);

      // printf("\n setting tvoc to %f\n", *lastTVOCPointer);
      // printf("\n setting aqi to %d\n", *lastAQIValuePointer);
    }
  }
};

struct HumditiySensor : Service::HumiditySensor
{
  SpanCharacteristic *relativeHumidity;
  float *lastRelativeHumidityPointer;

  HumditiySensor(float *lastHumidityReading) : Service::HumiditySensor()
  {
    relativeHumidity = new Characteristic::CurrentRelativeHumidity(50);
    lastRelativeHumidityPointer = lastHumidityReading;
  }

  void loop()
  {
    if (relativeHumidity->timeVal() > 5000)
    {
      relativeHumidity->setVal(*lastRelativeHumidityPointer);
    }
  }
};

void setupHomeKit(float *lastCO2Reading, int *lastTVOCReading, int *lastAQIValue, float *lastHumdityReading)
{
  homeSpan.begin(Category::Sensors, "ESP32-CO2");

  new SpanAccessory();

  new Service::AccessoryInformation();
  new Characteristic::Name("Air Sensor");

  new Characteristic::Manufacturer("HomeSpan");
  new Characteristic::SerialNumber("123-ABC");
  new Characteristic::Model("ESP32-SCD-30-SGP-30");
  new Characteristic::FirmwareRevision("1.0");

  new Characteristic::Identify();

  new Service::HAPProtocolInformation();
  new Characteristic::Version("1.1.0");

  (new CO2Sensor(lastCO2Reading))->setPrimary();
  new AirQualitySensor(lastTVOCReading, lastAQIValue);
  new HumditiySensor(lastHumdityReading);
}
void loopHomeKit()
{
  homeSpan.poll();
}
