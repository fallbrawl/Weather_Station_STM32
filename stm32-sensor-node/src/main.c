/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct __attribute__((packed)) {
    float temp;
    float hum;
    uint32_t ts;
} WeatherData;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint16_t dig_T1;
int16_t  dig_T2, dig_T3;
int32_t  t_fine;
uint8_t  dig_H1, dig_H3;
int16_t  dig_H2, dig_H4, dig_H5;
int8_t   dig_H6;
uint8_t dummy_rx[12];
WeatherData currentWeatherData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
float compensate_temperature(int32_t t_fine);
float compensate_humidity(uint8_t* raw_data, int32_t t_fine);
int32_t get_t_fine(uint8_t* raw_data);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
    uint8_t config_data[1];

    // Register 0xF4: Control Measurement
    // 0x27 means: 
    // [Temperature Oversampling x1] + [Pressure Oversampling x1] + [Normal Mode]
    config_data[0] = 0x27; 

    // We use Mem_Write to send the setting to the chip
    if (HAL_I2C_Mem_Write(&hi2c1, (0x76 << 1), 0xF4, I2C_MEMADD_SIZE_8BIT, config_data, 1, 100) == HAL_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Sensor Woken Up!\r\n", 18, 100);
    }
    uint8_t calib[6];
    if (HAL_I2C_Mem_Read(&hi2c1, (0x76 << 1), 0x88, I2C_MEMADD_SIZE_8BIT, calib, 6, 100) == HAL_OK) {
        // Combine the pairs of 8-bit registers into 16-bit values
        dig_T1 = (uint16_t)(calib[1] << 8 | calib[0]);
        dig_T2 = (int16_t)(calib[3] << 8 | calib[2]);
        dig_T3 = (int16_t)(calib[5] << 8 | calib[4]);
    }

    uint8_t h_cal[1];
    HAL_I2C_Mem_Read(&hi2c1, (0x76 << 1), 0xA1, 1, &dig_H1, 1, 100);
    uint8_t h_cal2[7];
    HAL_I2C_Mem_Read(&hi2c1, (0x76 << 1), 0xE1, 1, h_cal2, 7, 100);

    dig_H2 = (int16_t)(h_cal2[1] << 8 | h_cal2[0]);
    dig_H3 = (uint8_t)h_cal2[2];
    dig_H4 = (int16_t)(h_cal2[3] << 4 | (h_cal2[4] & 0x0F));
    dig_H5 = (int16_t)(h_cal2[5] << 4 | (h_cal2[4] >> 4));
    dig_H6 = (int8_t)h_cal2[6];

    HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t*)&currentWeatherData, dummy_rx, 12);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

while (1) {
    uint8_t raw_data[8];

    if (HAL_I2C_Mem_Read(&hi2c1, (0x76 << 1), 0xF7, 1, raw_data, 8, 100) == HAL_OK) {
        
        t_fine = get_t_fine(raw_data);
        float final_temp = compensate_temperature(t_fine);
        float final_hum  = compensate_humidity(raw_data, t_fine);

        send_telemetry(final_temp, final_hum);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); 
    }
}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  // Initializes the RCC Oscillators 
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void send_telemetry(float final_temp, float final_hum) {
    currentWeatherData.temp = final_temp;
    currentWeatherData.hum = final_hum;
    currentWeatherData.ts = HAL_GetTick();

}

int32_t get_t_fine(uint8_t* raw_data) {
    int32_t adc_T = (int32_t)(((uint32_t)raw_data[3] << 12) | ((uint32_t)raw_data[4] << 4) | ((uint32_t)raw_data[5] >> 4));
    int32_t v1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t v2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    return v1 + v2;
}

float compensate_temperature(int32_t t_fine) {
    return ((t_fine * 5 + 128) >> 8) / 100.0f;
}

float compensate_humidity(uint8_t* raw_data, int32_t t_fine) {
    int32_t adc_H = (int32_t)(((uint32_t)raw_data[6] << 8) | (uint32_t)raw_data[7]);
    int32_t h = (t_fine - ((int32_t)76800));
    h = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * h)) +
          ((int32_t)16384)) >> 15) * (((((((h * ((int32_t)dig_H6)) >> 10) *
          (((h * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
          ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    h = (h - (((((h >> 15) * (h >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    h = (h < 0 ? 0 : h);
    h = (h > 419430400 ? 419430400 : h);
    return (float)(h >> 12) / 1024.0f;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        HAL_SPI_TransmitReceive_IT(&hspi1, (uint8_t*)&currentWeatherData, dummy_rx, 12);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
