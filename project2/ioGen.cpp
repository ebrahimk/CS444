#include <bits/stdc++.h>
using namespace std;
const unsigned long long size=1024ULL*1024ULL;
unsigned long long a[size];

int main(){
	FILE* pFile;
	pFile=fopen("IOoutput.tmp","wb");
	for(unsigned long long j=0; j<1024; ++j){
		fwrite(a,1,size*sizeof(unsigned long long), pFile);
	}
	fclose(pFile);
	return 0;
}
