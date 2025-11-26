//when porting to other platform keep the exact order of the functions in all files

#ifdef ENUM
PRINTCHAR,
#elif defined(INIT)
PRINTCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PRINTCHAR_p(){//PRINT CALLFN: prints output
	printdebug("PRINTCHAR ");
	printf("%c",retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PRINTINT,
#elif defined(INIT)
PRINTINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PRINTINT_p(){//PRINT CALLFN: prints output
	printdebug(">>>>>>>>>PRINTINT ");
	printf("%d",retint);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PRINTFLOAT,
#elif defined(INIT)
PRINTFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PRINTFLOAT_p(){//PRINT CALLFN: prints output
	printdebug("PRINTFLOAT ");
	printf("%f",retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PRINTDOUBLE,
#elif defined(INIT)
PRINTDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else	
void PRINTDOUBLE_p(){//PRINT CALLFN: prints output
	printdebug("PRINTDOUBLE ");
	printf("%f",retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PRINTPTR,
#elif defined(INIT)
PRINTPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PRINTPTR_p(){//PRINT CALLFN: prints output
	printdebug("PRINTPTR ");
	printf("%p val %d\n",retvoid,*((int*)retvoid));
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INPUTCHAR,
#elif defined(INIT)
INPUTCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INPUTCHAR_p(){//PRINT CALLFN: prints output
	printdebug("INPUTCHAR ");
	scanf("%c",&retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INPUTINT,
#elif defined(INIT)
INPUTINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INPUTINT_p(){//PRINT CALLFN: prints output
	printdebug("INPUTINT ");
	scanf("%d",&retint);
	PC.ptr+=SIZEOFOP;
}
#endif

//#ifdef ENUM
//GETSP,
//#elif defined(INIT)
//GETSP_p,
//#elif defined(LEN)
//SIZEOFOP,
//#else
//void GETSP_p(){//PRINT CALLFN: prints output
//	printdebug("GETSP ");
//	retint=(int)SP.ptr;
//	PC.ptr+=SIZEOFOP;
//}
//#endif

#ifdef ENUM
INPUTFLOAT,
#elif defined(INIT)
INPUTFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INPUTFLOAT_p(){//PRINT CALLFN: prints output
	printdebug("INPUTFLOAT ");
	scanf("%f",&retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INPUTDOUBLE,
#elif defined(INIT)
INPUTDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INPUTDOUBLE_p(){//PRINT CALLFN: prints output
	printdebug("INPUTDOUBLE ");
	scanf("%lf",&retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INPUTPTR,
#elif defined(INIT)
INPUTPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INPUTPTR_p(){//PRINT CALLFN: prints output
	printdebug("INPUTPTR ");
	scanf("%p",&retvoid);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DEFARR,
#elif defined(INIT)
DEFARR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+2*SIZEOFINT,
#else
SIZEOFOP+2*SIZEOFINT+SIZEOFPTR*4,
#endif
#else
void DEFARR_p(){
	int elementsize;
	int offset;
	memptr a;
	//SP.ptr-=SIZEOFPTR;//we store pointer to array
	PC.ptr+=SIZEOFOP;
	offset=PC.Pu->Int;
	PC.ptr+=SIZEOFINT;	
	elementsize=PC.Pu->Int;
	a.ptr=BP.ptr+offset;//SPARRAY$

	//SP.Pu->PVoid=BP.ptr-a.Pu->Int-retint;
	PC.ptr+=SIZEOFINT;	
	SP.ptr-=retint;
	a.Pu->PVoid=SP.ptr;
#ifdef DODEBUG
	printdebug("DEFARR "); 
	printfid2();
	printf("[%d]\n",retint);
#endif
}
#endif 

#ifdef ENUM
CONST_CHAR,
#elif defined(INIT)
CONST_CHAR_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFCHAR,
#else
void CONST_CHAR_p(){//we call CONST_INT CONST(123) to supply constants
	PC.ptr+=SIZEOFOP;	
	retchar=PC.Pu->Char;
	printdebug("CONST_CHAR %c\n",retchar);
	PC.ptr+=SIZEOFCHAR;	
}
#endif

#ifdef ENUM
CONST_INT,
#elif defined(INIT)
CONST_INT_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFINT,
#else
void CONST_INT_p(){//we call CONST_INT CONST(123) to supply constants
	PC.ptr+=SIZEOFOP;	
	retint=PC.Pu->Int;
	printdebug("CONST_INT %d\n",retint);
	PC.ptr+=SIZEOFINT;	
}
#endif

#ifdef ENUM
CONST_FLOAT,
#elif defined(INIT)
CONST_FLOAT_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFFLOAT,
#else
void CONST_FLOAT_p(){
	PC.ptr+=SIZEOFOP;	
	retfloat=PC.Pu->Float;	
	printdebug("CONST_FLOAT %f\n",retfloat);
	PC.ptr+=SIZEOFFLOAT;
}
#endif

#ifdef ENUM
CONST_DOUBLE,
#elif defined(INIT)
CONST_DOUBLE_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFDOUBLE,
#else
void CONST_DOUBLE_p(){
	PC.ptr+=SIZEOFOP;
	retdouble=PC.Pu->Double;
	printdebug("CONST_DOUBLE %f\n",retdouble);
	PC.ptr+=SIZEOFDOUBLE;
}
#endif


#ifdef ENUM
PTRL,
#elif defined(INIT)
PTRL_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFPTRDIFF,
#else
void PTRL_p(){
	PC.ptr+=SIZEOFOP;
	retvoid=BP.ptr+PC.Pu->ptrd;
	printdebug("PTRL %p\n",retvoid);
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
PTRG,
#elif defined(INIT)
PTRG_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFPTR,
#else
void PTRG_p(){
	PC.ptr+=SIZEOFOP;
	retvoid=PC.Pu->PVoid;
	printdebug("PTRG %p\n",retvoid);
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
PUSHCHAR,
#elif defined(INIT)
PUSHCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PUSHCHAR_p(){//pushes retval on stack
	printdebug("PUSHCHAR ");
	SP.ptr-=SIZEOFCHAR;
	SP.Pu->Char=retchar;//initialize with 0
	printdebug("%c\n",retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PUSHINT,
#elif defined(INIT)
PUSHINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PUSHINT_p(){//pushes retval on stack
	printdebug("PUSHINT ");
	SP.ptr-=SIZEOFINT;
	SP.Pu->Int=retint;//initialize with 0
	printdebug("%d\n",retint);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PUSHFLOAT,
#elif defined(INIT)
PUSHFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PUSHFLOAT_p(){//pushes retval on stack
	printdebug("PUSHFLOAT ");
	SP.ptr-=SIZEOFFLOAT;
	SP.Pu->Float=retfloat;//initialize with 0
	printdebug("%f\n",retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PUSHDOUBLE,
#elif defined(INIT)
PUSHDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PUSHDOUBLE_p(){//pushes retval on stack
	printdebug("PUSHDOUBLE ");
	SP.ptr-=SIZEOFDOUBLE;
	SP.Pu->Double=retdouble;//initialize with 0
	printdebug("%f\n",retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PUSHPTR,
#elif defined(INIT)
PUSHPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void PUSHPTR_p(){//pushes retval on stack
	printdebug("PUSHPTR ");
	SP.ptr-=SIZEOFPTR;
	SP.Pu->PVoid=retvoid;//initialize with 0
	printdebug("%p\n",retvoid);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPPTRPUSHCHAR,
#elif defined(INIT)
POPPTRPUSHCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPPTRPUSHCHAR_p(){//pushes retval on stack
	printdebug("POPPTRPUSHCHAR ");
	retvoid=SP.Pu->PVoid;
	printdebug("%p ",retvoid);
	SP.ptr+=SIZEOFPTR;

	SP.ptr-=SIZEOFCHAR;
	SP.Pu->Char=retchar;//initialize with 0
	printdebug("%c\n",retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPPTRPUSHINT,
#elif defined(INIT)
POPPTRPUSHINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPPTRPUSHINT_p(){//pushes retval on stack
	printdebug("POPPTRPUSHINT ");
	retvoid=SP.Pu->PVoid;
	printdebug("%p ",retvoid);
	SP.ptr+=SIZEOFPTR;

	SP.ptr-=SIZEOFINT;
	SP.Pu->Int=retint;//initialize with 0
	printdebug("%d\n",retint);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPPTRPUSHFLOAT,
#elif defined(INIT)
POPPTRPUSHFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPPTRPUSHFLOAT_p(){//pushes retval on stack
	printdebug("POPPTRPUSHFLOAT ");
	retvoid=SP.Pu->PVoid;
	printdebug("%p ",retvoid);
	SP.ptr+=SIZEOFPTR;

	SP.ptr-=SIZEOFFLOAT;
	SP.Pu->Float=retfloat;//initialize with 0
	printdebug("%f\n",retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif


#ifdef ENUM
POPPTRPUSHDOUBLE,
#elif defined(INIT)
POPPTRPUSHDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPPTRPUSHDOUBLE_p(){//pushes retval on stack
	printdebug("POPPTRPUSHDOUBLE ");
	retvoid=SP.Pu->PVoid;
	printdebug("%p ",retvoid);
	SP.ptr+=SIZEOFPTR;

	SP.ptr-=SIZEOFDOUBLE;
	SP.Pu->Double=retdouble;//initialize with 0
	printdebug("%f\n",retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif


#ifdef ENUM
POPPTRPUSHPTR,
#elif defined(INIT)
POPPTRPUSHPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPPTRPUSHPTR_p(){//pushes retval on stack
	void *retvoid2=retvoid;
	printdebug("POPPTRPUSHPTR ");
	retvoid=SP.Pu->PVoid;
	printdebug("%p ",retvoid);
	//SP.ptr+=SIZEOFPTR;

	//SP.ptr-=SIZEOFPTR;
	SP.Pu->PVoid=retvoid2;//initialize with 0
	printdebug("%p\n",retvoid2);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPCHAR,
#elif defined(INIT)
POPCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPCHAR_p(){//pop values from stack
	printdebug("POPCHAR ");
	retchar=SP.Pu->Char;
	printdebug("%c\n",retchar);
	SP.ptr+=SIZEOFCHAR;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPINT,
#elif defined(INIT)
POPINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPINT_p(){//pop values from stack
	printdebug("POPINT ");
	retint=SP.Pu->Int;
	printdebug("%d\n",retint);
	SP.ptr+=SIZEOFINT;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPPTR,
#elif defined(INIT)
POPPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPPTR_p(){//pop values from stack
	printdebug("POPPTR ");
	retvoid=SP.Pu->PVoid;
	printdebug("%p\n",retvoid);
	SP.ptr+=SIZEOFPTR;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPFLOAT,
#elif defined(INIT)
POPFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPFLOAT_p(){//pop values from stack
	printdebug("POPFLOAT ");
	retfloat=SP.Pu->Float;
	printdebug("%f\n",retfloat);
	SP.ptr+=SIZEOFFLOAT;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPDOUBLE,
#elif defined(INIT)
POPDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void POPDOUBLE_p(){//pop values from stack
	printdebug("POPDOUBLE ");
	retdouble=SP.Pu->Double;
	printdebug("%f\n",retdouble);
	SP.ptr+=SIZEOFDOUBLE;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
POPN,
#elif defined(INIT)
POPN_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFINT,
#else
void POPN_p(){
	printdebug("POPN ");
	PC.ptr+=SIZEOFOP;
	SP.ptr+=PC.Pu->Int;
	printdebug("%d\n",PC.Pu->Int);
	PC.ptr+=SIZEOFINT;
}
#endif


#ifdef ENUM
PUSHN,
#elif defined(INIT)
PUSHN_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFINT,
#else
void PUSHN_p(){
	printdebug("PUSHN ");
	PC.ptr+=SIZEOFOP;
	SP.ptr-=PC.Pu->Int;
	printdebug("%d\n",PC.Pu->Int);
	PC.ptr+=SIZEOFINT;
}
#endif

#ifdef ENUM
ADDCHAR,
#elif defined(INIT)
ADDCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ADDCHAR_p(){
	char a2=retchar;
	printdebug("ADDCHAR ");
	POPCHAR_p();
	retchar+=a2;
	printdebug("%d\n",retint);
}
#endif

#ifdef ENUM
ADDPTR,
#elif defined(INIT)
ADDPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ADDPTR_p(){
	int a2=(int)retvoid;
	printdebug("ADDPTR ");
	POPPTR_p();
	retvoid=(char*)retvoid+a2;
	printdebug("%p\n",retvoid);
}
#endif

#ifdef ENUM
ADDINT,
#elif defined(INIT)
ADDINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ADDINT_p(){//adds retvalue to popped value on stack and puts result in retvalue
	int a2=retint;
	printdebug("ADDINT ");
	POPINT_p();
	retint+=a2;
	printdebug("%d\n",retint);
}
#endif

#ifdef ENUM
ADDFLOAT,
#elif defined(INIT)
ADDFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ADDFLOAT_p(){
	float a2=retfloat;
	printdebug("ADDFLOAT ");
	POPFLOAT_p();
	retfloat+=a2;
	printdebug("%f\n",retfloat);
}
#endif

#ifdef ENUM
ADDDOUBLE,
#elif defined(INIT)
ADDDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ADDDOUBLE_p(){
	double a2=retdouble;
	printdebug("ADDDOUBLE ");
	POPDOUBLE_p();
	retdouble+=a2;
	printdebug("%f\n",retdouble);		
}
#endif

#ifdef ENUM
CHAR2INT,
#elif defined(INIT)
CHAR2INT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void CHAR2INT_p(){
	printdebug("CHAR2INT ");
	retint=(int)retchar;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
CHAR2FLOAT,
#elif defined(INIT)
CHAR2FLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void CHAR2FLOAT_p(){
	printdebug("CHAR2FLOAT ");
	retfloat=(float)retchar;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
CHAR2DOUBLE,
#elif defined(INIT)
CHAR2DOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void CHAR2DOUBLE_p(){
	printdebug("CHAR2DOUBLE ");
	retdouble=(double)retchar;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INT2PTR,
#elif defined(INIT)
INT2PTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INT2PTR_p(){
	printdebug("INT2PTR ");
	retvoid=(void*)retint;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INT2CHAR,
#elif defined(INIT)
INT2CHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INT2CHAR_p(){
	printdebug("INT2CHAR ");
	retchar=(char)retint;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INT2FLOAT,
#elif defined(INIT)
INT2FLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INT2FLOAT_p(){
	printdebug("INT2FLOAT ");
	retfloat=(float)retint;
	PC.ptr+=SIZEOFOP;
}
#endif


#ifdef ENUM
INT2DOUBLE,
#elif defined(INIT)
INT2DOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INT2DOUBLE_p(){
	printdebug("INT2DOUBLE ");
	retdouble=retint;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
FLOAT2CHAR,
#elif defined(INIT)
FLOAT2CHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void FLOAT2CHAR_p(){
	printdebug("FLOAT2CHAR ");
	retchar=(char)retfloat;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
FLOAT2INT,
#elif defined(INIT)
FLOAT2INT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void FLOAT2INT_p(){
	printdebug("FLOAT2INT ");
	retint=(int)retfloat;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
FLOAT2DOUBLE,
#elif defined(INIT)
FLOAT2DOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void FLOAT2DOUBLE_p(){
	printdebug("FLOAT2DOUBLE ");
	retdouble=retfloat;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DOUBLE2CHAR,
#elif defined(INIT)
DOUBLE2CHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DOUBLE2CHAR_p(){
	printdebug("DOUBLE2CHAR ");
	retchar=(char)retdouble;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DOUBLE2INT,
#elif defined(INIT)
DOUBLE2INT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DOUBLE2INT_p(){
	printdebug("DOUBLE2INT ");
	retint=(int)retdouble;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DOUBLE2FLOAT,
#elif defined(INIT)
DOUBLE2FLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DOUBLE2FLOAT_p(){
	printdebug("DOUBLE2FLOAT ");
	retfloat=(float)retdouble;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INCCHAR,
#elif defined(INIT)
INCCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INCCHAR_p(){
	printdebug("INCCHAR ");
	++retchar;
	printdebug("%c\n",retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INCINT,
#elif defined(INIT)
INCINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INCINT_p(){
	printdebug("INCINT ");
	++retint;
	printdebug("%d\n",retint);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INCFLOAT,
#elif defined(INIT)
INCFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INCFLOAT_p(){
	printdebug("INCFLOAT ");
	++retfloat;
	printdebug("%f\n",retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INCDOUBLE,
#elif defined(INIT)
INCDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INCDOUBLE_p(){
	printdebug("INCDOUBLE ");
	++retdouble;
	printdebug("%f\n",retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
INCPTR,
#elif defined(INIT)
INCPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void INCPTR_p(){
	printdebug("INCPTR ");
	retvoid =(char*)retvoid+SIZEOFCHAR;
	printdebug("%p\n",retvoid);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DECCHAR,
#elif defined(INIT)
DECCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DECCHAR_p(){
	printdebug("DECCHAR ");
	retchar--;
	printdebug("%c\n",retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DECINT,
#elif defined(INIT)
DECINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DECINT_p(){
	printdebug("DECINT ");
	retint--;
	printdebug("%d\n",retint);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DECFLOAT,
#elif defined(INIT)
DECFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DECFLOAT_p(){
	printdebug("DECFLOAT ");
	retfloat--;
	printdebug("%f\n",retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DECDOUBLE,
#elif defined(INIT)
DECDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DECDOUBLE_p(){
	printdebug("DECDOUBLE ");
	retdouble--;
	printdebug("%f\n",retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
DECPTR,
#elif defined(INIT)
DECPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DECPTR_p(){
	printdebug("DECPTR ");
	retvoid=(char*)retvoid-SIZEOFCHAR;
	printdebug("%p\n",retvoid);
	PC.ptr+=SIZEOFOP;
}
#endif


#ifdef ENUM
NOTEQCHAR,
#elif defined(INIT)
NOTEQCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOTEQCHAR_p(){
	int a2=retchar;
	printdebug("NOTEQCHAR ");
	POPCHAR_p();
	retint= retchar!=a2;
}
#endif

#ifdef ENUM
NOTEQPTR,
#elif defined(INIT)
NOTEQPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOTEQPTR_p(){
	void* a2=retvoid;
	printdebug("NOTEQPTR ");
	POPPTR_p();
	retint= retvoid!=a2;
}
#endif

#ifdef ENUM
NOTEQINT,
#elif defined(INIT)
NOTEQINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOTEQINT_p(){
	int a2=retint;
	printdebug("NOTEQINT ");
	POPINT_p();
	retint= retint!=a2;
}
#endif

#ifdef ENUM
NOTEQFLOAT,
#elif defined(INIT)
NOTEQFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOTEQFLOAT_p(){
	float a2=retfloat;
	printdebug("NOTEQFLOAT ");
	POPFLOAT_p();
	retint= retfloat!=a2;
}
#endif

#ifdef ENUM
NOTEQDOUBLE,
#elif defined(INIT)
NOTEQDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOTEQDOUBLE_p(){
	double a2=retdouble;
	printdebug("NOTEQDOUBLE ");
	POPDOUBLE_p();
	retint= retdouble!=a2;
}
#endif

#ifdef ENUM
EQEQCHAR,
#elif defined(INIT)
EQEQCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void EQEQCHAR_p(){
	int a2=retchar;
	printdebug("EQEQCHAR ");
	POPCHAR_p();
	retint= retchar==a2;
}
#endif

#ifdef ENUM
EQEQPTR,
#elif defined(INIT)
EQEQPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void EQEQPTR_p(){
	void* a2=retvoid;
	printdebug("EQEQPTR ");
	POPPTR_p();
	retint= retvoid==a2;
}
#endif

#ifdef ENUM
EQEQINT,
#elif defined(INIT)
EQEQINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void EQEQINT_p(){
	int a2=retint;
	printdebug("EQEQINT ");
	POPINT_p();
	retint= retint==a2;
}
#endif

#ifdef ENUM
EQEQFLOAT,
#elif defined(INIT)
EQEQFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void EQEQFLOAT_p(){
	float a2=retfloat;
	printdebug("EQEQFLOAT ");
	POPFLOAT_p();
	retint= retfloat==a2;
}
#endif

#ifdef ENUM
EQEQDOUBLE,
#elif defined(INIT)
EQEQDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void EQEQDOUBLE_p(){
	double a2=retdouble;
	printdebug("EQEQDOUBLE ");
	POPDOUBLE_p();
	retint= retdouble==a2;
}
#endif

#ifdef ENUM
LTCHAR,
#elif defined(INIT)
LTCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTCHAR_p(){
	int a2=retchar;
	printdebug("LTCHAR ");
	POPCHAR_p();
	retint= retchar<a2;
}
#endif

#ifdef ENUM
LTPTR,
#elif defined(INIT)
LTPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTPTR_p(){
	void *a2=retvoid;
	printdebug("LTPTR ");
	POPPTR_p();
	retint= retvoid<a2;
}
#endif

#ifdef ENUM
LTINT,
#elif defined(INIT)
LTINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTINT_p(){
	int a2=retint;
	printdebug("LTINT ");
	POPINT_p();
	retint= retint<a2;
}
#endif

#ifdef ENUM
LTFLOAT,
#elif defined(INIT)
LTFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTFLOAT_p(){
	float a2=retfloat;
	printdebug("LTFLOAT ");
	POPFLOAT_p();
	retint= retfloat<a2;
}
#endif

#ifdef ENUM
LTDOUBLE,
#elif defined(INIT)
LTDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTDOUBLE_p(){
	double a2=retdouble;
	printdebug("LTDOUBLE ");
	POPDOUBLE_p();
	retint= retdouble<a2;
}
#endif

#ifdef ENUM
GTCHAR,
#elif defined(INIT)
GTCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTCHAR_p(){
	int a2=retchar;
	printdebug("GTCHAR ");
	POPCHAR_p();
	retint= retchar>a2;
}
#endif

#ifdef ENUM
GTPTR,
#elif defined(INIT)
GTPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTPTR_p(){
	void* a2=retvoid;
	printdebug("GTPTR ");
	POPPTR_p();
	retint= retvoid>a2;
}
#endif

#ifdef ENUM
GTINT,
#elif defined(INIT)
GTINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTINT_p(){
	int a2=retint;
	printdebug("GTINT ");
	POPINT_p();
	retint= retint>a2;
}
#endif

#ifdef ENUM
GTFLOAT,
#elif defined(INIT)
GTFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTFLOAT_p(){
	float a2=retfloat;
	printdebug("GTFLOAT ");
	POPFLOAT_p();
	retint= retfloat>a2;
}
#endif

#ifdef ENUM
GTDOUBLE,
#elif defined(INIT)
GTDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTDOUBLE_p(){
	double a2=retdouble;
	printdebug("GTDOUBLE ");
	POPDOUBLE_p();
	retint= retdouble>a2;
}
#endif

#ifdef ENUM
LTEQCHAR,
#elif defined(INIT)
LTEQCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTEQCHAR_p(){
	int a2=retchar;
	printdebug("LTEQCHAR ");
	POPCHAR_p();
	retint= retchar<=a2;
}
#endif

#ifdef ENUM
LTEQPTR,
#elif defined(INIT)
LTEQPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTEQPTR_p(){
	void* a2=retvoid;
	printdebug("LTEQPTR ");
	POPPTR_p();
	retint= retvoid<=a2;
}
#endif

#ifdef ENUM
LTEQINT,
#elif defined(INIT)
LTEQINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTEQINT_p(){
	int a2=retint;
	printdebug("LTEQINT ");
	POPINT_p();
	retint= retint<=a2;
}
#endif

#ifdef ENUM
LTEQFLOAT,
#elif defined(INIT)
LTEQFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTEQFLOAT_p(){
	float a2=retfloat;
	printdebug("LTEQFLOAT ");
	POPFLOAT_p();
	retint= retfloat<=a2;
}
#endif

#ifdef ENUM
LTEQDOUBLE,
#elif defined(INIT)
LTEQDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void LTEQDOUBLE_p(){
	double a2=retdouble;
	printdebug("LTEQDOUBLE ");
	POPDOUBLE_p();
	retint= retdouble<=a2;
}
#endif

#ifdef ENUM
GTEQCHAR,
#elif defined(INIT)
GTEQCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTEQCHAR_p(){
	int a2=retchar;
	printdebug("GTEQCHAR ");
	POPCHAR_p();
	retint= retchar>=a2;
}
#endif

#ifdef ENUM
GTEQPTR,
#elif defined(INIT)
GTEQPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTEQPTR_p(){
	void *a2=retvoid;
	printdebug("GTEQPTR ");
	POPPTR_p();
	retint= retvoid>=a2;
}
#endif

#ifdef ENUM
GTEQINT,
#elif defined(INIT)
GTEQINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTEQINT_p(){
	int a2=retint;
	printdebug("GTEQINT ");
	POPINT_p();
	retint= retint>=a2;
}
#endif

#ifdef ENUM
GTEQFLOAT,
#elif defined(INIT)
GTEQFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTEQFLOAT_p(){
	float a2=retfloat;
	printdebug("GTEQFLOAT ");
	POPFLOAT_p();
	retint= retfloat>=a2;
}
#endif

#ifdef ENUM
GTEQDOUBLE,
#elif defined(INIT)
GTEQDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void GTEQDOUBLE_p(){
	double a2=retdouble;
	printdebug("GTEQDOUBLE ");
	POPDOUBLE_p();
	retint= retdouble>=a2;
}
#endif

#ifdef ENUM
NOT,
#elif defined(INIT)
NOT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOT_p(){//negates retint
	printdebug("NOT\n");
	retint=!retint;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
SUBCHAR,
#elif defined(INIT)
SUBCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SUBCHAR_p(){
	int a2=retchar;
	printdebug("SUBCHAR ");
	POPCHAR_p();
	retchar-=a2;
	printdebug("%c\n",retchar);
}
#endif

#ifdef ENUM
SUBPTR,
#elif defined(INIT)
SUBPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SUBPTR_p(){
	int a2=(int)retvoid;
	printdebug("SUBPTR ");
	POPPTR_p();
	retvoid=(char*)retvoid-a2;
	printdebug("%p\n",retvoid);
}
#endif

#ifdef ENUM
SUBINT,
#elif defined(INIT)
SUBINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SUBINT_p(){
	int a2=retint;
	printdebug("SUBINT ");
	POPINT_p();
	retint-=a2;
	printdebug("%d\n",retint);
}
#endif

#ifdef ENUM
SUBFLOAT,
#elif defined(INIT)
SUBFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SUBFLOAT_p(){
	float a2=retfloat;
	printdebug("SUBFLOAT ");
	POPFLOAT_p();
	retfloat-=a2;
	printdebug("%f\n",retfloat);
}
#endif

#ifdef ENUM
SUBDOUBLE,
#elif defined(INIT)
SUBDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SUBDOUBLE_p(){
	double a2=retdouble;
	printdebug("SUBDOUBLE ");
	POPDOUBLE_p();
	retdouble-=a2;
	printdebug("%f\n",retdouble);
}
#endif

#ifdef ENUM
MULCHAR,
#elif defined(INIT)
MULCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void MULCHAR_p(){
	int a2=retchar;
	printdebug("MULCHAR ");
	POPCHAR_p();
	retchar*=a2;
	printdebug("%c\n",retchar);
}
#endif

#ifdef ENUM
MULINT,
#elif defined(INIT)
MULINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void MULINT_p(){
	int a2=retint;
	printdebug("MULINT ");
	POPINT_p();
	retint*=a2;
	printdebug("%d\n",retint);
}
#endif

#ifdef ENUM
MULFLOAT,
#elif defined(INIT)
MULFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void MULFLOAT_p(){
	float a2=retfloat;
	printdebug("MULFLOAT ");
	POPFLOAT_p();
	retfloat*=a2;
	printdebug("%f\n",retfloat);
}
#endif

#ifdef ENUM
MULDOUBLE,
#elif defined(INIT)
MULDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void MULDOUBLE_p(){
	double a2=retdouble;
	printdebug("MULDOUBLE ");
	POPDOUBLE_p();	
	retdouble*=a2;
	printdebug("%f\n",retdouble);		
}
#endif

#ifdef ENUM
DIVCHAR,
#elif defined(INIT)
DIVCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DIVCHAR_p(){
	int a2=retchar;
	printdebug("DIVCHAR ");
	POPCHAR_p();
 	retchar/=a2;
	printdebug("%c\n",retchar);
}
#endif

#ifdef ENUM
DIVINT,
#elif defined(INIT)
DIVINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DIVINT_p(){
	int a2=retint;
	printdebug("DIVINT ");
	POPINT_p();
 	retint/=a2;
	printdebug("%d\n",retint);
}
#endif

#ifdef ENUM
DIVFLOAT,
#elif defined(INIT)
DIVFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DIVFLOAT_p(){
	float a2=retfloat;
	printdebug("DIVFLOAT ");
	POPFLOAT_p();
	retfloat/=a2;
	printdebug("%f\n",retfloat);
}
#endif

#ifdef ENUM
DIVDOUBLE,
#elif defined(INIT)
DIVDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DIVDOUBLE_p(){
	double a2=retdouble;
	printdebug(" ");
	POPDOUBLE_p();	
	retdouble/=a2;
	printdebug("%f\n",retdouble);		
}
#endif

#ifdef ENUM
NOP,
#elif defined(INIT)
NOP_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NOP_p(){//no operation
	printdebug("NOP\n");
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
ENDP,
#elif defined(INIT)
ENDP_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ENDP_p(){//End Of Program //an and at the end of program terminates the initialization
	PC.ptr+=SIZEOFOP;
	RETURN_VAL_=-1;
	doexecuteloop=0;
}	
#endif

#ifdef ENUM
SETLSP,
#elif defined(INIT)
SETLSP_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFINT,
#else
void SETLSP_p(){
	int a;
	PC.ptr+=SIZEOFOP;	
	a=PC.Pu->Int;
	printdebug("SETLSP %d\n",a);
	PC.ptr+=SIZEOFINT;	
	SP.ptr=BP.ptr-a;
}
#endif

#ifdef ENUM
ENDFN,
#elif defined(INIT)
ENDFN_p,
#elif defined(LEN)
SIZEOFOP,
#else
void ENDFN_p(){
	printdebug("ENDFN");
	do_ret();
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
BEGINFN,
#elif defined(INIT)
BEGINFN_p,
#elif defined(LEN)
SIZEOFOP,
#else
void BEGINFN_p(){//local entry
	printdebug("BEGINFN\n");
	BP.ptr=SP.ptr;
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
PAR,
#elif defined(INIT)
PAR_p,
#ifdef INIT
#endif
#elif defined(LEN)
SIZEOFOP,
#else
void PAR_p(){//with this we define a variable which is a parameter: PAR name type          not yet offset: ksksks todo typecast
	printdebug("PAR \n");
	PC.ptr+=SIZEOFOP;
}
#endif



///ksksks
#ifdef ENUM
SETLCHAR,
#elif defined(INIT)
SETLCHAR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void SETLCHAR_p(){//!!!!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETLCHAR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	a.Pu->Char=retchar;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
SETLINT,
#elif defined(INIT)
SETLINT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void SETLINT_p(){//!!!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETLINT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	a.Pu->Int=retint;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
SETLPTR,
#elif defined(INIT)
SETLPTR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void SETLPTR_p(){//!!!!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETLPTR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	a.Pu->PVoid=retvoid;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
SETLFLOAT,
#elif defined(INIT)
SETLFLOAT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void SETLFLOAT_p(){//!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETLFLOAT ");	
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	a.Pu->Float=retfloat;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
SETLDOUBLE,
#elif defined(INIT)
SETLDOUBLE_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void SETLDOUBLE_p(){//!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETLDOUBLE ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	a.Pu->Double=retdouble;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
SETPTRCHAR,
#elif defined(INIT)
SETPTRCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SETPTRCHAR_p(){
	//PC.ptr+=SIZEOFOP;
	printdebug("SETPTRCHAR\n");
	POPCHAR_p();
	*((char *)retvoid)=retchar;
}
#endif

#ifdef ENUM
SETPTRINT,
#elif defined(INIT)
SETPTRINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SETPTRINT_p(){
	//PC.ptr+=SIZEOFOP;
	printdebug("SETPTRINT\n");
	POPINT_p();
	*((int *)retvoid)=retint;
}
#endif

#ifdef ENUM
SETPTRFLOAT,
#elif defined(INIT)
SETPTRFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SETPTRFLOAT_p(){
	//PC.ptr+=SIZEOFOP;
	printdebug("SETPTRFLOAT\n");
	POPFLOAT_p();
	*((float *)retvoid)=retfloat;
}
#endif

#ifdef ENUM
SETPTRDOUBLE,
#elif defined(INIT)
SETPTRDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SETPTRDOUBLE_p(){
	//PC.ptr+=SIZEOFOP;
	printdebug("SETPTRDOUBLE\n");
	POPDOUBLE_p();
	*((double *)retvoid)=retdouble;
}
#endif

#ifdef ENUM
SETPTRPTR,
#elif defined(INIT)
SETPTRPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void SETPTRPTR_p(){//TODO here is obviously a problem
	void *p;
	//PC.ptr+=SIZEOFOP;
	printdebug("SETPTRPTR\n");
	p=retvoid;
	POPPTR_p();
	*((void **)p)=retvoid;
}
#endif

#ifdef ENUM
SETGCHAR,
#elif defined(INIT)
SETGCHAR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGCHAR_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGCHAR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	a.Pu->Int=retchar;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGPTR,
#elif defined(INIT)
SETGPTR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGPTR_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGPTR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	a.Pu->PVoid=retvoid;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGPTRCHAR,
#elif defined(INIT)
SETGPTRCHAR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGPTRCHAR_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGPTRCHAR ");
	printfid2();
	printdebug("\n");
	//PC.ptr+=SIZEOFPTR;//!!!!XXXXXXXXXXXXXXX
#endif
	a.ptr=PC.Pu->Str;
	*((char *)a.Pu->PVoid)=retchar;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGPTRINT,
#elif defined(INIT)
SETGPTRINT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGPTRINT_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGPTRINT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	*((int *)a.Pu->PVoid)=retint;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGPTRFLOAT,
#elif defined(INIT)
SETGPTRFLOAT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGPTRFLOAT_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGPTRFLOAT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	*((float *)a.Pu->PVoid)=retfloat;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGPTRDOUBLE,
#elif defined(INIT)
SETGPTRDOUBLE_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGPTRDOUBLE_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGPTRDOUBLE ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	*((double *)a.Pu->PVoid)=retdouble;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGPTRPTR,
#elif defined(INIT)
SETGPTRPTR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGPTRPTR_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGPTRPTR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	*((void **)a.ptr)=retvoid;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGINT,
#elif defined(INIT)
SETGINT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGINT_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGINT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	a.Pu->Int=retint;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGFLOAT,
#elif defined(INIT)
SETGFLOAT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGFLOAT_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGFLOAT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	a.Pu->Float=retfloat;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
SETGDOUBLE,
#elif defined(INIT)
SETGDOUBLE_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void SETGDOUBLE_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("SETGDOUBLE ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	a.Pu->Double=retdouble;
	PC.ptr+=SIZEOFPTR;
}
#endif







#ifdef ENUM
GETLCHAR,
#elif defined(INIT)
GETLCHAR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void GETLCHAR_p(){//!!!!!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETLCHAR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	retchar=a.Pu->Char;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
GETLINT,
#elif defined(INIT)
GETLINT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void GETLINT_p(){//!!!!!!!!!!!!!!1
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETLINT ");
	printfid2();
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	printdebug(" @ %p\n",a.ptr);
	retint=a.Pu->Int;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
GETLFLOAT,
#elif defined(INIT)
GETLFLOAT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void GETLFLOAT_p(){//!!!!!!!!!!!!!!!!1
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETLFLOAT ");
	printfid2();
	printdebug("\n");
	//PC.ptr+=SIZEOFPTR;!!!!!!!!!!!!!!!!!!
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	retfloat=a.Pu->Float;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
GETLDOUBLE,
#elif defined(INIT)
GETLDOUBLE_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void GETLDOUBLE_p(){//!!!!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETLDOUBLE ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	retdouble=a.Pu->Double;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!
}
#endif

#ifdef ENUM
GETLPTR,
#elif defined(INIT)
GETLPTR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTRDIFF,
#else
SIZEOFOP+SIZEOFPTRDIFF+SIZEOFPTR*4,
#endif
#else
void GETLPTR_p(){//!!!!!!!!!!
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETLPTR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=BP.ptr+PC.Pu->ptrd;
	retvoid=a.Pu->Pu;
	PC.ptr+=SIZEOFPTRDIFF;
	//PC.ptr+=SIZEOFINT;!!!!
}
#endif

#ifdef ENUM
DEREFCHAR,
#elif defined(INIT)
DEREFCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DEREFCHAR_p(){
	PC.ptr+=SIZEOFOP;
	retchar=*((char*)retvoid);
	printdebug("DEREFCHAR%c\n",retchar);
}
#endif

#ifdef ENUM
DEREFINT,
#elif defined(INIT)
DEREFINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DEREFINT_p(){
	PC.ptr+=SIZEOFOP;
	retint=*((int*)retvoid);
	printdebug("DEREFINT%d\n",retint);
}
#endif

#ifdef ENUM
DEREFFLOAT,
#elif defined(INIT)
DEREFFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DEREFFLOAT_p(){
	PC.ptr+=SIZEOFOP;
	retfloat=*((float*)retvoid);
	printdebug("DEREFFLOAT%f\n",retfloat);
}
#endif

#ifdef ENUM
DEREFDOUBLE,
#elif defined(INIT)
DEREFDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DEREFDOUBLE_p(){
	PC.ptr+=SIZEOFOP;
	retdouble=*((double*)retvoid);
	printdebug("DEREFDOUBLE%f\n",retdouble);
}
#endif

#ifdef ENUM
DEREFPTR,
#elif defined(INIT)
DEREFPTR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void DEREFPTR_p(){
	PC.ptr+=SIZEOFOP;
	retvoid=*((void**)retvoid);
	printdebug("DEREFPTR%p\n",retvoid);
}
#endif

#ifdef ENUM
GETGCHAR,
#elif defined(INIT)
GETGCHAR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void GETGCHAR_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETGCHAR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	retchar=a.Pu->Char;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
GETGINT,
#elif defined(INIT)
GETGINT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void GETGINT_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETGINT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	printdebug(" @ %p\n",a.ptr);
	retint=a.Pu->Int;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
GETGFLOAT,
#elif defined(INIT)
GETGFLOAT_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void GETGFLOAT_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETGFLOAT ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	retfloat=a.Pu->Float;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
GETGDOUBLE,
#elif defined(INIT)
GETGDOUBLE_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void GETGDOUBLE_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETGDOUBLE ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	retdouble=a.Pu->Double;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
GETGPTR,
#elif defined(INIT)
GETGPTR_p,
#elif defined(LEN)
#ifndef DODEBUG
SIZEOFOP+SIZEOFPTR,
#else
SIZEOFOP+2*SIZEOFPTR*4,
#endif
#else
void GETGPTR_p(){
	memptr a;
	PC.ptr+=SIZEOFOP;
#ifdef DODEBUG
	printdebug("GETGPTR ");
	printfid2();
	printdebug("\n");
#endif
	a.ptr=PC.Pu->Str;
	retvoid=a.Pu->PVoid;
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
NEGCHAR,
#elif defined(INIT)
NEGCHAR_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NEGCHAR_p(){
	printdebug("NEGCHAR ");
	retchar=-retchar;
	printdebug("%c\n",retchar);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
NEGINT,
#elif defined(INIT)
NEGINT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NEGINT_p(){
	printdebug("NEGINT ");
	retint=-retint;
	printdebug("%d\n",retint);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
NEGFLOAT,
#elif defined(INIT)
NEGFLOAT_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NEGFLOAT_p(){
	printdebug("NEGFLOAT ");
	retfloat=-retfloat;
	printdebug("%f\n",retfloat);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
NEGDOUBLE,
#elif defined(INIT)
NEGDOUBLE_p,
#elif defined(LEN)
SIZEOFOP,
#else
void NEGDOUBLE_p(){
	printdebug("NEGDOUBLE ");
	retdouble=-retdouble;
	printdebug("%f\n",retdouble);
	PC.ptr+=SIZEOFOP;
}
#endif

#ifdef ENUM
RET,
#elif defined(INIT)
RET_p,
#elif defined(LEN)
SIZEOFOP,
#else
void RET_p(){//RET returns
	printdebug("RET \n");
	do_ret();
	PC.ptr+=SIZEOFPTR;
}
#endif

#ifdef ENUM
JMP,
#elif defined(INIT)
JMP_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFPTRDIFF,
#else
void JMP_p(){
	memptr PC2=PC;
	printdebug("JMP ");
	PC.ptr+=SIZEOFOP;
	printdebug("%d\n",PC.Pu->Int); 	
	PC.ptr=PC2.ptr+PC.Pu->ptrd+SIZEOFOP;
}
#endif

#ifdef ENUM
IFJMP,
#elif defined(INIT)
IFJMP_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFPTRDIFF,
#else
void IFJMP_p(){
	memptr PC2=PC;
	int condition=0;
	printdebug("IFJMP ");
	PC.ptr+=SIZEOFOP;
	if(retint) condition=1;
	printdebug("%d\n",PC.Pu->Int);
	if(condition) PC.ptr=PC2.ptr+PC.Pu->ptrd+SIZEOFOP;
	else PC.ptr+=SIZEOFPTRDIFF;
}
#endif

#ifdef ENUM
IFNOTJMP,
#elif defined(INIT)
IFNOTJMP_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFPTRDIFF,
#else
void IFNOTJMP_p(){
	memptr PC2 = PC;
	int condition = 0;
	printdebug("IFNOTJMP ");
	PC.ptr += SIZEOFOP;
	if (retint) condition = 1;
	printdebug("%d\n", PC.Pu->Int);
	if (!condition) PC.ptr = PC2.ptr + PC.Pu->ptrd + SIZEOFOP;
	else PC.ptr += SIZEOFPTRDIFF;
}
#endif

#ifdef ENUM
CALL,
#elif defined(INIT)
CALL_p,
#elif defined(LEN)
SIZEOFOP+SIZEOFPTR,
#else
void CALL_p(){
	printdebug("CALL_______________\n");
	if(SP.ptr<=stack) {
		printf("Stackoverflow\n");
		RETURN_VAL_=-1;
		doexecuteloop=0;
		return;
	}
	SP.ptr-=SIZEOFPTR;SP.Pu->PVoid=BP.ptr;
	SP.ptr-=SIZEOFPTR;
	PC.ptr+=SIZEOFOP;;
	SP.Pu->PVoid=PC.ptr;
	PC.ptr=PC.Pu->Str;
}
#endif