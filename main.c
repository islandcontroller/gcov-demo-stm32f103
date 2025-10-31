/*!****************************************************************************
 * @file
 * main.c
 *
 * @brief
 * gcov on STM32F103 demo project
 *
 * @date  29.10.2025
 ******************************************************************************/

/*- Header files -------------------------------------------------------------*/
#include "stm32f1xx.h"
#include "coverage.h"


/*!****************************************************************************
 * @brief
 * Main program entry point
 *
 * @date  29.10.2025
 ******************************************************************************/
int main(void)
{
  Coverage_vInit();

  HAL_Init();

  __HAL_RCC_GPIOC_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  HAL_GPIO_Init(GPIOC, &(GPIO_InitTypeDef){
    .Pin = GPIO_PIN_13,
    .Mode = GPIO_MODE_OUTPUT_OD,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_LOW
  });

  for (uint8_t i = 0; i < 6; ++i)
  {
    HAL_Delay(100);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  }

  Coverage_vDump("build/coverage.bin");
}

