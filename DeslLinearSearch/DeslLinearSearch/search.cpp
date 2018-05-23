#include "search.h"

si8 a[2][SBOX_NUMBER+1];//a[*][0]=-1

__m128i rshift(__m128i x,int i){
	__m128i tmp1,tmp2;
	switch(i){
	case 1:
		tmp1=_mm_slli_si128(x,2);//右移i个s盒
		tmp2=_mm_srli_si128(x,14);//左移8-i个S盒
		break;
	case 2:
		tmp1=_mm_slli_si128(x,4);//右移i个s盒
		tmp2=_mm_srli_si128(x,12);//左移8-i个S盒
		break;
	case 3:
		tmp1=_mm_slli_si128(x,6);//右移i个s盒
		tmp2=_mm_srli_si128(x,10);//左移8-i个S盒
		break;
	case 4:
		tmp1=_mm_slli_si128(x,8);//右移i个s盒
		tmp2=_mm_srli_si128(x,8);//左移8-i个S盒
		break;
	case 5:
		tmp1=_mm_slli_si128(x,10);//右移i个s盒
		tmp2=_mm_srli_si128(x,6);//左移8-i个S盒
		break;
	case 6:
		tmp1=_mm_slli_si128(x,12);//右移i个s盒
		tmp2=_mm_srli_si128(x,4);//左移8-i个S盒
		break;
	case 7:
		tmp1=_mm_slli_si128(x,14);//右移i个s盒
		tmp2=_mm_srli_si128(x,2);//左移8-i个S盒
		break;
	}
	return _mm_xor_si128(tmp1,tmp2);
}

bool isValid(__m128i tmp){
	__m128i mask;
	mask=_mm_set1_epi16(0x0003);
	
	__m128i tmp0,tmp1,tmp2,tmp3,tmpCmp,tmpRes,Result;

	//返回一个__m128i的寄存器，将寄存器_A中的8个16bit整数按照_Count进行相同的逻辑右移，
	//移位填充值为0,r0=srl(_A0, _Count), r1=srl(_A1, _Count), ... r7=srl(_A7, _Count), 
	//shifting in zeros
	tmp0=_mm_srli_epi16(tmp, 4);// ||**** → 0000||

	//返回一个__m128i的寄存器，r=srl(_A, _Imm * 8),   _Imm must be an immediate,  
	//shifting in zeros
	//print128(tmp);
	//print128(tmp0);
	//对于这里si128的字节移位，是方向反过来的。
	tmp1=_mm_srli_si128(tmp0, 2);//print128(tmp1);
	tmp2=_mm_slli_si128(tmp0, 14);//print128(tmp2);
	tmp3=_mm_xor_si128(tmp1,tmp2);//print128(tmp3);
	
	tmpCmp=_mm_xor_si128(tmp,tmp3);//print128(tmpCmp);
	tmpRes=_mm_and_si128(tmpCmp,mask);
	
	u64 *p = (u64 *) &tmpRes;
	return (p[0]==0&&p[1]==0);
}

void setAndPrint(){
	//Bnc[Round-1]=r_pr[Round-1];
	
	for(int r=0;r<Round;r++){
		for(int si=0;si<SBOX_NUMBER;si++){
			fprintf(fp_trails,"%02x ",r_od_l[r][si]);
		}fprintf(fp_trails,"\t");
		for(int si=0;si<SBOX_NUMBER;si++){
			fprintf(fp_trails,"%02x ",r_od_r[r][si]);
		}
		fprintf(fp_trails,"\t%f",r_pr[r]);
		fprintf(fp_trails,"\n");
	}
	

	fprintf(fp_trails,"---------------\n");
}

void getInfo(int r,__m128i tmp){
	__m128i a;
	__m128i b;
	__m128i c;
	__m128i d;
	int lm;
	int la;

	//获得r_an[r]
	a=_mm_setzero_si128();
	a=_mm_cmpgt_epi16(tmp,a);//tmp非零的16比特置为0xffff
	lm=_mm_movemask_epi8(a);
	la=_mm_popcnt_u32(lm)/2;
	r_an[r]=la;

	//获得r_ai[r][]
	c=_mm_load_si128((__m128i *)(&(W16v[lm][0])));
	_mm_storeu_si128((__m128i *)(r_ai[r]),c);
}

void Round__(int r,int j,prType pr_round,__m128i tmp0){
	si8 an=r_an[r-1];
	si16 ai=r_ai[r-1][j];//第j个活跃S盒下标
	si8 an_remain=an-j-1;
	prType prob;
	si8 s;
	si8 m;
	u16 idv=r_od_l[r-1][ai];
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(r_od_r[r-1]);

	for(si16 pr=0;pr<PR_NUMBER;pr++){//遍历概率
		prob=Prob[pr]+pr_round;
		if((prob+r_pr[r-2]+Bn[Round-r-1]+WMIN_S*an_remain)<(Bnc[Round-1]+1e-10)){
			s=PDT_0_Offset[ai][idv][pr][0];
			m=PDT_0_Number[ai][idv][pr];
			for(si16 k=s;k<s+m;k++){//遍历输出差分
				tmp1=_mm_xor_si128(tmp0,*(__m128i *)(iSiEiP[ai][idv][k]));
				if(an_remain==0){
					r_pr[r-1]=r_pr[r-2]+prob;
					_mm_store_si128(odp,tmp1);
					Round_(r+1);
				}else{
					Round__(r,j+1,prob,tmp1);
				}
			}
		}else break;
	}
}

