//search for n.identptr.Pu to find problems with ptrdiff in emitset

//we should handle struct by emitting A$membervar
//we have to use new IdentInfo structs with storage of char *pastebin[]
//which we use to construct kstrings
//!!!!!TODO debug 64 mode crashes 
//unimplemented:
//switch
//local static variables (use varstatic)
//string literals
//structs
//modulo
//typecast (int)....

//we should put the string to the source code string  behind stackend for interpreter
//then for execution we can put stackend at the end of code to have maximal memory
//and kill source
//assuming we safed it

//roadmap toward struct
//first we need to have global variables of standard types instead of enums of type
//they do not posses ptrcnt
//so TYPE is modified to replace the enum by a ptr to a type
//this type then has size information
//the enum type
//and points if necessary to a struct members.
//they live on isp with global lifetime
//oh we can have local structs and typedefs!
//also same name of structs and e.g int type is possible:
/*struct A {
	int a;
};
// typedef struct A A;
struct A b;
int A;
struct A a;
a.a = 7;
printf("Hello World %d\n", a.a);
return 0;
*/
//but not of typenames:
/*
struct A {
	int a;
};
 typedef struct A A;
struct A b;
int A;//error!
struct A a;
a.a = 7;
printf("Hello World %d\n", a.a);
return 0;
*/

//but
/*
struct A {
	int a;
};

struct A A;
*/
//is not ok
/*
#include <stdio.h>
struct A{
	  int a;
  };
  typedef struct A A;//ok
int main(void) {
  struct A{
	  int a;
  };
  //typedef struct A A;
  struct A b;

  struct A a;
   int A;//ok
  a.a=7;
  printf("Hello World %d\n",a.a);
  return 0;
}
*/
//is ok
//but
/*
#include <stdio.h>
struct A {
	int a;
};
typedef struct A A;
int main(void) {
	struct A {
		int a;
	};
	typedef struct A A;//error
	struct A b;

	struct A a;
	int A;//error
	a.a = 7;
	printf("Hello World %d\n", a.a);
	return 0;
}
*/
//we should als add const int ...
//and for those variables we should not emit getl or getg but const int ...
//this will also help for constant calculations on stack. which we can execute right away
//that means the type should also have a const field and value field
#include <string.h>//for memset
#include <stdio.h>//printf sprintf getchar
#include <math.h>
#include "kstring.h"
#include "compiler.h"
#include "interpreter.h"
//int SIZEOF[]=     {0         ,0		,SIZEOFPTR	,	SIZEOFINT	,SIZEOFFLOAT,SIZEOFDOUBLE	,SIZEOFCHAR};
#define FN
#include "kvm.h"
#ifdef BIG
#include "printit.h"
#endif
#undef FN

#define INIT
void (*mnemonic[modes][OPCODES+1])()={{
#include "kvm.h"
#ifdef BIG
0},{
#include "printit.h"
#endif
0}};
#undef INIT
//here we store length of mneomonics this is currently not needed but we keep the code for future use
#define LEN
int mnelen[modes][OPCODES+1]={{
#include "kvm.h"
#ifdef BIG
0},{
#include "printit.h"
#endif
0}};
#undef LEN

