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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"

#define SSD1306_I2C_ADDR 0x78
#define SHT30_ADDR 0x44 << 1 
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char M[36], HN[60], HG[60];
float temp,hum;
uint8_t SS=0, CDTGN=0, irqFlags, irq, len, length, TGN;
volatile uint8_t Co_ngat_nhanDL=0;
unsigned int AD[2], percent, percent1;
RTC_AlarmTypeDef sAlarm = {0};

static const uint16_t Font6x8[] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  // sp
0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, 0x2000, 0x0000,  // !
0x5000, 0x5000, 0x5000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  // "
0x5000, 0x5000, 0xf800, 0x5000, 0xf800, 0x5000, 0x5000, 0x0000,  // #
0x2000, 0x7800, 0xa000, 0x7000, 0x2800, 0xf000, 0x2000, 0x0000,  // $
0xc000, 0xc800, 0x1000, 0x2000, 0x4000, 0x9800, 0x1800, 0x0000,  // %
0x4000, 0xa000, 0xa000, 0x4000, 0xa800, 0x9000, 0x6800, 0x0000,  // &
0x3000, 0x3000, 0x2000, 0x4000, 0x0000, 0x0000, 0x0000, 0x0000,  // '
0x1000, 0x2000, 0x4000, 0x4000, 0x4000, 0x2000, 0x1000, 0x0000,  // (
0x4000, 0x2000, 0x1000, 0x1000, 0x1000, 0x2000, 0x4000, 0x0000,  // )
0x2000, 0xa800, 0x7000, 0xf800, 0x7000, 0xa800, 0x2000, 0x0000,  // *
0x0000, 0x2000, 0x2000, 0xf800, 0x2000, 0x2000, 0x0000, 0x0000,  // +
0x0000, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000, 0x2000, 0x0000,  // ,
0x0000, 0x0000, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000, 0x0000,  // -
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000, 0x0000,  // .
0x0000, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0000, 0x0000,  // /
0x7000, 0x8800, 0x9800, 0xa800, 0xc800, 0x8800, 0x7000, 0x0000,  // 0
0x2000, 0x6000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000,  // 1
0x7000, 0x8800, 0x0800, 0x7000, 0x8000, 0x8000, 0xf800, 0x0000,  // 2
0xf800, 0x0800, 0x1000, 0x3000, 0x0800, 0x8800, 0x7000, 0x0000,  // 3
0x1000, 0x3000, 0x5000, 0x9000, 0xf800, 0x1000, 0x1000, 0x0000,  // 4
0xf800, 0x8000, 0xf000, 0x0800, 0x0800, 0x8800, 0x7000, 0x0000,  // 5
0x3800, 0x4000, 0x8000, 0xf000, 0x8800, 0x8800, 0x7000, 0x0000,  // 6
0xf800, 0x0800, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0000,  // 7
0x7000, 0x8800, 0x8800, 0x7000, 0x8800, 0x8800, 0x7000, 0x0000,  // 8
0x7000, 0x8800, 0x8800, 0x7800, 0x0800, 0x1000, 0xe000, 0x0000,  // 9
0x0000, 0x0000, 0x2000, 0x0000, 0x2000, 0x0000, 0x0000, 0x0000,  // :
0x0000, 0x0000, 0x2000, 0x0000, 0x2000, 0x2000, 0x4000, 0x0000,  // ;
0x0800, 0x1000, 0x2000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0000,  // <
0x0000, 0x0000, 0xf800, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000,  // =
0x4000, 0x2000, 0x1000, 0x0800, 0x1000, 0x2000, 0x4000, 0x0000,  // >
0x7000, 0x8800, 0x0800, 0x3000, 0x2000, 0x0000, 0x2000, 0x0000,  // ?
0x7000, 0x8800, 0xa800, 0xb800, 0xb000, 0x8000, 0x7800, 0x0000,  // @
0x2000, 0x5000, 0x8800, 0x8800, 0xf800, 0x8800, 0x8800, 0x0000,  // A
0xf000, 0x8800, 0x8800, 0xf000, 0x8800, 0x8800, 0xf000, 0x0000,  // B
0x7000, 0x8800, 0x8000, 0x8000, 0x8000, 0x8800, 0x7000, 0x0000,  // C
0xf000, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0xf000, 0x0000,  // D
0xf800, 0x8000, 0x8000, 0xf000, 0x8000, 0x8000, 0xf800, 0x0000,  // E
0xf800, 0x8000, 0x8000, 0xf000, 0x8000, 0x8000, 0x8000, 0x0000,  // F
0x7800, 0x8800, 0x8000, 0x8000, 0x9800, 0x8800, 0x7800, 0x0000,  // G
0x8800, 0x8800, 0x8800, 0xf800, 0x8800, 0x8800, 0x8800, 0x0000,  // H
0x7000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000,  // I
0x3800, 0x1000, 0x1000, 0x1000, 0x1000, 0x9000, 0x6000, 0x0000,  // J
0x8800, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000, 0x8800, 0x0000,  // K
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xf800, 0x0000,  // L
0x8800, 0xd800, 0xa800, 0xa800, 0xa800, 0x8800, 0x8800, 0x0000,  // M
0x8800, 0x8800, 0xc800, 0xa800, 0x9800, 0x8800, 0x8800, 0x0000,  // N
0x7000, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, 0x0000,  // O
0xf000, 0x8800, 0x8800, 0xf000, 0x8000, 0x8000, 0x8000, 0x0000,  // P
0x7000, 0x8800, 0x8800, 0x8800, 0xa800, 0x9000, 0x6800, 0x0000,  // Q
0xf000, 0x8800, 0x8800, 0xf000, 0xa000, 0x9000, 0x8800, 0x0000,  // R
0x7000, 0x8800, 0x8000, 0x7000, 0x0800, 0x8800, 0x7000, 0x0000,  // S
0xf800, 0xa800, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000,  // T
0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, 0x0000,  // U
0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, 0x0000,  // V
0x8800, 0x8800, 0x8800, 0xa800, 0xa800, 0xa800, 0x5000, 0x0000,  // W
0x8800, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800, 0x8800, 0x0000,  // X
0x8800, 0x8800, 0x5000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000,  // Y
0xf800, 0x0800, 0x1000, 0x7000, 0x4000, 0x8000, 0xf800, 0x0000,  // Z
0x7800, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x7800, 0x0000,  // [
0x0000, 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0000, 0x0000,  /* \ */
0x7800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x7800, 0x0000,  // ]
0x2000, 0x5000, 0x8800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  // ^
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf800, 0x0000,  // _
0x6000, 0x6000, 0x2000, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000,  // `
0x0000, 0x0000, 0x6000, 0x1000, 0x7000, 0x9000, 0x7800, 0x0000,  // a
0x8000, 0x8000, 0xb000, 0xc800, 0x8800, 0xc800, 0xb000, 0x0000,  // b
0x0000, 0x0000, 0x7000, 0x8800, 0x8000, 0x8800, 0x7000, 0x0000,  // c
0x0800, 0x0800, 0x6800, 0x9800, 0x8800, 0x9800, 0x6800, 0x0000,  // d
0x0000, 0x0000, 0x7000, 0x8800, 0xf800, 0x8000, 0x7000, 0x0000,  // e
0x1000, 0x2800, 0x2000, 0x7000, 0x2000, 0x2000, 0x2000, 0x0000,  // f
0x0000, 0x0000, 0x7000, 0x9800, 0x9800, 0x6800, 0x0800, 0x0000,  // g
0x8000, 0x8000, 0xb000, 0xc800, 0x8800, 0x8800, 0x8800, 0x0000,  // h
0x2000, 0x0000, 0x6000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000,  // i
0x1000, 0x0000, 0x1000, 0x1000, 0x1000, 0x9000, 0x6000, 0x0000,  // j
0x8000, 0x8000, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000, 0x0000,  // k
0x6000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000,  // l
0x0000, 0x0000, 0xd000, 0xa800, 0xa800, 0xa800, 0xa800, 0x0000,  // m
0x0000, 0x0000, 0xb000, 0xc800, 0x8800, 0x8800, 0x8800, 0x0000,  // n
0x0000, 0x0000, 0x7000, 0x8800, 0x8800, 0x8800, 0x7000, 0x0000,  // o
0x0000, 0x0000, 0xb000, 0xc800, 0xc800, 0xb000, 0x8000, 0x0000,  // p
0x0000, 0x0000, 0x6800, 0x9800, 0x9800, 0x6800, 0x0800, 0x0000,  // q
0x0000, 0x0000, 0xb000, 0xc800, 0x8000, 0x8000, 0x8000, 0x0000,  // r
0x0000, 0x0000, 0x7800, 0x8000, 0x7000, 0x0800, 0xf000, 0x0000,  // s
0x2000, 0x2000, 0xf800, 0x2000, 0x2000, 0x2800, 0x1000, 0x0000,  // t
0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x9800, 0x6800, 0x0000,  // u
0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, 0x0000,  // v
0x0000, 0x0000, 0x8800, 0x8800, 0xa800, 0xa800, 0x5000, 0x0000,  // w
0x0000, 0x0000, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800, 0x0000,  // x
0x0000, 0x0000, 0x8800, 0x8800, 0x7800, 0x0800, 0x8800, 0x0000,  // y
0x0000, 0x0000, 0xf800, 0x1000, 0x2000, 0x4000, 0xf800, 0x0000,  // z
0x1000, 0x2000, 0x2000, 0x4000, 0x2000, 0x2000, 0x1000, 0x0000,  // {
0x2000, 0x2000, 0x2000, 0x0000, 0x2000, 0x2000, 0x2000, 0x0000,  // |
0x4000, 0x2000, 0x2000, 0x1000, 0x2000, 0x2000, 0x4000, 0x0000,  // }
0x4000, 0xa800, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,  // ~
};
void SSD1306_Ghilenh(uint8_t cmd) {
    uint8_t data[2];
    data[0] = 0x00;   // Co = 0, D/C = 0 
    data[1] = cmd;
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR, data, 2, 1000);
}
void SSD1306_Ghidulieu(uint8_t *data, uint16_t size) {
    uint8_t data_t[size+1];
    data_t[0] = 0x40; // Co = 0, D/C = 1
    memcpy(&data_t[1], data, size);
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDR,data_t, size+1, 1000);
}
void SSD1306_Init(void) {
    HAL_Delay(100); 

    SSD1306_Ghilenh(0xAE); // Display OFF
    SSD1306_Ghilenh(0x20); // Set Memory Addressing Mode
    SSD1306_Ghilenh(0x00); // Page addressing mode
    SSD1306_Ghilenh(0xB0); // Set Page Start Address
    SSD1306_Ghilenh(0xC8); // COM Output Scan Direction
    SSD1306_Ghilenh(0x00); // Low column address
    SSD1306_Ghilenh(0x10); // High column address
    SSD1306_Ghilenh(0x40); // Start line address
    SSD1306_Ghilenh(0x81); // Contrast control
    SSD1306_Ghilenh(0x7F); // Contrast value
    SSD1306_Ghilenh(0xA1); // Segment re-map
    SSD1306_Ghilenh(0xA6); // Normal display
    SSD1306_Ghilenh(0xA8); // Multiplex ratio
    SSD1306_Ghilenh(0x3F); // Duty = 1/64
    SSD1306_Ghilenh(0xA4); // Output follows RAM
    SSD1306_Ghilenh(0xD3); // Display offset
    SSD1306_Ghilenh(0x00); // No offset
    SSD1306_Ghilenh(0xD5); // Display clock divide ratio
    SSD1306_Ghilenh(0xF0); // Set divide ratio
    SSD1306_Ghilenh(0xD9); // Pre-charge period
    SSD1306_Ghilenh(0x22);
    SSD1306_Ghilenh(0xDA); // COM pins hardware config
    SSD1306_Ghilenh(0x12);
    SSD1306_Ghilenh(0xDB); // VCOMH deselect level
    SSD1306_Ghilenh(0x20);
    SSD1306_Ghilenh(0x8D); // Charge pump
    SSD1306_Ghilenh(0x14);
    SSD1306_Ghilenh(0xAF); // Display ON
		HAL_Delay(100);
}
void SSD1306_Xoamanhinh(void) {
    uint8_t clear[128];
    memset(clear, 0x00, 128);
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_Ghilenh(0xB0 + page); // chon page
        SSD1306_Ghilenh(0x00);        // cot thap
        SSD1306_Ghilenh(0x10);        // cot cao
        SSD1306_Ghidulieu(clear, 128);
    }
}
void SSD1306_Hienthikitu(uint8_t x, uint8_t y, char c) {
    uint8_t index = c - 32;
    uint8_t buf[6];

    for (int col = 0; col < 6; col++) {
        buf[col] = 0;
        for (int row = 0; row < 8; row++) {
            uint16_t val = Font6x8[index * 8 + row];
  
            if (val & (1 << (15 - col))) { //là cách đọc một pixel từ dữ liệu font bằng cách kiểm tra bit trong số 16-bit. Nó đảm bảo ta lấy đúng pixel theo cột hiện tại, từ bit cao xuống bit thấp.
                buf[col] |= (1 << row); //ghi pixel sáng vào đúng vị trí trong cột dữ liệu
            }
        }
    }
    SSD1306_Ghilenh(0xB0 + y); // chon page
    SSD1306_Ghilenh(((x & 0xF0) >> 4) | 0x10); // cột cao (4 bit cao)
    SSD1306_Ghilenh((x & 0x0F)); // cột thấp (4 bit thấp)
    SSD1306_Ghidulieu(buf, 6); // ghi 6 cột dữ liệu ký tự
}

