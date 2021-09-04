#include <Arduino_GFX_Library.h>

void renderGraph(int x, int y, int width, int height, Arduino_GFX *gfx, int dataCount, float data[])
{

  int startX = x - (width / 2);
  int startY = y - (height / 2);

  gfx->drawFastHLine(startX, startY + height, width, WHITE);
  gfx->drawFastVLine(startX, startY, height, WHITE);

  float step = (width * 1.0) / (dataCount - 1);
  int min = 400;
  int max = 10000;

  int divider = 400;
  float logMax = log10(max / divider);
  float logMin = log10(min / divider);
  // printf("\nstep: %f\n", step);
  // printf("\nlogMin: %f\n", logMin);
  // printf("\nlogMax: %f\n", logMax);

  for (int i = 1; i < dataCount; i++)
  {
    if (data[i] > min && data[i] < max)
    {
      int lastY = map(log10(data[i - 1] / divider) * 1000, logMin, logMax * 1000, startY + height, startY);
      int currentY = map(log10(data[i] / divider) * 1000, logMin, logMax * 1000, startY + height, startY);
      uint16_t colour = GREEN;
      if (data[i] > 2000)
      {
        colour = RED;
      }
      else if (data[i] > 1000)
      {
        colour = YELLOW;
      }

      gfx->drawLine(startX + ((i - 1) * step), lastY, startX + (i * step), currentY, colour);
    }
  }
}
