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
  FILE* pFile5;
  FILE* pFile6;
  	FILE*  pFile7;
    FILE* pFile8;
    FILE* pFile9;
    FILE* pFile10;



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

        pFile4=fopen("IOoutput4.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile4);
        }
        fclose(pFile4);

        pFile5=fopen("IOoutput5.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile5);
        }
        fclose(pFile5);

        pFile6=fopen("IOoutput6.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile6);
        }
        fclose(pFile6);

        pFile7=fopen("IOoutput7.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile7);
        }
        fclose(pFile7);

        pFile8=fopen("IOoutput8.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile8);
        }
        fclose(pFile8);

        pFile6=fopen("IOoutput9.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile9);
        }
        fclose(pFile9);

        pFile10=fopen("IOoutput10.tmp","wb");
        for(unsigned long long j=0; j<128; ++j){
                fwrite(a,1,size*sizeof(unsigned long long), pFile10);
        }
        fclose(pFile10);

return 0;
}