void Round_N_(int j,prType pr_round){
	si8 an=r_an[Round-1];
	si16 ai=r_ai[Round-1][j];//第j个活跃S盒下标
	si8 an_remain=an-j-1;
	prType prob;
	u16 idv=r_od_l[Round-1][ai];

	prob=PDT_MaxProb[ai][idv]+pr_round;
	if((prob+r_pr[Round-2]+WMIN_S*an_remain)<(Bnc[Round-1]+1e-10)){
		if(an_remain==0){
			r_pr[Round-1]=r_pr[Round-2]+prob;
			setAndPrint();
		}else{
			Round_N_(j+1,prob);
		}
	}
}

void Round_(int r){
	__m128i *dp1;
	__m128i *dp2;
	__m128i *idp;
	__m128i *odp;
	dp1=(__m128i *)(r_od_l[r-3]);
	dp2=(__m128i *)(r_od_r[r-2]);
	idp=(__m128i *)(r_od_l[r-1]);
	odp=(__m128i *)(r_od_r[r-1]);

	__m128i tmp0;
	__m128i tmp1;
	tmp0=_mm_xor_si128(*dp1,*dp2);
	tmp1=_mm_setzero_si128();
	_mm_store_si128(idp,tmp0);
	getInfo(r-1,*idp);
	
	
	if(r_an[r-1]==0){
		r_pr[r-1]=r_pr[r-2];
		_mm_store_si128(odp,tmp1);
		if(r==Round){setAndPrint();}
		else{Round_(r+1);}
	}else{
		if((r_pr[r-2]+Bn[Round-r-1]+WMIN_S*r_an[r-1])<(Bnc[Round-1]+1e-10)){}
		else return;
	}

	if(r==Round){
		Round_N_(0,0);//1是起始活跃S盒，0是起始概率
	}else{
		Round__(r,0,0,tmp1);//r是轮数，0是起始活跃S盒，0是起始概率，tmp1是起始输出差分
	}
}

void Round_2_(int j,int a_pre,prType pr_round,__m128i tmp0){
	prType prob;
	int a_cur;
	int odv_num;
	u16 odv;
	int s;
	int m;
	__m128i tmp1;
	__m128i *odp;
	odp=(__m128i *)(r_od_r[1]);

	for(a_cur=a_pre+1;a_cur<8;a_cur++){
		memset(r_od_l[1]+a_pre+1,0,sizeof(u16)*(a_cur-a_pre-1));
 
		for(si16 pr=0;pr<PR_NUMBER;pr++){//遍历概率
			prob=Prob[pr]+pr_round;
			if((prob+r_pr[0]+Bn[Round-3])<(Bnc[Round-1]+1e-10)){
				odv_num=PDT_1_Non0Num[a_cur][pr];
				for(si16 i=0;i<odv_num;i++){//遍历输出差分
					odv=PDT_1_Non0Val[a_cur][pr][i];
					r_od_l[1][a_cur]=odv;
					s=PDT_0_Offset[a_cur][odv][pr][0];
					m=PDT_0_Number[a_cur][odv][pr];
					for(si16 k=s;k<s+m;k++){//遍历输入差分
						tmp1=_mm_xor_si128(tmp0,*(__m128i *)(iSiEiP[a_cur][odv][k]));
						_mm_store_si128(odp,tmp1);
						r_pr[1]=prob+r_pr[0];
						memset(r_od_l[1]+a_cur+1,0,sizeof(u16)*(7-a_cur));
						Round_(3);//setAndPrint();
						Round_2_(j+1,a_cur,prob,tmp1);
					}
				}
			}else break;
		}
	}
}

void Round_2(){
	__m128i tmp;
	tmp=_mm_setzero_si128();
	if(firstRoundActive==true){
		__m128i *idp;
		__m128i *odp;
		idp=(__m128i *)(r_od_l[1]);
		odp=(__m128i *)(r_od_r[1]);
		_mm_store_si128(idp,tmp);
		_mm_store_si128(odp,tmp);
		r_pr[1]=r_pr[0];
		Round_(3);
		//setAndPrint();
	}
	Round_2_(1,-1,0,tmp);
}

void Round_1_(int j,int a_pre,prType pr_round){
	prType prob;
	int a_cur;
	u16 dy;

	for(a_cur=a_pre+1;a_cur<8;a_cur++){
		memset(r_od_l[0]+a_pre+1,0,sizeof(u16)*(a_cur-a_pre-1));
		
		for(si16 i=0;i<SBOX_OUTPUTS_NUMBER-1;i++){
			dy=outputForTravel[a_cur][i];
			r_od_l[0][a_cur]=dy;
			prob=PDT_MaxProb[a_cur][dy]+pr_round;
			if((prob+Bn[Round-2])<(Bnc[Round-1]+1e-10)){
				r_pr[0]=prob;
				memset(r_od_l[0]+a_cur+1,0,sizeof(u16)*(7-a_cur));
				Round_2();
				Round_1_(j+1,a_cur,prob);
			}else break;
		}
	}
}

void Round_1(){
	fn_trails_str=to_string((_ULonglong)Round)+"_trails.txt";
	errno_t err;
	err = fopen_s(&fp_trails, fn_trails_str.c_str(), "w" );

	__m128i *idp;
	__m128i tmp;
	tmp=_mm_setzero_si128();
	idp=(__m128i *)(r_od_l[0]);
	_mm_store_si128(idp,tmp);
	r_pr[0]=0;
	firstRoundActive=false;
	Round_2();
	firstRoundActive=true;
	Round_1_(1,-1,0);

	fclose(fp_trails);
}


		