void SSD1306_Ghi_chuoi(uint8_t x, uint8_t y, char *str) {
    while (*str) {
        SSD1306_Hienthikitu(x, y, *str);
        x += 6;
			 if (x + 6 > 128) 
				 {
					x = 0;
					y += 1; 
					if (y * 8 >= 64) 
						{
							break;
						}
				}
        str++;
    }
}
void SHT30_StartMeasurement() 
{
    uint8_t cmd[2] = {0x2C, 0x06}; // High repeatability, clock stretching disabled đo chính xác cao,đo không dùng clock stretching (master không bị giữ SCL), Clock stretching là cách để slave “kéo dài” xung clock nhằm trì hoãn giao tiếp khi chưa sẵn sàng
    HAL_I2C_Master_Transmit(&hi2c1, SHT30_ADDR, cmd, 2, 1000);
    HAL_Delay(20);
}

uint8_t SHT30_GetValue(uint8_t *data) {
    if (HAL_I2C_Master_Receive(&hi2c1, SHT30_ADDR, data, 6, 1000) == HAL_OK) {
        return 1; 
    }
    return 0; 
}

uint8_t SHT30_DDL(float *temperature, float *humidity) {
    uint8_t data[6];

    SHT30_StartMeasurement();

    if (!SHT30_GetValue(data)) return 0;

    uint16_t rawTemp = (data[0] << 8) | data[1];
    uint16_t rawHum  = (data[3] << 8) | data[4];

    *temperature = -45.0f + 175.0f * ((float)rawTemp / 65535.0f);
    *humidity    = 100.0f * ((float)rawHum / 65535.0f);

    return 1; 
}
void CBDAD(uint16_t v1) 
{
    const int16_t minVal = 450;
    const int16_t maxVal = 3300;

    int16_t v1_safe = v1;

    if (v1_safe < minVal) v1_safe = minVal;
    if (v1_safe > maxVal) v1_safe = maxVal;

    int32_t val = 100 - ((v1_safe - minVal) * 100 / (maxVal - minVal));

    if (val < 0) val = 0;
    if (val > 100) val = 100;

    percent = (uint8_t)val;
}

