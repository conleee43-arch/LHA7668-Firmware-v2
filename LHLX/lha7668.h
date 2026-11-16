/**
 * \copyright Copyright (C) 2020 Legendsemi Technology Co., Ltd - http://www.legendsemi.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Legendsemi Technology Co., Ltd nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef LHA7668_H
#define LHA7668_H

#include "main.h"
#include "spi.h"

/*============================================================================
 * Register addresses
 *============================================================================*/
#define REG_STATUS       0x00
#define REG_ADC_CTRL     0x01
#define REG_DATA         0x02
#define REG_IO_CTRL1     0x03
#define REG_IO_CTRL2     0x04
#define REG_ID           0x05
#define REG_ERROR        0x06
#define REG_ERROR_EN     0x07
#define REG_MCLK_COUNT   0x08
#define REG_CH0          0x09
#define REG_CH1          0x0A
#define REG_CH2          0x0B
#define REG_CH3          0x0C
#define REG_CH4          0x0D
#define REG_CH5          0x0E
#define REG_CH6          0x0F
#define REG_CH7          0x10
#define REG_CH8          0x11
#define REG_CH9          0x12
#define REG_CH10         0x13
#define REG_CH11         0x14
#define REG_CH12         0x15
#define REG_CH13         0x16
#define REG_CH14         0x17
#define REG_CH15         0x18
#define REG_CONFIG0      0x19
#define REG_CONFIG1      0x1A
#define REG_CONFIG2      0x1B
#define REG_CONFIG3      0x1C
#define REG_CONFIG4      0x1D
#define REG_CONFIG5      0x1E
#define REG_CONFIG6      0x1F
#define REG_CONFIG7      0x20
#define REG_FILTER0      0x21
#define REG_FILTER1      0x22
#define REG_FILTER2      0x23
#define REG_FILTER3      0x24
#define REG_FILTER4      0x25
#define REG_FILTER5      0x26
#define REG_FILTER6      0x27
#define REG_FILTER7      0x28
#define REG_OFFSET0      0x29
#define REG_OFFSET1      0x2A
#define REG_OFFSET2      0x2B
#define REG_OFFSET3      0x2C
#define REG_OFFSET4      0x2D
#define REG_OFFSET5      0x2E
#define REG_OFFSET6      0x2F
#define REG_OFFSET7      0x30
#define REG_GAIN0        0x31
#define REG_GAIN1        0x32
#define REG_GAIN2        0x33
#define REG_GAIN3        0x34
#define REG_GAIN4        0x35
#define REG_GAIN5        0x36
#define REG_GAIN6        0x37
#define REG_GAIN7        0x38

/* NOTE: on LHA7668B all gains are factory calibrated, so GAIN0..GAIN7 hold the
 * production trim values. Do not overwrite them with an internal full-scale
 * (system gain) calibration unless your external reference is more accurate. */

/*============================================================================
 * STATUS Register bits (Address 0x00) [Reset = 0x00]
 *============================================================================*/
#define STATUS_RDY        (1 << 7)
#define STATUS_ERR_FLAG   (1 << 6)
#define STATUS_POR_FLAG   (1 << 4)
#define STATUS_CH_MASK    0x0F

/*============================================================================
 * ADC_CONTROL Register bits (Address 0x01) [Reset = 0x0000]
 *============================================================================*/
#define CTRL_DOUT_RDY_DEL  (1 << 12)
#define CTRL_CONT_READ     (1 << 11)
#define CTRL_DATA_STATUS   (1 << 10)
#define CTRL_CS_EN         (1 << 9)
#define CTRL_REF_EN        (1 << 8)
#define CTRL_PWR_MODE_MASK (3 << 6)
#define CTRL_PWR_LOW       (0 << 6)
#define CTRL_PWR_MID       (1 << 6)
#define CTRL_PWR_HIGH      (2 << 6)
#define CTRL_MODE_MASK     (0xF << 2)
#define CTRL_MODE_CONT     (0 << 2)
#define CTRL_MODE_SINGLE   (1 << 2)
#define CTRL_MODE_STANDBY  (2 << 2)
#define CTRL_MODE_PDOWN    (3 << 2)
#define CTRL_MODE_IDLE     (4 << 2)
#define CTRL_MODE_OFFCAL   (5 << 2)
#define CTRL_MODE_GAINCAL  (6 << 2)
#define CTRL_MODE_SYSOFF   (7 << 2)
#define CTRL_MODE_SYSGAIN  (8 << 2)
#define CTRL_CLK_SEL_MASK  (3 << 0)
#define CTRL_CLK_INTERNAL  (0 << 0)
#define CTRL_CLK_OUT       (1 << 0)
#define CTRL_CLK_EXT_1M    (2 << 0)
#define CTRL_CLK_EXT_2M    (3 << 0)

