#include <bits/stdc++.h>
using namespace std;
const unsigned long long size=128ULL*128ULL;
unsigned long long a[size];

int main(){
        FILE* pFile;
	FILE* pFile1;
	FILE* pFile2;
	FILE*  pFile3;
	FILE*  pFile4;


for(int i = 0; i< 10; i++){
        pFile=fopen("IOoutput.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile);
        }
        fclose(pFile);
       
	pFile1=fopen("IOoutput1.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile1);
        }
        fclose(pFile1);
	
        pFile2=fopen("IOoutput2.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile2);
        }
      fclose(pFile2);

        pFile3=fopen("IOoutput3.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile3);
        }
        fclose(pFile3);
	
        pFile4=fopen("IOoutput.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile4);
        }
        fclose(pFile4);

        }
return 0;
}