char* itoa(int i, char b[],char b2[]) {
	char const digit[] = "0123456789";
	char* p = b;
	char* p2;


	if (i < 0) {
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do { //Move to where representation ends
		++p;
		shifter = shifter / 10;
	} while (shifter);
	p2 = p;
	do { //Move back, inserting digits as u go
		*--p = digit[i % 10];
		i = i / 10;
	} while (i);
	while (*b2) {
		*p2 = *b2;
		++p2;
		++b2;
	}
	*p2 = '\0';
	return b;
}

//memptr PC;//Program Counter
//identinfo for basic types
//VOID	,POINTER,		INT		,FLOAT		,DOUBLE			,CHAR	
const char * basictypeident[] = { S("void"),S("void*"),S("int"),S("float"),S("double"),S("CHAR") };
char* tempvarname[] = { S("$temp"),S("z") };


KStrRange tempvar;
//datastructure for hashmap
KStrRange kridentstr;					//the name of identifier
enum Ident identtype;			//type of identifier
u identptr;						//we use u.ptr to store position on stack (either offset to BP ) or global address for global variables but it is also used to store address of function forward patches and PC address for goto labels
union Identinfo* next;			//list of identifier with same hash
TYPE type;	// DOUBLE, FLOAT ,INT,VOID also returntype
struct IdentinfoNormal* parent;
ptrdiff offset;

IdentinfoNormal NOCHANGEt = {
   sizeof(IdentinfoNormal),//int mysize
   0,//int nextsize
   //{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
   {{0,0},{0,0}},
   NOCHANGE,//enum Ident identtype;
	0,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};
 IdentinfoNormal VOIDt = {
	sizeof(IdentinfoNormal),//int mysize
	0,//int nextsize
	//{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
	{{0,0},{0,0}},
	VOID,//enum Ident identtype;
	0,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};
 IdentinfoNormal POINTERt = {
	sizeof(IdentinfoNormal),//int mysize
	0,//int nextsize
	//{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
	{{0,0},{0,0}},
	POINTER,//enum Ident identtype;
	SIZEOFPTR,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};
 IdentinfoNormal INTt = {
	sizeof(IdentinfoNormal),//int mysize
	0,//int nextsize
	//{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
	{{0,0},{0,0}},
	INT,//enum Ident identtype;
	SIZEOFINT,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};
 IdentinfoNormal FLOATt = {
	sizeof(IdentinfoNormal),//int mysize
	0,//int nextsize
	//{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
	{{0,0},{0,0}},
	FLOAT,//enum Ident identtype;
	SIZEOFFLOAT,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};
 IdentinfoNormal DOUBLEt = {
	sizeof(IdentinfoNormal),//int mysize
	0,//int nextsize
	//{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
	{{0,0},{0,0}},
	DOUBLE,//enum Ident identtype;
	SIZEOFDOUBLE,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};
 IdentinfoNormal CHARt = {
	sizeof(IdentinfoNormal),//int mysize
	0,//int nextsize
	//{{&basictypeident[4],basictypeident[4]},{&basictypeident[6],basictypeident[6]} },//KStriRange kridentstr;
	{{0,0},{0,0}},
	CHAR,//enum Ident identtype;
	SIZEOFCHAR,//u.Size identptr
	0,//next
	{0,0},//ptrcnt
	0,//parent
	0,//offset
};


///starting with parser
///////////////////////
//tokens
const char *Typedef="typedef";
const char *Extern="extern";
const char *Static="static";
const char *Auto="auto";
const char *Register="register";
const char *Void="void";
const char *Char="char";
const char *Short="short";
const char *Int="int";
const char *Long="long";
const char *Float="float";
const char *Double="double";
const char *Signed="signed";
const char *Unsigned="unsigned";
const char *_int="_int";
const char *_COmplex="_Complex";
const char *CBraOpen="{";//Curly Bracket Open
const char *CBraClose="}";
const char *Struct="struct";
const char *Union="union";
const char *Const="const";
const char *Restrict="restrict";
const char *Volatile="volatile";
const char *Sizeof="sizeof";
const char *Enum="enum";
const char *Inline="inline";
const char *Case="case";
const char *Default="default";
const char *If="if";
const char *Else="else";
const char *Switch="switch";
const char *While="while";
const char *Do="do";
const char *For="for";
const char *Goto="goto";
const char *Continue="continue";
const char *Break="break";
const char *Return="return";
const char *BraOpen="(";// Bracket Open
const char *BraClose=")";
const char *SBraOpen="[";//Square Bracket Open
const char *SBraClose="]";
const char *Gt=">";
const char *Lt="<";
const char *Eq="=";
const char *DotDotDot="...";
const char *GtGtEq=">>=";
const char *LtLtEq="<<=";
const char *PlusEq="+=";
const char *MinusEq="-=";
const char *MulEq="*=";
const char *DivEq="/=";
const char *ModEq="%=";
const char *AndEq="&=";
const char *XorEq="^=";
const char *OrEq="|=";
const char *GtGt=">>";
const char *LtLt="<<";
const char *PlusPlus="++";
const char *MinusMinus="--";
const char *MinusGt="->";
const char *AndAnd="&&";
const char *OrOr="||";
const char *LtEq="<=";
const char *GtEq=">=";
const char *EqEq="==";
const char *NotEq="!=";
const char *Semicolon=";";
const char *Comma=",";
const char *Colon=":";
const char *Dot=".";
const char *And="&";
const char *Not="!";
const char *Tilde="~";
const char *Minus="-";
const char * Plus ="+";
const char *Mul="*";
const char *Div="/";
const char *Mod="%";
const char *Xor="^";
const char *Or="|";
const char *Question="?";
//sorted tokenlist define generatesortedtokenlist and add new elements at the end of both tokenlist and tokenliststring
const char **tokenlist0[]={&NotEq,&Not,&ModEq,&Mod,&AndAnd,&AndEq,&And,&BraOpen,&BraClose,&MulEq,&Mul,&PlusPlus,&PlusEq,&Plus,&Comma,&MinusMinus,&MinusEq,&MinusGt,&Minus,&DotDotDot,&Dot,&DivEq,&Div,&Colon,&Semicolon,&LtLtEq,&LtLt,&LtEq,&Lt,&EqEq,&Eq,&GtEq,&GtGtEq,&GtGt,&Gt,&Question,&SBraOpen,&SBraClose,&XorEq,&Xor,&_int,&_COmplex,&Auto,&Break,&Case,&Char,&Const,&Continue,&Default,&Double,&Do,&Else,&Enum,&Extern,&Float,&For,&Goto,&If,&Inline,&Int,&Long,&Register,&Restrict,&Return,&Short,&Signed,&Sizeof,&Static,&Struct,&Switch,&Typedef,&Union,&Unsigned,&Void,&Volatile,&While,&CBraOpen,&OrEq,&OrOr,&Or,&CBraClose,&Tilde};//we need to use &... because otherwise compiler complains that initializer is not constant
const char **tokenlist=(const char**)tokenlist0;
int tokenlookup[128-32];

//attention with emitlooplabel and emitexpression not use temporary string when an identinfo entry on isp is emitted.
//as these temporaries become invalid.

/*const char brek[10][11] = { "$break1:;","$break2:;","$break3:;","$break4:;","$break5:;","$break6:;","$break7:;","$break8:;","$break9:;","$break10:;" };
const char cont[10][14] = { "$continue1:;","$continue2:;","$continue3:;","$continue4:;","$continue5:;","$continue6:;","$continue7:;","$continue8:;","$continue9:;","$continue10:;" };
const char gobrek[10][16] = { "goto $break1;","goto $break2;","goto $break3;","goto $break4;","goto $break5;","goto $break6;","goto $break7;","goto $break8;","goto $break9;","goto $break10;" };
const char gocont[10][19] = { "goto $continue1;","goto $continue2;","goto $continue3;","goto $continue4;","goto $continue5;","goto $continue6;","goto $continue7;","goto $continue8;","goto $continue9;","goto $continue10;" };
*/

TYPE inttype = { &INTt,0 };

void gentokenlookup(){//!!! only call once globally otherwise we get SEH exception
	static int once = 0;
	if (once) return;
	once = 1;
	int i,n=sizeof(tokenlist0)/sizeof(const char**);
	for(i=0;i<n;++i) tokenlist[i]=*tokenlist0[i];//we make tokenlist a pointer to c strings
	for(i=0;i<128-32;++i) tokenlookup[i]=0;
	for(i=0;i<n;++i) {
		char c=(tokenlist[i][0]-32)&127;
		if(!tokenlookup[c]) tokenlookup[c]=i;
	}
	//now tokenlookup[(c-32)&127] points to the first element in tokenlist with the same starting letter
}
//#define generatesortedtokenlist

#ifdef generatesortedtokenlist
const char *tokenliststring[]={"NotEq","Not","ModEq","Mod","AndAnd","AndEq","And","BraOpen","BraClose","MulEq","Mul","PlusPlus","PlusEq","Plus","Comma","MinusMinus","MinusEq","MinusGt","Minus","DotDotDot","Dot","DivEq","Div","Colon","Semicolon","LtLtEq","LtLt","LtEq","Lt","EqEq","Eq","GtEq","GtGtEq","GtGt","Gt","Question","SBraOpen","SBraClose","XorEq","Xor","_int","_COmplex","Auto","Break","Case","Char","Const","Continue","Default","Double","Do","Else","Enum","Extern","Float","For","Goto","If","Inline","Int","Long","Register","Restrict","Return","Short","Signed","Sizeof","Static","Struct","Switch","Typedef","Union","Unsigned","Void","Volatile","While","CBraOpen","OrEq","OrOr","Or","CBraClose","Tilde"};//we need to use ... because otherwise compiler complains that initializer is not constant
int strcmp2(const char *s1,const char *s2){
	while(1){
		int v1=*s1;
		int v2=*s2;
		if(v1==0) v1=256;
		if(v2==0) v2=256;
		if(v1>v2) return 1;
		else if(v1<v2) return -1;
		else {
			if((*s1==0) && (*s2==0)) return 0;
			else if(*s1==0) return 1;
			else if(*s2==0) return -1;
			++s1;++s2;
		}
	}
}
void sorttokenlist(){
	int i,j,n=sizeof(tokenlist0)/sizeof(const char**);
	for(i=0;i<n;++i){
		int m=i;
		const char **p=tokenlist0[i];
		const char *p2=tokenliststring[i];

		for(j=n-1;j>i;--j){
			if(strcmp2(*tokenlist0[m],*tokenlist0[j])>0) {
				m=j;
			}
		}
		tokenlist0[i]=tokenlist0[m];
		tokenlist0[m]=p;

		tokenliststring[i]=tokenliststring[m];
		tokenliststring[m]=p2;


	}
	for(i=0;i<n;++i) printf("&%s,",tokenliststring[i]);
	printf("\n");
		for(i=0;i<n;++i) printf("\"%s\",",tokenliststring[i]);
		printf("\n\n\nignore\n");
	for(i=0;i<n;++i) printf("\"%s\",",*tokenlist0[i]);
}
#endif
int lifetime[] = {
#include "identtypes.h"
};
	
typedef enum parsetype {unknown,vardef,varset,varget,adrget,functionname,parameter,typedefinition,enumdef,structdef,uniondef,structdefforward,uniondefforward,looplabel} parsetype;//the type of the currently parsed identifier:variable definition, assignement of a value to variable, evaluation of variable,adress evaluation, function, parameter, not yet implemented: typedef, enum, struct, union,looplabel are $continue: $break labels for internal loop handling
typedef struct iddata{				//this type is used for recursive storage of identifier as local stack variables. We give over a pointer to all functions which might need to write or read the identifier
	KStrRange krstr;						//pointer to identifier string
	Identinfo* idinfo;				//pointer to Identifierinfo is set delayed after identifier is recognized when inserthash
	enum parsetype parsing;			//denotes the current type we are parsing
	Identinfo* structIdentifier;		//points to struct identifier struct A{int a;} so it points to A
	int struct1union0;				//1 for struct 0 for union
	memptr ptrpushed;
	memptr structunionSP;			//is used in struct defs to get the offset
}iddata;
void error(pos *p,char *err,KStr kerrpos,KStrRange krerrtoken,int prio){
	p->i->err=err;
	p->i->kerrpos=kerrpos;
	p->i->krerrtoken=krerrtoken;
	p->i->errprio=prio;
}
char zerostr=0;
char *emptystr[]={&zerostr,0};

KStr empty={emptystr,&zerostr};
KStrRange emptyr={{emptystr,&zerostr},{emptystr,&zerostr}};
Identinfo* pushISP(Identinfo *ISP,int size,int del){
	Identinfo* i=(Identinfo*)((char*)ISP-size);
	if (del) memset(i, 0, size);//solved this was important to solve SEH exception problems
	//but when used we have a problem with example in main the long one with mainx
	ISP->n.nextsize=size;
	i->n.mysize=size;
	return i;
}
Identinfo* incISP(Identinfo *ISP){
	Identinfo* i;
	i=(Identinfo*)((char*)ISP+ISP->n.mysize);
	return i;
}
Identinfo* multiIncISP(Identinfo *ISP,int cnt){
    int i;
	for(i=0;i<cnt;++i) ISP=incISP(ISP);
	return ISP;
}
Identinfo* decISP(Identinfo *ISP){
	return (Identinfo*)((char*)ISP- ISP->n.nextsize);
}
Identinfo* multiDecISP(Identinfo *ISP,int cnt){
    int i;
	for(i=0;i<cnt;++i) ISP=decISP(ISP);
	return ISP;
}
void inittables(info *i,char *s,mem *stack,pos *p){
	tempvar.begin.s = &tempvarname[0];
	tempvar.begin.c = tempvarname[0];
	tempvar.end.s = &tempvarname[2];
	tempvar.end.c = tempvarname[2];
	p->getptremited=0;
	p->allowingdollar=0;
	p->i=i;
	p->neednewlookup=1;
	i->kstr[0]=s;
	i->kstr[1]=0;
	p->k.s=i->kstr;
	p->k.c=*(p->k.s);
	error(p,0,p->k,emptyr,0);
	i->fnforward=0;
	i->in_function_definition = 0;
	i->in_function_body = 0;
	p->PC.ptr=stack;
	p->ISP=(Identinfo*)stackend.ptr;//i->identinfovarend;//Stack for Identinfo for variables
	--p->ISP;//!!! check should we not use decISP but it dies not know values
	memset(p->ISP, 0, sizeof(Identinfo));
	p->SP.ptr=stackend.ptr;
	p->BP.ptr=stackend.ptr;
	memset(i->hashmap,0,hashsize*sizeof(Identinfo *));
	i->forwhiledoswitchlevel = -1;
}
int hash(KStrRange rs){			//here we store all found identifier strings
	int n;
	int hash=0;
	if(cmpS(rs.begin,rs.end)==0) return 0;
	for(n=0;getS(&rs.begin),cmpS(rs.begin,rs.end)<0 ;++rs.begin.c,++n){
		if(n&1) hash^=(*rs.begin.c&63)<<6;
		else hash^=*rs.begin.c&63;
	}
	return hash;
}
int strcmpid(iddata *id,KStrRange r){
	KStr k;
	for(k=id->krstr.begin;getS(&k) && getS(&r.begin) && cmpS(k,id->krstr.end)<0 && cmpS(r.begin,r.end)<0 ;++k.c,++r.begin.c){
		if(*k.c!=*r.begin.c) return 1;
	}
	return !(getS(&k)==0 || cmpS(k,id->krstr.end)>=0) && (getS(&r.begin)==0 || cmpS(r.begin,r.end)>=0);
}
//!!!struct todo we should allow for parent class in search
/*
class A{
int a;
}
A b;
b.a=1

is resoved: searchhash b
we get class A;
then we should searchhash a but add indicator that parent must be A 

*/
Identinfo *searchhash(info *i,iddata *id,int cnt, Identinfo *parent){//returns the first found entry for cnt=0, or null if not found
	int h;
	Identinfo *idinfo;
	int cnt2;
	if(id->idinfo && cnt==0) return id->idinfo; //cashing
	if(getS(&id->krstr.begin)==0) {
		id->idinfo=0;
		return 0;
	}
	h=hash(id->krstr);
	idinfo=i->hashmap[h];
	cnt2=0;
	while(idinfo){
		if(strcmpid(id,idinfo->n.kridentstr)==0) {
			
			
			if (parent == 0 &&
				(idinfo->n.identtype != structmembervarglobal) &&
				(idinfo->n.identtype != structmembervarlocal) &&
				(idinfo->n.identtype != unionmembervarglobal) &&
				(idinfo->n.identtype != unionmembervarlocal)
				) {
				if (cnt2 == cnt) {
					id->idinfo = idinfo;
					return idinfo;//todo use last id->idinfo to continue so we use cnt as relative value
				}
				else ++cnt2;
			}
			if (parent && idinfo->n.parent==parent && (
				(idinfo->n.identtype == structmembervarglobal) ||
				(idinfo->n.identtype == structmembervarlocal) ||
				(idinfo->n.identtype == unionmembervarglobal) ||
				(idinfo->n.identtype == unionmembervarlocal))
				) {
				if (cnt2 == cnt) {
					id->idinfo = idinfo;
					return idinfo;//todo use last id->idinfo to continue so we use cnt as relative value
				}
				else ++cnt2;
			}
		}
		idinfo=idinfo->n.next;
	}
	id->idinfo=0;
	return 0;//not existing
}
void inserthash(info *i,Identinfo *id){//we insert element at top of list matching the hash
	int h=hash(id->n.kridentstr);
	if (i->hashmap[h] !=id) {
		id->n.next = i->hashmap[h];
		i->hashmap[h] = id;
	}
	//else {
	//	printf("doublehash\n");
	//}
}
int removehash(info *i,Identinfo *j){
	int h=hash(j->n.kridentstr);
	Identinfo *id=i->hashmap[h];
	Identinfo *prev=0;
	while(id){
		if(id==j){
			//id->n.type.ptrcnt=0;///!!!XXX!!! here we should not set ptrcnt to zero as we removehash for function paramters but we do not remove it from the isp
			if(!prev) i->hashmap[h]=id->n.next;
			else prev->n.next=id->n.next;
			return 1;
		}
		prev=id;
		id=id->n.next;
	}
	return 0;//not existing
}
//this function moves IdentifierInfos with higher Lifetime than the liftime 
//of current scope at the higher end of stack, after ISP cleanup
void compressISP(pos *p,Identinfo *ISPmin,Identinfo *ISPmax,enum Lifetime lt){
	Identinfo* ISP;
	for(ISP=decISP(ISPmax);ISP>=ISPmin;ISP=decISP(ISP)){
		if(lifetime[ISP->n.identtype]>lt){
			int size=ISP->n.mysize;
			p->ISP=pushISP(p->ISP,ISP->n.mysize,1);//solved: we should not delete data otherwise the big code in main does not run for all other cases we delete such that we do not get seh exception
			//*(p->ISP)=*ISP;//needs to be replaced;
			memcpy(p->ISP,ISP,size);
			inserthash(p->i,p->ISP); 
			if(ISP==ISPmin) break;
		}
	}
}
//We need begin and end if the rule has several steps such as: "while" "(" expression ")" statement
//We need if(...){Begin... End} if we have many choices such as:
//        "do" statement "while" "(" expression ")" ";"
//        "for" "(" expression? ";" expression? ";" expression? ")" statement

#ifdef RELEASEDEBUG
#define Beginfn {pos p0=*p;printf( __FUNCTION__);printf("\n");{
#define Endfn *p=p0;}}
#define Begin {pos p0=*p;printf("\t%d\n",__LINE__);{
#else
#define Beginfn {pos p0=*p;{
#define Endfn *p=p0;}}
#define Begin {pos p0=*p;{
#endif
#define End }reject(&p0,p);}
int reject(pos *p0,pos *p){
#ifdef RELEASEDEBUG
	printf("\treject\n");
#endif
	while(p->ISP<p0->ISP) {
		if(p->ISP->n.identtype!=ignore){
			if(getS(&p->ISP->n.kridentstr.begin)!=0){
				if(p->ISP->n.identtype!=ilbp) removehash(p->i,p->ISP);
			}
		}
		p->ISP=incISP(p->ISP);
	}
	*p=*p0;
	return 0;
}
void advance(pos *p0,pos *p){
	*p0=*p;
}
void patch(voidfn p,void* PC){
	*((voidfn*)PC)=p;
}
void emitptr(pos *p,u *ptr){
	p->PC.Pu->Pu=ptr;(p->PC.ptr)+=SIZEOFPTR;
}
void emitKRange(pos *p,KStrRange krstr)
{
	if(krstr.begin.s==krstr.end.s || (krstr.begin.s[2]==*krstr.end.s && *(krstr.end.s)==krstr.end.c)){//when we have char *s[] as a stack variable then the begin.s and end.s entries are not valid when executing vm. Therefore we have to only use the begin.c and end.c entries which are still valid, as they point into the original string
		emitptr(p,(u*)0);
		emitptr(p,(u*)krstr.begin.c);
		emitptr(p,(u*)0);
		emitptr(p,(u*)krstr.end.c);
	}else
	{
		emitptr(p,(u*)krstr.begin.s);//we should only do these calls if begin and end works on the global char *s[] array
		emitptr(p,(u*)krstr.begin.c);
		emitptr(p,(u*)krstr.end.s);
		emitptr(p,(u*)krstr.end.c);
	}
}
void emitdonotchange(pos *p, voidfn ptr){//This emits nothing, but increments the ProgramCounter
	p->lastOP_PC2 = p->lastOP_PC;
	p->lastOP_PC = p->PC;
	(p->PC.ptr) += SIZEOFOP;
}
void emit(pos *p,voidfn ptr){
	p->lastOP_PC2 = p->lastOP_PC;
	p->lastOP_PC = p->PC;
	patch(ptr,p->PC.ptr);(p->PC.ptr)+=SIZEOFOP;
}
//!!!!!we have a problem with temporary and stack strings when they are put into identinfostack
//so what happens is that the strings are not valid anymore as we only keep references
//so for goto and breaks we actually have a global string table with break1, break2 break3 ,...
int emitexpression(pos *p,int (*fn)(pos*,iddata*),char *str,iddata *id){//this function is used to emit a c-style expression which can be constructed by the compiler
	pos temppos;
	int ret;
	int neednewlookup=p->neednewlookup;
	int allowingdollarpre;
	p->neednewlookup=1;
	temppos=*p;
	
	temppos.k.s=0;
	temppos.k.c=str;
	allowingdollarpre = p->allowingdollar;
	temppos.allowingdollar=1;//now we can use the dollar sign for identifiers, this is for internal compiler usage such that no interference with user can occur
	ret=fn(&temppos,id);
	temppos.k=p->k;
	*p=temppos;
	p->allowingdollar= allowingdollarpre;
	p->neednewlookup=neednewlookup;
	return ret;
}
int emitexpressionKStr(pos *p,int (*fn)(pos*,iddata*),char *str[],iddata *id){//this function is used to emit a c-style expression which can be constructed by the compiler, we use kstr such that we can concat strings without using new memory reusing the old string
	pos temppos;
	int ret;
	int neednewlookup=p->neednewlookup;
	int allowingdollarpre;
	p->neednewlookup=1;
	temppos=*p;
	temppos.k.s=str;
	temppos.k.c=*str;
	allowingdollarpre = p->allowingdollar;
	temppos.allowingdollar=1;//now we can use the dollar sign for identifiers, this is for internal compiler usage such that no interference with user can occur
	ret=fn(&temppos,id);
	temppos.k=p->k;
	*p=temppos;
	p->allowingdollar= allowingdollarpre;
	p->neednewlookup=neednewlookup;
	return ret;
}
void emitptrd(pos *p,ptrdiff ptr){
	p->PC.Pu->ptrd=ptr;(p->PC.ptr)+=sizeof(ptrdiff);
}
void emitchar(pos *p,char c){
	p->PC.Pu->Char=c;(p->PC.ptr)+=SIZEOFCHAR;
}
void emitint(pos *p,int ptr){
	p->constintpatchpos=p->PC;
	p->PC.Pu->Int=ptr;(p->PC.ptr)+=SIZEOFINT;
}
void emitdouble(pos *p,double d){
	p->PC.Pu->Double=d;(p->PC.ptr)+=SIZEOFDOUBLE;
}
void emitTyped(pos *p,voidfn charop,voidfn intop,voidfn floatop,voidfn doubleop,voidfn ptrop){
	if(p->i->type.ptrcnt) {
		if(ptrop==DECPTR){
			emit(p,PUSHPTR);
			emit(p,CONST_INT);
			if(p->i->type.ptrcnt==1) emitint(p,p->i->type.t->identptr.Size);//!!!!!!here we need instead of currenttype the type of first operant of pointerarithmetic
			else emitint(p,POINTERt.identptr.Size);
			emit(p,INT2PTR);
			emit(p,SUBPTR);
		}
		else if(ptrop==INCPTR){
			emit(p,PUSHPTR);
			emit(p,CONST_INT);
			if(p->i->type.ptrcnt==1) emitint(p,p->i->type.t->identptr.Size);//!!!!!!here we need instead of currenttype the type of first operant of pointerarithmetic
			else emitint(p, POINTERt.identptr.Size);
			emit(p,INT2PTR);
			emit(p,ADDPTR);
		}
		else {
			if(ptrop==GETLPTR) p->getptremited=1;
			if(ptrop==GETGPTR) p->getptremited=1;
			if(ptrop==DEREFPTR) p->getptremited = 1;
			emit(p,ptrop);
		}
		return;
	}
	switch(p->i->type.t->identtype){
	case CHAR:		emit(p,charop);break;
	case  INT:		emit(p,intop);break;
	case  FLOAT:	emit(p,floatop);break;
	case  DOUBLE:	emit(p,doubleop);break;
	case POINTER:
		emit(p,ptrop);
		if(ptrop==GETLPTR) p->getptremited=1;
		if(ptrop==GETGPTR) p->getptremited=1;
		if (ptrop == DEREFPTR) p->getptremited = 1;
		break;
	default: break;
	}
}
void emitTypeCast(pos *p,TYPE from,TYPE to){
	switch(from.t->identtype){//!!!TODO KILI typecast from pointer
	case CHAR:
		switch(to.t->identtype){
		case CHAR:	break;
		case INT:	emit(p,CHAR2INT);p->i->type.t=&INTt;p->i->type.ptrcnt=0;break;
		case FLOAT:	emit(p,CHAR2FLOAT);p->i->type.t=&FLOATt;p->i->type.ptrcnt=0;break;
		case DOUBLE:emit(p,CHAR2DOUBLE);p->i->type.t=&DOUBLEt;p->i->type.ptrcnt=0;break;
		default:break;
		}
		break;
	case INT:
		switch(to.t->identtype){
		case CHAR:	emit(p,INT2CHAR);p->i->type.t=&CHARt;p->i->type.ptrcnt=0;break;
		case INT:	break;
		case FLOAT:	emit(p,INT2FLOAT);p->i->type.t=&FLOATt;p->i->type.ptrcnt=0;break;
		case DOUBLE:emit(p,INT2DOUBLE);p->i->type.t=&DOUBLEt;p->i->type.ptrcnt=0;break;
		default:	break;
		}
		break;
	case FLOAT:
		switch(to.t->identtype){
		case CHAR:	emit(p,FLOAT2CHAR);p->i->type.t=&CHARt;p->i->type.ptrcnt=0;break;
		case INT:	emit(p,FLOAT2INT);p->i->type.t=&INTt;p->i->type.ptrcnt=0;break;
		case FLOAT:	break;
		case DOUBLE:emit(p,FLOAT2DOUBLE);p->i->type.t=&DOUBLEt;p->i->type.ptrcnt=0;break;
		default:break;
		}
		break;
	case DOUBLE:
		switch(to.t->identtype){
		case CHAR:	emit(p,DOUBLE2CHAR);p->i->type.t=&CHARt;p->i->type.ptrcnt=0;break;
		case INT:	emit(p,DOUBLE2INT);p->i->type.t=&INTt;p->i->type.ptrcnt=0;break;
		case FLOAT:	emit(p,DOUBLE2FLOAT);p->i->type.t=&FLOATt;p->i->type.ptrcnt=0;break;
		case DOUBLE:break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
void emitPUSH(pos *p){
	memptr pc0;
	p->ISP=pushISP(p->ISP,sizeof(CompileTypeStack),1);
	if((char*)p->ISP<p->PC.ptr){
		printf( "Stackoverflow in compile time type calculation.\n");
		return;
	}
	p->ISP->c.type=p->i->type;
	p->ISP->c.typecastPC=p->PC;
	pc0=p->PC;
	emitTyped(p,PUSHCHAR,PUSHINT,PUSHFLOAT,PUSHDOUBLE,PUSHPTR);
	p->ISP->c.size=p->PC.ptr-pc0.ptr;
}
void emitPOP(pos *p,TYPE cast){
	TYPE type=p->ISP->c.type;
	p->ISP=incISP(p->ISP);
	if(cast.t->identtype ==NOCHANGE) {
		if(type.ptrcnt>0){
			emit(p,POPPTR);
		}
		else switch(type.t->identtype){
		case CHAR:	emit(p,POPCHAR);break;
		case INT:	emit(p,POPINT);break;
		case FLOAT:	emit(p,POPFLOAT);break;
		case DOUBLE:emit(p,POPDOUBLE);break;
		default:	break;
		}
	}
	else{//do typecasts
		if(cast.ptrcnt>0)  emit(p,POPPTR);
		else switch(cast.t->identtype){
		case CHAR:		emit(p,POPCHAR);break;
		case INT:		emit(p,POPINT);break;
		case FLOAT:		emit(p,POPFLOAT);break;
		case DOUBLE:	emit(p,POPDOUBLE);break;
		case POINTER:	emit(p,POPPTR);break;
		default:		break;
		}
	}
}
//!!!!!!!!!!!!We might have to patch copyops for !GOTO!
void copyops(pos *p,memptr from,memptr to,int size){//copys opcodes starting from "from" to current PC and move it to "to" an opcode of size "size" will be overwritten at position "to"
	ptrdiff d=p->PC.ptr-from.ptr;//size of data to be copied
	ptrdiff d2=from.ptr-to.ptr;//moving distance
	memcpy(p->PC.ptr-size,from.ptr,d);//copies code from "from" to PC, if size is given, then the block will be moved upwards before PC
	memmove(to.ptr+d,to.ptr+size,d2-size);//makes a hole at "to" of source data size reduced by "size"
	memcpy(to.ptr,p->PC.ptr-size,d);//copies from upmoved part to "to" overwriting "size" starting at "to"
	memset(p->PC.ptr-size,0,d);
	p->PC.ptr-=size;
}
void emitTypeCastedOp(pos *p,voidfn charop,voidfn intop,voidfn floatop,voidfn doubleop,voidfn ptrop){	
	TYPE poptype=p->ISP->c.type;
	memptr patchpos=p->ISP->c.typecastPC;
	int size=p->ISP->c.size;
	memptr from;
	p->ISP=incISP(p->ISP);
	
	if(p->i->type.ptrcnt>0){
		if(poptype.ptrcnt>0) emit(p,ptrop);
		else if (poptype.t->identtype == INT) {
			from = p->PC;
		
			if (poptype.ptrcnt == 1) {
				if (poptype.t->identptr.Size > 1) {
					emit(p, PUSHINT);
					emit(p, CONST_INT);
					emitint(p, poptype.t->identptr.Size);//!!!!!!here we need instead of currenttype the type of first operant of pointerarithmetic
					emit(p, MULINT);
				}
			
			}
			else {
				emit(p, PUSHINT);
				emit(p, CONST_INT);
				emitint(p, POINTERt.identptr.Size);
				emit(p, MULINT);
			}
			
			emit(p,INT2PTR);
			emit(p,PUSHPTR);
			copyops(p,from,patchpos,size);
			emit(p,ptrop);
		}
		else printf("uncompatible type pop\n");
	}
	else switch(p->i->type.t->identtype){	case CHAR: {
		if (poptype.ptrcnt == 0) {
			switch (poptype.t->identtype) {
			case CHAR:	emit(p, charop); break;
			case INT:	emit(p, CHAR2INT); emit(p, intop); p->i->type.t = &INTt; p->i->type.ptrcnt = 0;	break;
			case FLOAT:	emit(p, INT2FLOAT); emit(p, floatop); p->i->type.t = &FLOATt; p->i->type.ptrcnt = 0;		break;
			case DOUBLE:	emit(p, INT2DOUBLE); emit(p, doubleop); p->i->type.t = &DOUBLEt; p->i->type.ptrcnt = 0;		break;
			default:			printf("unknown type POP\n");
			}
		}
		else {//!!!in order to allow for c['x']=*(c+'x')... we have p->i->type.t=CHAR/ptrcnt=0 and poptype.t=CHAR/ptrcnt=1
			emit(p, CHAR2INT);
			
			
			if (poptype.ptrcnt == 1) {
				if (poptype.t->identptr.Size > 1) {
					emit(p, PUSHINT);
					emit(p, CONST_INT);
					emitint(p, poptype.t->identptr.Size);//!!!!!!here we need instead of currenttype the type of first operant of pointerarithmetic
					emit(p, MULINT);
				}

			}
			else {
				emit(p, PUSHINT);
				emit(p, CONST_INT);
				emitint(p, POINTERt.identptr.Size);
				emit(p, MULINT);
			}
			emit(p, INT2PTR);
			emit(p, ptrop); p->i->type.t = poptype.t; p->i->type.ptrcnt = poptype.ptrcnt;		break;
		}
		break;
	}
	
		case INT:{
			if(poptype.ptrcnt==0){
				switch(poptype.t->identtype){
				case CHAR:	from=p->PC;emit(p,CHAR2INT);emit(p,PUSHINT);copyops(p,from,patchpos,size);emit(p,intop);p->i->type.t=&INTt;p->i->type.ptrcnt=0;	break;
				case INT:	emit(p,intop);	break;
				case FLOAT:	emit(p,INT2FLOAT);emit(p,floatop);p->i->type.t=&FLOATt;p->i->type.ptrcnt=0;		break;
				case DOUBLE:	emit(p,INT2DOUBLE);emit(p,doubleop);p->i->type.t=&DOUBLEt;p->i->type.ptrcnt=0;		break;
				default:			printf("unknown type POP\n");
				}
			}
			else{
				if (poptype.ptrcnt == 1) {
					if (poptype.t->identptr.Size > 1) {
						emit(p, PUSHINT);
						emit(p, CONST_INT);
						emitint(p, poptype.t->identptr.Size);//!!!!!!here we need instead of currenttype the type of first operant of pointerarithmetic
						emit(p, MULINT);
					}

				}
				else {
					emit(p, PUSHINT);
					emit(p, CONST_INT);
					emitint(p, POINTERt.identptr.Size);
					emit(p, MULINT);
				}
				emit(p,INT2PTR);
				emit(p,ptrop);p->i->type.t=poptype.t;p->i->type.ptrcnt=poptype.ptrcnt;		break;
			}
			break;
		}
		case FLOAT:{
			switch(poptype.t->identtype){
			case CHAR:	from=p->PC;emit(p,CHAR2FLOAT);emit(p,PUSHFLOAT);copyops(p,from,patchpos,size);emit(p,floatop);p->i->type.t=&FLOATt;p->i->type.ptrcnt=0;	break;
			case INT:	from=p->PC;emit(p,INT2FLOAT);emit(p,PUSHFLOAT);copyops(p,from,patchpos,size);emit(p,floatop);p->i->type.t=&FLOATt;p->i->type.ptrcnt=0;	break;
			case FLOAT:	emit(p,floatop);break;
			case DOUBLE:emit(p,FLOAT2DOUBLE);emit(p,doubleop);p->i->type.t=&DOUBLEt;p->i->type.ptrcnt=0;	break;
			default:			printf("unknown type POP\n");
			}	
			break;
		}
		case DOUBLE:{
			switch(poptype.t->identtype){
			case CHAR:	from=p->PC;emit(p,CHAR2DOUBLE);emit(p,PUSHDOUBLE);copyops(p,from,patchpos,size);break;
			case INT:	from=p->PC;emit(p,INT2DOUBLE);emit(p,PUSHDOUBLE);copyops(p,from,patchpos,size);break;
			case FLOAT:	from=p->PC;emit(p,FLOAT2DOUBLE);emit(p,PUSHDOUBLE);copyops(p,from,patchpos,size);break;
			case DOUBLE:break;
			default:			printf("unknown type POP\n");
			}
			emit(p,doubleop);
			p->i->type.t=&DOUBLEt;p->i->type.ptrcnt=0;
			break;
		}
		default:	printf("unknown type\n");
	}
	
}
//!!TODO print(c[' ']); does not work because ADDPTR does not typecast char properly
void emitSET(pos *p,TYPE ptrtype,u *setptr,KStrRange kridstr,voidfn charop,voidfn intop,voidfn floatop,voidfn doubleop,voidfn ptrop,voidfn getptr){//TODO do integer to ptr typecast@@@@@@@@@@@@@
	int emitsetptr=1;
	int cnt;
	if(ptrtype.ptrcnt!=0){	//!!!!!!TODO proper error checking
		if(p->i->type.ptrcnt==0){//we have a dereved lhs ptr: *(ptr)=
			emitsetptr=0;
			switch(ptrtype.t->identtype){//!!!!!!TODO we need to make multiples dereff when **pp=...
			case CHAR:
				switch(p->i->type.t->identtype){
				case CHAR:	break;
				case INT:	emit(p,INT2CHAR);break;
				case FLOAT:	emit(p,FLOAT2CHAR);break;
				case DOUBLE:emit(p,DOUBLE2CHAR);break;
				default:	break;
				}
				emit(p,POPPTRPUSHCHAR);
				for(cnt=1;cnt<ptrtype.ptrcnt;cnt++) emit(p,DEREFPTR);
				emit(p,SETPTRCHAR);
				break;
			case INT:
				switch(p->i->type.t->identtype){
				case CHAR:	emit(p,CHAR2INT);break;
				case INT:	break;
				case FLOAT:	emit(p,FLOAT2INT);break;
				case DOUBLE:emit(p,DOUBLE2INT);break;
				default:	break;
				}
				emit(p,POPPTRPUSHINT);
			
				for(cnt=1;cnt<ptrtype.ptrcnt;cnt++) emit(p,DEREFPTR);
				emit(p,SETPTRINT);
				break;
			case FLOAT:
				switch(p->i->type.t->identtype){
				case CHAR:	emit(p,CHAR2FLOAT);break;
				case INT:	emit(p,INT2FLOAT);break;
				case FLOAT:	break;
				case DOUBLE:emit(p,DOUBLE2FLOAT);break;
				default:	break;
				}
				emit(p,POPPTRPUSHFLOAT);
				for(cnt=1;cnt<ptrtype.ptrcnt;cnt++) emit(p,DEREFPTR);
				emit(p,SETPTRFLOAT);
				break;
			case DOUBLE:
				switch(p->i->type.t->identtype){
				case CHAR:	emit(p,CHAR2DOUBLE);break;
				case INT:	emit(p,INT2DOUBLE);break;
				case FLOAT:	emit(p,FLOAT2DOUBLE);break;
				case DOUBLE:break;
				default:	break;
				}
				emit(p,POPPTRPUSHDOUBLE);

				for(cnt=1;cnt<ptrtype.ptrcnt;cnt++) emit(p,DEREFPTR);
				emit(p,SETPTRDOUBLE);
				break;
			default:
				break;
		}
		}
		else if(ptrtype.t==p->i->type.t)
			if(ptrtype.ptrcnt+1==p->i->type.ptrcnt) {
				emit(p,POPPTRPUSHPTR);
				for(cnt=1;cnt<ptrtype.ptrcnt;cnt++) emit(p,DEREFPTR);//here is error we should not deref
				emit(p,SETPTRPTR);
			}
			else if(ptrtype.ptrcnt==p->i->type.ptrcnt)  emit(p,ptrop);
		else printf("ERROR emitSET Pointer wrong type!");
	}
	else switch(ptrtype.t->identtype){
		case CHAR:
			switch(p->i->type.t->identtype){
			case CHAR:	break;
			case INT:	emit(p,INT2CHAR);break;
			case FLOAT:	emit(p,FLOAT2CHAR);break;
			case DOUBLE:emit(p,DOUBLE2CHAR);break;
			default:	break;
			}
			emit(p,charop);
			break;
		case INT:
			switch(p->i->type.t->identtype){
			case CHAR:	emit(p,CHAR2INT);break;
			case INT:	break;
			case FLOAT:	emit(p,FLOAT2INT);break;
			case DOUBLE:emit(p,DOUBLE2INT);break;
			default:	break;
			}
			emit(p,intop);
			break;
		case FLOAT:
			switch(p->i->type.t->identtype){
			case CHAR:	emit(p,CHAR2FLOAT);break;
			case INT:	emit(p,INT2FLOAT);break;
			case FLOAT:	break;
			case DOUBLE:emit(p,DOUBLE2FLOAT);break;
			default:	break;
			}
			emit(p,floatop);
			break;
		case DOUBLE:
			switch(p->i->type.t->identtype){
			case CHAR:	emit(p,CHAR2DOUBLE);break;
			case INT:	emit(p,INT2DOUBLE);break;
			case FLOAT:	emit(p,FLOAT2DOUBLE);break;
			case DOUBLE:break;
			default:	break;
			}
			emit(p,doubleop);
			break;
		default:
			break;
	}
	if(p->i->debugmode){
		if(emitsetptr) {
			emitKRange(p,kridstr);
		}
		printf("emitSET ");
		printfid(kridstr.begin,kridstr.end);
		printf("\n");
	}
	if(emitsetptr) emitptr(p,setptr);
}

void emitGETL(pos *p,iddata *id){
	emitTyped(p,GETLCHAR,GETLINT,GETLFLOAT,GETLDOUBLE,GETLPTR);
	if(p->i->debugmode){
		emitKRange(p,id->krstr);
		printf("emitGETL ");
		printfid(id->krstr.begin,id->krstr.end);
		printf("\n");
	}
}
void emitGETG(pos *p,iddata *id){
	emitTyped(p,GETGCHAR,GETGINT,GETGFLOAT,GETGDOUBLE,GETGPTR);
	if(p->i->debugmode){
		emitKRange(p,id->krstr);
		printf("emitGETG ");
		printfid(id->krstr.begin,id->krstr.end);
		printf("\n");
	}
}
int D(pos *p,int *digit){//[0-9]
	char **c=&p->k.c;
	if ((**c>='0') && (**c<='9')){*digit=**c-'0';incS(&p->k);return 1;}
	else return 0;
}
int L(pos *p){//[a-zA-Z_] if allowingdollar==1 [a-zA-Z_$]
	char **c=&p->k.c;
	if((**c>='a') && (**c<='z')){incS(&p->k);return 1;}
	else if((**c>='A') && (**c<='Z')){incS(&p->k);return 1;}
	else if(**c=='_') {incS(&p->k);return 1;}
	else {
		if(p->allowingdollar) {if(**c=='$') {incS(&p->k);return 1;}}
	    return 0;
	}
}
int H(pos *p){//[a-fA-F0-9]
	char **c=&p->k.c;
	int digit;
	if(D(p,&digit)) return 1;
	else if((**c>='a') && (**c<='f')) {incS(&p->k);return 1;}
	else if((**c>='A') && (**c<='F')) {incS(&p->k);return 1;}
	else return 0;
}
int E(pos *p,int *num){//[Ee][+-]?{D}+
	char **c=&p->k.c;
	int sign=1,digit;
	KStr d=p->k;
	*num=0;
	if(**c=='e' || **c=='E'){
		incS(&p->k);
		if( (**c=='+') || **c=='-'){
			if(**c=='-') sign=-1;
			incS(&p->k);
		}
		if(D(p,&digit)) {
			*num=digit;
			while(D(p,&digit)){
				*num=*num*10+digit;
			}
			*num*=sign;
			return 1;
		}
		else {p->k=d;return 0;}
	}
	else {p->k=d;return 0;}
}
int FS(pos *p){//(f|F|l|L) //floating point suffix
	char **c=&p->k.c;
	if(**c=='f') {incS(&p->k);return 1;}
	else if(**c=='F') {incS(&p->k);return 1;}
	else if(**c=='l') {incS(&p->k);return 1;}
	else if(**c=='L') {incS(&p->k);return 1;}
	else return 0;
}
int IS(pos *p){//(u|U|l|L)*
	char **c=&p->k.c;
	if(**c=='u') {incS(&p->k);return 1;}
	else if(**c=='U') {incS(&p->k);return 1;}
	else if(**c=='l') {incS(&p->k);return 1;}
	else if(**c=='L') {incS(&p->k);return 1;}
	else return 0;

}
int space(pos *p){
	char **c=&p->k.c;
	KStr d=p->k;
	while(1){
		if(**c==' ') {incS(&p->k);}
		else if(**c=='\n') {incS(&p->k);}
		else if(**c=='\r') {incS(&p->k);}
		else if(**c=='\t') {incS(&p->k);}
		else if(**c=='\v') {incS(&p->k);}
		else if(**c=='\f') {incS(&p->k);}
		else if(**c=='/'){
			KStr d2=p->k;
			incS(&p->k);
			if(**c=='/'){
				incS(&p->k);
				while(1){
					if(**c=='\n'){incS(&p->k); break;}
					if(**c=='\r'){incS(&p->k); break;}
					if(**c==0){ break;}
					else {incS(&p->k);}
				}
			}
			else if(**c=='*'){
				incS(&p->k);
				while(1){
					if(**c=='*'){
						incS(&p->k);
						if(**c=='/'){incS(&p->k);break;}
					}
					else {incS(&p->k);}
				}
			}
			else{p->k=d2;break;}
		}
		else break;
	}
	if(p->k.s==d.s && p->k.c==d.c) return 0;
	else return 1;
}
int identifier(pos *p,iddata *id){//{L}({L}|{D})*
	info *i=p->i;
	char **c=&p->k.c;
	int digit;
	KStr d;
	space(p);
	d=p->k;
	if(L(p)){
		while(L(p) || D(p,&digit) );
		id->krstr.begin=d;
		id->krstr.end=p->k;
		id->idinfo=0;
		p->neednewlookup=1;
		return 1;
	}
	else {
		if((cmpS(p->k,i->kerrpos)>=0) && (2>=i->errprio)) error(p,"ERROR Identifier expected",p->k,emptyr,2);
		p->k=d;
		return 0;
	}
}
int hexconstant(pos *p){//0[xX]{H}+{IS}?	
	info *i=p->i;
	char **c=&p->k.c;
	KStr d;
	space(p);
	d=p->k;
	if(**c=='0'){
		incS(&p->k);
		if((**c=='x') || (**c=='X')){
			incS(&p->k);
		}
		else {p->k=d;return 0;}
		if(H(p)){
			while(H(p));
			IS(p);
			p->neednewlookup=1;
			return 1;
		}
		else {
			if((cmpS(p->k,i->kerrpos)>=0) && (0>=i->errprio)) error(p,"ERROR in hexadecimal format",p->k,emptyr,0);
			p->k=d;
			return 0;
		}
	}
	else {p->k=d;return 0;}
}
int octconstant(pos *p){//0{D}+{IS}?
	char **c=&p->k.c;
	int digit;
	KStr d;
	space(p);
	d=p->k;
	if(**c=='0'){
		incS(&p->k);
		if(D(p,&digit));
		else{p->k=d;return 0;}
		while (D(p,&digit));
		IS(p);
		p->neednewlookup=1;
		return 1;
	}
	else {p->k=d;return 0;}
}
int intconstant(pos *p){//{D}+{IS}?
	info *i=p->i;
	KStr d;	
	int intconst,digit;
	space(p);
	d=p->k;
	if(D(p,&digit)){
		intconst=digit;
		while(D(p,&digit)) 
		intconst=intconst*10+digit;
		IS(p);
		i->type.t=&INTt;
		i->type.ptrcnt=0;
		emit(p,CONST_INT);//2
		emitint(p,intconst);//2
		p->neednewlookup=1;
		return 1;
	}
	else{p->k=d;return 0;}
}
int charconstant(pos *p){//L?'(\\.|[^\\'])+'
	char **c=&p->k.c;
	KStr d;
	char ch;
	space(p);
	d=p->k;
	if(**c=='\''){
		incS(&p->k);
		if(**c!='\''){
			ch=**c;//!!!TODO we need to handle escape characters, currently it is not detected by test routines as escape characters are directly replaced in test code generation
			incS(&p->k);
			if(**c=='\'') {
				incS(&p->k);
				p->i->type.t=&CHARt;
				p->i->type.ptrcnt=0;
				emit(p,CONST_CHAR);
				emitchar(p,ch);
				p->neednewlookup=1;
				return 1;
			}
			else {p->k=d;return 0;}
		}
		else{p->k=d; return 0;}
	}
	else{p->k=d; return 0;}
}
int doubleconstant1(pos *p){//{D}+{E}{FS}?	
	info *i=p->i;
	char **c=&p->k.c;
	KStr d;
	int expo=0,digit;
	double dc=0.;
	space(p);
	d=p->k;
	if(D(p,&digit)) {
		dc=digit;
		while(D(p,&digit)) dc=dc*10.+digit;
		if(E(p,&expo));
		else {p->k=d;return 0;}
		FS(p);
		i->type.t=&DOUBLEt;
		i->type.ptrcnt=0;
		emit(p,CONST_DOUBLE);//2
		emitdouble(p,dc);//2
		p->neednewlookup=1;
		return 1;
	}
	else{p->k=d; return 0;}
}
int doubleconstant2(pos *p){//{D}*"."{D}+({E})?{FS}?  //{D}+"."{D}*({E})?{FS}?
	info *i=p->i;
	char **c=&p->k.c;
	int pre=0,post=0,expo=0,digit;
	double postnum=0.1;
	double dc=0;
	KStr d;
	space(p);
	d=p->k;
	if(D(p,&digit)){
		pre=1;
		dc=digit;
	}
	while(D(p,&digit)) dc=dc*10.+digit;
	if(**c=='.') incS(&p->k);
	else{p->k=d; return 0;}
	if(D(p,&digit)) {
		dc+=digit*postnum;
		postnum*=0.1;
		post=1;
	}
	while(D(p,&digit)) {
		dc+=postnum*digit;
		postnum*=0.1;
	}
	if(E(p,&expo)) dc*=pow(10.,expo);
	FS(p);
	if(pre || post){
		i->type.t=&DOUBLEt;
		i->type.ptrcnt=0;
		emit(p,CONST_DOUBLE);//2
		emitdouble(p,dc);//2
		p->neednewlookup=1;
		return 1;
	}
	else {p->k=d;return 0;}
}
int stringliteral(pos *p, iddata* id){//L?\"(\\.|[^\\"])*\"
	char **c=&p->k.c;
	KStr d, pre,d2;

	space(p);
	d=p->k;
	if(**c=='"') {
		
		incS(&p->k);
	
		d2 = p->k;
	
		while(1){
			pre = p->k;
			if(! **c) {p->k=d;return 0;}
			if(**c=='"')
				if(*(*c-1)!='\\') {
					incS(&p->k);
					p->neednewlookup=1;
					id->krstr.begin = d2;
					id->krstr.end = pre;
					return 1;
				}
			incS(&p->k);
		}
	}
	else {p->k=d;return 0;}
}
int match(const char *token, pos *p){//token must be a string constant! 
	info *i=p->i;
	char **c=&p->k.c;
	const char *tokenlistiter;
	int k;
	if(!**c) return 0;
	if(p->neednewlookup){//we need new lookup or comparison
		space(p);
		p->knext=p->k;
		p->token=0;
		if(!**c) return 0;
		k=tokenlookup[(**c-32)&127];
		p->neednewlookup=0;
		if(k){//first letter match
			do{//try all tokens with same starting letter
				tokenlistiter=tokenlist[k];
				incS(&p->knext);//advance cnext
				++tokenlistiter;
				while(*tokenlistiter){
					if(*(p->knext.c)!=*tokenlistiter) break;
					++tokenlistiter;
					incS(&p->knext);
				}
				if(!(*tokenlistiter)) {
					p->token=tokenlist[k];//found it//we need to try longest match
					break;
				}
				++k;//try next token from tokenlist
				p->knext=p->k;//restart
			}while((k<128-32)&&(tokenlist[k][0]==**c));//continue if we have still same startingletter
		}
	}
	if(p->token==token){
		p->k=p->knext;//advance
		p->neednewlookup=1;
		return 1;
	}
	else {
		if((cmpS(p->knext,i->kerrpos)>=0) && (0>=i->errprio)){
			error(p,"ERROR: Expecting :",p->knext,ksr(token),0);
		}
		return 0;
	}
}
int matchstring(const char *token, pos *p){//token must be a string constant! is used for user defined functions 
	info *i=p->i;
	char **c=&p->k.c;
	KStr d;
	space(p);
	d=p->k;
	p->token=token;
	while(*token){
		if(**c!=*token) break;
		++token;
		incS(&p->k);
	}
	if(!(*token)) {
//		p->token=i->errtoken;
		p->neednewlookup=1;
		return 1;
	}
	else{
		if((cmpS(p->k,i->kerrpos)>=0) && (0>=i->errprio)) error(p,"ERROR: Expecting :",p->k,ksr(p->token),0);
		p->k=d;
		return 0;
	}
}
int expression(pos *p,iddata *id);
int statement(pos *p,iddata *id);
int constant_expression(pos *p,iddata *id);
//jump-statement:
//        "goto" identifier ";"
//        "continue" ";"
//        "break" ";"
//        "return" expression? ";"
int jump_statement(pos *p,iddata *id){
	Beginfn
		//but c99 standard is more general: it forbits jumps over variable length arrays;
		//int main() {
		//  int n = 5;
		//  goto label; // <- ERROR: illegal jump
		//  int a[n];
		//label:;
		//}
		if(match(Goto,p)){//TODO for hints see begining of code also take care of stack! use pushn and popn where n needs to be calculated as the stack differnce between goto and label
			if(identifier(p,id)){
				if(match(Semicolon,p)) {

//////////////////////////////////
					//!GOTO!first we search for goto label
					//if we do not find it we have to insert a labelpatch on ISP
					//upon retrival of label we have to patch all labelpatches
					//We also have to emit stack adjustment POPN or PUSHN allways relate it relative to current BP!
					//we use copyops and mnelen for calculation of patch pos
					Identinfo *ii;

					ii=searchhash(p->i,id,0,0);//check for multiple labels later at the end of function, also check for undefined labels
					if( ii){
						if(ii->g.identtype==label) {
							if(p->BP.ptr-p->SP.ptr!=ii->g.SPvalforgoto){
								emit(p,SETLSP);
								emitint(p,ii->g.SPvalforgoto/*-(p->BP.ptr-p->SP.ptr)*/);
							}
							emit(p,JMP);
							emitptrd(p,ii->g.identptr.Str-p->PC.ptr);
						}
					}
					else{//label not found so we have to store a labelpatch
						p->ISP=pushISP(p->ISP,sizeof(IdentinfoGoto),1);
						p->ISP->g.identtype=labelpatch;
						p->ISP->g.kridentstr.begin=id->krstr.begin;
						p->ISP->g.kridentstr.end=id->krstr.end;
						p->ISP->g.identptr.Str=p->PC.ptr;
						p->ISP->g.SPvalforgoto=(p->BP.ptr-p->SP.ptr);
						inserthash(p->i,p->ISP);
						emit(p,JMP);
						emitptrd(p,0);
						emit(p,SETLSP);
						emitint(p,0);
						

					//here you have to be careful with vararrays and with the proper construction of the stack!
					//!GOTO!

					}

					

					
					/////////////////////////////////////
					return 1;
				}
			}
			return reject(&p0,p);
		}
		if(match(Continue,p)){
			if(match(Semicolon,p)){
				emitexpression(p,jump_statement, p->i->brekcont[GOCONT],id);
				return  1;
			}
			return reject(&p0,p);
		}
		if(match(Break,p)){
			if(match(Semicolon,p)) {
				emitexpression(p, jump_statement, p->i->brekcont[GOBREK], id);
				return 1;

			}
			return reject(&p0,p);
		}
		if(match(Return,p)){
			if(expression(p,id));
			if(match(Semicolon,p)) {
				emitTypeCast(p,p->i->type,p->i->fnrettype);		
				emit(p,RET);
				return 1;
			}
			return reject(&p0,p);
		}	
		return reject(&p0,p);
	Endfn
}

int labeled_statement(pos *p, iddata *id);

void emitlooplabel(char *s,pos *p, iddata *id){
	parsetype tmp;
	tmp = id->parsing;
	id->parsing = looplabel;
	emitexpression(p, labeled_statement, s, id);
	id->parsing = tmp;
}

	//iteration-statement:
	//        "while" "(" expression ")" statement
	//        "do" statement "while" "(" expression ")" ";"
	//        "for" "(" expression? ";" expression? ";" expression? ")" statement
	//this line is wrong        "for" "(" declaration expression? ";" expression? ")" statement
int iteration_statement(pos *p,iddata *id){
	Beginfn
		memptr jmp2,jmppos1;
		Identinfo *continueid=0;
		Identinfo *breakid=0;
		if (match(While, p)){
			if(match(BraOpen,p)){
				char brek[13] = "$break";
				char cont[16] = "$continue";
				char gobrek[18] = "goto $break";
				char gocont[21] = "goto $continue";
				char* brekcont2[] = { brek,cont,gobrek,gocont };
				char** brekcontold = p->i->brekcont;

				p->i->forwhiledoswitchlevel++;
				if (p->i->forwhiledoswitchlevel < 0 || p->i->forwhiledoswitchlevel > 99999) {
					error(p, "break/continue on wrong level. Only 99999 levels of loops allowed!\n", p->k, emptyr, 4);
					return 0;
				}
				p->i->brekcont = brekcont2;

				itoa(p->i->forwhiledoswitchlevel, brekcont2[CONT] + 9, ":;");
				itoa(p->i->forwhiledoswitchlevel, brekcont2[BREK] + 6, ":;");
				itoa(p->i->forwhiledoswitchlevel, brekcont2[GOBREK] + 11, ";");
				itoa(p->i->forwhiledoswitchlevel, brekcont2[GOCONT] + 14, ";");
				jmp2=p->PC;
				emitlooplabel(p->i->brekcont[CONT], p, id);
				continueid=p->ISP;//!!!!!--
				if(expression(p,id)){
					if(match(BraClose,p)){
						emitTypeCast(p, p->i->type, inttype);
						emit(p,IFNOTJMP);
					jmppos1=p->PC;
						
						emitptrd(p,0);
						if(statement(p,id)) {
							emit(p,JMP);
							emitptrd(p,jmp2.ptr-p->PC.ptr);
							jmppos1.Pu->ptrd=p->PC.ptr-jmppos1.ptr;
							emitlooplabel(p->i->brekcont[BREK], p, id);
		//					p->ISP--;
							breakid=p->ISP;//!!!!!!!!!--
							if(breakid) removehash(p->i,breakid);
							if (continueid) { removehash(p->i, continueid); p->ISP = continueid; }
							id->idinfo=0;
							p->i->brekcont = brekcontold;
							p->i->forwhiledoswitchlevel--;
							return 1;
						}
					}
				}
				p->i->brekcont = brekcontold;
				p->i->forwhiledoswitchlevel--;
			}
			return reject(&p0,p);
		}
		if(match(Do,p)){
			Identinfo *continueid;
			Identinfo *breakid;
			memptr jmp1=p->PC;
			char brek[13] = "$break";
			char cont[16] = "$continue";
			char gobrek[18] = "goto $break";
			char gocont[21] = "goto $continue";
			char* brekcont2[] = { brek,cont,gobrek,gocont };
			char** brekcontold = p->i->brekcont;

			p->i->forwhiledoswitchlevel++;
			p->i->brekcont = brekcont2;
			if (p->i->forwhiledoswitchlevel < 0 || p->i->forwhiledoswitchlevel > 99999) {
				error(p, "break/continue on wrong level. Only 99999 levels of loops allowed!\n", p->k, emptyr, 4);
				return 0;
			}
			itoa(p->i->forwhiledoswitchlevel, brekcont2[CONT] + 9, ":;");
			itoa(p->i->forwhiledoswitchlevel, brekcont2[BREK] + 6, ":;");
			itoa(p->i->forwhiledoswitchlevel, brekcont2[GOBREK] + 11, ";");
			itoa(p->i->forwhiledoswitchlevel, brekcont2[GOCONT] + 14, ";");
			emitlooplabel(p->i->brekcont[CONT], p, id);
			continueid=p->ISP;//!!!!!!!!!--
			if(statement(p,id)){
				if(match(While,p)){
					if(match(BraOpen,p)){
						if(expression(p,id)){
							if(match(BraClose,p)){
								if(match(Semicolon,p)){
									//char brek[] = "$break     ";
									//itoa(p->i->forwhiledoswitchlevel, brek + 6, ":;");
									emitTypeCast(p, p->i->type, inttype);
									emit(p,IFJMP);
									emitptrd(p,jmp1.ptr-p->PC.ptr);
									//emitlooplabel("$break:;", p, id);
									emitlooplabel(p->i->brekcont[BREK], p, id);
									breakid=p->ISP;//!!!!!!!!!--;
									if (breakid) removehash(p->i, breakid); 
									if (continueid) { removehash(p->i, continueid); p->ISP = continueid; }
									p->i->forwhiledoswitchlevel--;
									p->i->brekcont = brekcontold;
									return 1;
								}
							}
						}
					}
				}
			}
			p->i->forwhiledoswitchlevel--;
			p->i->brekcont = brekcontold;
			return reject(&p0,p);
		}
		if(match(For,p)){
			if (match(BraOpen, p)) {
				char brek[13] = "$break";
				char cont[16] = "$continue";
				char gobrek[18] = "goto $break";
				char gocont[21] = "goto $continue";
				
				char* brekcont2[] = { brek,cont,gobrek,gocont };
				char** brekcontold = p->i->brekcont;

				p->i->forwhiledoswitchlevel++;
				if (p->i->forwhiledoswitchlevel < 0 || p->i->forwhiledoswitchlevel > 99999){
					error(p, "break/continue on wrong level. Only 99999 levels of loops allowed!\n", p->k, emptyr, 4);
					return 0;
				}
				p->i->brekcont = brekcont2;

				itoa(p->i->forwhiledoswitchlevel, brekcont2[CONT] +9, ":;");
				itoa(p->i->forwhiledoswitchlevel, brekcont2[BREK] + 6, ":;");
				itoa(p->i->forwhiledoswitchlevel, brekcont2[GOBREK] + 11, ";");
				itoa(p->i->forwhiledoswitchlevel, brekcont2[GOCONT] + 14, ";");

				if(expression(p,id));//init
					if(match(Semicolon,p)){
						memptr jmp1=p->PC;//a
						memptr jmppos2;
						memptr jmppos3;
						memptr jmp4;
						int havecondition=0;
						
						if(expression(p,id)){//condition
							emitTypeCast(p, p->i->type, inttype);//!!!TODO handle pointer
							emit(p,IFNOTJMP);
							jmppos2=p->PC;//to d
							emitptrd(p,0);
							emit(p,JMP);
							jmppos3=p->PC;//to b
							emitptrd(p,0);
							havecondition=1;
						}
						if(match(Semicolon,p)){
							emitlooplabel(p->i->brekcont[CONT], p, id);
							continueid = p->ISP;//!!!!--;
							jmp4=p->PC;//c
							if(expression(p,id));//inc
							emit(p,JMP);
							emitptrd(p,jmp1.ptr-p->PC.ptr);// to a
							if(havecondition) jmppos3.Pu->ptrd=p->PC.ptr-jmppos3.ptr;//b
							if(match(BraClose,p)){
								if(statement(p,id)){
									emit(p,JMP);
									emitptrd(p,jmp4.ptr-p->PC.ptr);// to c
									if(havecondition) jmppos2.Pu->ptrd=p->PC.ptr-jmppos2.ptr;//d
									emitlooplabel(p->i->brekcont[BREK], p, id);
									breakid=p->ISP;//!!!!!!!!--;
									if (breakid) removehash(p->i, breakid); 
									if (continueid) { removehash(p->i, continueid); p->ISP = continueid; }
									p->i->brekcont = brekcontold;
									p->i->forwhiledoswitchlevel--;
									return 1;
								}
							}
						}
					}
					p->i->forwhiledoswitchlevel--;
					p->i->brekcont = brekcontold;
				}
			return reject(&p0,p);
		}
		return reject(&p0,p);
	Endfn
}
	//selection-statement:
	//        "if" "(" expression ")" statement
	//        "if" "(" expression ")" statement "else" statement
	//        "switch" "(" expression ")" statement
int selection_statement(pos *p,iddata *id){
	Beginfn
		info *i=p->i;
		memptr jmppos1,jmppos2;
		int haveGoto;
		int negated = 0;
		if(match(If,p)){
				if(match(BraOpen,p)){
					if(expression(p,id)){
						if(match(BraClose,p)){
							if (p->lastOP_PC.Pu->Voidfn == NOT) {
								negated = 1;
								p->lastOP_PC2 = p->lastOP_PC;
								p->PC = p->lastOP_PC;

							}
							
							Begin
								haveGoto = match(Goto, p);//look forward   if we have if(expr) goto , then we emit IFJMP
							End
							if (!haveGoto){
								if (negated) emit(p, IFJMP);//!!!!need to typecast
								else emit(p, IFNOTJMP);
						jmppos1 = p->PC;//-> 1
								emitptrd(p, 0);
							}
							
							if(statement(p,id)){//attention goto seems to emit SETLSP which can be patched away then the jump labels are wrong!!!1
								if (haveGoto){
									if (p->lastOP_PC2.Pu->Voidfn == JMP){//JMP can be at 2 positions depending if we need SETLSP before
										if (negated) p->lastOP_PC2.Pu->Voidfn=IFNOTJMP;//!!!!need to typecast
										else p->lastOP_PC2.Pu->Voidfn = IFJMP;
									}
									else if (p->lastOP_PC.Pu->Voidfn == JMP){
										if (negated) p->lastOP_PC.Pu->Voidfn = IFNOTJMP;//!!!!need to typecast
										else p->lastOP_PC.Pu->Voidfn = IFJMP;
									}
								}
								Begin//if we do not have else  we backtrack
									emit(p,JMP);
						jmppos2=p->PC;//->2
									emitptrd(p,0);
									if (!haveGoto) jmppos1.Pu->ptrd = p->PC.ptr - jmppos1.ptr;//1			
									if(match(Else,p)){
										if(statement(p,id)) {
											jmppos2.Pu->ptrd=p->PC.ptr-jmppos2.ptr;//2
											return 1;
										}
									}
								End
								if (!haveGoto) jmppos1.Pu->ptrd = p->PC.ptr - jmppos1.ptr;//1
								return 1;
							}
						}
					}
				}
			return reject(&p0,p);
		}
		if(match(Switch,p)){//!GOTO! switch unimplemented!!!!!
			if(match(BraOpen,p)){
				if(expression(p,id)){
					if(match(BraClose,p)){
						if(statement(p,id)) return 1;
					}
				}
			}
		}
		return reject(&p0,p);
	Endfn
}
//expression-statement:
//        expression? ";"
int expression_statement(pos *p,iddata *id){
	Beginfn
		if(expression(p,id));
		if(match(Semicolon,p)) return 1;
		return reject(&p0,p);
	Endfn
}
//labeled-statement:
//        identifier ":" statement
//        "case" constant-expression ":" statement
//        "default" ":" statement
int labeled_statement(pos *p,iddata *id){
	Beginfn
		if(identifier(p,id)){
			if(match(Colon,p)){
				Identinfo *ii;
				ii=searchhash(p->i,id,0,0);//check for multiple labels
				if(ii){
					if (ii->g.identtype == label) {
						if (id->parsing!=looplabel){//$continue and $break are allowed to occur multiple times
							if ((cmpS(p->k, p->i->kerrpos) >=0) && (4 >= p->i->errprio)) error(p, "label definition uses already label name\n", p->k, id->krstr, 4);
							return reject(&p0, p);
						}
					}
				}


				//here you have to be careful with vararrays and with the proper construction of the stack!
				//!GOTO!

				//KSKSK so this works with nested loop but ignores break
				//if you comment out && (id->parsing != looplabel)
				//then break works but not nested loops
				//but I think we fixed it with break1, break2 ,... loop labels
				int cnt=0;
				while (ii /*&& (id->parsing != looplabel)*/) {//!!!!!we patch the calls to forward defined functions//todo we should when compiler is finished check that there are no fnforwardpatches available
						
					if( ii->g.identtype==labelpatch) {
//patching
//						emit(p, JMP);
//						emitptrd(p, 0);
//						emit(p, SETLSP);
//						emitint(p, 0);

						memptr PC2;
						PC2=p->PC;
						p->PC.ptr=ii->g.identptr.Str;	//ii->identptr.Str=p->PC.ptr;patch jump and stack

						if(p->BP.ptr-p->SP.ptr!=ii->g.SPvalforgoto){//we could have JMP IFJMP IFNOTJMP
							voidfn fn = p->PC.Pu->Voidfn;
							emit(p,SETLSP);
							emitint(p,p->BP.ptr-p->SP.ptr);
							emit(p,fn);
							emitptrd(p,PC2.ptr-p->PC.ptr);
						}
						else{
							int len,i;
							emitdonotchange(p,JMP);
							emitptrd(p,PC2.ptr-p->PC.ptr);	
							//we have to kill SETLSP
							len=mnelen[p->i->mode][SETLSP];
							printf("%d", len);
							if (mnelen[p->i->mode][NOP]==1) for (i = 0; i < len; ++i) emit(p, NOP);//ATTENTION ASSUMING SIZEOF NOP=1
							else printf("Assumption that sizeof NOP is equal 1 failed !");
							
						}
						p->PC=PC2;
						if(removehash(p->i,ii)) {
							ii->g.identtype=ignore;
							id->idinfo=0;//we have to set idinfo to zero otherwise searchhash uses the cashed value
							
						}

					}

//!!!!attention here is problem that we do not want to delete label but if we do not then we will not searchhash the others
					ii=searchhash(p->i,id,cnt++,0);
				} 
				
				p->ISP=pushISP(p->ISP,sizeof(IdentinfoGoto),1);
				p->ISP->g.identtype=label;
				p->ISP->g.kridentstr.begin=id->krstr.begin;
				p->ISP->g.kridentstr.end=id->krstr.end;
				p->ISP->g.identptr.Str=p->PC.ptr;
				p->ISP->g.SPvalforgoto=(p->BP.ptr-p->SP.ptr);
				inserthash(p->i,p->ISP);

				if(statement(p,id)) return 1;
			}
			reject(&p0,p);
		}
		if(match(Case,p)){//!GOTO!
			if(constant_expression(p,id)){
				if(match(Colon,p)){
					if(statement(p,id)) return 1;
				}
			}
			return reject(&p0,p);
		}
		if(match(Default,p)){//!GOTO!
			if(match(Colon,p)){
				if(statement(p,id)) return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
int compound_statement(pos *p,iddata *id);
int expression_statement(pos *p,iddata *id);
int user_defined_statement(pos *p,iddata *id){
	Beginfn
		if(matchstring("print",p)){
			if(match(BraOpen,p))
				if(expression(p,id))
					if(match(BraClose,p)){
						emitTyped(p,PRINTCHAR,PRINTINT,PRINTFLOAT,PRINTDOUBLE,PRINTPTR);
						return 1;
					}
			reject(&p0,p);
		}
		if(matchstring("input",p)){
			if(match(BraOpen,p))
				if(expression(p,id))
					if(match(BraClose,p)){
						emitTyped(p,INPUTCHAR,INPUTINT,INPUTFLOAT,INPUTDOUBLE,INPUTPTR);
						return 1;
					}
		}
		return reject(&p0,p);
	Endfn
}
//statement:
//        labeled-statement
//        compound-statement
//        expression-statement
//        selection-statement
//        iteration-statement
//        jump-statement
int statement(pos *p,iddata *id){
	if(labeled_statement(p,id)) return 1;
	else if(compound_statement(p,id)) return 1;
	else if(expression_statement(p,id)) return 1;
	else if(selection_statement(p,id)) return 1;
	else if(iteration_statement(p,id)) return 1;
    else if(jump_statement(p,id)) return 1;
	else if(user_defined_statement(p,id)) return 1;
	return 0;
}
int declaration(pos *p);
//block-item:
//        declaration
//        statement
int block_item(pos *p,iddata *id){
	if(declaration(p)) return 1;
	else if(statement(p,id)) return 1;
	return 0;
}
//block-item-list:
//        block-item
//        block-item-list block-item
int block_item_list(pos *p,iddata *id){
	if(block_item(p,id)){
		while(1){
			if(block_item(p,id)) continue;
			else return 1;
		}
	}
	return 0;
}
//compound-statement:
//        "{" block-item-list? "}"
int compound_statement(pos *p,iddata *id){
	Beginfn
		info *i=p->i;
		Identinfo *ILBPtemp;
	if(match(CBraOpen,p)){
			int BPminusSP;
			//the comments below with SPARRAY storing the position... is not realy right
			//varsize arrays (currently we treat them all like that) are
			//handled by emitting push N
			//and using sparray as the sp pointer variable right at basepointer
			//then we emit all variables
			//arrays are allocating a pointer
			//then after all we have an offset which we use to patch push N
			//so the stack currently does not have any array only pointer to first entry
			//but the code we also emit for the block will be called after the patched push N
			//and this can allocate calloc like the remaining memory for the variable size arrays
			//!!todo for structs we cannot do this
			//so we need to also treat non varsize arrays
			//directly in the first block by increasing SP
			char *sparray;//there is one SPARRAY per block. It stores the position of the first new place onstack for arrays. on entry in new block. BP and SP is set to sparray. and SPARRAY is set to stack position upon block exit assuming that each array is a pointer. each array alloc decrements sparray. upon block exit SPARRAY is set to BP. BP and SP are poped from stack.
			memptr patchpos;
			int firstCBraOpenInFn=i->firstCBraOpenInFn;//we avoid the local begin if we are in a function after the first CBraOpen
			//we have a local bracket
			i->firstCBraOpenInFn=0;
			if(!firstCBraOpenInFn){//if we do not have first { then we generate a local stack context
				BPminusSP=p->BP.ptr-p->SP.ptr;
				//we need to change LBP
				//!!!ksksks todo we do not detect fn(int a){int a;
				p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);
				//we should do this but it is not needed i->ISP->identptr.PVoid=i->SP.ptr;
				ILBPtemp=incISP(p->ISP);
				
				p->ISP->n.kridentstr.begin=empty;
				p->ISP->n.identtype=ilbp;
				p->ISP->n.identptr.PVoid=p->ILBP;//thus we abuse identptr to store the ILBP
				p->ILBP=ILBPtemp;
			}
			sparray=p->SP.ptr;//first entry on stack is used for correction factor for variable arrays: an array is a pointer to first element, which is placed at the end of stack for normal variables including the pointers to the arrays
			emit(p,PUSHN);
			patchpos=p->PC;
			emitint(p,0);//correction factor for variable size arrays on stack !!!this is wrong for 64 bit maybe
			if(block_item_list(p,id));//after we have emited all block_list_items we can now patch the stack correction value for variable arrays on stack
			patchpos.Pu->Int=(int)(sparray-p->SP.ptr);//correction factor for variable size arrays on stack !!!this is wrong for 64 bit
			//printf("%d!!!!", patchpos.Pu->Int);
			if (match(CBraClose,p)){//now we clean up
				if(!firstCBraOpenInFn){//if we have not final } we clean up local stack context
					int docompressISP=0;
					Identinfo* ISPmin=p->ISP;
					Identinfo* ILBPmax=decISP(p->ILBP);
					emit(p,SETLSP);
					emitint(p,BPminusSP);
					while(p->ISP < ILBPmax) {
						int inc;
						if(lifetime[p->ISP->n.identtype]>SCOPE) docompressISP=1;
						if (p->ISP->n.identtype == varloc) {
							if (p->ISP->n.type.ptrcnt == 0) inc = p->ISP->n.type.t->identptr.Size;
							else inc = POINTERt.identptr.Size;
						}
						
						if(!removehash(i,p->ISP)) {
							p->ISP->n.identtype=ignore;//if Identinfo is already removed from hash then we mark it with ignore such that compressISP does not reinsert it
							
						}
						else if(p->ISP->n.identtype==varloc) p->SP.ptr+=inc;//!!!!!!!!!!!!!!!here something is wrong
						p->ISP=incISP(p->ISP);
					}
					id->idinfo=0;
					p->ISP=decISP(p->ILBP);
					p->ILBP=(Identinfo *)p->ISP->n.identptr.PVoid;//abused identptr for storage of ilbp
					p->ISP=incISP(p->ISP);
					if(docompressISP) compressISP(p,ISPmin,p->ISP,SCOPE);
				}
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
int declarator(pos *p,iddata *id);
int initializer(pos *p,iddata *id);
int handledef(pos *p,int parameter,iddata *id){//parameter=0: not parameter def, parameter=1: parameter def, parameter=2: parameter of fnforward  so we do not inseart hash because they are of no importance eg: void fn(int unimportant,double unimportant2);
	Beginfn
						
		info *i=p->i;
		
     	Identinfo *ii;
		if(parameter != 2){
			if(!getS(&id->krstr.begin)) {
				printf("empty identifier name. can happen in forward decl.\n");
				return reject(&p0,p);
			}
			ii=searchhash(i,id,0,0);
			if(ii){
				if(ii->n.identtype==fn) {
					if(cmpS(p->k,i->kerrpos)>=0 && (4>=i->errprio) ) error(p,"variable declaration uses already defined function name\n",p->k,id->krstr,4);
					return reject(&p0,p);
				}
				if(i->global && ii->n.identtype==varglob) {
					if(cmpS(p->k,i->kerrpos)>=0 && (4>=i->errprio)) error(p,"Already defined global variable\n",p->k,id->krstr,4);
					return reject(&p0,p);
				}
				if((!i->global) && (ii->n.identtype==varloc) && (ii<p->ILBP)) {//(ii<=p->ILBP) avoids that error is triggered when we overwrite local variables from higher scope but we get  problems with parameters
					if(cmpS(p->k,i->kerrpos)>=0 && (4>=i->errprio)) error(p,"Already defined local variable\n",p->k,id->krstr,4);
					return reject(&p0,p);
				}
					if((!i->global) && (ii->n.identtype==varloc) && (ii>=p->IBP)) {//(ii<=p->/) avoids that error is triggered when we overwrite local variables from higher scope but we get  problems with parameters
					if(cmpS(p->k,i->kerrpos)>=0&& (4>=i->errprio)) error(p,"Already defined parameter variable\n",p->k,id->krstr,4);
					return reject(&p0,p);
				}
			}
		}
		p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);//!!!!!!!todo handle KString variable size continue and break label do not call handledef so currently the object should be of type IdentInfoNormal

		if((char*)p->ISP<p->PC.ptr){	
			error(p,"Identinfovar stack overflow\n",p->k,emptyr,4);
			return reject(&p0,p);
		}
		p->ISP->n.kridentstr.begin=id->krstr.begin;
		if(id->parsing==structdef || id->parsing==uniondef){
			if(id->structIdentifier){//struct members when structIdentifier is already defined
				int dec;//!!!!!!!!!!!!!!!!!!
				if (id->parsing == structdef) {
					if (p->i->global) p->ISP->n.identtype = structmembervarglobal;
					else p->ISP->n.identtype = structmembervarlocal;
				}
				else {
					if (p->i->global) p->ISP->n.identtype = unionmembervarglobal;
					else p->ISP->n.identtype = unionmembervarlocal;
				}
				p->ISP->n.type=i->type;
				 if (i->type.ptrcnt == 0) dec = i->type.t->identptr.Size;//!!!!struct fails in structforward
				else dec= POINTERt.identptr.Size;
				
				if (id->parsing == structdef) {
					//!!! does not work for structs inside structs as we use n.identptr.Size as size.
					//or maybe it works because only the type variable does that?
					//because only in struct def n.identptr.Size is size
					//but in struct member var of type struct this is the pointer to the struct identifier
					p->ISP->n.identptr.PVoid = id->structunionSP.ptr;//pointer to to sp
					p->ISP->n.parent = id->structIdentifier;
					id->structunionSP.ptr += dec;
				}
				else {
					p->ISP->n.parent = id->structIdentifier;
					p->ISP->n.identptr.PVoid = 0;//pointer to to struct identifier
					if (dec >= id->structunionSP.ptr)id->structunionSP.ptr = dec;
				}
				//!!!todo: allow   struct A{int a} c;  and struct{int a} d; and struct A{ struct B;int a}; 

				//we allow for search hash to search for n.identptr.PVoid to point to parent!!
				//so we do not need to do strincat.
				//todo we should concat name of p->ISP->structIdentifier with current like A$x where struct A{int x;};
				//therefore we need global char *pastestr[]  for concatenated identifieres
				//we need in pos a new variable showing empty slots in char *s[]
				//and the concat should be handled at the beginning of handledef
				//alternatively we could just not insert them into the hashtable and search linearly


				//but with identifier it is better to make the ISP variable size
				//first entry is now size
				//and we can then replace ISP-- by (void*)ISP-=ISP.size;
				//also search for p->ISP= and ++ISP and ++p->ISP
				//ISP2 and --(p->ISP)
				//search for /**/ and if you find /**/// be carefull with post modifier
			}
			else{//definition of struct
				id->structIdentifier=p->ISP;
				if (id->parsing == structdef) {
					if (i->global) p->ISP->n.identtype = structdefglobal;//todo distinguish local
					else p->ISP->n.identtype = structdeflocal;//todo distinguish local
				}
				else {
					if (i->global) p->ISP->n.identtype = uniondefglobal;//todo distinguish local
					else p->ISP->n.identtype = uniondeflocal;//todo distinguish local
				}

			}
		}
		else if (id->parsing == structdefforward) {
			if(i->global) p->ISP->n.identtype = structforwardglobal;
			else p->ISP->n.identtype = structdefforwardlocal;
		}
		else if (id->parsing == uniondefforward) {
			if (i->global) p->ISP->n.identtype = unionforwardglobal;
			else p->ISP->n.identtype = unionforwardlocal;
		}
		
		else
		{
			if(i->decltype2.t->identtype != NOCHANGE && i->decltype2.t!=i->type.t)//!!!!struct also check new typesystem !!!!!!!!!!!!!!kind of a hack because we used type always. so only for array definition we need decltype
				//such that the array uses the right type. decltype2 is the declared type of an array: eg int a[b*2]; it is int
				i->type = i->decltype2;//!!array here we use the type of the declaration 

			if(i->global) {
				int dec;
				p->ISP->n.identtype=varglob;
				p->ISP->n.type=i->type;
				if(i->type.ptrcnt==0) dec=i->type.t->identptr.Size;
				else dec= POINTERt.identptr.Size;
				p->SP.ptr-=dec;
				p->ISP->n.identptr.PVoid=p->SP.ptr;
			}
			else{
				int dec;
				p->ISP->n.identtype=varloc;
				p->ISP->n.type=i->type;
				if(i->type.ptrcnt==0) dec=i->type.t->identptr.Size;
				else dec= POINTERt.identptr.Size;
				p->SP.ptr-=dec;
				p->ISP->n.identptr.ptrd=(p->SP.ptr-p->BP.ptr);//!!
			}
			p->ISP->n.type = i->type;
		}

		p->ISP->n.kridentstr.end=id->krstr.end;
		if(i->fnforward){
			p->ISP->n.identtype= fnforward;
			p->ISP->n.type=i->fnrettype;
		}
		if(parameter != 2) inserthash(i,p->ISP);
		return 1;
	Endfn
}
//init-declarator:
//        declarator
//        declarator "=" initializer
int init_declarator(pos *p,iddata *id){
	Beginfn
		int num;
		if(num=declarator(p,id)){
			if(p->i->fnforward){
				//we have now fn, paramcnt,lastparam,...,firstparam on ISP[pos],ISP[pos+1]... but would like to have 
				p->ISP->n.identptr.Int=p->i->paramcnt;//so we store fn,then cnt,then all parameters in ISP[pos], ISP[pos+1],ISP[pos...]
				p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),0);//important we should not delete here as we somehow have set things there otherwise the big code in main does not work
				if (!handledef(p,0,id)){
					return reject(&p0,p);
				}
				return 1;
			}
			else{
				if (!handledef(p,0,id)){
					return reject(&p0,p);
				}
				if(p->i->type.ptrcnt!=0){
					if(num==3 || num==9){//we have array def: num==3 is variable size array and num==9 is fixed size array
						//!! FIXEDSIZEARRAY todo implement code for fixed size array relevant for arrays in structs
						//!! FIXEDSIZEARRAY we need const ptr. this is then used to emit different code at array readout
						if (p->i->decltype2.ptrcnt == 1) {
							if (p->i->decltype2.t->identptr.Size > 1) {
								emit(p, PUSHINT);
								emit(p, CONST_INT);
								emitint(p, p->i->decltype2.t->identptr.Size);//fixed: we always get int as that is the last pushed dimension. but we need the other type.   //!!!!!!here we need instead of currenttype the type of first operant of pointerarithmetic
								emit(p, MULINT);
							}

						}
						else {
							emit(p, PUSHINT);
							emit(p, CONST_INT);
							emitint(p, POINTERt.identptr.Size);
							emit(p, MULINT);
						}
						emit(p,DEFARR);
						emitint(p, p->ISP->n.identptr.Int); //offset
						if(p->i->decltype2.ptrcnt==1) emitint(p,p->i->decltype2.t->identptr.Size);//element size
						else emitint(p, POINTERt.identptr.Size);
						if(p->i->debugmode){
							emitKRange(p,id->krstr);
							printf("emitDEFPTR ");
							printfid(id->krstr.begin,id->krstr.end);
							printf("\n");
						}		
					}
				}
				advance(&p0,p);
				if(match(Eq,p)){
					//now we can replace a set by set2local or set2global
					u* setptr=p->ISP->n.identptr.Pu;//!!!!!KS this is the reason for the char error printing many printchar because we have zeros in the field due to mismatch of 64 and 32 bit ptrdiff sizes
					TYPE ptrtype=p->ISP->n.type;	
					int setglobal=p->i->global;
					if(initializer(p,id)){
						if(setglobal) emitSET(p,ptrtype,setptr,id->krstr,SETGCHAR,SETGINT,SETGFLOAT,SETGDOUBLE,SETGPTR,GETGPTR);//2
						else  emitSET(p,ptrtype,setptr,id->krstr,SETLCHAR,SETLINT,SETLFLOAT,SETLDOUBLE,SETLPTR,GETLPTR);//2
						return 1;
					}
				}
			}
			reject(&p0,p);
			return 1;
		}
		return reject(&p0,p);
	Endfn
}
//init-declarator-list:
//        init-declarator
//        init-declarator-list "," init-declarator
int init_declarator_list(pos *p,iddata *id){
	Beginfn
		if(init_declarator(p,id)){
			while(1){
				advance(&p0,p);//advance does not work
				if(match(Comma,p)){
					if(init_declarator(p,id)) continue;
				}
				else{
					reject(&p0, p);
					return 1;
				}
			}
		}
		return reject(&p0,p);
	Endfn
}
int declaration_specifiers(pos *p);
//declaration:
//        declaration-specifiers init-declarator-list? ";"
int declaration(pos *p){
	Beginfn
		iddata vardefid;
		vardefid.parsing=vardef;
		vardefid.ptrpushed.ptr = 0;
		vardefid.krstr.begin=empty;
		if(declaration_specifiers(p)){
			if(!p->i->in_function_definition) //from this we get return type for forward defined functions or also forward struct
				p->i->fnrettype=p->i->type;
			if(init_declarator_list(p,&vardefid));
			if(match(Semicolon,p)) return 1;
		}
		return 	reject(&p0,p);
	Endfn
}
//declaration-list:
//        declaration
//        declaration-list declaration
int declaration_list(pos *p){
	if(declaration(p)){
		while(1){
			if(declaration(p)) continue;
			else return 1;
		}
	}
	return 0;
}
//function-specifier:
//        "inline"
int function_specifier(pos *p){
	if(match(Inline,p)) return 1;
	return 0;
}
//typedef-name:
//        identifier
int typedef_name(pos *p){
	Beginfn
		iddata id;
		id.ptrpushed.ptr = 0;
		id.parsing=typedefinition;
		id.krstr.begin=empty;
		if( identifier(p,&id) ) {///!
			//WE HAVE TO CHECK IF TYPEDEF EXISTS
			//if 1
			//return 1;
			//else
			return reject(&p0,p);
		}
		return 0;
	Endfn
}
//int enumeration_constant(info *i);
//enumerator:
//        enumeration-constant
//        enumeration-constant "=" constant-expression
int enumerator(pos *p,iddata *id){
	Beginfn
		if(identifier(p,id)){
			advance(&p0,p);
				if(match(Eq,p)){
					if(constant_expression(p,id)) return 1;
				}
			reject(&p0,p);
			return 1;
		}	
		return reject(&p0,p);
	Endfn
}
//enumerator-list:
//        enumerator
//        enumerator-list "," enumerator
int enumerator_list(pos *p,iddata *id){
	Beginfn
		if(enumerator(p,id)){
			while(1){
				advance(&p0,p);
				if(match(Comma,p)){
					if(enumerator(p,id)) continue;	
				}
				reject(&p0,p);
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//enum-specifier:
//        "enum" identifier? "{" enumerator-list "}"
//        "enum" identifier? "{" enumerator-list "," "}"
//        "enum" identifier
int enum_specifier(pos *p){
	Beginfn
		iddata id;
		int identifierrecog=0;
		id.ptrpushed.ptr = 0;
		id.parsing=enumdef;
		id.krstr.begin=empty;
		if(match(Enum,p)){
			if(identifier(p,&id)) identifierrecog=1;
			Begin
				if(match(CBraOpen,p)){
					if(enumerator_list(p,&id)){
						if(match(Comma,p));
						if(match(CBraClose,p)) return 1;
					}
				}
			End
			if(identifierrecog) return 1;
		}
		return reject(&p0,p);
	Endfn
}
//identifier-list:
//        identifier
//        identifier-list "," identifier
int identifier_list(pos *p,iddata *id){
	Beginfn
		if(identifier(p,id)){
			while(1){
				advance(&p0,p);
				if(match(Comma,p)){
						if(identifier(p,id)) continue;
				}
				else{
					reject(&p0, p);
					return 1;
				}
			}
		}
		return reject(&p0,p);
	Endfn
}
//assignment-operator:
//        "="
//        "*="
//        "/="
//        "%="
//        "+="
//        "-="
//        "<<="
//        ">>="
//        "&="
//        "^="
//        "|="
const char * assignment_operator(pos *p){
	if(match(Eq,p)) return Eq;
	else if(match(MulEq,p)) return MulEq;
	else if(match(DivEq,p)) return DivEq;
	else if(match(ModEq,p)) return ModEq;
	else if(match(PlusEq,p)) return PlusEq;
	else if(match(MinusEq,p)) return MinusEq;
	else if(match(LtLtEq,p)) return LtLtEq;
	else if(match(GtGtEq,p)) return GtGtEq;
	else if(match(AndEq,p)) return AndEq;
	else if(match(XorEq,p)) return XorEq;
	else if(match(OrEq,p)) return OrEq;
	else return 0;
}
//unary-operator:
//        "&"
//        "*"
//        "+"
//        "-"
//        "~"
//        "!"
int unary_operator(pos *p){
	if(match(And,p)) return 1;
	else if(match(Mul,p)) return 1;
	else if(match(Plus,p)) return 1;
	else if(match(Minus,p)) return 1;
	else if(match(Tilde,p)) return 1;
	else if(match(Not,p)) return 1;
	else return 0;
}
int assignment_expression(pos *p,iddata *id);
int initializer_list(pos *p,iddata *id);
//initializer:
//        assignment-expression
//        "{" initializer-list "}"
//        "{" initializer-list "," "}"
int initializer(pos *p,iddata *id){
	Beginfn
		if(assignment_expression(p,id)) return 1;
		else if(match(CBraOpen,p)){
			if(initializer_list(p,id)){
				if(match(Comma,p));
				if(match(CBraClose,p)) return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
int conditional_expression(pos *p,iddata *id);
//constant-expression:
//       conditional-expression
int constant_expression(pos *p,iddata *id){
	return conditional_expression(p,id);
}
//designator:
 //       "[" constant-expression "]"
 //       "." identifier
int designator(pos *p,iddata *id){
	Beginfn
		if(match(SBraOpen,p)){
			if(constant_expression(p,id)){
				if(match(SBraClose,p)) return 1;
			}
		}
		else if(match(Dot,p)){
			if(identifier(p,id)) return 1;
		}
		return reject(&p0,p);
	Endfn
}
//designator-list:
//        designator
//        designator-list designator
int designator_list(pos *p,iddata *id){
	if(designator(p,id)){
		while(1){
			if(designator(p,id)) continue;
			else return 1;
		}
	}
	return 0;
}
//designation:
//        designator-list "="
int designation(pos *p,iddata *id){
	Beginfn
		if(designator_list(p,id)){
			if(match(Eq,p)) return 1;
		}
		return 	reject(&p0,p);
	Endfn
}
//initializer-list:
// designation? initializer
//        initializer-list "," designation? initializer
int initializer_list(pos *p,iddata *id){
	Beginfn
		if(designation(p,id));
		if(initializer(p,id)){
			while(1){
				advance(&p0,p);
				if(match(Comma,p)){
					if(designation(p,id));
					if(initializer(p,id)) continue;
				}
				else return 1;
				reject(&p0,p);
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
int abstract_declarator(pos *p,iddata *id);
//parameter-declaration:
//        declaration-specifiers declarator
//        declaration-specifiers abstract-declarator?
int parameter_declaration(pos *p){
	Beginfn
		iddata parameterid;
		parameterid.parsing=parameter;
		parameterid.ptrpushed.ptr = 0;
		parameterid.krstr.begin=empty;
		if(declaration_specifiers(p)){
			if(declarator(p,&parameterid)) {
				emit(p,PAR);//0
				if(!handledef(p,1,&parameterid)) {			
					return reject(&p0,p);
				}
				//ksksks todo same things like after def
				return 1;
			}
			if(abstract_declarator(p,&parameterid));
			else if(!p->i->in_function_definition){//we have forward declaration of function
				
				parameterid.krstr.begin.s=0;parameterid.krstr.begin.c="XXX";
				parameterid.idinfo=0;
				
				if(!handledef(p,2,&parameterid)) {			
					return reject(&p0,p);
				}
					
			}
			if(p->i->in_function_definition){
				p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);
				p->ISP->n.identtype=ignore;//if we are parsing function forward definition with function_definition we get uninitialized entry for all params in case the defintion looks like int fn(int,int,int) instead of int fn(int a,int b,int c
				//TODO if we have a function definition as int fn(int a,int b,int) then we might fail because we do not store any type now
				//function_definition should detect an ignore type parameter and print error message!
			}
			return 1;
		}
		return reject(&p0,p);
	Endfn
}
//we will move the stack addresses of parameters before BP, and also we use 2xSIZEOFPTR for the fact
//that call puts return address and bp on stack
//for forward declarations the offset is zero
void adjustParametersInISP(pos *p){//this function puts the paramters to the right position
	ptrdiff offset=((p->BP.ptr-p->SP.ptr));//this is the size of all parameters
	Identinfo* ISP2=p->ISP;
	p->SP=p->BP;
	while(ISP2<p->IBP) {
		ISP2->n.identptr.Str+=offset+SIZEOFPTR+SIZEOFPTR;
		ISP2=incISP(ISP2);
	}
}
//parameter-list:
//        parameter-declaration
//        parameter-list "," parameter-declaration
int parameter_list(pos *p){
	Beginfn
		int cnt=0;
		p->i->paramcnt=0;
		if(parameter_declaration(p)){
			++cnt;
			while(1){
				advance(&p0,p);
				if(match(Comma,p)){
					if(parameter_declaration(p)) {
						++cnt;
						continue;
					}
				}
				else{
					p->i->paramcnt=cnt;
					if(p->i->in_function_definition) adjustParametersInISP(p);
					return 1;
				}
				reject(&p0,p);
				p->i->paramcnt=cnt;
				if(p->i->in_function_definition) adjustParametersInISP(p);
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//parameter-type-list:
//        parameter-list
//        parameter-list "," "..."
int parameter_type_list(pos *p,iddata *id){
	Beginfn
		if(parameter_list(p)){
			advance(&p0,p);
				if(match(Comma,p)){
					if(match(DotDotDot,p)) return 1;
				}
			reject(&p0,p);
			return 1;
		}
		return reject(&p0,p);;
	Endfn
}
int type_qualifier_list(pos *p);
//direct-abstract-declarator:
//        "(" abstract-declarator ")"
//        direct-abstract-declarator? "[" type-qualifier-list? assignment-expression? "]"
//        direct-abstract-declarator? "[" "static" type-qualifier-list? assignment-expression "]"
//        direct-abstract-declarator? "[" type-qualifier-list "static" assignment-expression "]"
//        direct-abstract-declarator? "[" "*" "]"
//        direct-abstract-declarator? "(" parameter-type-list? ")"
int direct_abstract_declarator(pos *p,iddata *id){
	Beginfn
		int abstractdeclrecog=0,otherrecog=0;
		do{//"(" abstract-declarator ")"
			Begin
				if(match(BraOpen,p)){
					if(abstract_declarator(p,id)){
						if(match(BraClose,p)) {abstractdeclrecog=1;advance(&p0,p);break;}
					}
				}
			End
		}while(0);
		while(1){
			Begin
				if(match(SBraOpen,p)){
					Begin//direct-abstract-declarator? "[" type-qualifier-list? assignment-expression? "]"
						if(type_qualifier_list(p));
						if(assignment_expression(p,id));
						if(match(SBraClose,p)) {otherrecog=1;advance(&p0,p);continue;}
						reject(&p0,p);
						if(match(Static,p)){
							if(type_qualifier_list(p));
							if(assignment_expression(p,id)){
								if(match(SBraClose,p)) {otherrecog=1;advance(&p0,p);continue;}
							}
						}
						reject(&p0,p);
						if(type_qualifier_list(p)){
							if(match(Static,p)){
								if(assignment_expression(p,id)){
									if(match(SBraClose,p)){otherrecog=1;advance(&p0,p);continue;}
								}
							}
						}
						reject(&p0,p);
						if(match(Mul,p)){
							if(match(SBraClose,p)){otherrecog=1;advance(&p0,p);continue;}
						}
					End
				}
				else if(match(BraOpen,p)){
					if(parameter_type_list(p,id));
					if(match(BraClose,p)) {otherrecog=1;advance(&p0,p);continue;}
				}
				else{advance(&p0,p);break;}
			End
			break;
		};
		if(abstractdeclrecog || otherrecog) return 1;		
		return reject(&p0,p);
	Endfn
}
int pointer(pos *p);
//abstract-declarator:
//       pointer
//        pointer? direct-abstract-declarator
int abstract_declarator(pos *p,iddata *id){
	Beginfn
		int pointerrecog=0;
		if(pointer(p)) pointerrecog=1;
		if(direct_abstract_declarator(p,id)) return 1;
		if(pointerrecog) return 1;
		return reject(&p0,p);
	Endfn
}
int specifier_qualifier_list(pos *p);
//type-name:
//        specifier-qualifier-list abstract-declarator?
int type_name(pos *p,iddata *id){
	Beginfn
		if(specifier_qualifier_list(p)){
			if(abstract_declarator(p,id));
			return 1;
		}
		return reject(&p0,p);
	Endfn
}
//argument-expression-list:
//        assignment-expression
//        argument-expression-list "," assignment-expression
int argument_expression_list(pos *p){
	Beginfn
		int cnt=0;
		iddata varsetid;
		varsetid.ptrpushed.ptr= 0;
		varsetid.parsing=varset;
		varsetid.krstr.begin=empty;
		if(assignment_expression(p,&varsetid)){
			emitPUSH(p);++cnt;
			while(1){
				advance(&p0,p);
					if(match(Comma,p)){
						if(assignment_expression(p,&varsetid)){
							emitPUSH(p);
							++cnt;
							continue;
						}
					}
					else return cnt;
				reject(&p0,p);
				return cnt;
			}
		}
		return reject(&p0,p);
	Endfn
}
//expression:
//        assignment-expression
//        expression "," assignment-expression
int expression(pos *p,iddata *id){
	Beginfn
		if(assignment_expression(p,id)){
			while(1){
				advance(&p0,p);
				if(match(Comma,p)){
					if(assignment_expression(p,id)) continue;
				}
				else return 1;
				reject(&p0,p);
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//primary-expression:
//        identifier
//        constant
//        string-literal
//        "(" expression ")"
int primary_expression(pos *p,iddata *id){
	Beginfn
		info *i=p->i;
		//*id=0;
		if(identifier(p,id)){
			Identinfo *idinfo=searchhash(i,id,0,0);
			if(id->parsing==varget || id->parsing==adrget /*&& id->parsing!=parsetype::varset*/){
				
				if(idinfo==0){
					if(cmpS(p->k,i->kerrpos)>=0 && (4>=i->errprio)) error(p,"Unknown identifier:\n",p->k,id->krstr,4);
					return reject(&p0,p);
				}
				if(idinfo->n.identtype==varglob){
					i->type=idinfo->n.type;//ksksksks 
					if(id->parsing==varget) emitGETG(p,id);//4
					else {
						emit(p,PTRG) ;//adrget
						i->type.ptrcnt++;
					}
					emitptr(p,idinfo->n.identptr.Pu);//4
					i->currentvar=idinfo->n.identptr.Pu;
					i->currenttype=i->type;
					i->type=i->currenttype;
					i->currentvarglobal=1;
					i->currentcall=0;
				}
				else if(idinfo->n.identtype==varloc){
					i->type=idinfo->n.type;//ksksksks 
					if(id->parsing==varget) emitGETL(p,id);//4
					else{
						emit(p,PTRL);
						i->type.ptrcnt++;
					}
					emitptr(p,idinfo->n.identptr.Pu);//4
					i->currentvar=idinfo->n.identptr.Pu;
					i->currenttype=i->type;
					i->type=i->currenttype;
					i->currentvarglobal=0;
					i->currentcall=0;
				}
				else if (idinfo->n.identtype==fn){//4
					i->currentcall=idinfo->n.identptr.Pu;
					i->type=idinfo->n.type;
					p->functionIdentinfo=idinfo;
				}
				else if (idinfo->n.identtype==fnforward){//here we have to push on isp a fnforwardpatch
					i->currentcall=idinfo->n.identptr.Pu;
					i->type=idinfo->n.type;
					p->functionIdentinfo=idinfo;
				}
				else{
					if(cmpS(p->k,i->kerrpos)>=0 && (3>=i->errprio)) error(p,"Unknown use of identifier:\n",p->k,id->krstr,3);
					return reject(&p0,p);
				}
			}
			//varset
			if(idinfo && id->parsing==varset ) i->type=idinfo->n.type;//ksksksks 
			return 1;
		}
		else if(hexconstant(p)) return 1;//!!!!!not implemented
		else if(octconstant(p)) return 1;
		else if(doubleconstant1(p)) return 1;
		else if(doubleconstant2(p)) return 1;
		else if(intconstant(p))	return 1;
		else if(charconstant(p)) return 1;
		else if (stringliteral(p,id)) {//!!!stringliteral]
			//!!!DO IT NOW todo now we have to handle a stringliteral, if we scan from flash rom, then we can also use pointer
			//steps to do
			//patch stringliteral(p) such that we have KStr of stringliteral
			//keep a global pointer of stringposition
			//we might also emit something to patch the place lateron in case we relocate the string
			//original idea is that we emit all strings directly after the program
			//we need to set pointer to string to: idinfo->n.identptr.Pu

			//we should first convert stringliteral into a real string in memory
			//then we insert a hash for it such that we get 
			//idinfo->n.identptr.Pu pointing to it.
			//we should call inserthash
			/*
			Identinfo *idinfo;
						
							p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);
							p->ISP->n.identtype=fnforwardpatch;
							p->ISP->n.kridentstr.begin=id->krstr.begin;
							p->ISP->n.kridentstr.end=id->krstr.end;
							p->ISP->n.identptr.Str=fnforwardpatchPC.ptr;
							inserthash(i,p->ISP);



			idinfo=searchhash(p->i,id,0);

			at the point where we use the ptr:
			*/
			//but for now we emit string right here and jump over it
			int len;
			memptr jmppos1, strpos;
			emit(p, JMP);
			jmppos1 = p->PC;
			emitptrd(p, 0);
			getR(id->krstr, p->PC.ptr,(char*)p->ISP-p->PC.ptr,&len);
			strpos = p->PC;
			p->PC.ptr += len;
			jmppos1.Pu->ptrd = p->PC.ptr - jmppos1.ptr;//we jump here
			emit(p, PTRG);
			emitptr(p, strpos.ptr);
			//now only question is how do we emit a ptr to the beginning of string;
			//to //!!!stringliteral at 3263 3274

			//id->idinfo->n.identptr.Pu = sss;
			//idinfo->n.identptr.Pu
			return 1;
		}
		else if(match(BraOpen,p)){
			if(expression(p,id)){
				if(match(BraClose,p)){							
					return 1;
				}
			}
		}
		return reject(&p0,p);
	Endfn
}
int unary_expression(pos *,iddata*);
//postfix-expression:
//        primary-expression
//        postfix-expression "[" expression "]"
//        postfix-expression "(" argument-expression-list? ")"
//        postfix-expression "." identifier
//        postfix-expression "->" identifier
//        postfix-expression "++"
//        postfix-expression "--"
//        "(" type-name ")" "{" initializer-list "}"
//        "(" type-name ")" "{" initializer-list "," "}"
int postfix_expression(pos *p,iddata *id){
	Beginfn
		info *i=p->i;
		pos startpos=*p;
		KStr endpos;
		KStr startexpression;
		KStr endexpression;
		int j;
		u* currentcall=0;
		Identinfo *functionIdentinfo;
		TYPE rettype;
		i->currentcall=0;
		do{
			if(primary_expression(p,id)) {//!!!STRING
				currentcall=i->currentcall;
				rettype=i->type;
				functionIdentinfo=p->functionIdentinfo;//!!!!!!!todo check if string octal,... are working primary-expression is responsible
				break;
			}
			else if(match(BraOpen,p)){
				if(type_name(p,id)){
					if(match(BraClose,p)){
						if(match(CBraOpen,p)){
							if(initializer_list(p,id)){
								if(match(Comma,p));
								if(match(CBraClose,p)) break;
							}
						}
					}
				}
			}
			return reject(&p0,p);
		}while(0);
		// for struct.access
		int glob = -1;
		char* structunionbase = 0;
		Identinfo* structunionisp = 0;
		Identinfo* membervar=0;
		int offset = 0;
		Identinfo* classvar = 0;
		while(1){
			
			///
			Begin
				endpos=p->k;
				if(match(SBraOpen,p)){//ssssssssss here we have p[..] should become *(p+....)
					startexpression=p->k;
					if(expression(p,id)){
						endexpression=p->k;
						if(match(SBraClose,p)) {//handling of arrays:we translate x[n] to *(x+n)
							char *c=p->k.c;
							char *cnext=p->knext.c;
							int ret;
							int index=2;
							int neednewlookup=p->neednewlookup;
							char ss[]="*(+)";//!!Attention might not work for kstring generated by preprocessor
							char *s[20]={ss,ss+2};//*(
							copyStartStop(&index,s,startpos.k,endpos);//ptr
							s[index++]=ss+2;//+
							s[index++]=ss+3;//
							copyStartStop(&index,s,startexpression,endexpression);//offset
							s[index++]=ss+3;//
							s[index++]=ss+4;//)
							s[index++]=0;
							advance(&p0,p);
							*p = startpos;//!!!!attention we do something wrong because multidimensional arrays do not work with this.
							ret=emitexpressionKStr(p,assignment_expression,s,id);//!!we have to distinguis between rhs and lhs
							p->k.c=c;
							p->knext.c=cnext;
							p->neednewlookup=neednewlookup;
							continue;
						}
					}
					return reject(&p0,p);
				}
				else if(match(BraOpen,p)){
					int cnt;
					if(!currentcall) return reject(&p0,p);
					if(cnt=argument_expression_list(p));//ksks this is function call
					if(match(BraClose,p)){
						memptr fnforwardpatchPC;
						Identinfo* fni;
						int jmax;
						fnforwardpatchPC.ptr=0;

						emit(p,CALL);
						if(functionIdentinfo->n.identtype==fnforward){//we are calling a forward declared function so we have to store fnforwardpatch on ISP and we have to make a list such that we can later patch efficiently
							fnforwardpatchPC.ptr=p->PC.ptr;
							emitptr(p,0);
						}
						else{
							emitptr(p,currentcall);
						}
						if(cnt!=incISP(functionIdentinfo)->n.identptr.Int){//check amount of parameters
							if(cmpS(p->k,i->kerrpos)>=0 &&  (5>=i->errprio)){
								static char err[200];
								char functionstr[100];
								int len;
								getR(functionIdentinfo[0].n.kridentstr,functionstr,sizeof(functionstr),&len);
								sprintf(err,"Function call ");
								
								sprintf(err," with %d parameters but %d needed.\n",cnt,incISP(functionIdentinfo)->n.identptr.Int);
								error(p,err,p->k,ksr(p->token),5);
							}
							return reject(&p0,p);
						}
						jmax=incISP(functionIdentinfo)->n.identptr.Int;
						for(j=1,fni=multiIncISP(functionIdentinfo,2);j<=jmax;++j,fni=incISP(fni)){
							TYPE poptype=p->ISP->c.type;
							memptr patchpos=p->ISP->c.typecastPC;
							int size=p->ISP->c.size;
							memptr from;
							p->ISP=incISP(p->ISP);
							switch(poptype.t->identtype){//this is the type pushed  //!!!!TODO treat pointer
							case INT:{
								switch(fni->n.type.t->identtype){//this is the type of function call
								case CHAR:  from = p->PC; emit(p, INT2CHAR); emit(p, PUSHCHAR); copyops(p, from, patchpos, size); break;
								case INT:	break;
								case FLOAT:	from=p->PC;emit(p,INT2FLOAT);emit(p,PUSHFLOAT);copyops(p,from,patchpos,size);break;
								case DOUBLE:from=p->PC;emit(p,INT2DOUBLE);emit(p,PUSHDOUBLE);copyops(p,from,patchpos,size);break;
								default:			printf("unknown type POP\n");
								}
								break;
							}
							case FLOAT:{
								switch(fni->n.type.t->identtype){
								case CHAR:  from = p->PC; emit(p, FLOAT2CHAR); emit(p, PUSHCHAR); copyops(p, from, patchpos, size); break;
								case INT:	from=p->PC;emit(p,FLOAT2INT);emit(p,PUSHINT);copyops(p,from,patchpos,size);break;
								case FLOAT:	break;
								case DOUBLE:from=p->PC;emit(p,FLOAT2DOUBLE);emit(p,PUSHDOUBLE);copyops(p,from,patchpos,size);	break;
								default:			printf("unknown type POP\n");
								}
								break;
							}
							case DOUBLE:{
								switch(fni->n.type.t->identtype){
								case CHAR:  from = p->PC; emit(p, DOUBLE2CHAR); emit(p, PUSHCHAR); copyops(p, from, patchpos, size); break;
								case INT:	from=p->PC;emit(p,DOUBLE2INT);emit(p,PUSHINT);copyops(p,from,patchpos,size);break;
								case FLOAT:	from=p->PC;emit(p,DOUBLE2FLOAT);emit(p,PUSHFLOAT);copyops(p,from,patchpos,size);break;
								case DOUBLE: break;
								default:			printf("unknown type POP\n");
								}
								break;
							}
							case CHAR: {
								switch (fni->n.type.t->identtype) {
								case CHAR: break;
								case INT:	from = p->PC; emit(p, CHAR2INT); emit(p, PUSHINT); copyops(p, from, patchpos, size); break;
								case FLOAT:	from = p->PC; emit(p, CHAR2FLOAT); emit(p, PUSHFLOAT); copyops(p, from, patchpos, size); break;
								case DOUBLE: from = p->PC; emit(p, CHAR2DOUBLE); emit(p, PUSHDOUBLE); copyops(p, from, patchpos, size); break;
								default:			printf("unknown type POP\n");
								}
								break;
							}
							
							default:				printf("unknown type\n");//!!!generated when calling subfunction with ptr to char. here we get error because char and also ptr are note handled
							}
						}
						emit(p,POPN);
						emitint(p,cnt);
						i->type=rettype;
						if(fnforwardpatchPC.ptr){
							p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);
							p->ISP->n.identtype=fnforwardpatch;
							p->ISP->n.kridentstr.begin=id->krstr.begin;
							p->ISP->n.kridentstr.end=id->krstr.end;
							p->ISP->n.identptr.Str=fnforwardpatchPC.ptr;
							inserthash(i,p->ISP);
						}
						advance(&p0,p);continue;
					}
					return reject(&p0,p);
				}
				else if(match(Dot,p)){
					if(!classvar) classvar = id->idinfo;
					if (glob < 0) {//glob is <0 if we have the Dot or -> operator for the first time
						glob = id->idinfo->n.identtype == varglob;
						if (id->idinfo->n.type.t->identtype == structdefglobal || id->idinfo->n.type.t->identtype == structdeflocal || id->idinfo->n.type.t->identtype == uniondefglobal || id->idinfo->n.type.t->identtype == uniondeflocal) {
							structunionbase = id->idinfo->n.identptr.Pu;
							structunionisp = id->idinfo->n.type.t;

						}
					}
					else if (membervar) structunionisp = membervar->n.type.t;
						//id->idinfo->n.identptr
						//id->idinfo->n.type.t.identtype=structdefglobal structdeflocal
						//then searchhash restricted to indentptr==id->idinfo->n.type.t
						//searching the new identifier coming next.
						//the found inentptr.Size is the offset
						//we have an issue the varloc and varglob type is not properly adjusted in the structmemberlocal structmemberglobal variables
						//their global local properties are the global local property of the struct definition
						//we might have to adjust this upon handledef
						//one sollution is to keep local global properties in p->i  such as localstruct. 
						
					if (identifier(p, id)) {
						char* c = p->k.c;
						char* cnext = p->knext.c;
						int ret;
						int index = 2;
						int neednewlookup = p->neednewlookup;
					

						
						
						
						advance(&p0, p);
						//Identinfo* isp2 = p->ISP;
						*p = startpos;//!!!!attention we do something wrong because multidimensional arrays do not work with this.
						//p->ISP = isp2;
						p->ISP = pushISP(p->ISP, sizeof(IdentinfoNormal), 1);
						p->ISP->n.kridentstr = tempvar;
						
						membervar = searchhash(i, id, 0, structunionisp);//this failed for recursive structs
						p->ISP->n.offset = 0;// membervar->n.identptr.Size;
						p->ISP->n.type = membervar->n.type;
						//p->ISP->n.type.ptrcnt++;
						//the type of the instance global or local defines the member type

						//if(membervar->n.identtype==STRUCTME)
						p->ISP->n.identtype = classvar->n.identtype;
						offset += membervar->n.identptr.ptrd;
						p->ISP->n.identptr.Str = classvar->n.identptr.Str + offset;
						//!!!@@@todo !@@we need to check that in deref we have dereved a ptr to struct
						
						inserthash(i, p->ISP);
						//!!!@@@@ it is not a good idea to avoid calling when we have structdefglobal
						//would be better to check the end of x.y.z and then emit
						//also we need to intgrate with ->  and [] so the chain should be handled properly
						if (!(membervar->n.type.t->identtype == structdefglobal)) ret = emitexpression(p, assignment_expression, "$temp", id);;//!!we have to distinguis between rhs and lhs
						p->k.c = c;
						p->knext.c = cnext;
						p->neednewlookup = neednewlookup;
						continue;



						//removehash(i, &tempvar);
						//we have to add
						//structunionbase + membervar->n.identptr.Size;
						//in repetetive loops
						//and then do things as in primary expression.
						//p->i->type = membervar->n.type;
						
						//we could emit *((float*)((char*)structunionbase+membervar->n.identptr.Size+...))
						//but even better is to emit *($)
						//where $ is a variable of type float* with an additional offset.
						//so we add to the type variable the possiblity of an offset.
						//so $.identptr.

						
						
						//!struct /!@!@!@!@

						//searchhash with base structunionisp;
						//!!!struct handle.!!!!!
						advance(&p0,p);continue;
					}
					return reject(&p0,p);
				}
				else if(match(MinusGt,p)){
				if (!classvar) classvar = id->idinfo;
				if (glob < 0) {//glob is <0 if we have the Dot or -> operator for the first time
					glob = id->idinfo->n.identtype == varglob;
					if (id->idinfo->n.type.t->identtype == structdefglobal || id->idinfo->n.type.t->identtype == structdeflocal || id->idinfo->n.type.t->identtype == uniondefglobal || id->idinfo->n.type.t->identtype == uniondeflocal) {
						structunionbase = id->idinfo->n.identptr.Pu;
						structunionisp = id->idinfo->n.type.t;

					}
				}
				else if (membervar) structunionisp = membervar->n.type.t;
				//id->idinfo->n.identptr
				//id->idinfo->n.type.t.identtype=structdefglobal structdeflocal
				//then searchhash restricted to indentptr==id->idinfo->n.type.t
				//searching the new identifier coming next.
				//the found inentptr.Size is the offset
				//we have an issue the varloc and varglob type is not properly adjusted in the structmemberlocal structmemberglobal variables
				//their global local properties are the global local property of the struct definition
				//we might have to adjust this upon handledef
				//one sollution is to keep local global properties in p->i  such as localstruct. 

				if (identifier(p, id)) {
					char* c = p->k.c;
					char* cnext = p->knext.c;
					int ret;
					int index = 2;
					int neednewlookup = p->neednewlookup;


					//!!!todo we need to emit *($temp2).


					advance(&p0, p);
					//Identinfo* isp2 = p->ISP;
					*p = startpos;//!!!!attention we do something wrong because multidimensional arrays do not work with this.
					//p->ISP = isp2;
					p->ISP = pushISP(p->ISP, sizeof(IdentinfoNormal), 1);
					p->ISP->n.kridentstr = tempvar;

					membervar = searchhash(i, id, 0, structunionisp);//this failed for recursive structs
					p->ISP->n.offset = membervar->n.identptr.Size;
					p->ISP->n.type = membervar->n.type;
					//p->ISP->n.type.ptrcnt++;
					//the type of the instance global or local defines the member type

					//if(membervar->n.identtype==STRUCTME)
					p->ISP->n.identtype = classvar->n.identtype;
					offset += membervar->n.identptr.ptrd;
					p->ISP->n.identptr.Str = classvar->n.identptr.Str + offset;

					//tempvar.
					inserthash(i, p->ISP);
					if (!(membervar->n.type.t->identtype == structdefglobal)) ret = emitexpression(p, assignment_expression, "*($temp)", id);;//!!we have to distinguis between rhs and lhs
					p->k.c = c;
					p->knext.c = cnext;
					p->neednewlookup = neednewlookup;
					continue;



					//removehash(i, &tempvar);
					//we have to add
					//structunionbase + membervar->n.identptr.Size;
					//in repetetive loops
					//and then do things as in primary expression.
					//p->i->type = membervar->n.type;

					//we could emit *((float*)((char*)structunionbase+membervar->n.identptr.Size+...))
					//but even better is to emit *($)
					//where $ is a variable of type float* with an additional offset.
					//so we add to the type variable the possiblity of an offset.
					//so $.identptr.



					//!struct /!@!@!@!@

					//searchhash with base structunionisp;
					//!!!struct handle.!!!!!
					advance(&p0, p); continue;
				}
				return reject(&p0, p);
				}
				else if(match(PlusPlus,p)){//!array: error see array test *(x)++ does not work as it does not increase the content
				//!!!todo change to (expr,expr=expr+1)
					TYPE nochange;
					nochange.t = &NOCHANGEt;
					nochange.ptrcnt=0;
					if (i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
						if (id->ptrpushed.ptr) patch(PUSHPTR, id->ptrpushed.ptr);//@@@@
						id->ptrpushed.ptr = 0;
					}
					emitPUSH(p);
				
					emitTyped(p,INCCHAR,INCINT,INCFLOAT,INCDOUBLE,INCPTR);
					if (i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
						emitTyped(p, SWAPPTRCHAR, SWAPPTRINT, SWAPPTRFLOAT, SWAPPTRDOUBLE, SWAPPTRPTR);//!!!Very important!!!
					
					}
					//ksksksks todo check lvalue use
					//we cannot use retvoid for the emitSET address as might have a *(ptrtoptr)++
					//todo search for dereffptr 
					if(i->currentvarglobal){
						emitSET(p,i->currenttype,i->currentvar,id->krstr,SETGCHAR,SETGINT,SETGFLOAT,SETGDOUBLE,SETGPTR,GETGPTR);
					}
					else{
						emitSET(p,i->currenttype,i->currentvar,id->krstr,SETLCHAR,SETLINT,SETLFLOAT,SETLDOUBLE,SETLPTR,GETLPTR);
					}
					emitPOP(p,nochange);
					advance(&p0,p);continue;
				}
				else if(match(MinusMinus,p)){//!array: error see array test *(x)++ does not work as it does not increase the content
				//!!!todo change to (expr,expr=expr-1)
					TYPE nochange;
					nochange.t=&NOCHANGEt;//no typecast
					emitPUSH(p); 
					if (i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
						if (id->ptrpushed.ptr) patch(PUSHPTR, id->ptrpushed.Pu->PVoid);//@@@@
						id->ptrpushed.ptr = 0;
					}
					
					emitTyped(p,DECCHAR,DECINT,DECFLOAT,DECDOUBLE,DECPTR);
					if (i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
						emitTyped(p, SWAPPTRCHAR, SWAPPTRINT, SWAPPTRFLOAT, SWAPPTRDOUBLE, SWAPPTRPTR);//!!!Very important!!!
					}					//ksksksks todo check lvalue use
					if(i->currentvarglobal){
						emitSET(p,i->currenttype,i->currentvar,id->krstr,SETGCHAR,SETGINT,SETGFLOAT,SETGDOUBLE,SETGPTR,GETGPTR);
					}
					else{
						emitSET(p,i->currenttype,i->currentvar,id->krstr,SETLCHAR,SETLINT,SETLFLOAT,SETLDOUBLE,SETLPTR,GETLPTR);
					}
					emitPOP(p,nochange);
					advance(&p0,p);continue;
				}
				else return 1; 
			End
			return 1;
		}
	Endfn
}
int cast_expression(pos *,iddata *id);
//unary-expression:
//        postfix-expression
//        "++" unary-expression
//        "--" unary-expression
//        unary-operator cast-expression
//        "sizeof" unary-expression
//        "sizeof" "(" type-name ")"
int unary_expression(pos *p,iddata *id){
	Beginfn
		const char *lastmatch=0;
		int cnt=0;
		while(1){
			if(match(PlusPlus,p)) {
				lastmatch=PlusPlus;
				cnt++;
				continue;
			}
			else if(match(MinusMinus,p)) {
				lastmatch=MinusMinus;
				cnt--;
				continue;
			}
			else if(match(Sizeof,p)) {lastmatch=Sizeof;continue;}
			else break;
		}
		if(lastmatch==Sizeof){
			Begin
				iddata typeidentifier;
				typeidentifier.ptrpushed.ptr = 0;
				typeidentifier.parsing=typedefinition;
				typeidentifier.krstr.begin=empty;
				if(match(BraOpen,p)){
					if(type_name(p,&typeidentifier)){
						if(match(BraClose,p)) return 1;
					}
				}
			End
		}
		if(postfix_expression(p,id)) {//a++,a--,... !!todo change to expr=expr+1
			if(cnt!=0){
				if (p->i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
					if (id->ptrpushed.ptr) patch(PUSHPTR, id->ptrpushed.ptr);//@@@@
					id->ptrpushed.ptr = 0;
				}
				
				
				if(cnt==1) emitTyped(p,INCCHAR,INCINT,INCFLOAT,INCDOUBLE,INCPTR);
				else if(cnt==-1) emitTyped(p,INCCHAR,DECINT,DECFLOAT,DECDOUBLE,DECPTR);
				else{
					emitPUSH(p);
					emit(p,CONST_INT);
					emitint(p,cnt);
					emitTypeCastedOp(p,ADDCHAR,ADDINT,ADDFLOAT,ADDDOUBLE,ADDPTR);
				}
				//ksksksks todo check lvalue use
				if(p->i->currentvarglobal){
					emitSET(p,p->i->currenttype,p->i->currentvar,id->krstr,SETGCHAR,SETGINT,SETGFLOAT,SETGDOUBLE,SETGPTR,GETGPTR);
				}
				else{
					emitSET(p,p->i->currenttype,p->i->currentvar,id->krstr,SETLCHAR,SETLINT,SETLFLOAT,SETLDOUBLE,SETLPTR,GETLPTR);
				}
				
				
			}
			
		
			
			return 1;
		}
		else if(unary_operator(p)){
			const char *unaryop=p->token;

			if(unaryop==And) {
				iddata adrgetid;
				adrgetid.ptrpushed.ptr = 0;
				adrgetid.parsing=adrget;
				adrgetid.krstr.begin=empty;
				if(cast_expression(p,&adrgetid)){
					*id = adrgetid;
					return 1;
				}
				return reject(&p0,p);
				
			}
			else if(unaryop==Mul){//ptr deref
				int getptremitedbak=p->getptremited;
				int getptremited;
				int cast_expression_ok;
				p->getptremited=0;
				cast_expression_ok=cast_expression(p,id);
				getptremited=p->getptremited;
				p->getptremited=getptremitedbak;
				if(cast_expression_ok) {
					if(getptremited==0) {//ssssss
						Identinfo *idinfo;
						if(p->i->global) emitGETG(p,id);
						else emitGETL(p,id);
						idinfo=searchhash(p->i,id,0,0);
						if(idinfo==0) return reject(&p0,p);//this was if (id==0)...
						emitptr(p,idinfo->n.identptr.Pu);
					}//!!!!TODO here we might need to typecast char to int see also as done when searching for emit(p,IF
					p->i->type.ptrcnt--;//!!! I think this is ok as it should only change current type and not type in ISP
					if(p->i->type.ptrcnt<0) return reject(&p0,p);
					if (p->i->type.t->identtype == structdefglobal || p->i->type.t->identtype == structdeflocal) p->i->type.ptrcnt++;//!!@@@ (*structvar) I do not know if this right
					//if (id->parsing == varset) {
					id->ptrpushed = p->PC;
					emit(p, NOP);//@@@@ to be patched by PUSHPTR if we have lvalue
					//}
					if(id->parsing==varget ){
						if(p->i->type.ptrcnt==0) emitTyped(p,DEREFCHAR,DEREFINT,DEREFFLOAT,DEREFDOUBLE,DEREFPTR);//!!@@@@todo (*structvar). fails because derefptr is not emitted for identype structdef
						else emitTyped(p,DEREFPTR,DEREFPTR,DEREFPTR,DEREFPTR,DEREFPTR);//TODO check if last DEREFPTR makes sense
					}
					return 1;
				}
				return reject(&p0,p);
				
			}
			else if(cast_expression(p,id)) {
				if(unaryop==Plus);
				else if(unaryop==Minus) {
					if(p->i->type.t->identtype==VOID) return reject(&p0,p);
					else emitTyped(p,NEGCHAR,NEGINT,NEGFLOAT,NEGDOUBLE,0);//3
				}
				else if (unaryop == Tilde);
				else if(unaryop==Not) emit(p,NOT);//!!!might be needed not shure TODO: emit typecast if p->i.type is different to INT
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}	
//cast-expression:
//        unary-expression
//        "(" type-name ")" cast-expression
int cast_expression(pos *p,iddata *id){
	Beginfn
		while(1){
				if(match(BraOpen,p)){
					iddata typeidentifier;
					typeidentifier.parsing=typedefinition;
					typeidentifier.krstr.begin=empty;
					typeidentifier.ptrpushed.ptr = 0;
					if(type_name(p,&typeidentifier)){//!!not yet implemented
						if(match(BraClose,p)) {advance(&p0,p);continue;}
					}
				}
			reject(&p0,p);
			break;
		}
		if(unary_expression(p,id)) return 1;//!!!we should distinguish between lhs and rhs
		return reject(&p0,p);
	Endfn
}
//multiplicative-expression:
//        cast-expression
//        multiplicative-expression "*" cast-expression
//        multiplicative-expression "/" cast-expression
//        multiplicative-expression "%" cast-expression
int multiplicative_expression(pos *p,iddata *id){
	Beginfn
		if(cast_expression(p,id)){
			while(1){
				if(match(Mul,p)){
					emitPUSH(p);//7
					if(cast_expression(p,id)) emitTypeCastedOp(p,MULCHAR,MULINT,MULFLOAT,MULDOUBLE,0);//7
					else break;
				}
				else if(match(Div,p)){//7
					if(cast_expression(p,id)) emitTypeCastedOp(p,DIVCHAR,DIVINT,DIVFLOAT,DIVDOUBLE,0);//7
					else break;
				}
				else if(match(Mod,p)){//!!!!!!!! modulo not yet implemented as we did not know how to handle floats
					if(cast_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//additive-expression:
//        multiplicative-expression
//        additive-expression "+" multiplicative-expression
//        additive-expression "-" multiplicative-expression
int additive_expression(pos *p,iddata *id){
	Beginfn
		if(multiplicative_expression(p,id)){
			while(1){
				if(match(Plus,p)){
					emitPUSH(p);
					if(multiplicative_expression(p,id)) 
						emitTypeCastedOp(p,ADDCHAR,ADDINT,ADDFLOAT,ADDDOUBLE,ADDPTR);//6
					else break;
				}
				else if(match(Minus,p)){
					emitPUSH(p);
					if(multiplicative_expression(p,id)) 
						emitTypeCastedOp(p,SUBCHAR,SUBINT,SUBFLOAT,SUBDOUBLE,SUBPTR);//6
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//shift-expression:
//        additive-expression
//        shift-expression "<<" additive-expression
//        shift-expression ">>" additive-expression
int shift_expression(pos *p,iddata *id){
	Beginfn
		if(additive_expression(p,id)){
			while(1){
				if(match(LtLt,p)){
					if(additive_expression(p,id));
					else break;
				}
				else if(match(GtGt,p)){
					if(additive_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//relational-expression:
//        shift-expression
//        relational-expression "<" shift-expression
//        relational-expression ">" shift-expression
//        relational-expression "<=" shift-expression
//        relational-expression ">=" shift-expression
int relational_expression(pos *p,iddata *id){
	Beginfn
		if(shift_expression(p,id)){
			while(1){
				 if(match(LtEq,p)){
					emitPUSH(p);
					if (shift_expression(p, id)) {
						emitTypeCastedOp(p, LTEQCHAR, LTEQINT, LTEQFLOAT, LTEQDOUBLE, LTEQPTR);
						p->i->type = inttype;
					}
					else break;
				}
				else if(match(GtEq,p)){
					emitPUSH(p);
					if (shift_expression(p, id)) {
						emitTypeCastedOp(p, GTEQCHAR, GTEQINT, GTEQFLOAT, GTEQDOUBLE, GTEQPTR);
						p->i->type = inttype;
					}
					else break;
				}
				else if(match(Lt,p)){
					emitPUSH(p);
					if (shift_expression(p, id)) {
						emitTypeCastedOp(p, LTCHAR, LTINT, LTFLOAT, LTDOUBLE, LTPTR);
						p->i->type = inttype;
					}
					else break;
				}
				else if(match(Gt,p)){
					emitPUSH(p);
					if (shift_expression(p, id)) {
						emitTypeCastedOp(p, GTCHAR, GTINT, GTFLOAT, GTDOUBLE, GTPTR);
						p->i->type = inttype;
					}
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//equality-expression:
//        relational-expression
//        equality-expression "==" relational-expression
//        equality-expression "!=" relational-expression
int equality_expression(pos *p,iddata *id){
	Beginfn
		if(relational_expression(p,id)){
			while(1){
				if(match(EqEq,p)){
					emitPUSH(p);
					if(relational_expression(p,id)){
						emitTypeCastedOp(p,EQEQCHAR,EQEQINT,EQEQFLOAT,EQEQDOUBLE,EQEQPTR);
						p->i->type = inttype;
					}
					else break;
				}
				else if(match(NotEq,p)){
					emitPUSH(p);
					if(relational_expression(p,id)){
						emitTypeCastedOp(p,NOTEQCHAR,NOTEQINT,NOTEQFLOAT,NOTEQDOUBLE,NOTEQPTR);
						p->i->type = inttype;
					}
					else break;
				}
				else return 1;
			}
		}
		return 	reject(&p0,p);
	Endfn
}
//AND-expression:
//        equality-expression
//        AND-expression "&" equality-expression
int AND_expression(pos *p,iddata *id){
	Beginfn
		if(equality_expression(p,id)){
			while(1){
				if(match(And,p)){
					if(equality_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//exclusive-OR-expression:
//        AND-expression
//        exclusive-OR-expression "^" AND-expression
int exclusive_OR_expression(pos *p,iddata *id){
	Beginfn
		if(AND_expression(p,id)){
			while(1){
				if(match(Xor,p)){
					if(AND_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//inclusive-OR-expression:
//        exclusive-OR-expression
//        inclusive-OR-expression "|" exclusive-OR-expression
int inclusive_OR_expression(pos *p,iddata *id){
	Beginfn
		if(exclusive_OR_expression(p,id)){
			while(1){
				if(match(Or,p)){
					if(exclusive_OR_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//logical-AND-expression:
//        inclusive-OR-expression
//        logical-AND-expression "&&" inclusive-OR-expression
int logical_AND_expression(pos *p,iddata *id){
	Beginfn
		if(inclusive_OR_expression(p,id)){
			while(1){
				if(match(AndAnd,p)){
					if(inclusive_OR_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//logical-OR-expression:
//        logical-AND-expression
//        logical-OR-expression "||" logical-AND-expression
int logical_OR_expression(pos *p,iddata *id){
	Beginfn
		if(logical_AND_expression(p,id)){
			while(1){
				if(match(OrOr,p)){
					if(logical_AND_expression(p,id));
					else break;
				}
				else return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//conditional-expression:
//        logical-OR-expression
//        logical-OR-expression "?" expression ":" conditional-expression

int conditional_expression(pos *p,iddata *id){
	Beginfn
		if(logical_OR_expression(p,id)){
			if (match(Question, p)) {
				memptr jmppos,jmppos2;
				emitTypeCast(p, p->i->type, inttype);
				emit(p, IFNOTJMP);
				jmppos = p->PC;
				emitptrd(p, 0);
				if(expression(p,id)){
					if(match(Colon,p)){
						emit(p, JMP);
						jmppos2 = p->PC;
						emitptrd(p, 0);
						jmppos.Pu->ptrd = p->PC.ptr - jmppos.ptr;//we jump here
						if (conditional_expression(p, id)) {
							jmppos2.Pu->ptrd = p->PC.ptr - jmppos2.ptr;//we jump here
							return 1;
						}
					}
				}
			}
			else return 1;
		}
		return reject(&p0,p);
	Endfn
}
//assignment-expression:
//        conditional-expression
//        unary-expression assignment-operator assignment-expression
//!!todo we should transfrom bot good due to sideeffects
//a+=7 to a=a+7, then *(x)+=7 would also work
//!!struct also the struct asignments should be handled with pointer deref and increments
// but before we need typecasts to work.
int assignment_expression(pos *p,iddata *id){
	Beginfn
		info *i=p->i;
		const char *assignement_operator_token=0;
		iddata varsetid = *id;
		iddata vargetid = *id;
		varsetid.parsing=varset;
		varsetid.krstr.begin=empty;
		vargetid.parsing=varget;
		vargetid.krstr.begin=empty;
		if(unary_expression(p,&varsetid)){//!!!we should distinguish between lhs and rhs

			if(assignement_operator_token=assignment_operator(p)){
				Identinfo *idinfo=searchhash(i,&varsetid,0,0);
				if(idinfo==0){
					if(cmpS(p->k,i->kerrpos)>=0 && (5>=i->errprio)) error(p,"Unknown identifier:\n",p->k,varsetid.krstr,5);
					return reject(&p0,p);
				}
				
				memptr PCX = p->PC;
				//emit(p, NOP);
				int wehavederefandpushedptr = (p->i->currenttype.ptrcnt != 0 && i->type.ptrcnt == 0);//wehavederefandpushedptr is used to handle *(i)+=1; //!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
						
				if (wehavederefandpushedptr && assignement_operator_token != Eq) {
					emitPUSH(p);//wehavederefandpushedptr is used to handle *(i)+=1
				}
				else if(idinfo->n.type.ptrcnt > 0) 
					emit(p,PUSHPTR);//!! error *(2+i)=1;//*(i+2)=1; worked
				
				
				if(assignment_expression(p, id)) {
					if (assignement_operator_token == Eq) {}
					else{
						if (!wehavederefandpushedptr) {
							emitPUSH(p);//wehavederefandpushedptr is used to handle *(i)+=1

						}
						
						if(idinfo->n.identtype==varglob && !wehavederefandpushedptr){//wehavederefandpushedptr is used to handle *(i)+=1
							//todo we need to use the type 
							i->type=idinfo->n.type;
							emitGETG(p,&varsetid);//5
							emitptr(p,idinfo->n.identptr.Pu);//5
						}
						else if(idinfo->n.identtype==varloc && !wehavederefandpushedptr){//wehavederefandpushedptr is used to handle *(i)+=1
							i->type=idinfo->n.type;
							emitGETL(p,&varsetid);//5
							emitptr(p,idinfo->n.identptr.Pu);//5
						}
						
						if(assignement_operator_token==MulEq){
							
							emitTypeCastedOp(p,MULCHAR,MULINT,MULFLOAT,MULDOUBLE,0);	
						}
						else if(assignement_operator_token==DivEq){
							
							emitTypeCastedOp(p,DIVCHAR,DIVINT,DIVFLOAT,DIVDOUBLE,0);
						}
						else if(assignement_operator_token==PlusEq){
							
							emitTypeCastedOp(p,ADDCHAR,ADDINT,ADDFLOAT,ADDDOUBLE,ADDPTR);
						}
						else if(assignement_operator_token==MinusEq){
							
							emitTypeCastedOp(p,SUBCHAR,SUBINT,SUBFLOAT,SUBDOUBLE,SUBPTR);
						}
						/*!!!!are unimplemented because we do not know how to handle float
						else if(assignement_operator_token==ModEq){
							emitTypeCastedOp(i,MODINT,MODFLOAT,MODDOUBLE);
						}
						else if(assignement_operator_token==LtLtEq){
							emitTypeCastedOp(i,SHLINT,SHLFLOAT,SHLDOUBLE);
						}
						else if(assignement_operator_token==GtGtEq){
							emitTypeCastedOp(i,SHRINT,SHRFLOAT,SHRDOUBLE);
						}
						else if(assignement_operator_token==AndEq){
							emitTypeCastedOp(i,ANDINT,ANDFLOAT,ANDDOUBLE);
						}
						else if(assignement_operator_token==XorEq){
							emitTypeCastedOp(i,XORINT,XORFLOAT,XORDOUBLE);
						}
						else if(assignement_operator_token==OrEq){
							emitTypeCastedOp(i,ORINT,ORFLOAT,ORDOUBLE);
						}
						*/
						else{
							printf( "Unknown assignement operator%s\n",assignement_operator_token);//remove this line if we have all the others implemented
							return reject(&p0,p);
						}
						
					}
					if(idinfo->n.identtype==varglob){//!!!stringliteral todo: we could emit a ptrpatch such that we can put all string literals after executable
						//the perfect solution would be that upon stringliteral
						//we push on isp the kstr with hash of the stringliteral content
						//then here we generate a corresponding push on isp
						//for a pointer patch
						//such that we can put all string literals after executable
						//and then patch all corresponding code positions
						//we could do equally with static local variables and global variables
						//to avoid jumping over functions
						if (i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
							if (varsetid.ptrpushed.ptr) 
								patch(PUSHPTR, varsetid.ptrpushed.ptr);//to handle *(i)+=1
							varsetid.ptrpushed.ptr = 0;
						}
						emitSET(p, idinfo->n.type, idinfo->n.identptr.Pu, idinfo->n.kridentstr, SETGCHAR, SETGINT, SETGFLOAT, SETGDOUBLE, SETGPTR, GETGPTR);//5

//						emitSET(p, i->currenttype, i->currentvar,idinfo->n.kridentstr,SETGCHAR,SETGINT,SETGFLOAT,SETGDOUBLE,SETGPTR,GETGPTR);//5
					}
					else if(idinfo->n.identtype==varloc){//!!!stringliteral todo: we could emit a ptrpatch such that we can put all string literals after executable
						//details see above
						///uiuiuiui
						if (i->currenttype.ptrcnt != 0 && p->i->type.ptrcnt == 0) {//!!!!!!TODO proper error checking//we have a dereved lhs ptr: *(ptr)=
							if (varsetid.ptrpushed.ptr) 
								patch(PUSHPTR, varsetid.ptrpushed.ptr);//to handle *(i)+=1
							varsetid.ptrpushed.ptr = 0;
						}
						emitSET(p, idinfo->n.type, idinfo->n.identptr.Pu, idinfo->n.kridentstr, SETLCHAR, SETLINT, SETLFLOAT, SETLDOUBLE, SETLPTR, GETLPTR);//5

						//emitSET(p, i->currenttype, i->currentvar,idinfo->n.kridentstr,SETLCHAR,SETLINT,SETLFLOAT,SETLDOUBLE,SETLPTR,GETLPTR);//5
					}
					else if (idinfo->n.identtype == structmembervarlocal || idinfo->n.identtype == structmembervarglobal ) {
						//!struct todo
						int i;
						i = 0;
					}
					else if (idinfo->n.identtype == unionmembervarlocal || idinfo->n.identtype == unionmembervarglobal) {
						//!struct todo
						int i;
						i = 0;
					}
					else{
						if(cmpS(p->k,i->kerrpos)>=0 && (5>=i->errprio)) error(p,"Unknown use of identifier:\n",p->k,varsetid.krstr,5);
						return reject(&p0,p);
					}
					if(id->parsing==varget) {
						*id=varsetid;
						id->parsing=varget;
					}
					return 1;
				}
			}
		}
		reject(&p0,p);
		if(conditional_expression(p,&vargetid)) {
			if (id->parsing == varget) {
				*id = vargetid;
				id->parsing = varget;
			}
			if (id->parsing == varset) {
				*id = varsetid;
				id->parsing = varset;
			}
			return 1;
		}
		return reject(&p0,p);
	Endfn
}
//direct-declarator:
// 1       identifier
// 2       "(" declarator ")"
// 3       direct-declarator "[" type-qualifier-list? assignment-expression? "]" //varsizearray
// 4       direct-declarator "[" "static" type-qualifier-list? assignment-expression "]"
// 5       direct-declarator "[" type-qualifier-list "static" assignment-expression "]"
// 6       direct-declarator "[" type-qualifier-list? "*" "]"
// 7       direct-declarator "(" parameter-type-list ")"
// 8       direct-declarator "(" identifier-list? ")"
//9 as in 3 but for fixed sizearray
int direct_declarator(pos *p,iddata *id){
	Beginfn
		int num=0;
		int num2 = 0;
		do{
			if(identifier(p,id)){
				num=1;
				break;
			}
			else if (match(BraOpen,p)){
				if(declarator(p,id)){
					if(match(BraClose,p)){
						num=2;
						break;
					}
				}
			}
			return reject(&p0,p);
		}while(0);
		while(1){
			if(match(SBraOpen,p)){
				Begin
					memptr PCX;
					//  direct-declarator "[" type-qualifier-list? assignment-expression? "]"
					if(type_qualifier_list(p));
					PCX = p->PC;
					num2 = 3;
					if(assignment_expression(p,id)){//!!!FIXEDSIZEARRAY check if only emitted CONSTINT 99
						if (p->lastOP_PC.ptr == PCX.ptr && p->lastOP_PC.Pu->Char == CONST_INT) {
							num2 = 9;
						}
					}
					if (match(SBraClose,p)) {
						num=num2;
						advance(&p0,p);continue;//!!arr here we need to refer to direct_declarator identifier's type
					}
					reject(&p0,p);
					//direct-declarator "[" "static" type-qualifier-list? assignment-expression "]"
					if(match(Static,p)){
						if(type_qualifier_list(p));
						if(assignment_expression(p,id)){
							if(match(SBraClose,p)) {
								num=4;
								advance(&p0,p);continue;
							}
						}
					}
					reject(&p0,p);
					//direct-declarator "[" type-qualifier-list "static" assignment-expression "]"
					if(type_qualifier_list(p)){
						if(match(Static,p)){
							if(assignment_expression(p,id)){
								if(match(SBraClose,p)){
									num=5;
									advance(&p0,p);continue;
								}
							}
						}
					}
					//direct-declarator "[" type-qualifier-list? "*" "]"
					reject(&p0,p);
					if(type_qualifier_list(p));
					if(match(Mul,p)){
						if(match(SBraClose,p)) {
							num=6;
							advance(&p0,p);continue;
						}
					}
					break;
				End;
			}
			else if(match(BraOpen,p)){
				Begin
					//direct-declarator "(" parameter-type-list ")"
					if(parameter_type_list(p,id)){
						if(match(BraClose,p)) {
							num=7;
							advance(&p0,p);continue;//TODO here we have forward function definition
						}
					}
					//direct-declarator "(" identifier-list? ")"
					reject(&p0,p);
					if(identifier_list(p,id));
					else{
						//TODO here we have forward function definition without paramters
						
					}
					if(match(BraClose,p)) {
						num=8;
						advance(&p0,p);continue;
					}
					break;
				End
			}
			else return num;//no BraOpen or SBraOpen
		}
		return reject(&p0,p);
	Endfn
}
int type_qualifier(pos *);
//type-qualifier-list:
//        type-qualifier
//        type-qualifier-list type-qualifier
int type_qualifier_list(pos *p){
	if(type_qualifier(p)){
		while(type_qualifier(p));
		return 1;
	}
	return 0;
}        
//pointer:
//        "*" type-qualifier-list?
//        "*" type-qualifier-list? pointer
int pointer(pos *p){
	p->i->type.ptrcnt=0;
	if(match(Mul,p)){
		p->i->type.ptrcnt++;
		if(type_qualifier_list(p));
		while(match(Mul,p)){
			p->i->type.ptrcnt++;
			if(type_qualifier_list(p));
		}
		return 1;
	}
	return 0;	
}
//declarator:
//       pointer? direct-declarator
int declarator(pos *p,iddata *id){
	Beginfn
		int num;
		if(pointer(p));//!!!!!!!todo this is the point where we might handle pointer to functions
		if(num=direct_declarator(p,id)){//!!todo we should distinguish betwen varsizearr and arr (relevant for structs)
			if (num == 3 || num==9) {//3 is variable size array definition, 9 is fixed size array definition
				p->i->type.ptrcnt++;//we have int a[100]
				p->i->decltype2.ptrcnt++; 
			}
			if(num==7 && id->parsing==vardef) p->i->fnforward=1;
			else p->i->fnforward=0;
			return num;
		}
		return reject(&p0,p);
	Endfn
}
//struct-declarator:
//       declarator
//        declarator? ":" constant-expression
int struct_declarator(pos *p, iddata *id){
	Beginfn
		int declaratorsupplied=0,constantexpressionsupplied=0;
		
		int num;
		id->parsing=id->struct1union0?structdef:uniondef;//!!!union
		id->krstr.begin=empty;
		if(num=declarator(p,id)) {//uiuiuiuiui we work on structs KSKSKSKS here next!!!
			declaratorsupplied=1;
			if (!handledef(p,0,id)){
					return reject(&p0,p);
			}
		}
		/*
		int num;
		if(num=declarator(p,id)){
			
				if (!handledef(p,0,id)){
		*/

		if(match(Colon,p)){
			if(constant_expression(p,id)) constantexpressionsupplied=1;
		}
		if(constantexpressionsupplied) return 1;
		else if(declaratorsupplied) return 1;
		else return reject(&p0,p);
	Endfn
}
//struct-declarator-list:
//        struct-declarator
//        struct-declarator-list "," struct-declarator
int struct_declarator_list(pos *p, iddata *id){
	Beginfn
		if(struct_declarator(p,id)){
			while(1){
				advance(&p0,p);
				if(match(Comma,p)){
					if(struct_declarator(p,id));
					else {return reject(&p0,p);}
				}
				else return 1;
				reject(&p0,p);
				return 1;
			}
		}
		return reject(&p0,p);
	Endfn
}
//type-qualifier:
//        "const"
//        "restrict"
//        "volatile"
int type_qualifier(pos *p){
	if(match(Const,p)) return 1;
	else if(match(Restrict,p)) return 1;
	else if(match(Volatile,p)) return 1;
	return 0;
}
int type_specifier(pos *p);
//specifier-qualifier-list:
//        type-specifier specifier-qualifier-list?
//        type-qualifier specifier-qualifier-list?
int specifier_qualifier_list(pos *p){
	if(type_specifier(p)){
		if(specifier_qualifier_list(p));
		return 1;
	}
	else if(type_qualifier(p)){
		if(specifier_qualifier_list(p));
		return 1;
	}
	return 0;
}
//struct-declaration:
//        specifier-qualifier-list struct-declarator-list ";"
int struct_declaration(pos *p, iddata *id){
	Beginfn
		if(specifier_qualifier_list(p)){
			if(struct_declarator_list(p,id)){
				if(match(Semicolon,p)) return 1;
			}
		}	
		return reject(&p0,p);;
	Endfn
}
//struct-declaration-list:
//        struct-declaration
//        struct-declaration-list struct-declaration
int struct_declaration_list(pos *p, iddata *id){//!!not yet implemented
	if(struct_declaration(p,id)){
		while(struct_declaration(p,id));
		return 1;
	}
	return 0;
}
//struct-or-union:
//        "struct"
//        "union"
int struct_or_union(pos *p){
	if(match(Struct,p)) return structdef;
	else if(match(Union,p)) return uniondef;	
	//!!!union should  be handled very similarly to structs. just the SP is not incremented and we take as size the maximum element.
	return 0;
}
//struct-or-union-specifier:
//        struct-or-union identifier? "{" struct-declaration-list "}"
//        struct-or-union identifier
int struct_or_union_specifier(pos *p){
	Beginfn
		int identifiersupplied=0,structdecllistsupplied=0;
		iddata id;
		id.ptrpushed.ptr = 0;
		id.structIdentifier = 0;
		id.krstr.begin=empty;
		if(id.parsing=(parsetype)struct_or_union(p)){
			
			id.struct1union0 = id.parsing == structdef;
			if(identifier(p,&id)) {
				identifiersupplied=1;
				

				
			}
			//todo we should distinguish between structT and structTforward
			if(match(CBraOpen,p)){//we have structT
				if(p->i->in_function_definition && ! p->i->in_function_body) return reject(&p0, p);
				
			
				//we reset relative offset counter for struct variables
				id.structunionSP.ptr=0;//!!!!!struct does not allow for recursive type definition with nested structs error
				//struct todo regiser structdef
			
				handledef(p, 0, &id);
				if(struct_declaration_list(p,&id)){
					if(match(CBraClose,p)) structdecllistsupplied=1;				
				}
				//!struct
				
				id.structIdentifier->n.identptr.Size = id.structunionSP.ptr;//handles size for struct and maxsize for union
				p->i->type.t = id.structIdentifier;
				p->i->type.ptrcnt = 0;//!!struct check if we need to give different number
				p->i->decltype2 = p->i->type;
				//todo chain types
				id.structIdentifier=0;
			}
			else{//we have structTforward when we have semikolon other wise we have struct instantiation
				if (p->token == Semicolon) {
					if (p->i->in_function_definition && !p->i->in_function_body) return reject(&p0, p);//!!struct todo we might have a struct as function return type
					if (id.parsing == structdef) id.parsing = structdefforward;
					else id.parsing = uniondefforward;
					handledef(p, 0, &id);
				}
				else {//struct instantiation
					Identinfo* idinfo;
					
					if (idinfo=searchhash(p->i, &id, 0,0)) {
						p->i->type.ptrcnt = 0;
						p->i->decltype2.ptrcnt = 0;
						p->i->type.t = idinfo;
						p->i->decltype2.t = idinfo;
					}
					else {
						error(p, "Unknown struct or union\n", p->k, emptyr, 5);
						return reject(&p0, p);
					}
					

				
				}
			}
			if( structdecllistsupplied) return 1;//!!!not yet handled
			else if(identifiersupplied ) return 1;
			else return reject(&p0, p);
		}
		return reject(&p0,p);
	Endfn
}
//type-specifier:
//        "void"
//        "char"
//        "short"
//        "int"
//        "long"
//        "float"
//        "double"
//        "signed"
//        "unsigned"
//        "_int"
//        "_Complex"
//        struct-or-union-specifier
//        enum-specifier
//       typedef-name
int type_specifier(pos* p) {
	if (match(Void, p)) {
		p->i->type.t = &VOIDt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &VOIDt; p->i->decltype2.ptrcnt = 0;
		return 1;
	}
	else if (match(Char, p)) {
		p->i->type.t = &CHARt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &CHARt; p->i->decltype2.ptrcnt = 0;
		return 1;
	}
	else if (match(Short, p)) {
		p->i->type.t = &INTt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &INTt; p->i->decltype2.ptrcnt = 0;
		return 1;
	}
	else if (match(Int, p)) {
		p->i->type.t = &INTt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &INTt; p->i->decltype2.ptrcnt = 0;
		return 1;
	}
	else if (match(Long, p)) {
		p->i->type.t = &INTt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &INTt; p->i->decltype2.ptrcnt = 0;
		return 1;
	}
	else if (match(Float, p)) {
		p->i->type.t = &FLOATt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &FLOATt; p->i->decltype2.ptrcnt = 0;
		return 1;
	}
	else if (match(Double, p)) {
		p->i->type.t = &DOUBLEt; p->i->type.ptrcnt = 0;
		p->i->decltype2.t = &DOUBLEt; p->i->decltype2.ptrcnt = 0;//!!! I think we have to set somewhere decltype2 also to nodef but where?
		return 1;
	}
	else if(match(Signed,p)) return 1;
	else if(match(Unsigned,p)) return 1;
	else if(match(_int,p)) return 1;
	else if(match(_COmplex,p)) return 1;
	else if (struct_or_union_specifier(p)) {//!!todo id for recursive
		return 1;
	}
	else if(enum_specifier(p))return 1; 
	else if(typedef_name(p))return 1;	
	return 0;
}
//storage-class-specifier:
//        "typedef"
//        "extern"
//        "static"
//        "auto"
//        "register"
int storage_class_specifier(pos *p){
	if(match(Typedef,p)) return 1;
	else if(match(Extern,p)) return 1;
	else if(match(Static,p)) return 1;
	else if(match(Auto,p)) return 1;
	else if(match(Register,p)) return 1;	
	return 0;
}

//declaration-specifiers:
//        storage-class-specifier declaration-specifiers?
//        type-specifier declaration-specifiers?
//        type-qualifier declaration-specifiers?
//        function-specifier declaration-specifiers?
int declaration_specifiers(pos *p){
	Beginfn
		int recog=0;
		while(1){
			if (storage_class_specifier(p));
			else if (type_specifier(p));
			else if(type_qualifier(p));
			else if(function_specifier(p));
			else break;
			recog=1;
		}
		if(!recog) return reject(&p0,p);
		return 1;
	Endfn
}
//function-definition:
//        declaration-specifiers declarator declaration-list? compound-statement
int function_definition(pos *p){
	Beginfn
		info *i=p->i;
		memptr jmppos1, PCfn;
		int num;
		int cnt,paramcnt;
		iddata functionid;
		functionid.ptrpushed.ptr = 0;
		i->in_function_definition=1;
		i->fnforward=0;
		i->global=0;
		functionid.parsing=functionname;
		functionid.krstr.begin=empty;
		if(declaration_specifiers(p)){
			i->fnrettype=i->type;
			p->BP=p->SP;//
			emit(p,JMP);//we jump over a function during first init call
			jmppos1=p->PC;
			emitptrd(p,0);
			PCfn=p->PC;
			emit(p,BEGINFN);
			p->IBP=p->ISP;
			p->ILBP=p->IBP;
			num=declarator(p,&functionid);
			if((num==7) || (num==8)){
				if(declaration_list(p));
				i->firstCBraOpenInFn=1;
				{
					Identinfo *ii=searchhash(i,&functionid,0,0);
					Identinfo *fnforwardii=0;
#ifdef _DEBUG
					char functionstr[100];
					int len;
					getR(functionid.krstr,functionstr,sizeof(functionstr),&len);
					//convert2cstring(functionid.krstr.begin,functionid.krstr.end,functionstr,sizeof(functionstr));
					printdebug("fn: %s ",functionstr);
#endif
					while(ii) {//we patch the calls to forward defined functions//todo we should when compiler is finished check that there are no fnforwardpatches available
						if(( ii->n.identtype!=fnforwardpatch ) && ( ii->n.identtype!=fnforward)){
						
						//TODO### check parameters and types
						//	break;//should be fnforward when  breaked
						}
						if( ii->n.identtype==fnforwardpatch) ii->n.identptr.Pu->Pu=PCfn.Pu;
						if(ii->n.identtype==fnforward) fnforwardii=ii;
						ii->n.identtype=ignore;
						if(removehash(i,ii)) functionid.idinfo=0;//we have to set idinfo to zero otherwise searchhash uses the cashed value
						ii=searchhash(i,&functionid,0,0);
					}
					if(ii){
						if(ii->n.identtype==fn) {
							if(cmpS(p->k,i->kerrpos)>=0 && (5>=i->errprio)) error(p,"Function definition uses already defined function name\n",p->k,functionid.krstr,5);
							i->in_function_definition=0;
							return reject(&p0,p);
						}
						if(ii->n.identtype==varglob) {
							if(cmpS(p->k,i->kerrpos)>=0 && (5>=i->errprio)) error(p,"Function definition uses already defined global variable name\n",p->k,functionid.krstr,5);
							i->in_function_definition=0;
							return reject(&p0,p);
						}
						//ksksktodo type?
					}
					//remove local variables and types
					p->ISP=p->IBP;
					p->ISP=multiDecISP(p->ISP,i->paramcnt);//we keep the parameter types;
					p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);
					p->ISP->n.identptr.Int=i->paramcnt;//so we store fn,then cnt,then all parameters in ISP[pos], ISP[pos+1],ISP[pos...]
					p->ISP->n.identtype=ignore;
					p->ISP=pushISP(p->ISP,sizeof(IdentinfoNormal),1);
					if((char*)p->ISP<p->PC.ptr){
						error(p,"Identinfo stack overflow\n",p->k,emptyr,0);
						i->in_function_definition=0;
						return reject(&p0,p);
					}
					p->ISP->n.kridentstr.begin=functionid.krstr.begin;
					p->ISP->n.identtype=fn;
					p->ISP->n.kridentstr.end=functionid.krstr.end;
					p->ISP->n.identptr.Pu=PCfn.Pu;
					p->ISP->n.type=i->fnrettype;
					inserthash(i,p->ISP);
					p->IBP=p->ISP;
					p->ILBP=p->IBP;
					p->i->in_function_body = 1;
					if(compound_statement(p,&functionid)){
						//!GOTO!!!!!!!!!!!!!TODO perform check for unpatched labels and write error
						Identinfo *ISPmin=p->ISP;
						Identinfo* fnforwardii2;
						int docompressISP=0;
						emit(p,ENDFN);
						jmppos1.Pu->ptrd=p->PC.ptr-jmppos1.ptr;//we jump over a function
						//TODO this is also where we should handle static vars
						//we should use anon_global and emit setg,..
						//here while cleaning the isp
						while(p->ISP<p->IBP) {//delete all local vars
							if(lifetime[p->ISP->n.identtype]>FUNC) docompressISP=1;
							if(removehash(i,p->ISP)){//on the stack there should not be any labelpatch
								if(p->ISP->n.identtype==labelpatch){
									error(p,"unresolved label\n",p->ISP->n.kridentstr.begin,p->ISP->n.kridentstr,0);
									i->in_function_definition=0;
									return reject(&p0,p);
								}
							}
							p->ISP=incISP(p->ISP);
						}
						//now ISP has value from * and points to fn
						//now delete all parameters from hash
						p->IBP=p->ISP;//we store it temporarilys
						p->IBP=incISP(p->IBP);
						//TODO### here we should compare the function rettype
						if(fnforwardii){
							if(p->ISP->n.type.t!=fnforwardii->n.type.t || p->ISP->n.type.ptrcnt!=fnforwardii->n.type.ptrcnt){
								error(p,"Function return type does not match forward defined return type.",p->ISP->n.kridentstr.begin,fnforwardii->n.kridentstr,4);
								return reject(&p0,p);
							}
						}
						p->ISP=incISP(p->ISP);
						paramcnt=p->ISP->n.identptr.Int;//this is amount of params
						if(fnforwardii)
							if(paramcnt!=incISP(fnforwardii)->n.identptr.Int){
								error(p,"Function amount of paramters does not match forward defined amount of parameterse.",(decISP(p->ISP))->n.kridentstr.begin,fnforwardii->n.kridentstr,5);
								return reject(&p0,p);
							}
						p->ISP=incISP(p->ISP);
						if(fnforwardii) fnforwardii2=multiIncISP(fnforwardii,2);
						for(cnt=0;cnt<paramcnt;++cnt){//we get amount of params;
							if(fnforwardii) {
								
								if(p->ISP->n.type.t!=fnforwardii2->n.type.t || p->ISP->n.type.ptrcnt!=fnforwardii2->n.type.ptrcnt){
									error(p,"Function parameter does not match forward defined paramter type.",p->ISP->n.kridentstr.begin,emptyr,5);
									return reject(&p0,p);
								}
								fnforwardii2=incISP(fnforwardii2);
							}
							//###todo compare param
							if(lifetime[p->ISP->n.identtype]>FUNC) docompressISP=1;
							removehash(i,p->ISP);//!!!XXX!!! here we set otrcnt to zero
							p->ISP=incISP(p->ISP);
						}
						p->ISP=decISP(p->IBP);//restore isp
						p->SP=p->BP;
						if(docompressISP) compressISP(p,ISPmin,p->ISP,FUNC);
						i->in_function_definition=0;
						return 1;
					}
				}
			}
		}
		i->in_function_definition=0;
		return reject(&p0,p);
	Endfn
}
//external-declaration:
//        function-definition
//        declaration
int external_declaration(pos *p){
	if(function_definition(p)) return 1;
	else{
		Begin
		
			p->i->global=1;
			p->i->fnforward=0;
			
			if(declaration(p)) return 1;
		End
	}
	return 0;
}
//translation-unit:
//        external-declaration
//        translation-unit external-declaration
int translation_unit(pos *p){
	Beginfn
		Identinfo *ISPmax=p->ISP;
		char *sparray=p->SP.ptr;
		memptr patchpos;
		emit(p,PUSHN);//global variable arrays
		patchpos=p->PC;
		emitint(p,0);//!!!64 bit?
		while(external_declaration(p));
		space(p);
		if(getS(&p->k)==0){
			iddata id;
			Identinfo *uu;
			id.krstr.begin.s=0;id.krstr.begin.c="main";
			id.krstr.end.s=0;id.krstr.end.c=id.krstr.begin.c+strlen(id.krstr.begin.c);
			id.idinfo=0;
			id.ptrpushed.ptr = 0;
			uu=searchhash(p->i,&id,0,0);
			patchpos.Pu->Int=sparray-p->SP.ptr;//global variable arrays
			emit(p,ENDP);
			emit(p,CALL);
			if(uu) emitptr(p,uu->n.identptr.Pu);
			else{
				error(p,"Missing main function\n",empty,emptyr,4);
				return reject(&p0,p);
			}
			emit(p,ENDP);
			while(p->ISP<ISPmax) {
				if(p->ISP->n.identtype==fnforward){
					error(p,"Undefined forward function declaration. Missing function definition\n",p->ISP->n.kridentstr.begin,p->ISP->n.kridentstr,4);
					return reject(&p0,p);
				}
				p->ISP=incISP(p->ISP);
			}
			return 1;
		}
		return reject(&p0,p);
	Endfn
}
