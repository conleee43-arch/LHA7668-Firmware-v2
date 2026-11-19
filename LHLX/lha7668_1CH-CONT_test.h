/**
 * @file    lha7668_1CH-CONT_test.h
 * @brief   Single-channel continuous single-ended, unipolar test for LHA7668B-8.
 *          Selects one of 16 single-ended channels (AIN0+/AVSS- .. AIN15+/AVSS-)
 *          via LHA7668_1CH_CONT_AIN_CH macro and stops after 500 samples.
 */

#ifndef LHA7668_1CH_CONT_TEST_H
#define LHA7668_1CH_CONT_TEST_H

#include "lha7668.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Select single-ended positive input channel: 0..15 -> (AIN0+/AVSS- .. AIN15+/AVSS-) */
#define LHA7668_1CH_CONT_AIN_CH      5U

#define LHA7668_1CH_CONT_VREF_MV     2500U
#define LHA7668_1CH_CONT_PGA_GAIN    1U
#define LHA7668_1CH_CONT_BIPOLAR     0U   /* 0: unipolar (0..+VREF/gain) */
#define LHA7668_1CH_CONT_MAX_SAMPLES 500U

#if (LHA7668_1CH_CONT_AIN_CH > 15U)
#error "LHA7668_1CH_CONT_AIN_CH must be in range 0..15"
#endif

void lha7668_1ChContTest_Init(void);
void lha7668_1ChContTest_ReadDataWithStatus(uint32_t *code, uint8_t *status);
void lha7668_1ChContTest_ProcessSample(void);

#ifdef __cplusplus
}
#endif

#endif /* LHA7668_1CH_CONT_TEST_H */
