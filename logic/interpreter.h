#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "commondefs.h"
#include "kstring.h"
extern memptr PC;//Program Counter
extern memptr SP;//Stackpointer
extern memptr BP;//Basepointer points to first stackvariable after return adress
extern int RETURN_VAL_;
extern int doexecuteloop;
extern mem stack[];//Stack
extern memptr stackend;
void printfid(KStr begin,KStr end);
void printfid2();
void do_ret();
void run(Mode m, int debugmode);
#define STACKSIZE 10000
#endif