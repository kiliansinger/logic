#include <string.h>//for memset
#include <stdio.h>//printf sprintf getchar
#include <math.h>
#include "kstring.h"
#include "commondefs.h"
#include "compiler.h"
#include "interpreter.h"


void *retvoid;
int retint;
float retfloat;
double retdouble;
char retchar;

//return values and type
int doexecuteloop;

Mode mode;
memptr PC;//Program Counter

mem stack[STACKSIZE];//Stack
int sizeoflastpushed;
memptr stackend={stack+STACKSIZE};
memptr SPGLOB;//end of global stack points to last valid entry in the global SP
memptr SP;//Stackpointer
memptr BP;//Basepointer points to first stackvariable after return adress
//BIG functions can be called from interpreter
//small functions are for internal use
void printfid(KStr begin,KStr end){
	KStrRange r={begin,end};
	printR(r);
	//for(;getS(&begin),cmpS(begin,end)<0;++begin.c){
	//	putchar(*begin.c);	
	//}
}
void printfid2(){//if in debugmode, the compiler can put identifier strings as KStr on the stack. These are then popped and printed during vm execution
	KStr begin,end;

	begin.s=(char**)PC.Pu->Str;
	PC.ptr+=SIZEOFPTR;
	begin.c=PC.Pu->Str;
	PC.ptr+=SIZEOFPTR;
	end.s=(char**)PC.Pu->Str;
	PC.ptr+=SIZEOFPTR;
	end.c=PC.Pu->Str;
	PC.ptr+=SIZEOFPTR;
	printfid(begin,end);
	
}
int ENDPROG=0;//inteans for what to do after return
int RETURN_=0;//inteans for what to do after return
int RETURN_VAL_=0;
void initinterpreter(){
	ENDPROG=0;//inteans for what to do after return
	RETURN_=0;//inteans for what to do after return
	RETURN_VAL_=0;
	doexecuteloop=1;
}
void do_ret(){//perform a return
	SP.ptr=BP.ptr;
	PC.ptr=SP.Pu->Str;
	SP.ptr+=SIZEOFPTR;
	BP.ptr=SP.Pu->Str;
	SP.ptr+=SIZEOFPTR;
}





int execute(Mode m,int debugmode){
	mode=m;
	RETURN_VAL_=0;
	doexecuteloop=1;
	if(debugmode) while(doexecuteloop)
#ifdef BIG
		mnemonic[printit][PC.Pu->Voidfn]();
#else
		mnemonic[m][PC.Pu->Voidfn]();
#endif
	else while(doexecuteloop)
		mnemonic[m][PC.Pu->Voidfn]();
	if(ENDPROG) return -1;
	return RETURN_VAL_;
}
void run(Mode m, int debugmode){
	initinterpreter();
	BP.ptr=stackend.ptr;
	SP.ptr=stackend.ptr;
	PC.ptr=stack;
	SPGLOB=SP;
	
	execute(m,debugmode);//execute global initializers
	SPGLOB=SP;
	BP=SP;
	execute(m,debugmode);
}