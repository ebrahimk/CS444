#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;
const unsigned long long size=32*1024ULL;	//filesize of around 256MB
unsigned long long a[size];

int main(){
	pid_t pid=fork();
	if(pid==0){
		printf("Child 1 Created\n");
		FILE* pFile;
		pFile=fopen("IO1.tmp","wb");
		for(unsigned long long j=0; j<1024; ++j){
			fwrite(a,1,size*sizeof(unsigned long long), pFile);
		}
		fclose(pFile);
		printf("Child 1 Complete\n");
		return 0;
	}
	pid=fork();
	if(pid==0){
		printf("Child 2 Created\n");
		FILE* pFile;
		pFile=fopen("IO2.tmp","wb");
		for(unsigned long long j=0; j<1024; ++j){
			fwrite(a,1,size*sizeof(unsigned long long), pFile);
		}
		fclose(pFile);
		printf("Child 2 Complete\n");
		return 0;
	}
	pid=fork();
	if(pid==0){
		printf("Child 3 Created\n");
		FILE* pFile;
		pFile=fopen("IO3.tmp","wb");
		for(unsigned long long j=0; j<1024; ++j){
			fwrite(a,1,size*sizeof(unsigned long long), pFile);
		}
		fclose(pFile);
		printf("Child 3 Complete\n");
		return 0;
	}
	pid=fork();
	if(pid==0){
		printf("Child 4 Created\n");
		FILE* pFile;
		pFile=fopen("IO4.tmp","wb");
		for(unsigned long long j=0; j<1024; ++j){
			fwrite(a,1,size*sizeof(unsigned long long), pFile);
		}
		fclose(pFile);
		printf("Child 4 Complete\n");
		return 0;
	}
	return 0;
}