/*============================================================================
 * IO_CONTROL_1 Register bits (Address 0x03) [Reset = 0x000000]
 *============================================================================*/
#define IO1_GPIO_DAT4     (1 << 23)
#define IO1_GPIO_DAT3     (1 << 22)
#define IO1_GPIO_DAT2     (1 << 21)
#define IO1_GPIO_DAT1     (1 << 20)
#define IO1_GPIO_CTRL4    (1 << 19)
#define IO1_GPIO_CTRL3    (1 << 18)
#define IO1_GPIO_CTRL2    (1 << 17)
#define IO1_GPIO_CTRL1    (1 << 16)
#define IO1_PDSW          (1 << 15)
#define IO1_IOUT1_MASK    (7 << 11)
#define IO1_IOUT1_OFF     (0 << 11)
#define IO1_IOUT1_50U     (1 << 11)
#define IO1_IOUT1_100U    (2 << 11)
#define IO1_IOUT1_250U    (3 << 11)
#define IO1_IOUT1_500U    (4 << 11)
#define IO1_IOUT1_750U    (5 << 11)
#define IO1_IOUT1_1000U   (6 << 11)
#define IO1_IOUT0_MASK    (7 << 8)
#define IO1_IOUT0_OFF     (0 << 8)
#define IO1_IOUT0_50U     (1 << 8)
#define IO1_IOUT0_100U    (2 << 8)
#define IO1_IOUT0_250U    (3 << 8)
#define IO1_IOUT0_500U    (4 << 8)
#define IO1_IOUT0_750U    (5 << 8)
#define IO1_IOUT0_1000U   (6 << 8)
#define IO1_IOUT1_CH_MASK (0xF << 4)
#define IO1_IOUT0_CH_MASK 0x0F

/* IOUT1/IOUT0 channel selection */
#define IOUT_CH_AIN0      0x0
#define IOUT_CH_AIN1      0x1
#define IOUT_CH_AIN2      0x2
#define IOUT_CH_AIN3      0x3
#define IOUT_CH_AIN4      0x4
#define IOUT_CH_AIN5      0x5
#define IOUT_CH_AIN6      0x6
#define IOUT_CH_AIN7      0x7
#define IOUT_CH_AIN8      0x8
#define IOUT_CH_AIN9      0x9
#define IOUT_CH_AIN10     0xA
#define IOUT_CH_AIN11     0xB
#define IOUT_CH_AIN12     0xC
#define IOUT_CH_AIN13     0xD
#define IOUT_CH_AIN14     0xE
#define IOUT_CH_AIN15     0xF

/*============================================================================
 * IO_CONTROL_2 Register bits (Address 0x04) [Reset = 0x0000]
 *============================================================================*/
#define VBIAS15_MASK      (1 << 15)
#define VBIAS14_MASK      (1 << 14)
#define VBIAS13_MASK      (1 << 13)
#define VBIAS12_MASK      (1 << 12)
#define VBIAS11_MASK      (1 << 11)
#define VBIAS10_MASK      (1 << 10)
#define VBIAS9_MASK       (1 << 9)
#define VBIAS8_MASK       (1 << 8)
#define VBIAS7_MASK       (1 << 7)
#define VBIAS6_MASK       (1 << 6)
#define VBIAS5_MASK       (1 << 5)
#define VBIAS4_MASK       (1 << 4)
#define VBIAS3_MASK       (1 << 3)
#define VBIAS2_MASK       (1 << 2)
#define VBIAS1_MASK       (1 << 1)
#define VBIAS0_MASK       (1 << 0)

