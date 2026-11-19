/**
 * @file    lha7668_seq15_test.c
 * @brief   15-channel sequential pseudo-differential ADC acquisition test.
 *
 * Configuration summary (aligned with lha7668_seq16_test):
 *   - Channels CH0..CH14 enabled in sequence: (AIN0+ / AIN15-) .. (AIN14+ / AIN15-)
 *   - Channel CH15 disabled
 *   - Polarity controlled by LHA7668_SEQ15_BIPOLAR (default 0: unipolar),
 *     PGA = 1, all input/ref buffers off
 *   - Internal 2.5V reference (CTRL_REF_EN + CONFIG_REF_INTERNAL)
 *   - Low-power mode + Sinc4 filter with FS = 0x78 (unchanged ODR settings)
 *   - DATA_STATUS enabled so each sample carries its 4-bit active channel ID
 *   - Stops automatically after capturing LHA7668_SEQ15_MAX_SAMPLES samples
 */

#include "lha7668_seq15_test.h"
#include "gpio.h"

extern volatile uint8_t falg_data_ready;

static uint32_t s_sample_count = 0U;
static uint8_t s_test_done = 0U;

static const uint16_t kSeq15AinpMap[LHA7668_SEQ15_CH_COUNT] = {
    AINP_AIN0,  AINP_AIN1,  AINP_AIN2,  AINP_AIN3,
    AINP_AIN4,  AINP_AIN5,  AINP_AIN6,  AINP_AIN7,
    AINP_AIN8,  AINP_AIN9,  AINP_AIN10, AINP_AIN11,
    AINP_AIN12, AINP_AIN13, AINP_AIN14
};

static uint16_t lha7668_Seq15Test_ReadReg16(uint8_t addr)
{
    uint16_t val = 0;
    lha7668_ReadRegister(addr, (uint8_t *)&val, 2);
    return val;
}

static uint32_t lha7668_Seq15Test_ReadReg24(uint8_t addr)
{
    uint32_t val = 0;
    lha7668_ReadRegister(addr, (uint8_t *)&val, 3);
    return val;
}

/**
  * @brief  Initializes LHA7668B-8 for 15-channel sequential pseudo-differential
  *         conversion (AIN0+..AIN14+ vs AIN15-) with internal 2.5V ref and Sinc4 FS=0x78.
  */