void CBAS(uint16_t v1) 
{
    const int16_t minVal = 0;
    const int16_t maxVal = 4095;

    int16_t v1_safe = v1;

    if (v1_safe < minVal) v1_safe = minVal;
    if (v1_safe > maxVal) v1_safe = maxVal;

    int32_t val = 100 - ((v1_safe - minVal) * 100 / (maxVal - minVal));

    if (val < 0) val = 0;
    if (val > 100) val = 100;

    percent1 = (uint8_t)val;
}
void LORA_WriteReg(uint8_t addr, uint8_t value) {
    uint8_t buf[2];
    buf[0] = addr | 0x80; // bit7=1 => ghi
    buf[1] = value;

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
    HAL_SPI_Transmit(&hspi1, buf, 2, 1000);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
}

uint8_t LORA_ReadReg(uint8_t addr) {
    uint8_t buf[2];
    uint8_t rx[2];
    buf[0] = addr & 0x7F; // bit7=0 => đọc

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
    HAL_SPI_TransmitReceive(&hspi1, buf, rx, 2, 1000);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);

    return rx[1]; // byte dữ liệu
}
void LORA_Init(void) {
		HAL_Delay(1000);
    // Đat che do Sleep
    LORA_WriteReg(0x01, 0x00);

    // Chuyen sang che do LoRa
    LORA_WriteReg(0x01, 0x80);
		HAL_Delay(10);
    // Cấu hình tần số 433 MHz
    LORA_WriteReg(0x06, 0x6C); // FrfMsb
    LORA_WriteReg(0x07, 0x80); // FrfMid
    LORA_WriteReg(0x08, 0x00); // FrfLsb

    // Cau hinh bang thong, coding rate, spreading factor
    LORA_WriteReg(0x1D, 0x78); // BW=125kHz, CR=4/8, Implicit Header OFF
    LORA_WriteReg(0x1E, 0xC4); // SF=12, CRC on
		LORA_WriteReg(0x26, 0x0C);	//LowDataRateOptimize=ON, AGC=ON 0x04
		//Dat lai dia chi mac dinh cho FIFO, base, tx, rx
		LORA_WriteReg(0x0D, 0x00);
		LORA_WriteReg(0x0E, 0x00);
		LORA_WriteReg(0x0F, 0x00);

		// Công suất phát 17dBm, gioi han dong phat 100mA
    LORA_WriteReg(0x09, 0x8F);
		LORA_WriteReg(0x0B, 0x2B);		
	
		LORA_WriteReg(0x39, 0x12);// Dong bo voi ben node1
		
		LORA_WriteReg(0x40, 0x00);//bat ngat RXdone
		LORA_WriteReg(0x12, 0xFF);// Xoa co TRQ FIFO

    // Chế độ RX continous
    LORA_WriteReg(0x01, 0x85);	
}

