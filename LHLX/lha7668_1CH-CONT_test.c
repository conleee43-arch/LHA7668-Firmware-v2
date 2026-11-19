/**
 * @file    lha7668_1CH-CONT_test.c
 * @brief   Single-channel continuous single-ended, unipolar ADC acquisition test.
 *
 * Configuration summary (aligned with lha7668_seq16_test):
 *   - Only channel CH[LHA7668_1CH_CONT_AIN_CH] is enabled as (AINx+ / AVSS-)
 *   - All other 15 channels are disabled for single-channel continuous conversion
 *   - Unipolar mode (CONFIG_UNIPOLAR), PGA = 1, all input/ref buffers off
 *   - Internal 2.5V reference (CTRL_REF_EN + CONFIG_REF_INTERNAL)
 *   - Low-power mode + Sinc4 filter with FS = 0x78 (unchanged ODR settings)
 *   - Stops automatically after capturing LHA7668_1CH_CONT_MAX_SAMPLES samples
 */

#include "lha7668_1CH-CONT_test.h"
#include "gpio.h"

extern volatile uint8_t falg_data_ready;

static uint32_t s_sample_count = 0U;
static uint8_t s_test_done = 0U;

static const uint16_t k1ChAinpMap[16] = {
    AINP_AIN0,  AINP_AIN1,  AINP_AIN2,  AINP_AIN3,
    AINP_AIN4,  AINP_AIN5,  AINP_AIN6,  AINP_AIN7,
    AINP_AIN8,  AINP_AIN9,  AINP_AIN10, AINP_AIN11,
    AINP_AIN12, AINP_AIN13, AINP_AIN14, AINP_AIN15
};

static uint16_t lha7668_1ChContTest_ReadReg16(uint8_t addr)
{
    uint16_t val = 0;
    lha7668_ReadRegister(addr, (uint8_t *)&val, 2);
    return val;
}

static uint32_t lha7668_1ChContTest_ReadReg24(uint8_t addr)
{
    uint32_t val = 0;
    lha7668_ReadRegister(addr, (uint8_t *)&val, 3);
    return val;
}

/**
  * @brief  Initializes LHA7668B-8 for single-channel continuous single-ended
  *         unipolar conversion on (AIN[LHA7668_1CH_CONT_AIN_CH]+ / AVSS-).
  */
void lha7668_1ChContTest_Init(void)
{
    uint8_t id_val = 0;
    uint16_t adc_ctrl = 0;

    s_sample_count = 0U;
    s_test_done = 0U;

    lha7668_SoftwareReset();

    lha7668_ReadRegister(REG_ID, &id_val, 1);
    printf("[1CH-CONT] ID: 0x%02x\r\n", (unsigned)id_val);
    if (id_val != LHA7668_EXPECTED_ID) {
        printf("[1CH-CONT] ID mismatch, expected 0x%02x\r\n", (unsigned)LHA7668_EXPECTED_ID);
        Error_Handler();
        return;
    }

    /* Enter IDLE mode with DATA_STATUS enabled so REG_DATA appends STATUS byte */
    adc_ctrl = CTRL_MODE_IDLE | CTRL_DATA_STATUS | CTRL_REF_EN | CTRL_PWR_LOW;
    lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t *)&adc_ctrl, 2);
    printf("[1CH-CONT] ADC_CTRL(IDLE): 0x%04x\r\n", (unsigned)lha7668_1ChContTest_ReadReg16(REG_ADC_CTRL));

    /* Setup 0: Unipolar, Internal 2.5V ref, PGA=1, all buffers off */
    lha7668_ConfigSetup(0, LHA7668_1CH_CONT_BIPOLAR, CONFIG_BURNOUT_OFF,
                        0, 0, 0, 0, CONFIG_REF_INTERNAL, CONFIG_PGA_1);
    printf("[1CH-CONT] CONFIG0: 0x%04x\r\n", (unsigned)lha7668_1ChContTest_ReadReg16(REG_CONFIG0));

    /* Filter 0: Sinc4, FS=120 (0x78), single-cycle off (unchanged ODR config) */
    lha7668_ConfigFilter(0, FILTER_SINC4, 0, 0, 0, 0x78);
    printf("[1CH-CONT] FILTER0: 0x%06x\r\n", (unsigned)lha7668_1ChContTest_ReadReg24(REG_FILTER0));

    /* Enable only the selected channel (AINx+ / AVSS-) and disable all others */
    for (uint8_t ch = 0; ch < 16U; ch++) {
        uint8_t enable = (ch == LHA7668_1CH_CONT_AIN_CH) ? 1U : 0U;
        lha7668_ConfigChannel(ch, enable, 0, k1ChAinpMap[ch], AINM_AVSS);
    }
    printf("[1CH-CONT] Active CH%02u(AIN%02u-AVSS): 0x%04x\r\n",
           (unsigned)LHA7668_1CH_CONT_AIN_CH,
           (unsigned)LHA7668_1CH_CONT_AIN_CH,
           (unsigned)lha7668_1ChContTest_ReadReg16((uint8_t)(REG_CH0 + LHA7668_1CH_CONT_AIN_CH)));

    /* Start continuous conversion with DATA_STATUS enabled */
    adc_ctrl = CTRL_MODE_CONT | CTRL_DATA_STATUS | CTRL_REF_EN | CTRL_PWR_LOW;
    lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t *)&adc_ctrl, 2);
    printf("[1CH-CONT] ADC_CTRL(CONT): 0x%04x\r\n", (unsigned)lha7668_1ChContTest_ReadReg16(REG_ADC_CTRL));

    /* Reset modulator and digital filter */
    SYNC_LOW;
    delay_us(10);
    SYNC_HIGH;
}

