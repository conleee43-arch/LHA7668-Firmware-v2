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

#include "lha7668.h"


//****************************************************************************
// Variables
//****************************************************************************



//****************************************************************************
// Public functions
//****************************************************************************

/**
  * @brief  Performs a software reset of the LHA7668.
  *         Sends 64 SCLK cycles with DIN high.
  * @param  None
  * @retval None
  */
void lha7668_SoftwareReset(void)
{
    uint8_t reset_tx[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t reset_rx[8] = {0};
    CSN_LOW;
    HAL_SPI_TransmitReceive(&hspi1, reset_tx, reset_rx, 8, HAL_MAX_DELAY);
    CSN_HIGH;
    delay_us(2000);
}

/**
  * @brief  Initializes the LHA7668 ADC.
  *         Performs software reset and sets default configuration.
  * @param  None
  * @retval None
  */
void lha7668_Init(void)
{
		// Perform software reset
    lha7668_SoftwareReset();

		uint16_t regval;
		
		// Read ID register to verify communication
    lha7668_ReadRegister(REG_ID, (uint8_t*)&regval, 1);
		printf("ID: 0x%x\r\n",regval);

    // Enter IDLE mode before configuration
    uint16_t adc_ctrl = CTRL_MODE_IDLE | CTRL_REF_EN | CTRL_PWR_LOW;
    lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t*)&adc_ctrl, 2);
		lha7668_ReadRegister(REG_ADC_CTRL, (uint8_t*)&regval, 2);
		printf("ADC_CTRL: 0x%x\r\n",regval);

    // Configure Channel 0: Enable, Setup 0, AIN0(+), AIN1(-)
		uint16_t ch_config = CH_EN | CH_SETUP0 | AINP_AIN0 | AINM_AIN1;
    lha7668_WriteRegister(REG_CH0, (uint8_t*)&ch_config, 2);
		lha7668_ReadRegister(REG_CH0, (uint8_t*)&regval, 2);
		printf("CH0: 0x%x\r\n",regval);

    // Configure CONFIG0: Bipolar, PGA=1, Int Ref, All buffers off
    uint16_t config = CONFIG_BIPOLAR | CONFIG_REF_INTERNAL | CONFIG_PGA_1;
    lha7668_WriteRegister(REG_CONFIG0, (uint8_t*)&config, 2);
		lha7668_ReadRegister(REG_CONFIG0, (uint8_t*)&regval, 2);
		printf("REG_CONFIG0: 0x%x\r\n",regval);

    // Configure FILTER0: Sinc4, FS=120, Single Cycle off
    uint16_t filter = FILTER_SINC4 | (0x78 & FILTER_FS_MASK); // 20SPS
    lha7668_WriteRegister(REG_FILTER0, (uint8_t*)&filter, 3);
		lha7668_ReadRegister(REG_FILTER0, (uint8_t*)&regval, 3);
		printf("REG_FILTER0: 0x%x\r\n",regval);
		
		// Switch to CONTINUOUS conversion mode
    adc_ctrl = CTRL_MODE_CONT | CTRL_REF_EN | CTRL_PWR_LOW;
    lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t*)&adc_ctrl, 2);
		lha7668_ReadRegister(REG_ADC_CTRL, (uint8_t*)&regval, 2);
		printf("ADC_CTRL: 0x%x\r\n",regval);
		
		// Clear digital filter
		SYNC_LOW;
		delay_us(10);
		SYNC_HIGH;
}

/**
  * @brief  Writes data to a specified register.
  * @param  addr: Register address.
  * @param  data: Pointer to data buffer.
  * @param  num: Number of bytes to write.
  * @retval None
  */
void lha7668_WriteRegister(uint8_t addr, uint8_t *data, uint8_t num)
{
    if (data == NULL || num == 0) {
        return;
    }

    // Communications byte: Write operation, address
    uint8_t comms_byte = (0 << 6) | (addr & 0x3F);

    // Transmit buffer: comms_byte + data (MSB-first for SPI)
    uint8_t tx_buffer[4];
    uint8_t rx_buffer[4];
    tx_buffer[0] = comms_byte;

    // Reverse byte order to convert from little-endian to big-endian
    // data[0] is LSB, data[num-1] is MSB on STM32
    // SPI requires MSB sent first
    for (uint8_t i = 0; i < num; i++) {
        tx_buffer[i + 1] = data[num - 1 - i];
    }

    CSN_LOW;
    HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, num + 1, HAL_MAX_DELAY);
    CSN_HIGH;
}

