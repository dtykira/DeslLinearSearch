#pragma once
#include "types.h"

u32 iExpansion(u16 index, u32 input);
u32 iPermutation(u32 x);
void word2SboxOutput(u16* output,u32 input);

void iEiP(u16* output, u16 index, u16 input);