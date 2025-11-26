#ifndef COMMONDEFS_H
#define COMMONDEFS_H
//#define RELEASEDEBUG
#ifdef _DEBUG
//#define DODEBUG //comment out if you do tests in debug mode
#endif
#ifndef DODEBUG
#define printdebug
#else
#define printdebug printf
#endif 
#if defined(__LP64__) || defined(_LP64) || defined(WIN64)
#define BUILD_64   1
#endif

#ifdef BUILD_64
typedef ptrdiff_t  ptrdiff;
#else
typedef ptrdiff_t  ptrdiff;
#endif
//sizes with fillers on amd64 systems
#define SIZEOFOP sizeof(voidfn)//here we would like to use voidfn instead of void* but the VoidFnArr patching does not work otherwise, error occurs at init run when jumping over functions
#define SIZEOFPTR sizeof(void*)
#define SIZEOFINT sizeof(int)//amd64 uses fillers for int such that they take 64 bits but only 32 bits are used
#define SIZEOFFLOAT sizeof(float)
#define SIZEOFDOUBLE sizeof(double)
#define SIZEOFCHAR sizeof(char)//we have a problem when using 1 because of alignment
#define SIZEOFPTRDIFF sizeof(ptrdiff)

extern void *retvoid;
extern int retint;
extern float retfloat;
extern double retdouble;
extern char retchar;
//typedef void ((*voidfn)());//type of internal functions
//typedef enum OPCODE voidfn;
typedef unsigned char voidfn;//this is the opcode size
typedef union u{
	voidfn Voidfn;//token number
	void *PVoid;
	double Double;//works
	int Int;
	float Float;
	char *Str;//for pointer and debugging
	char Char;
	union u *Pu;
	int Size;//size for basic types and types, structs, union
	ptrdiff ptrd;
} u;
typedef char mem;
typedef union memptr{
	char *ptr;
	union u* Pu;
} memptr;
//#define BIG
#ifdef BIG
typedef enum Mode { kvm, printit, modes } Mode;//kvm_compact not yet implemented
#else
typedef enum Mode { kvm, modes } Mode;//kvm_compact not yet implemented
#endif
#endif