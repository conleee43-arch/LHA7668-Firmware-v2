/**
 * @file    lha7668_seq16_test.h
 * @brief   16-channel sequential single-ended, unipolar test for LHA7668B-8.
 *          Channels CH0..CH15 map (AIN0+ / AVSS-) .. (AIN15+ / AVSS-).
 */

#ifndef LHA7668_SEQ16_TEST_H
#define LHA7668_SEQ16_TEST_H

#include "lha7668.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LHA7668_SEQ16_CH_COUNT     16U
#define LHA7668_SEQ16_VREF_MV      2500U
#define LHA7668_SEQ16_PGA_GAIN     1U
#define LHA7668_SEQ16_MAX_SAMPLES  500U

void lha7668_Seq16Test_Init(void);
void lha7668_Seq16Test_ReadDataWithStatus(uint32_t *code, uint8_t *status);
void lha7668_Seq16Test_ProcessSample(void);

#ifdef __cplusplus
}
#endif

#endif /* LHA7668_SEQ16_TEST_H */
