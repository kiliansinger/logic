#ifndef COMPILER_H
#define COMPILER_H
#include "commondefs.h"
#define ENUM
enum OPCODE {
#include "kvm.h"
OPCODES
};
#undef ENUM
extern void (*mnemonic[modes][OPCODES+1])();
//typedef enum TYPEENUM {NOCHANGE=0,VOID	,POINTER,		INT		,FLOAT		,DOUBLE			,CHAR	} TYPEENUM;
enum Lifetime { DUMP, SCOPE, FUNC, GLOBANONYM, GLOBALLIFE };//SCOPE:local {}, FUNC: inside function, GLOBALLIFE: global lifetime, globanonym: global lifetime but not accessible such as local static variables

//attention when inserting type also adjust the lifttime table
//the type of identifier: function,forward declaration to function,position where forward declaration is used and needs to be patched once the real function is compiled, global variable, localvariable, static variable,  identinfo local base pointer, jump label, goto break and continue where a unknown label is mentioned and needs to be patched once the label is found,structvar: struct member,unionvar: unionmember, structT: structTypename
//!!TODO add localstruct, globalstruct, localstructT,globalstructT, localtypedef, globaltypedf,
//also struct forward, local global
//and also adjust liftime

#define IDENTTYPES_ENUM
typedef enum Ident {
#include "identtypes.h"
} Ident;
#undef IDENTTYPES_ENUM

	//maybe we should not use struct TYPEDEF but rather use ISP
//Type is part of IDentinfoNormal
//but maybe then we should get rid of other type and instead point directly to ISP
//the idea is that all types become similar, basic types, typedefs and structs
//sizeof should be part of the type system
// struct TYPEDEF {//this should also contain const info and value
//	TYPEENUM t;
//	//int SIZEOF;
//}TYPEDEF;
struct IdentinfoNormal;
typedef struct TYPE{
	//Ident t;//TYPEENUM !!!to be replaced by the td
	//TYPEDEF* td;
	struct IdentinfoNormal* t;
	int ptrcnt;									//counts how many int ***a; stars we use for pointers
}TYPE;

union Identinfo;
typedef struct IdentinfoParent{
	int mysize;	//size of element at current position in ISP
	int nextsize; //size of element next element which will be located at ISP-nextsize
} IdentinfoParent;
struct IdentinfoNormal;
typedef struct IdentinfoNormal//attention only add to the end, because otherwise the initialization in compiler.c fail
#ifdef __cplusplus
	:public IdentinfoParent{
#else
{IdentinfoParent;
#endif
	//datastructure for hashmap
	KStrRange kridentstr;					//the name of identifier
	enum Ident identtype;			//type of identifier
	u identptr;						//we use u.ptr to store position on stack (either offset to BP ) or global address for global variables but it is also used to store address of function forward patches and PC address for goto labels
	union Identinfo *next;			//list of identifier with same hash
	TYPE type;	// DOUBLE, FLOAT ,INT,VOID also returntype
	struct IdentinfoNormal* parent;
	ptrdiff offset;
} IdentinfoNormal;

typedef struct IdentinfoGoto
#ifdef __cplusplus
:public IdentinfoNormal{
#else
{IdentinfoNormal;
#endif
	ptrdiff SPvalforgoto;			//there are only local gotos thus store: p->ISP->stackforgoto.ptrd=(p->SP.ptr-p->BP.ptr); 
} IdentinfoGoto;


typedef struct CompileTypeStack
#ifdef __cplusplus
:public IdentinfoParent{
#else
{IdentinfoParent;
#endif
	//type used for accessing last push such that typecast can be put before, we use the Identinfo ISP stack and push the compileTypeStack there
	//so the way it works is that with each emitPush we also push the type on the ISP, such that later we can do proper type casting.
	//it is a bit like doing the real calculation but only keeping the type
	//!!!todothese calculations could be extended to treat calculations with constant numbers
	//to optimize
	memptr typecastPC;				//address where to put the typecasting operations
	TYPE type;						//the type at the position to be patched
	int size;						//the size of the first operation which should be overwritten
} CompileTypeStack;

typedef union Identinfo
{
	IdentinfoNormal n;
	IdentinfoGoto g;
	CompileTypeStack c;
} Identinfo;

#define hashsize 4096								//hashmap size
struct info;
struct pos;
typedef struct pos{					//contains all position dependent informations which might need backtracking
	int getptremited;				//GETLPTR or GETGPTR emited
	int allowingdollar;				//if true then identifier can also contain $ (used in emitexpression)
	struct pos *parent;				//points to the pos structure of previous call
	//char *c;						//points to current character
	KStr k;
	struct info *i;					//this is the global datastructure for the compiler
	int neednewlookup;				//when low level parsers have recognized something we set neednewlookup to true
	//SP and BP are used to simulate the SP and BP for the kvm during compilation such that correct address offsets are calculated
	memptr SP;						//this stores the stackpointer for the kvm during compilation
	memptr BP;						//this stores the basepointer for the kvm during compilation
	//ISP is the identifier Stack Pointer, which stores the identifier information
	Identinfo *ISP;					//identifierStackPointer
	//IBP the identifier Stack Base Pointer is used as stack frame for the identifier stack
	Identinfo *IBP;					//identifierBasePointer
	Identinfo *ILBP;				//local Basepointer for local contexts which are inside { }
	Identinfo *functionIdentinfo;	//identifierStackPointer for parameters
	memptr PC;						//the programm pointer where mnemonic is emitted
	memptr constintpatchpos;		//the position where we should patch an integer. Is set whenever emitint is called.
	KStr knext;					//pointer to next character
	const char *token;				//pointer to recognized token
	memptr lastOP_PC;				//last op emited
	memptr lastOP_PC2;				//last last op emited
}pos;
typedef enum BREKCONT { BREK, CONT, GOBREK, GOCONT } BREKCONT;
typedef struct info{				//stores global data for compiler
	char *kstr[1000];
	Mode mode;						//i386 print kvm
	int debugmode;					//if set we should output additional debug information in the kvm
	char *err;						//error message
	int errprio;					//priority of error, this is used to get resonable error messages from recursive calls
	KStr kerrpos;					//position in code
	KStrRange krerrtoken;			//pointer to errortoken
	int in_function_definition;		//we are parsing a function definition
	int in_function_body;			//we are inside function body
	int fnforward;					//a forward definition is parsed
//the following variables are used in declarations
	TYPE type;						//stores the current type
	TYPE decltype2;					//stores the type in a declaration
	int paramcnt;					//stores the amount of paramters in a function
	TYPE fnrettype;					//used to get rettype during function definition
	int global;						//is 1 when we are in global scope
	int firstCBraOpenInFn;			//is 1 when we have identified the first { in a function
	Identinfo *hashmap[hashsize];	//new elements are inserted at beginning in list, bra and ket of while,... are not inserted in hashmap but only on ISP
	u* currentcall;					//if parser has identified a function call such as fn(1); currentcall stores the entry address to emit such as emit(p,CALL);emotptr(p,currentcall);
	u* currentvar;					//currentvariable where emit(p,GETX) is performed where X is either L or G
	TYPE currenttype;				//currenttype of primary expression
	int currentvarglobal;			//1 if currentvariable where emit(p,GETG) is performed 
	int forwhiledoswitchlevel;
	char** brekcont;
} info;
int translation_unit(pos *p);
void inittables(info *i,char *s,mem *stack,pos *p);
void gentokenlookup();
#endif