/**
  * @brief  Reads data from a specified register.
  * @param  addr: Register address.
  * @param  data: Pointer to buffer to store data.
  * @param  num: Number of bytes to read.
  * @retval None
  */
void lha7668_ReadRegister(uint8_t addr, uint8_t *data, uint8_t num)
{
    if (data == NULL || num == 0) {
        return;
    }
		
		// Communications byte: Read operation, address
    uint8_t comms_byte = (1 << 6) | (addr & 0x3F);

    // Transmit: comms_byte + num dummy bytes (to provide clock)
    uint8_t tx_buffer[4];
    uint8_t rx_buffer[4];
    tx_buffer[0] = comms_byte;
    for (uint8_t i = 1; i <= num; i++) {
        tx_buffer[i] = 0x00;
    }

    CSN_LOW;
    HAL_SPI_TransmitReceive(&hspi1, tx_buffer, rx_buffer, num + 1, HAL_MAX_DELAY);
    CSN_HIGH;

    // SPI returns MSB-first (big-endian).
    // rx_buffer[1] = MSB, rx_buffer[2] = MID, ..., rx_buffer[num] = LSB
    // STM32 is little-endian, so store LSB first in data[0].
    // Reverse the order: data[0] = LSB, data[num-1] = MSB
    for (uint8_t i = 0; i < num; i++) {
        data[i] = rx_buffer[num - i];
    }
}

/**
  * @brief  Reads the ADC conversion result (24-bit data only).
  * @param  None
  * @retval 24-bit ADC conversion result (aligned to MSB, LSB 8 bits are zero).
  */
uint32_t lha7668_ReadData(void)
{
    uint8_t data[4] = {0};
    lha7668_ReadRegister(REG_DATA, data, 3);
    // After ReadRegister, data[0]=LSB, data[1]=MID, data[2]=MSB
    // Reconstruct 24-bit value with MSB at top
    return ((uint32_t)data[2] << 16) |
           ((uint32_t)data[1] << 8)  |
           ((uint32_t)data[0] << 0);
}

/**
  * @brief  Converts ADC code to voltage in millivolts (mV).
  * @param  code: Raw ADC code from lha7668_ReadData().
  * @param  ref_mV: Reference voltage in millivolts.
  * @param  bipolar: 0 for unipolar, 1 for bipolar.
  * @param  gain: PGA gain value (1, 2, 4, 8, 16, 32, 64, 128).
  * @retval Voltage in millivolts.
  */
float lha7668_CodeToVoltage(uint32_t code, uint32_t ref_mV, uint8_t bipolar, uint8_t gain)
{
    float voltage = 0.0f;
    int32_t signed_code = 0;
    uint32_t max_code = 0xFFFFFF;

    uint32_t raw = code & 0xFFFFFF;

    if (bipolar) {
        signed_code = (int32_t)(raw - 0x800000);
        voltage = ((float)signed_code / 0x800000) * ref_mV / (float)gain;
    } else {
        voltage = ((float)raw / (float)max_code) * ref_mV / (float)gain;
    }

    return voltage;
}

/**
  * @brief  Converts ADC code to temperature in C (LHA7668).
  *         Formula: T(C) = ((code - 0x800000) / 13584) - 272.5
  * @param  code: Raw ADC code from lha7668_ReadData().
  * @retval Temperature in C.
  */
float lha7668_CodeToTemperature(uint32_t code)
{
    int32_t signed_code = (int32_t)((code >> 8) & 0xFFFFFF) - 0x800000;
    return ((float)signed_code / 13584.0f) - 272.5f;
}

/**
  * @brief  Configures a specific channel.
  * @param  channel: Channel number (0-15).
  * @param  enable: 1 to enable, 0 to disable.
  * @param  setup: Setup number (0-7).
  * @param  ainp: Positive input selection (AINP_* defines, already shifted).
  * @param  ainm: Negative input selection (AINM_* defines).
  * @retval None
  */
