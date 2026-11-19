/**
 * @file    lha7668_seq15_test.h
 * @brief   15-channel sequential pseudo-differential test for LHA7668B-8.
 *          Channels CH0..CH14 map (AIN0+ / AIN15-) .. (AIN14+ / AIN15-).
 */

#ifndef LHA7668_SEQ15_TEST_H
#define LHA7668_SEQ15_TEST_H

#include "lha7668.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LHA7668_SEQ15_CH_COUNT     15U
#define LHA7668_SEQ15_VREF_MV      2500U
#define LHA7668_SEQ15_PGA_GAIN     1U
#define LHA7668_SEQ15_BIPOLAR      0U   /* 0: unipolar (0..+VREF/gain), 1: bipolar (+/-VREF/gain) */
#define LHA7668_SEQ15_MAX_SAMPLES  500U

void lha7668_Seq15Test_Init(void);
void lha7668_Seq15Test_ReadDataWithStatus(uint32_t *code, uint8_t *status);
void lha7668_Seq15Test_ProcessSample(void);

#ifdef __cplusplus
}
#endif

#endif /* LHA7668_SEQ15_TEST_H */
