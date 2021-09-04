#include <Arduino.h>
#include <algorithm>
#include <iterator>

int getCO2Index(float co2)
{
  int co2Index = 1;

  if (co2 > 2000)
  {
    co2Index = 5;
  }
  else if (co2 > 1500)
  {
    co2Index = 4;
  }
  else if (co2 > 1000)
  {
    co2Index = 3;
  }
  else if (co2 > 400)
  {
    co2Index = 2;
  }

  return co2Index;
}

int getTempHumdityIndex(double temp, double humidity)
{
  // int temps[13] = {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28};
  // int humidity[9] = {90, 80, 70, 60, 50, 40, 30, 20, 10};

  int indexes[] = {6, 5, 4, 3, 3, 3, 3, 4, 5, 5, 6, 6, 6,
                   5, 4, 2, 2, 2, 2, 2, 3, 4, 5, 5, 5, 6,
                   5, 4, 3, 2, 1, 1, 1, 2, 3, 4, 5, 5, 6,
                   5, 4, 3, 2, 1, 1, 1, 2, 2, 3, 4, 5, 6,
                   5, 5, 4, 3, 2, 1, 1, 1, 2, 3, 4, 5, 6,
                   6, 5, 5, 4, 3, 3, 3, 2, 2, 3, 4, 5, 6,
                   6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6,
                   6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,
                   6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};

  int tempIndex = map(constrain(temp, 16, 28), 16, 28, 0, 12);
  int humdityIndex = map(constrain(humidity, 10, 90), 90, 10, 0, 8);

  // printf("\nindex t: %d, h: %d\n", tempIndex, humdityIndex);
  // printf("\ntemp + humid IAQ %d \n", indexes[humdityIndex * 13 + tempIndex]);
  return min(5, indexes[humdityIndex * 13 + tempIndex]);
}

int getVOCIndex(int tVOC)
{
  if (tVOC < 51)
  {
    return 1;
  }
  else if (tVOC < 101)
  {
    return 2;
  }
  else if (tVOC < 151)
  {
    return 3;
  }
  else if (tVOC < 201)
  {
    return 4;
  }
  else
  {
    return 5;
  }
}

int calculateAQI(float co2, int tVOC, float relativeHumidity, float temperature)
{
  // https://www.breeze-technologies.de/blog/calculating-an-actionable-indoor-air-quality-index/
  // but adapter for the HomeKit ENUMS
  // https://developer.apple.com/documentation/homekit/hmcharacteristicvalueairquality
  // (6 and 5) both become "poor"

  int co2Index = getCO2Index(co2);
  int humdityIndex = getTempHumdityIndex(temperature, relativeHumidity);
  int TVOCIndex = getVOCIndex(tVOC);
  return max(co2Index, max(humdityIndex, TVOCIndex));
}