void lha7668_ConfigChannel(uint8_t channel, uint8_t enable, uint8_t setup, uint16_t ainp, uint16_t ainm)
{
    if (channel > 15) return;

    uint16_t reg_value = 0;
    uint8_t reg_addr = REG_CH0 + channel;

    if (enable) {
        reg_value = CH_EN | (setup << 12) | (ainp & 0x03E0) | (ainm & 0x001F);
    } else {
        reg_value = (setup << 12) | (ainp & 0x03E0) | (ainm & 0x001F);
    }

    lha7668_WriteRegister(reg_addr, (uint8_t*)&reg_value, 2);
}

/**
  * @brief  Configures a specific setup.
  * @param  setup: Setup number (0-7).
  * @param  bipolar: 1 for bipolar, 0 for unipolar.
  * @param  burnout: Burnout current (CONFIG_BURNOUT_* defines).
  * @param  ref_buf_p: 1 to enable REF+ buffer.
  * @param  ref_buf_m: 1 to enable REF- buffer.
  * @param  ain_buf_p: 1 to enable AINP buffer.
  * @param  ain_buf_m: 1 to enable AINM buffer.
  * @param  ref_sel: Reference selection (CONFIG_REF_* defines).
  * @param  pga: PGA gain (CONFIG_PGA_* defines).
  * @retval None
  */
void lha7668_ConfigSetup(uint8_t setup, uint8_t bipolar, uint8_t burnout, uint8_t ref_buf_p, uint8_t ref_buf_m,
                          uint8_t ain_buf_p, uint8_t ain_buf_m, uint8_t ref_sel, uint8_t pga)
{
    if (setup > 7) return;

    uint16_t reg_value = 0;

    if (bipolar) reg_value |= CONFIG_BIPOLAR;
    reg_value |= (burnout & 3) << 9;
    if (ref_buf_p) reg_value |= CONFIG_REF_BUF_P;
    if (ref_buf_m) reg_value |= CONFIG_REF_BUF_M;
    if (ain_buf_p) reg_value |= CONFIG_AIN_BUF_P;
    if (ain_buf_m) reg_value |= CONFIG_AIN_BUF_M;
    reg_value |= (ref_sel & 3) << 3;
    reg_value |= (pga & 7);

    lha7668_WriteRegister(REG_CONFIG0 + setup, (uint8_t*)&reg_value, 2);
}

/**
  * @brief  Configures the filter for a specific setup.
  * @param  setup: Setup number (0-7).
  * @param  filter_type: FILTER_* defines.
  * @param  rej60: 1 to enable 60Hz rejection notch.
  * @param  post_filter: Post-filter selection (FILTER_POST_* defines).
  * @param  single_cycle: 1 to enable single cycle settling.
  * @param  fs: Filter word (0-2047).
  * @retval None
  */
void lha7668_ConfigFilter(uint8_t setup, uint8_t filter_type, uint8_t rej60, uint8_t post_filter,
                           uint8_t single_cycle, uint16_t fs)
{
    if (setup > 7) return;

    uint32_t reg_value = 0;
    uint8_t reg_addr = REG_FILTER0 + setup;

    reg_value |= (filter_type & 0xE0) << 16;
    if (rej60) reg_value |= (1 << 20);
    reg_value |= (post_filter & 7) << 17;
    if (single_cycle) reg_value |= (1 << 16);
    reg_value |= (fs & 0x07FF);

    lha7668_WriteRegister(reg_addr, (uint8_t*)&reg_value, 4);
}

/**
  * @brief  Sets the excitation current source.
  * @param  iout_sel: 0 for IOUT0, 1 for IOUT1.
  * @param  channel: Channel selection (IOUT_CH_AINx defines).
  * @param  current: Current value (IO1_IOUTx_* defines).
  * @retval None
  */
void lha7668_SetIOUT(uint8_t iout_sel, uint8_t channel, uint8_t current)
{
    uint32_t reg_value = 0;
    lha7668_ReadRegister(REG_IO_CTRL1, (uint8_t*)&reg_value, 4);

    if (iout_sel == 0) {
        reg_value &= ~(IO1_IOUT0_MASK | IO1_IOUT0_CH_MASK);
        reg_value |= (current & 7) << 8;
        reg_value |= (channel & 0x0F);
    } else {
        reg_value &= ~(IO1_IOUT1_MASK | (0xF << 4));
        reg_value |= (current & 7) << 11;
        reg_value |= (channel & 0x0F) << 4;
    }

    lha7668_WriteRegister(REG_IO_CTRL1, (uint8_t*)&reg_value, 4);
}

