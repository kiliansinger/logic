#include <string.h>//for memset
#include <stdio.h>//printf sprintf getchar
#include <math.h>
#include "kstring.h"
#include "compiler.h"
#include "interpreter.h"


int main(int argc, char* argv[]) {//the stack points to the return address
	setbuf(stdout, NULL);
#ifdef generatesortedtokenlist
	sorttokenlist();
	while(1);
#else
gentokenlookup();
#endif
//	PRINTINT_p();
	printf("sizeof ptrdiff %d",sizeof(ptrdiff));
	printf("sizeof int %d",sizeof(int));

	while (1) {
		char ss0[] = "\n\
	void main() {\n\
			int i[2];\n\
			i[1]=2;\n\
			print(++i[1]);print(i[1]);\n\
	}\n\
	";
		char ss[] = "\n\
	void main() {\n\
			int a = 42; \n\
		int b = 24; \n\
		char* c; \n\
		c = \"hello\"; \n\
		\n\
		print(a); \n\
		print('a');//we have problem with char in 64 bit mode because we print it too many times\n\
		print(b); \n\
		print('\n'); \n\
		print(0 ? 0 ? 3 : 4 : 3); \n\
		print(1 ? 0 ? 3 : 4 : 3); \n\
		print(0 ? 1 ? 3 : 4 : 3); \n\
		print(1 ? 1 ? 3 : 4 : 3); \n\
		print(0 ? 3 : 1 ? 4 : 5); \n\
		print(1 ? 3 : 0 ? 4 : 5); \n\
		print(1 ? 3 : 1 ? 4 : 5); \n\
		print(0 ? 3 : 0 ? 4 : 5); \n\
		print('\n');\n\
	}\n\
	";
		//char ss[] = "\n\
			//struct A;\n\
			struct A{\n\
				int f;\n\
			};\n\
			struct B{\n\
				char c;\n\
				int d;\n\
				struct C{\n\
					int iii;\n\
					int iiii;//works now\n\
					struct A aaa;\n\
				} cc;\n\
				double ddd;\n\
			};\n\
			void main() {\n\
				int i=3;\n\
				int ii[5];\n\
				////struct B b;\n\
				////struct B *bp;\n\
////struct C c;\n\
////struct A a;\n\
				////bp=&b;\n\
				//b.d+=1;\n\
				//b.c='a';\n\
				//b.d++;\n\
				//b.c++;\n\
				//b.cc.iii=2;\n\
				////(*bp).cc.iii=99;\n\
				//b.cc.iii+=9;\n\
				//b.cc.aaa.f=1;//We do not have float double support for setting values 1.2 it somehow affects b.cc.iii\n\
				//++b.cc.aaa.f;\n\
				//print(b.cc.aaa.f);\n\
				//print(' ');\n\
				////print(b.cc.iii);\n\
				//print(' ');\n\
				//print(b.c);\n\
				//print(' ');\n\
				//print(b.d++);print(' ');print(b.d);print(' ');\n\
				//print(& (b.d));print(' ');\n\
				//print(& b);print(' ');\n\
			}\n\
		";


		char c;
		int m = 99, n = 99;


		info i;

		pos p;
	
		KStr hallo = { 0,"hallo" };//char equivalent
		printS(hallo);//hallo

		char* s[] = { S("hallo"),S(" wie gehts uns?"),S("kilix"),0 };//concat string
		KStr hallo2 = { s,*s+2 };//llo wie
		printS(hallo2);
		printf("invalid");
		KStr hallo3 = { s,s[2] + 3 };//invalid
		printf("valid");
		printS(hallo3);
		KStr hallo4 = { &s[2],s[2] + 3 };//valid
		printS(hallo4);
		printf("\n****************\n");
		char* str[] = { S("int"),S("float"),0 };
		KStrRange rr = { {str,str[0]},{&str[2],str[2]} };
		printR(rr); printf("X");
		KStrRange rr2 = { {&str[2],str[2]},{&str[3],str[3]} };
		printR(rr2); printf("X");
		//while(1);

		//here we set the mode
		i.mode = kvm;//printit
#ifdef DODEBUG
		i.debugmode = 1;
#else
		i.debugmode = 0;
#endif
		inittables(&i, ss, stack, &p);

		printf("\n");

		if (translation_unit(&p)) {
			printf(" ok\n");
			run(i.mode, i.debugmode);
		}
		else {
			//char errtoken[100];
			//convert2cstring(i.errtoken,i.errtokenlen,errtoken,sizeof(errtoken));
			printf("\n");

			printS(p.k);

			printf("%s ", i.err);
			printR(i.krerrtoken);
			printf(" POS");
			printS(i.kerrpos);
		}
		printf("Press a key!\n");
		c = getchar();
		return 0;
	}
}
/*
// asmtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream"
#include "windows.h"
using namespace std;
#define MOV(a,b) _asm _emit (a>>8)&0x00 _asm _emit b&0xff

#define mov_eax_(b) 

inline void NOP(){
	_asm _emit 0x90
}

#define aa bb
#undef aa
#define aa cc

int _tmain(int argc, _TCHAR* argv[])
{
goto jump3;
jump:	_asm ret;

jump2:	NOP();
jump3:
void *p;
_asm {

	lea eax,jump
	mov [p],eax
	
}
unsigned char *code= (unsigned char*)VirtualAlloc(NULL, 3, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

code[0]=144;
code[1]=144;
code[2]=195;
//[]={144,144,195};

//cout <<(int)((unsigned char*)(p))[0]<<endl;
//((unsigned char*)(p))[0]=0;

_asm call code 
	return 0;
}

	goto xx;
	for( m=0;m<10;m++){
	int uuu=332;
		for(n=0;n<3;n++){
			xx:
			printf("%d %d %d\n",m,n,uuu);
			switch(n){
				int i=0;
				i++;
			}
		}
	}
	
*/
