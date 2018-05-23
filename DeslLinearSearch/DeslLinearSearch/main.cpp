#include "types.h"
#include "GenPermTable.h"
#include "GenPrTable.h"
#include "search.h"

int main(){
	Gen_iEiP_Table();
	GenPrTable();
	clock_t start,end;

	
	Bn[0]=0;
	Bn[1]=0.678072;
	Bn[2]=1.356144;
	Bn[3]=3.034215;
	Bn[4]=4.712287;

	ALIGNED_TYPE_(u16,8) r_tmp[21][8];
	__m128i *id;
	__m128i temp;
	id=(__m128i *)(r_tmp[0]);
	temp=_mm_setzero_si128();
	_mm_store_si128(id,temp);
	//printf("%02x",r_tmp[20][7]);

	/*Round=3;
	Bnc[Round-1]=(int)Bn[Round-2]+3;
	start=clock();
	Round_1();
	end = clock();
	printf("%d&%f\n",Round,(double)(end-start)/CLK_TCK);*/

	for(Round=3;Round<=20;Round++){
		Bnc[Round-1]=(int)Bn[Round-2]+4;

		start=clock();
		Round_1();
		end = clock();

		Bn[Round-1]=Bnc[Round-1];
		printf("%d&%f&%f\n",Round,Bn[Round-1],(double)(end-start)/CLK_TCK);
	}
	system("pause");
	return 0;
}