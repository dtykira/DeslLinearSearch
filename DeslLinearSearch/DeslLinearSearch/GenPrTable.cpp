#include "GenPrTable.h"
#include <algorithm>

int PDT[SBOX_NUMBER][SBOX_INPUTS_NUMBER][SBOX_OUTPUTS_NUMBER];
int PDT_Number[SBOX_NUMBER];
int PDT_0_Number[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][PR_NUMBER];
int PDT_0_Offset[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][PR_NUMBER][2];
int PDT_1_Number[SBOX_NUMBER][PR_NUMBER];
int PDT_1_Offset[SBOX_NUMBER][PR_NUMBER][2];
int PDT_1_Non0Num[SBOX_NUMBER][PR_NUMBER];
int PDT_1_Non0Val[SBOX_NUMBER][PR_NUMBER][SBOX_OUTPUTS_NUMBER];



u16 iS_LAYER[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][MAX_INPUTDIFFS_NUMBER];

ALIGNED_TYPE_(u16,8) iSiEiP[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER][MAX_INPUTDIFFS_NUMBER][SBOX_NUMBER];

#define PRINT 1

static const u8 Wti[64] = {0x0, //1
	0x1, 0x2, 0x4, 0x8, 0x10, 0x20, //6
	0x3, 0x5, 0x6, 0x9, 0xa, 0xc, 0x11, 0x12, 0x14, 0x18, 0x21, 0x22, 0x24, 0x28, 0x30, //15
	0x7, 0xb, 0xd, 0xe, 0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, 0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x31, 0x32, 0x34, 0x38,//20
	0xf, 0x17, 0x1b, 0x1d, 0x1e, 0x27, 0x2b, 0x2d, 0x2e, 0x33, 0x35, 0x36, 0x39, 0x3a, 0x3c,//15
	0x1f, 0x2f, 0x37, 0x3b, 0x3d, 0x3e,//6
	0x3f//1
};
static const u8 Wto[16] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x3, 0x5, 0x6, 0x9, 0xa, 0xc, 0x7, 0xb, 0xd, 0xe, 0xf};

/*static const u8 WtiForTravel[SBOX_INPUTS_NUMBER-1] = {
	0x1, 0x2, 0x4, 0x8, 0x10, 0x20, //6
	0x3, 0x5, 0x6, 0x9, 0xa, 0xc, 0x11, 0x12, 0x14, 0x18, 0x21, 0x22, 0x24, 0x28, 0x30, //15
	0x7, 0xb, 0xd, 0xe, 0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, 0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x31, 0x32, 0x34, 0x38,//20
	0xf, 0x17, 0x1b, 0x1d, 0x1e, 0x27, 0x2b, 0x2d, 0x2e, 0x33, 0x35, 0x36, 0x39, 0x3a, 0x3c,//15
	0x1f, 0x2f, 0x37, 0x3b, 0x3d, 0x3e,//6
	0x3f//1
};*/

prType PDT_MaxProb[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER];
u8 outputForTravel[SBOX_NUMBER][SBOX_OUTPUTS_NUMBER-1] = {
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	{0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 ,0xa, 0xb, 0xc, 0xd, 0xe, 0xf},
	};

int cmp0(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[0][x]<PDT_MaxProb[0][y];
}
int cmp1(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[1][x]<PDT_MaxProb[1][y];
}
int cmp2(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[2][x]<PDT_MaxProb[2][y];
}
int cmp3(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[3][x]<PDT_MaxProb[3][y];
}
int cmp4(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[4][x]<PDT_MaxProb[4][y];
}
int cmp5(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[5][x]<PDT_MaxProb[5][y];
}
int cmp6(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[6][x]<PDT_MaxProb[6][y];
}
int cmp7(int x,int y)                    //排序是依据a数组进行的  
{  
    return PDT_MaxProb[7][x]<PDT_MaxProb[7][y];
}