/*============================================================================
 * ID Register value (Address 0x05)
 * Reset value is variant dependent: 0x28 for LHA7668B-8, 0x14 for LHA7668-8
 *============================================================================*/
#define ID_7668_4        0x04    /* LHA7668-4   */
#define ID_7668_8        0x14    /* LHA7668-8   */
#define ID_7668_B4       0x18    /* LHA7668B-4  */
#define ID_7668_B8       0x28    /* LHA7668B-8  */

/* The evaluation board is fitted with LHA7668B-8; lha7668_Init() verifies this. */
#define LHA7668_EXPECTED_ID  ID_7668_B8

/*============================================================================
 * ERROR Register bits (Address 0x06) [Reset = 0x000000]
 *============================================================================*/
#define ERR_LDO_CAP        (1 << 19)
#define ERR_ADC_CAL        (1 << 18)
#define ERR_ADC_CONV       (1 << 17)
#define ERR_ADC_SAT        (1 << 16)
#define ERR_AINP_OV        (1 << 15)
#define ERR_AINP_UV        (1 << 14)
#define ERR_AINM_OV        (1 << 13)
#define ERR_AINM_UV        (1 << 12)
#define ERR_REF_DET        (1 << 11)
#define ERR_DLDO_PSM       (1 << 9)
#define ERR_ALDO_PSM       (1 << 7)
#define ERR_SPI_IGNORE     (1 << 6)
#define ERR_SPI_SCLK_CNT   (1 << 5)
#define ERR_SPI_READ       (1 << 4)
#define ERR_SPI_WRITE      (1 << 3)
#define ERR_SPI_CRC        (1 << 2)
#define ERR_MM_CRC         (1 << 1)

/*============================================================================
 * ERROR_EN Register bits (Address 0x07) [Reset = 0x000040]
 *============================================================================*/
#define ERREN_MCLK_CNT     (1 << 22)
#define ERREN_LDO_CAP_TEST (1 << 21)
#define ERREN_LDO_CAP_MASK (3 << 19)
#define ERREN_LDO_CAP_OFF  (0 << 19)
#define ERREN_LDO_CAP_ALDO (1 << 19)
#define ERREN_LDO_CAP_DLDO (2 << 19)
#define ERREN_ADC_CAL      (1 << 18)
#define ERREN_ADC_CONV     (1 << 17)
#define ERREN_ADC_SAT      (1 << 16)
#define ERREN_AINP_OV      (1 << 15)
#define ERREN_AINP_UV      (1 << 14)
#define ERREN_AINM_OV      (1 << 13)
#define ERREN_AINM_UV      (1 << 12)
#define ERREN_REF_DET      (1 << 11)
#define ERREN_DLDO_TRIP    (1 << 10)
#define ERREN_DLDO_PSM     (1 << 9)
#define ERREN_ALDO_TRIP    (1 << 8)
#define ERREN_ALDO_PSM     (1 << 7)
#define ERREN_SPI_IGNORE   (1 << 6)
#define ERREN_SPI_SCLK_CNT (1 << 5)
#define ERREN_SPI_READ     (1 << 4)
#define ERREN_SPI_WRITE    (1 << 3)
#define ERREN_SPI_CRC      (1 << 2)
#define ERREN_MM_CRC       (1 << 1)

/*============================================================================
 * CHANNEL Register bits (Address 0x09 ~ 0x18) [Reset CH0=0x8001, others=0x0000]
 *============================================================================*/