void LORA_Send(uint8_t *data, uint8_t length) {
	
		LORA_WriteReg(0x01, 0x81);// Che do standby
		LORA_WriteReg(0x12, (1 << 3));// Xoa co TX done
	
    // Đặt lai địa chỉ FIFO TX
    LORA_WriteReg(0x0E, 0x00);
    LORA_WriteReg(0x0D, 0x00);

    // Ghi dữ liệu vào thanh ghi FIFO
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
    uint8_t addr = 0x00 | 0x80;
    HAL_SPI_Transmit(&hspi1, &addr, 1, 1000);
    HAL_SPI_Transmit(&hspi1, data, length, 1000);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);

    // Đặt độ dài gói tin
    LORA_WriteReg(0x22, length);

    // Chuyển sang chế độ TX de truyen
    LORA_WriteReg(0x01, 0x83);

		irqFlags=LORA_ReadReg(0x12);
		while(!(LORA_ReadReg(0x12) & 0x08));// Cho truyen xong
		LORA_WriteReg(0x12, (1 << 3));// Xoa co TX done Bit 7: RxTimeout Bit 6: RxDone Bit 5: PayloadCrcError Bit 4: ValidHeader Bit 3: TxDone Bit 2: CadDone Bit 1: FhssChangeChannel Bit 0: CadDetected
		
		LORA_WriteReg(0x01, 0x85);//Chuyen lai RX continous

}
uint8_t LORA_Receive(uint8_t *buffer) 
{
		
		irq = LORA_ReadReg(0x12);
    if (!(irq & 0x40)) { // ktr bit 0x40 (RxDone)
        return 0;			
    }
		else
		{
    length = LORA_ReadReg(0x13); // Kiem tra so byte nhan duoc
    uint8_t fifoAddr = LORA_ReadReg(0x10); // dia chi thanh ghi fifo hien tai

    LORA_WriteReg(0x0D, fifoAddr);// Dat con tro fifo toi vi tri de nhan du lieu

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
	
    uint8_t addr = 0x00 & 0x7F;
    HAL_SPI_Transmit(&hspi1, &addr, 1, 1000);//Gui dia chi doc fifo
		
    HAL_SPI_Receive(&hspi1, buffer, length, 1000);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);

		LORA_WriteReg(0x12, (1 << 6));
		return length;
		}
}
void Set_Alarm(uint8_t sleepMinutes)
{
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    memset(&sAlarm, 0, sizeof(sAlarm));

    uint32_t totalMinutes = sTime.Hours * 60 + sTime.Minutes + sleepMinutes;
    
    sAlarm.AlarmTime.Hours   = (totalMinutes / 60) % 24;
    sAlarm.AlarmTime.Minutes = totalMinutes % 60;
    sAlarm.AlarmTime.Seconds = sTime.Seconds;

		sAlarm.Alarm = RTC_ALARM_A;

    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
}

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,0);
	SSD1306_Init();
	SSD1306_Xoamanhinh();
	LORA_Init();
	SSD1306_Ghi_chuoi(0,3,"Cho tin hieu khoi dong tu node 1");
	while(!SS)
	{	
		len = LORA_Receive(HN); 
    if (len > 0) 
    {
			HN[len] = '\0'; 
			if (strstr((char*)HN, "hello") != NULL)
			{
					sprintf(HG,"hi");
					LORA_Send((uint8_t*)HG,strlen(HG));
					SS = 1;
		}	
		}
	}
	SSD1306_Xoamanhinh();
	SSD1306_Ghi_chuoi(0,3,"Cho cai dat thoi gian ngu");
	while(!CDTGN)
	{   
    len = LORA_Receive(HN); 
    if (len > 0) 
    {
        HN[len] = '\0'; 
        int isNumber = 1;
        for (int i = 0; i < len; i++)
        {
            if (!isdigit(HN[i]))
            {
                isNumber = 0;
                break;
            }
        }
        if (isNumber)
        {           
            TGN= atoi((char*)HN);
            sprintf(HG, "Alarm set");
            LORA_Send((uint8_t*)HG, strlen(HG));
            CDTGN = 1; 
        }
    }
	}	
	SSD1306_Xoamanhinh();
	HAL_ADC_Start_DMA(&hadc1,AD,2);
  while (1)
  {
		CBAS(AD[0]);
		CBDAD(AD[1]);	
		SHT30_DDL(&temp, &hum);
		
		sprintf(M,"Cd anh sang: %d %%  ",percent1);
		SSD1306_Ghi_chuoi(0,0,M);
		HAL_Delay(10);
		sprintf(M,"Do am dat: %d %%  ",percent);
		SSD1306_Ghi_chuoi(0,1,M);
		HAL_Delay(10);
		sprintf(M,"Nhiet do: %.1f C  ",temp);
		SSD1306_Ghi_chuoi(0,2,M);
		HAL_Delay(10);
		sprintf(M,"Do am khong khi: %.1f %%  ",hum);
		SSD1306_Ghi_chuoi(0,3,M);
		
    if (Co_ngat_nhanDL)
		{ 
    len =LORA_Receive(HN); 
    
    if (len > 0) 
			{
					HN[len] = '\0';

					if (strstr((char*)HN, "ts") != NULL) 
					{
							sprintf(HG,"%d,%d,%.2f,%.2f",percent1,percent,temp,hum);
							LORA_Send((uint8_t*)HG, strlen(HG));
					}
					else if (strstr((char*)HN, "sleep") != NULL)
					{						
							LORA_WriteReg(0x12, 0xFF);
							LORA_WriteReg(0x01, 0x00); 
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,1);
							SSD1306_Ghilenh(0xAE); // Display OFF
							Set_Alarm(TGN);
							HAL_SuspendTick();
							HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);	

							HAL_ResumeTick(); 
							SystemClock_Config();
							SSD1306_Ghilenh(0xAF); // Display ON
							HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,0);
							HAL_Delay(1000);
							LORA_WriteReg(0x01, 0x85);	
					}
			}
		Co_ngat_nhanDL = 0;
		LORA_WriteReg(0x12, (1 << 6));
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
