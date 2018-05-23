#pragma once
#include "types.h"
#include "GenPermTable.h"

#define MAX_INPUTDIFFS_NUMBER (64)

extern int PDT[SBOX_NUMBER][SBOX_INPUTS_NUMBER][SBOX_OUTPUTS_NUMBER];

extern int PDT_Number[SBOX_NUMBER];
extern int PDT_0_Number[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][PR_NUMBER];
extern int PDT_0_Offset[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][PR_NUMBER][2];
extern int PDT_1_Number[SBOX_NUMBER][PR_NUMBER];
extern int PDT_1_Offset[SBOX_NUMBER][PR_NUMBER][2];

extern int PDT_1_Non0Num[SBOX_NUMBER][PR_NUMBER];
extern int PDT_1_Non0Val[SBOX_NUMBER][PR_NUMBER][SBOX_OUTPUTS_NUMBER];



extern u16 iS_LAYER[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][MAX_INPUTDIFFS_NUMBER];//??

extern ALIGNED_TYPE_(u16,8) iSiEiP[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][MAX_INPUTDIFFS_NUMBER][SBOX_NUMBER];
extern const u8 Wti[64];
extern const u8 Wto[16];

extern prType PDT_MaxProb[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER];
extern u8 outputForTravel[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER-1];

extern u16 *sbox;
u16 Substitution(u16 si,u16 input);

extern prType Prob[PR_NUMBER];

void GenProb();
void GenPDT();
void Statistics();
void Storage();
void PDTStatistics();
void GenPrTable();