#define CH_EN              (1 << 15)
#define CH_SETUP_MASK      (7 << 12)
#define CH_SETUP0          (0 << 12)
#define CH_SETUP1          (1 << 12)
#define CH_SETUP2          (2 << 12)
#define CH_SETUP3          (3 << 12)
#define CH_SETUP4          (4 << 12)
#define CH_SETUP5          (5 << 12)
#define CH_SETUP6          (6 << 12)
#define CH_SETUP7          (7 << 12)
/* AINP selections (5-bit, Bits 9:5) */
#define AINP_MASK          (0x1F << 5)
#define AINP_AIN0          (0x00 << 5)
#define AINP_AIN1          (0x01 << 5)
#define AINP_AIN2          (0x02 << 5)
#define AINP_AIN3          (0x03 << 5)
#define AINP_AIN4          (0x04 << 5)
#define AINP_AIN5          (0x05 << 5)
#define AINP_AIN6          (0x06 << 5)
#define AINP_AIN7          (0x07 << 5)
#define AINP_AIN8          (0x08 << 5)
#define AINP_AIN9          (0x09 << 5)
#define AINP_AIN10         (0x0A << 5)
#define AINP_AIN11         (0x0B << 5)
#define AINP_AIN12         (0x0C << 5)
#define AINP_AIN13         (0x0D << 5)
#define AINP_AIN14         (0x0E << 5)
#define AINP_AIN15         (0x0F << 5)
#define AINP_TEMP          (0x10 << 5)
#define AINP_AVSS          (0x11 << 5)
#define AINP_INT_REF       (0x12 << 5)
#define AINP_DGND          (0x13 << 5)
#define AINP_AVDD_DIV6_P   (0x14 << 5)
#define AINP_AVDD_DIV6_N   (0x15 << 5)
#define AINP_IOVDD_DIV6_P  (0x16 << 5)
#define AINP_IOVDD_DIV6_N  (0x17 << 5)
#define AINP_ALDO_DIV6_P   (0x18 << 5)
#define AINP_ALDO_DIV6_N   (0x19 << 5)
#define AINP_DLDO_DIV6_P   (0x1A << 5)
#define AINP_DLDO_DIV6_N   (0x1B << 5)
#define AINP_20MV_P        (0x1C << 5)
#define AINP_20MV_N        (0x1D << 5)
/* AINM selections (5-bit, Bits 4:0) */
#define AINM_MASK          0x1F
#define AINM_AIN0          0x00
#define AINM_AIN1          0x01
#define AINM_AIN2          0x02
#define AINM_AIN3          0x03
#define AINM_AIN4          0x04
#define AINM_AIN5          0x05
#define AINM_AIN6          0x06
#define AINM_AIN7          0x07
#define AINM_AIN8          0x08
#define AINM_AIN9          0x09
#define AINM_AIN10         0x0A
#define AINM_AIN11         0x0B
#define AINM_AIN12         0x0C
#define AINM_AIN13         0x0D
#define AINM_AIN14         0x0E
#define AINM_AIN15         0x0F
#define AINM_TEMP          0x10
#define AINM_AVSS          0x11
#define AINM_INT_REF       0x12
#define AINM_DGND          0x13
#define AINM_AVDD_DIV6_P   0x14
#define AINM_AVDD_DIV6_N   0x15
#define AINM_IOVDD_DIV6_P  0x16
#define AINM_IOVDD_DIV6_N  0x17
#define AINM_ALDO_DIV6_P   0x18
#define AINM_ALDO_DIV6_N   0x19
#define AINM_DLDO_DIV6_P   0x1A
#define AINM_DLDO_DIV6_N   0x1B
#define AINM_20MV_P        0x1C
#define AINM_20MV_N        0x1D

/*============================================================================
 * CONFIG Register bits (Address 0x19 ~ 0x20) [Reset = 0x0860]
 *============================================================================*/
#define CONFIG_BIPOLAR      (1 << 11)
#define CONFIG_UNIPOLAR     (0 << 11)
#define CONFIG_BURNOUT_MASK (3 << 9)
#define CONFIG_BURNOUT_OFF  (0 << 9)
#define CONFIG_BURNOUT_0_5U (1 << 9)
#define CONFIG_BURNOUT_2U   (2 << 9)
#define CONFIG_BURNOUT_4U   (3 << 9)
#define CONFIG_REF_BUF_P    (1 << 8)
#define CONFIG_REF_BUF_M    (1 << 7)
#define CONFIG_AIN_BUF_P    (1 << 6)
#define CONFIG_AIN_BUF_M    (1 << 5)
#define CONFIG_REF_SEL_MASK (3 << 3)
#define CONFIG_REF_EXT1     (0 << 3)
#define CONFIG_REF_EXT2     (1 << 3)
#define CONFIG_REF_INTERNAL (2 << 3)
#define CONFIG_REF_AVDD     (3 << 3)