void lha7668_Seq15Test_Init(void)
{
    uint8_t id_val = 0;
    uint16_t adc_ctrl = 0;

    s_sample_count = 0U;
    s_test_done = 0U;

    lha7668_SoftwareReset();

    lha7668_ReadRegister(REG_ID, &id_val, 1);
    printf("[SEQ15] ID: 0x%02x\r\n", (unsigned)id_val);
    if (id_val != LHA7668_EXPECTED_ID) {
        printf("[SEQ15] ID mismatch, expected 0x%02x\r\n", (unsigned)LHA7668_EXPECTED_ID);
        Error_Handler();
        return;
    }

    /* Enter IDLE mode with DATA_STATUS enabled so REG_DATA appends STATUS byte */
    adc_ctrl = CTRL_MODE_IDLE | CTRL_DATA_STATUS | CTRL_REF_EN | CTRL_PWR_LOW;
    lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t *)&adc_ctrl, 2);
    printf("[SEQ15] ADC_CTRL(IDLE): 0x%04x\r\n", (unsigned)lha7668_Seq15Test_ReadReg16(REG_ADC_CTRL));

    /* Setup 0: Internal 2.5V ref, PGA=1, all buffers off */
    lha7668_ConfigSetup(0, LHA7668_SEQ15_BIPOLAR, CONFIG_BURNOUT_OFF,
                        0, 0, 0, 0, CONFIG_REF_INTERNAL, CONFIG_PGA_1);
    printf("[SEQ15] CONFIG0: 0x%04x\r\n", (unsigned)lha7668_Seq15Test_ReadReg16(REG_CONFIG0));

    /* Filter 0: Sinc4, FS=120 (0x78), single-cycle off (unchanged ODR config) */
    lha7668_ConfigFilter(0, FILTER_SINC4, 0, 0, 0, 0x78);
    printf("[SEQ15] FILTER0: 0x%06x\r\n", (unsigned)lha7668_Seq15Test_ReadReg24(REG_FILTER0));

    /* Enable CH0..CH14 sequentially: (AIN0+ / AIN15-) .. (AIN14+ / AIN15-) */
    for (uint8_t ch = 0; ch < LHA7668_SEQ15_CH_COUNT; ch++) {
        lha7668_ConfigChannel(ch, 1, 0, kSeq15AinpMap[ch], AINM_AIN15);
    }
    /* Ensure CH15 is disabled */
    lha7668_ConfigChannel(15, 0, 0, AINP_AIN15, AINM_AIN15);

    printf("[SEQ15] CH0: 0x%04x, CH14: 0x%04x, CH15: 0x%04x\r\n",
           (unsigned)lha7668_Seq15Test_ReadReg16(REG_CH0),
           (unsigned)lha7668_Seq15Test_ReadReg16(REG_CH14),
           (unsigned)lha7668_Seq15Test_ReadReg16(REG_CH15));

    /* Start continuous conversion with DATA_STATUS enabled */
    adc_ctrl = CTRL_MODE_CONT | CTRL_DATA_STATUS | CTRL_REF_EN | CTRL_PWR_LOW;
    lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t *)&adc_ctrl, 2);
    printf("[SEQ15] ADC_CTRL(CONT): 0x%04x\r\n", (unsigned)lha7668_Seq15Test_ReadReg16(REG_ADC_CTRL));

    /* Reset modulator and digital filter to start sequence cleanly from CH0 */
    SYNC_LOW;
    delay_us(10);
    SYNC_HIGH;
}

/**
  * @brief  Reads 24-bit ADC conversion data plus appended 8-bit STATUS byte.
  * @param  code:   Output 24-bit raw ADC code (0x000000..0xFFFFFF).
  * @param  status: Output STATUS byte (bits[3:0] = active channel 0..14).
  */
void lha7668_Seq15Test_ReadDataWithStatus(uint32_t *code, uint8_t *status)
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
  *         Switches MISO to SPI, reads data + channel status, prints voltage
  *         in mV, then arms MISO EXTI interrupt until LHA7668_SEQ15_MAX_SAMPLES.
  */
void lha7668_Seq15Test_ProcessSample(void)
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
    lha7668_Seq15Test_ReadDataWithStatus(&adc_code, &status);

    ch = status & STATUS_CH_MASK;
    volt_mv = lha7668_CodeToVoltage(adc_code,
                                    LHA7668_SEQ15_VREF_MV,
                                    LHA7668_SEQ15_BIPOLAR,
                                    LHA7668_SEQ15_PGA_GAIN);

    s_sample_count++;
    printf("[%03u/%03u] CH%02u(AIN%02u-AIN15): 0x%06x, %.4f mV%s\r\n",
           (unsigned)s_sample_count,
           (unsigned)LHA7668_SEQ15_MAX_SAMPLES,
           (unsigned)ch,
           (unsigned)ch,
           (unsigned)adc_code,
           volt_mv,
           (status & STATUS_ERR_FLAG) ? " [ERR]" : "");

    if (s_sample_count >= LHA7668_SEQ15_MAX_SAMPLES) {
        uint16_t adc_ctrl = CTRL_MODE_IDLE | CTRL_REF_EN | CTRL_PWR_LOW;
        lha7668_WriteRegister(REG_ADC_CTRL, (uint8_t *)&adc_ctrl, 2);
        CSN_HIGH;
        s_test_done = 1U;
        printf("[SEQ15] Done: %u samples captured.\r\n", (unsigned)s_sample_count);
        return;
    }

    miso_int_init();
    CSN_LOW;
}
