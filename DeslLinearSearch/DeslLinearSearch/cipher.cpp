#include "cipher.h"

//P置换按比特的表
int unPTable[32] = { 8, 16, 22, 30, 12, 27, 1, 17,
23, 15, 29, 5, 25, 19, 9, 0,
7, 13, 24, 2, 3, 28, 10, 18,
31, 11, 21, 6, 4, 26, 14, 20
};

//扩展置换E
u32 iExpansion(u16 index, u16 input){
	u32 temp=input;
	if(index==7){
		return (temp<<31|temp>>1);
	}else if(index==0){
		return (temp>>5|temp<<27);
	}else{
		return temp<<(3+(6-index)*4);
	}
}

//P置换
u32 iPermutation(u32 x){
	u32 y = 0;
	u32 t = 0x80000000;
	for (int i = 0; i<32; i++, t = t >> 1){
		if (unPTable[i] - i>0) y |= (x << (unPTable[i] - i))&t;
		else y |= (x >> (i - unPTable[i])) & t;
	}
	return y;
}

//32比特串转换成8个4比特数组
void word2SboxOutput(u16* output,u32 input){
	for(int i=0;i<8;i++){
		*(output+i)=((input>>(28-4*i))&0xf);
	}
}

//输入是S盒输出，是8个4比特数组
//输出是E的输出，是8个6比特数组
//实现线性层
void iEiP(u16* output, u16 index, u16 input){
	u32 temp=iExpansion(index,input);
	temp=iPermutation(temp);
	word2SboxOutput(output,temp);
}