/**
  * @brief  Enables/disables bias voltage on selected pins.
  * @param  vbias_mask: Bit mask of pins to enable VBIAS on (VBIASx_MASK).
  * @retval None
  */
void lha7668_SetVBIAS(uint16_t vbias_mask)
{
    lha7668_WriteRegister(REG_IO_CTRL2, (uint8_t*)&vbias_mask, 2);
}

/**
  * @brief  Controls the power-down switch (PDSW).
  * @param  enable: 1 to close switch, 0 to open.
  * @retval None
  */
void lha7668_SetPDSW(uint8_t enable)
{
    uint32_t reg_value = 0;
    lha7668_ReadRegister(REG_IO_CTRL1, (uint8_t*)&reg_value, 4);

    if (enable) {
        reg_value |= IO1_PDSW;
    } else {
        reg_value &= ~IO1_PDSW;
    }

    lha7668_WriteRegister(REG_IO_CTRL1, (uint8_t*)&reg_value, 4);
}

/**
  * @brief  Configures the GPIO pins (P1-P4).
  * @param  gpio_num: GPIO number (1-4).
  * @param  enable: 1 to enable as output, 0 to disable (use as analog input).
  * @param  value: Output value (only valid when enable=1).
  * @retval None
  */
void lha7668_SetGPIO(uint8_t gpio_num, uint8_t enable, uint8_t value)
{
    uint32_t reg_value = 0;
    lha7668_ReadRegister(REG_IO_CTRL1, (uint8_t*)&reg_value, 4);

    switch (gpio_num) {
        case 1:
            if (enable) reg_value |= IO1_GPIO_CTRL1;
            else reg_value &= ~IO1_GPIO_CTRL1;
            if (value) reg_value |= IO1_GPIO_DAT1;
            else reg_value &= ~IO1_GPIO_DAT1;
            break;
        case 2:
            if (enable) reg_value |= IO1_GPIO_CTRL2;
            else reg_value &= ~IO1_GPIO_CTRL2;
            if (value) reg_value |= IO1_GPIO_DAT2;
            else reg_value &= ~IO1_GPIO_DAT2;
            break;
        case 3:
            if (enable) reg_value |= IO1_GPIO_CTRL3;
            else reg_value &= ~IO1_GPIO_CTRL3;
            if (value) reg_value |= IO1_GPIO_DAT3;
            else reg_value &= ~IO1_GPIO_DAT3;
            break;
        case 4:
            if (enable) reg_value |= IO1_GPIO_CTRL4;
            else reg_value &= ~IO1_GPIO_CTRL4;
            if (value) reg_value |= IO1_GPIO_DAT4;
            else reg_value &= ~IO1_GPIO_DAT4;
            break;
        default:
            return;
    }

    lha7668_WriteRegister(REG_IO_CTRL1, (uint8_t*)&reg_value, 4);
}

/**
  * @brief  Reads the error register.
  * @param  None
  * @retval 32-bit error register value.
  */
uint32_t lha7668_ReadError(void)
{
    uint32_t err = 0;
    lha7668_ReadRegister(REG_ERROR, (uint8_t*)&err, 4);
    return err;
}

/**
  * @brief  Enables/disables diagnostic functions.
  * @param  err_en_mask: Bit mask of diagnostic enable bits (ERREN_* defines).
  * @retval None
  */
void lha7668_EnableDiagnostics(uint32_t err_en_mask)
{
    lha7668_WriteRegister(REG_ERROR_EN, (uint8_t*)&err_en_mask, 4);
}

/**
  * @brief  Reads the MCLK counter value.
  * @param  None
  * @retval 8-bit MCLK counter value.
  */
uint8_t lha7668_ReadMCLKCount(void)
{
    uint8_t count = 0;
    lha7668_ReadRegister(REG_MCLK_COUNT, &count, 1);
    return count;
}