//替换函数S
u16 Sbox[SBOX_NUMBER][4][16] =//8个S盒
{
	{
		// S1
		{ 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
		{ 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8 },
		{ 4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0 },
		{ 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13 }
	},
	{
		// S2
		{ 15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10 },
		{ 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5 },
		{ 0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15 },
		{ 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9 }
	},
	{
		// S3
		{ 10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8 },
		{ 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1 },
		{ 13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7 },
		{ 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12 }
	},
	{
		// S4
		{ 7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15 },
		{ 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9 },
		{ 10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4 },
		{ 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14 }
	},
	{
		// S5
		{ 2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9 },
		{ 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6 },
		{ 4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14 },
		{ 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3 }
	},
	{
		// S6
		{ 12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11 },
		{ 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8 },
		{ 9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6 },
		{ 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13 }
	},
	{
		// S7
		{ 4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1 },
		{ 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6 },
		{ 1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2 },
		{ 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12 }
	},
	{
		// S8
		{ 13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7 },
		{ 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2 },
		{ 7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8 },
		{ 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 }
	}
};

u16 Substitution(u16 si,u16 input){
	u16 x=input,x1,x2;
	x1=(x&0x1)|((x>>4)&0x2);
	x2=(x>>1)&0xf;
	return Sbox[si][x1][x2];
}

prType Prob[PR_NUMBER];

//生成float型概率势数数组，概率从大到小
void GenProb(){
#if (PRINT)
	FILE *fp=fopen("Prob.txt","w");
#endif
	for(int i=0;i<PR_NUMBER;i++){
		Prob[i]=5.0-log((prType)((PR_NUMBER-i)*2))/log(2.0);
#if (PRINT)
		fprintf(fp,"%f\t",Prob[i]);
#endif
	}
#if (PRINT)
	fclose(fp);
#endif
}

#if (DLS==LS)
u16 inline xorSumInput(u16 x){
	u8 sum=0;
	for(int i=0;i<6;i++){
		sum^=(x>>i)&0x1;
	}
	return sum;
}

u16 inline xorSumOutput(u16 x){
	u8 sum=0;
	for(int i=0;i<4;i++){
		sum^=(x>>i)&0x1;
	}
	return sum;
}

void GenPDT(){
	for(u16 si=0;si<SBOX_NUMBER;si++){
		for(u16 il=0x0;il<SBOX_INPUTS_NUMBER;il++){
			for(u16 ol=0x0;ol<SBOX_OUTPUTS_NUMBER;ol++){
				PDT[si][il][ol]=0x0;
			}
		}
		for(u16 ii=0x0;ii<SBOX_INPUTS_NUMBER;ii++){
			u16 oi=Substitution(si,ii);
			for(u16 ol=0x0;ol<SBOX_OUTPUTS_NUMBER;ol++){
				for(u16 il=0x0;il<SBOX_INPUTS_NUMBER;il++){
					if(xorSumInput(ii&il)==xorSumOutput(oi&ol)){
						PDT[si][il][ol]++;
					}
				}
			}
		}
		for(u16 ol=0x0;ol<SBOX_OUTPUTS_NUMBER;ol++){
			for(u16 il=0x0;il<SBOX_INPUTS_NUMBER;il++){
				PDT[si][il][ol]=(PDT[si][il][ol]>32)?(PDT[si][il][ol]-32):(32-PDT[si][il][ol]);
				//PDT[si][il][ol]<<=1;
			}
		}
	}

#if (PRINT)
	FILE *fp=fopen("PDT_Table.txt","w");
	for(int si=0;si<8;si++){
		fprintf(fp,"\nSBOX:%d-----------\n.",si);
		for(u16 i=0x0;i<SBOX_INPUTS_NUMBER;i++){
			fprintf(fp,"/*0x%02x*/{",i);
			for(u16 j=0x0;j<SBOX_OUTPUTS_NUMBER;j++){
				fprintf(fp,"%d,",PDT[si][i][j]);
			}
			fprintf(fp,"},\n");
		}
	}
	fclose(fp);
#endif
}
#endif

//生成PDT统计信息，Storage()里用
void Statistics(){
	for(u16 si=0;si<SBOX_NUMBER;si++){
		PDT_Number[si]=0;
		memset(PDT_1_Number[si],0,PR_NUMBER*sizeof(int));
		memset(PDT_0_Number[si],0,PR_NUMBER*SBOX_OUTPUTS_NUMBER*sizeof(int));
		//初始化为0

		for(int o=0;o<SBOX_OUTPUTS_NUMBER;o++){
			for(int i=0;i<SBOX_INPUTS_NUMBER;i++){
				if(PDT[si][i][o]!=0&&PDT[si][i][o]!=32){
					if(PDT[si][i][o]==14&&si==0&&o==0x01)
					PDT_Number[si]++;
					PDT_1_Number[si][(PR_NUMBER-PDT[si][i][o]/2)]++;
					PDT_0_Number[si][o][(PR_NUMBER-PDT[si][i][o]/2)]++;
				}
			}
			PDT_0_Offset[si][o][0][0]=0;
			PDT_0_Offset[si][o][0][1]=PDT_0_Number[si][o][0];
			for(int p=1;p<PR_NUMBER;p++){
				PDT_0_Offset[si][o][p][0]=PDT_0_Offset[si][o][p-1][1];
				PDT_0_Offset[si][o][p][1]=PDT_0_Offset[si][o][p-1][1]+PDT_0_Number[si][o][p];
			}
		}
		PDT_1_Offset[si][0][0]=0;
		PDT_1_Offset[si][0][1]=PDT_1_Number[si][0];
		for(int p=1;p<PR_NUMBER;p++){
			PDT_1_Offset[si][p][0]=PDT_1_Offset[si][p-1][1];
			PDT_1_Offset[si][p][1]=PDT_1_Offset[si][p-1][1]+PDT_1_Number[si][p];
		}
	}

#if (PRINT)
	FILE *fp=fopen("Statistics_0_Table.txt","w");
	FILE *fq=fopen("Statistics_1_Table.txt","w");
	for(u16 i=0x0;i<SBOX_OUTPUTS_NUMBER;i++){
		fprintf(fp,"*/0x%02x*/",i);
		for(u16 p=0x0;p<PR_NUMBER;p++){
			fprintf(fp,"%d:%d-(%d,%d)\t",p,PDT_0_Number[0][i][p],PDT_0_Offset[0][i][p][0],PDT_0_Offset[0][i][p][1]);
		}
		fprintf(fp,"\n");
	}
	for(u16 p=0x0;p<PR_NUMBER;p++){
		fprintf(fq,"%d:%d-(%d,%d)\t",p,PDT_1_Number[0][p],PDT_1_Offset[0][p][0],PDT_1_Offset[0][p][1]);
	}
	fprintf(fq,"\n");
	fclose(fp);
	fclose(fq);
#endif

}

void Storage(){
	int PDT_1_Count[PR_NUMBER];
	int PDT_0_Count[PR_NUMBER];
	for(int si=0;si<SBOX_NUMBER;si++){
		for(int p=0;p<PR_NUMBER;p++){
			PDT_1_Count[p]=PDT_1_Offset[si][p][0];
		}
		for(u16 o=0;o<SBOX_OUTPUTS_NUMBER;o++){
			for(int p=0;p<PR_NUMBER;p++){
				PDT_0_Count[p]=PDT_0_Offset[si][Wto[o]][p][0];
			}
			for(u16 i=0;i<SBOX_INPUTS_NUMBER;i++){
				if(PDT[si][Wti[i]][Wto[o]]!=0&&PDT[si][Wti[i]][Wto[o]]!=32){
					memcpy(iSiEiP[si][Wto[o]][PDT_0_Count[PR_NUMBER-PDT[si][Wti[i]][Wto[o]]/2]],iEiP_Table[si][Wti[i]],SBOX_NUMBER*sizeof(u16));
					//iS_LAYER[si][Wto[o]][PDT_0_Count[PR_NUMBER-PDT[si][Wti[i]][Wto[o]]/2]]=Wti[i];
					PDT_1_Count[PR_NUMBER-PDT[si][Wti[i]][Wto[o]]/2]++;
					PDT_0_Count[PR_NUMBER-PDT[si][Wti[i]][Wto[o]]/2]++;
				}
			}
		}
	}

#if (PRINT)
	FILE *fp=fopen("Storage_Table.txt","w");
	for(int si=0;si<8;si++){
		fprintf(fp,"\nSBOX:%d-----------\n.",si);
		for(u16 i=0x0;i<SBOX_OUTPUTS_NUMBER;i++){
			fprintf(fp,"*/0x%02x*/\t",i);
			for(u16 p=0x0;p<PR_NUMBER;p++){
				fprintf(fp,"%d:\t",p);
				for(u16 k=PDT_0_Offset[si][i][p][0];k<PDT_0_Offset[si][i][p][1];k++){
					fprintf(fp,"(");
					for(int l=0;l<8;l++){
						fprintf(fp,"%02x,",iSiEiP[si][i][k][l]);
					}
					fprintf(fp,")\t");
				}
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);
#endif
}

void PDTStatistics(){
	for(int si=0;si<SBOX_NUMBER;si++){
		for(int p=0;p<PR_NUMBER;p++){
			PDT_1_Non0Num[si][p]=0;
		}
		for(int p=0;p<PR_NUMBER;p++){
			for(u16 i=0;i<SBOX_OUTPUTS_NUMBER;i++){
				if(PDT_0_Number[si][i][p]!=0){
					PDT_1_Non0Val[si][p][PDT_1_Non0Num[si][p]]=i;
					PDT_1_Non0Num[si][p]++;
				}
			}
		}
	}

	for(int si=0;si<8;si++){
		PDT_MaxProb[si][0]=0;
		for(int p=PR_NUMBER-1;p>=0;p--){
			for(u16 i=0;i<PDT_1_Non0Num[si][p];i++){
				PDT_MaxProb[si][PDT_1_Non0Val[si][p][i]]=Prob[p];
			}
		}
	}
	sort(outputForTravel[0],outputForTravel[0]+SBOX_OUTPUTS_NUMBER-1,cmp0);
	sort(outputForTravel[1],outputForTravel[1]+SBOX_OUTPUTS_NUMBER-1,cmp1);
	sort(outputForTravel[2],outputForTravel[2]+SBOX_OUTPUTS_NUMBER-1,cmp2);
	sort(outputForTravel[3],outputForTravel[3]+SBOX_OUTPUTS_NUMBER-1,cmp3);
	sort(outputForTravel[4],outputForTravel[4]+SBOX_OUTPUTS_NUMBER-1,cmp4);
	sort(outputForTravel[5],outputForTravel[5]+SBOX_OUTPUTS_NUMBER-1,cmp5);
	sort(outputForTravel[6],outputForTravel[6]+SBOX_OUTPUTS_NUMBER-1,cmp6);
	sort(outputForTravel[7],outputForTravel[7]+SBOX_OUTPUTS_NUMBER-1,cmp7);

#if (PRINT)
	FILE *fp=fopen("PDTStatistics_Table.txt","w");
	for(int p=0;p<PR_NUMBER;p++){
		fprintf(fp,"%d:%d\n",p,PDT_1_Non0Num[0][p]);
		for(u16 i=0;i<PDT_1_Non0Num[0][p];i++){
			fprintf(fp,"%02x\t",PDT_1_Non0Val[0][p][i]);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);

	FILE *fq=fopen("PDT_MaxProb.txt","w");
	for(int i=0;i<SBOX_OUTPUTS_NUMBER-1;i++){
		fprintf(fq,"%02x:%f\n",outputForTravel[0][i],PDT_MaxProb[0][outputForTravel[0][i]]);
	}
	fclose(fq);
#endif
}

void GenPrTable(){
	GenProb();
	GenPDT();
	Statistics();
	Storage();
	PDTStatistics();
}