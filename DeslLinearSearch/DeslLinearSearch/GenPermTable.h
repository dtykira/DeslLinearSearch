#pragma once
#include "types.h"
#include "cipher.h"

extern ALIGNED_TYPE_(u16,8) iEiP_Table[SBOX_NUMBER][SBOX_INPUTS_NUMBER][SBOX_NUMBER];

void Gen_iEiP_Table();