/**
  * @brief  Reads 24-bit ADC conversion data plus appended 8-bit STATUS byte.
  * @param  code:   Output 24-bit raw ADC code (0x000000..0xFFFFFF).
  * @param  status: Output STATUS byte (bits[3:0] = active channel).
  */
void lha7668_1ChContTest_ReadDataWithStatus(uint32_t *code, uint8_t *status)
{
    uint8_t data[4] = {0};

    /* With DATA_STATUS=1, SPI returns [DATA_MSB, DATA_MID, DATA_LSB, STATUS].
     * lha7668_ReadRegister reverses the 4 bytes into data[0..3]. */
    lha7668_ReadRegister(REG_DATA, data, 4);

    if (status != NULL) {
        *status = data[0];
    }
    if (code != NULL) {
        *code = ((uint32_t)data[3] << 16) |
                ((uint32_t)data[2] << 8)  |
                ((uint32_t)data[1] << 0);
    }
}

/**
  * @brief  Handles one DRDY interrupt event in the main loop.
  *         Switches MISO to SPI, reads data + status, prints voltage in mV,
  *         and stops after LHA7668_1CH_CONT_MAX_SAMPLES samples.
  */
void lha7668_1ChContTest_ProcessSample(void)
{
    uint32_t adc_code = 0;
    uint8_t status = 0;
    uint8_t ch = 0;
    float volt_mv = 0.0f;

    if (s_test_done || !falg_data_ready) {
        return;
    }

    falg_data_ready = 0;
    miso_spi_init();
    lha7668_1ChContTest_ReadDataWithStatus(&adc_code, &status);

    ch = status & STATUS_CH_MASK;
    volt_mv = lha7668_CodeToVoltage(adc_code,
                                    LHA7668_1CH_CONT_VREF_MV,
                                    LHA7668_1CH_CONT_BIPOLAR,
                                    LHA7668_1CH_CONT_PGA_GAIN);

    s_sample_count++;
    printf("[%03u/%03u] CH%02u(AIN%02u-AVSS): 0x%06x, %.4f mV%s\r\n",
           (unsigned)s_sample_count,
           (unsigned)LHA7668_1CH_CONT_MAX_SAMPLES,
           (unsigned)ch,
           (unsigned)ch,
           (unsigned)adc_code,
           volt_mv,
           (status & STATUS_ERR_FLAG) ? " [ERR]" : "");

    if (s_sample_count >= LHA7668_1CH_CONT_MAX_SAMPLES) {
        uint16_t adc_ctrl = CTRL_MODE_IDLE | CTRL_REF_EN | CTRL_PWR_LOW;
        lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t *)&adc_ctrl, 2);
        CSN_HIGH;
        s_test_done = 1U;
        printf("[1CH-CONT] Done: %u samples captured.\r\n", (unsigned)s_sample_count);
        return;
    }

    miso_int_init();
    CSN_LOW;
}