/* Internal reference initial accuracy: +/-0.1% (LHA7668B), +/-0.2% (LHA7668).
 * Voltage conversion uses the nominal 2500 mV, so this dominates scale error. */
#define CONFIG_PGA_MASK     (7 << 0)
#define CONFIG_PGA_1        (0 << 0)
#define CONFIG_PGA_2        (1 << 0)
#define CONFIG_PGA_4        (2 << 0)
#define CONFIG_PGA_8        (3 << 0)
#define CONFIG_PGA_16       (4 << 0)
#define CONFIG_PGA_32       (5 << 0)
#define CONFIG_PGA_64       (6 << 0)
#define CONFIG_PGA_128      (7 << 0)

/*============================================================================
 * FILTER Register bits (Address 0x21 ~ 0x28) [Reset = 0x060180]
 *============================================================================*/
#define FILTER_TYPE_MASK    (7 << 21)
#define FILTER_SINC4        (0 << 21)
#define FILTER_SINC3        (2 << 21)
#define FILTER_FAST_SINC4   (4 << 21)
#define FILTER_FAST_SINC3   (5 << 21)
#define FILTER_POST         (7 << 21)
#define FILTER_REJ60        (1 << 20)
#define FILTER_POST_MASK    (7 << 17)
#define FILTER_POST_27SPS   (2 << 17)
#define FILTER_POST_25SPS   (3 << 17)
#define FILTER_POST_20SPS   (5 << 17)
#define FILTER_POST_16SPS   (6 << 17)
#define FILTER_SINGLE_CYCLE (1 << 16)
#define FILTER_FS_MASK      0x07FF

/*============================================================================
 * Function prototypes
 *============================================================================*/

void lha7668_SoftwareReset(void);
void lha7668_Init(void);
void lha7668_WriteRegister(uint8_t addr, uint8_t *data, uint8_t num);
void lha7668_ReadRegister(uint8_t addr, uint8_t *data, uint8_t num);
uint32_t lha7668_ReadData(void);
float lha7668_CodeToVoltage(uint32_t code, uint32_t ref, uint8_t bipolar, uint8_t gain);
float lha7668_CodeToTemperature(uint32_t code);

/* Field selection parameters take the pre-shifted macros defined in this header
 * (AINP_x, CONFIG_x, FILTER_x, IO1_x, IOUT_CH_x). Never pass raw field values.
 * The setup parameter is an exception: it is a raw index (0-7) because it is used
 * both as a register offset and as the SETUP field, so never pass CH_SETUPx. */
void lha7668_ConfigChannel(uint8_t channel, uint8_t enable, uint8_t setup, uint16_t ainp, uint16_t ainm);
void lha7668_ConfigSetup(uint8_t setup, uint8_t bipolar, uint16_t burnout, uint8_t ref_buf_p, uint8_t ref_buf_m,
                          uint8_t ain_buf_p, uint8_t ain_buf_m, uint8_t ref_sel, uint8_t pga);
void lha7668_ConfigFilter(uint8_t setup, uint32_t filter_type, uint8_t rej60, uint32_t post_filter, uint8_t single_cycle, uint16_t fs);

void lha7668_SetIOUT(uint8_t iout_sel, uint8_t channel, uint16_t current);
void lha7668_SetVBIAS(uint16_t vbias_mask);
void lha7668_SetPDSW(uint8_t enable);
void lha7668_SetGPIO(uint8_t gpio_num, uint8_t enable, uint8_t value);

uint32_t lha7668_ReadError(void);
void lha7668_EnableDiagnostics(uint32_t err_en_mask);
uint8_t lha7668_ReadMCLKCount(void);

#endif