# LOGIC - small c compiler with virtual machine

This is a proof of concept of a small c compiler with virtual machine. It can handly c99 standard except for structs and unions. It was written as an educational project to understand compiler design and to understand the design of the c programming language.

//Roadmap:
//preprocessor could be run at the successively
//this has advantage that backtracking is easy with
//kstring as we can be sure that there are no inserts after the 
//current position
//implement typecasts
//implement typecasts of pointers
//now we have unions for free
//then implement structs as arrays and use typecasts
//but a big difference is when passing by value a struct all elements are copied.
//passing an array by value is not possible in c



//search for !GOTO! to find places to be patched
//BECAUSE WE HAVE NO BEGIN AND END ANYMORE WE NEED NO PATCHING AND INSERTING
//use mnelen array to parse over emited code

//http://www.coding-guidelines.com/cbook/cbook1_1.pdf
//http://c0x.coding-guidelines.com/index.html
//we should compare types of forward def
//we use the pointer of  Identinfo in ISP to chain forward declarations and static variables and labels for jump, also take care to adjust stack
//if we find a jump where label is yet unknown we put it into the isp and when label is finally found we can go backwards the chain to patch the gotos or break continue
//also switch will be implemented with goto
//on isp cleanup we normally cleanup
//then we traverse the stack in the oposite direction
//and move all elements to be kept to the beginning
//
//todo testing interface which checks for right results
//static local variables should be properly handled
//
//emit(p,CONSTINT,0)
//emitint(p,20,1); if last value is 1 then we call the code to emit machine code, printit text ...
//
//we should handle goto labels, then while loop, usw also break continue can be handled
//it should also use the ISP
//handling other machine code can be done immediately meaning we can immediately output machine code eg x86
//when postponed code which cannot be immediately translated due to missing infos, then kvm code should be shifted at end of memory
//then other machine code grows from beginning of memory
//while kvm shrinks
//
//constant values should be in place evaluated by parser by calling the interpreter
//the ISP stores now also the value in case of const expressions
//the code genereator should modify addresses for const variables such
//that the interpreter during parsing can get the right values.
//
//printit should only output text to pc
//
//
//todo ptr=ptr+1 should not use addint

//KSVM Virtual Machine for fast execution
//And c99 compatible compiler into KSVM mnemonic
//Copyright 4.9.2012 by Kilian Singer
//email@kilian-singer.de
//All rights reserved.

//todo
//error reporting should correct for expecting ... errors
//and ignore unknown identifier errors.

//datastructure: we have a permanent tag on ISP
//if marked amount structure is memorized in permanentISP
//then upon pop the structure is kept on stack
//and when other elements are pushed structure is first relocated and then new element is pushed
//this compactifies the permanent data until upon empty stack
//all elements are compressed on stackend
//pointers to permanent objects are valid until we increment isp beyond the permanent obj
//then valid pointers can only obtained when fully compressed stack is kept
//if pointer is needed after compression we store the value of permanentISP which is a virtual isp decremented and incremented only for permanent object
//then after all unpermanent elements are removed we have fully valid pointers
//this can be used for all strings for debug
//but also for future char s[] and array initializers.

//we should store strings on ISP with size last and string first and alignement, but the pointer in hashmap is on the string.
//also avoid double storage in identifier2string by looking in hashmap and reusing the string. do not use local var strings for global vars and function (this should not happen due to design anyways). if it is not hashmap we store the pointer to the string in i->identifier; such that upon inserting hash we can use this pointer. then we should null i->identifier.
//when we delete a hash we should only delete string when ISP is > than stringadr which is always the case due to design.
//so when ISP is increased (stack emptied) we have to watch for string size stuff to properly decrement stack.
//Problem is when we use many identifiers, like in function definition, thereis functionname and params then this tempstring thing fails!
//DEBUG mode must permanently store strings! no idea how to do this
//also shifting the strings is a problem in debug mode
//forward def: we have to store calls to forward functions on stack ISP and also make a hash entry such as(FORWARD$$) Then on stack cleanup we have to start from IBP until last ISP and if we have a forward def we have to put it right after the function adress on stack. When everything is finished we go through the FORWARD$$ hash list and correct the adresses.
//break continue here we also store it on stack ISP if we do not know adress yet. Then on stack cleanup of for loop or while do loop we can correct adresses
//struct
//fnptr
//error:
//int a;
//a(3)// is not detected
//
//todo for a++ and 8++ findout when we have ltype and rtype
//todo int i;float i; does not cause error

//also we have to put on ILBP the current break and continue jump positions and all break and continue commands such that on END we can adjust the jump positions
// gcc -fms-extensions logic.c  in linux
// gcc -m32 -fms-extensions logic.c if you want 32bit mode in linux

//#include "stdafx.h" we use precompiled headers with Fi force include option


//we would like to be able to insert strings without copying such as:
//abcdefghijklmn
//by inserting  0 and pointer (or offset)
//abcd0xxxxjklmn
//becomes
//abcd12345efghijk
//where xxxx is pointing to 12345efgh0yyyy
//where yyyy is pointing backwards

//we need a call like
//capture(fn*) which returns the captured string boundaries but does not emit code.
//this in combination with the string insertion code can then be used to generate makros and emitexpression(p,jump_statement,"goto $break;",id) 


========================================================================
/*
If one alternative is an initial substring of another alternative, the longer alternative should be attempted first.
Avoid left recursion!
translation-unit:
        external-declaration
        translation-unit external-declaration
external-declaration:
        function-definition
        declaration
function-definition:
        declaration-specifiers declarator declaration-list? compound-statement
declaration-specifiers:
        storage-class-specifier declaration-specifiers?
        type-specifier declaration-specifiers?
        type-qualifier declaration-specifiers?
        function-specifier declaration-specifiers?
storage-class-specifier:
        "typedef"
        "extern"
        "static"
        "auto"
        "register"
type-specifier:
        "void"
        "char"
        "short"
        "int"
        "long"
        "float"
        "double"
        "signed"
        "unsigned"
        "_int"
        "_Complex"
        struct-or-union-specifier
        enum-specifier
        typedef-name
struct-or-union-specifier:
        struct-or-union identifier? "{" struct-declaration-list "}"
        struct-or-union identifier
struct-or-union:
        "struct"
        "union"
struct-declaration-list:
        struct-declaration
        struct-declaration-list struct-declaration
struct-declaration:
        specifier-qualifier-list struct-declarator-list ";"
specifier-qualifier-list:
        type-specifier specifier-qualifier-list?
        type-qualifier specifier-qualifier-list?
type-qualifier:
        "const"
        "restrict"
        "volatile"
struct-declarator-list:
        struct-declarator
        struct-declarator-list "," struct-declarator
struct-declarator:
        declarator
        declarator? ":" constant-expression
declarator:
        pointer? direct-declarator
pointer:
        "*" type-qualifier-list?
        "*" type-qualifier-list? pointer
type-qualifier-list:
        type-qualifier
        type-qualifier-list type-qualifier
direct-declarator:
        identifier
        "(" declarator ")"
        direct-declarator "[" type-qualifier-list? assignment-expression? "]"
        direct-declarator "[" "static" type-qualifier-list? assignment-expression "]"
        direct-declarator "[" type-qualifier-list "static" assignment-expression "]"
        direct-declarator "[" type-qualifier-list? "*" "]"
        direct-declarator "(" parameter-type-list ")"
        direct-declarator "(" identifier-list? ")"
assignment-expression:
        conditional-expression
        unary-expression assignment-operator assignment-expression
conditional-expression:
        logical-OR-expression
        logical-OR-expression "?" expression ":" conditional-expression
logical-OR-expression:
        logical-AND-expression
        logical-OR-expression "||" logical-AND-expression
logical-AND-expression:
        inclusive-OR-expression
        logical-AND-expression "&&" inclusive-OR-expression
inclusive-OR-expression:
        exclusive-OR-expression
        inclusive-OR-expression "|" exclusive-OR-expression
exclusive-OR-expression:
        AND-expression
        exclusive-OR-expression "^" AND-expression
AND-expression:
        equality-expression
        AND-expression "&" equality-expression
equality-expression:
        relational-expression
        equality-expression "==" relational-expression
        equality-expression "!=" relational-expression
relational-expression:
        shift-expression
        relational-expression "<" shift-expression
        relational-expression ">" shift-expression
        relational-expression "<=" shift-expression
        relational-expression ">=" shift-expression
shift-expression:
        additive-expression
        shift-expression "<<" additive-expression
        shift-expression ">>" additive-expression
additive-expression:
        multiplicative-expression
        additive-expression "+" multiplicative-expression
        additive-expression "-" multiplicative-expression
multiplicative-expression:
        cast-expression
        multiplicative-expression "*" cast-expression
        multiplicative-expression "/" cast-expression
        multiplicative-expression "%" cast-expression
cast-expression:
        unary-expression
        "(" type-name ")" cast-expression
unary-expression:
        postfix-expression
        "++" unary-expression
        "--" unary-expression
        unary-operator cast-expression
        "sizeof" unary-expression
        "sizeof" "(" type-name ")"
postfix-expression:
        primary-expression
        postfix-expression "[" expression "]"
        postfix-expression "(" argument-expression-list? ")"
        postfix-expression "." identifier
        postfix-expression "->" identifier
        postfix-expression "++"
        postfix-expression "--"
        "(" type-name ")" "{" initializer-list "}"
        "(" type-name ")" "{" initializer-list "," "}"
primary-expression:
        identifier
        constant
        string-literal
        "(" expression ")"
expression:
        assignment-expression
        expression "," assignment-expression
argument-expression-list:
        assignment-expression
        argument-expression-list "," assignment-expression
type-name:
        specifier-qualifier-list abstract-declarator?
abstract-declarator:
        pointer
        pointer? direct-abstract-declarator
direct-abstract-declarator:
        "(" abstract-declarator ")"
        direct-abstract-declarator? "[" type-qualifier-list? assignment-expression? "]"
        direct-abstract-declarator? "[" "static" type-qualifier-list? assignment-expression "]"
        direct-abstract-declarator? "[" type-qualifier-list "static" assignment-expression "]"
        direct-abstract-declarator? "[" "*" "]"
        direct-abstract-declarator? "(" parameter-type-list? ")"
parameter-type-list:
        parameter-list
        parameter-list "," "..."
parameter-list:
        parameter-declaration
        parameter-list "," parameter-declaration
parameter-declaration:
        declaration-specifiers declarator
        declaration-specifiers abstract-declarator?
initializer-list:
        designation? initializer
        initializer-list "," designation? initializer
designation:
        designator-list "="
designator-list:
        designator
        designator-list designator
designator:
        "[" constant-expression "]"
        "." identifier
constant-expression:
        conditional-expression
initializer:
        assignment-expression
        "{" initializer-list "}"
        "{" initializer-list "," "}"
unary-operator:
        "&"
        "*"
        "+"
        "-"
        "~"
        "!"
assignment-operator:
        "="
        "*="
        "/="
        "%="
        "+="
        "-="
        "<<="
        ">>="
        "&="
        "^="
        "|="
identifier-list:
        identifier
        identifier-list "," identifier
enum-specifier:
        "enum" identifier? "{" enumerator-list "}"
        "enum" identifier? "{" enumerator-list "," "}"
        "enum" identifier
enumerator-list:
        enumerator
        enumerator-list "," enumerator
enumerator:
        enumeration-constant
        enumeration-constant "=" constant-expression
typedef-name:
        identifier
function-specifier:
        "inline"
declaration-list:
        declaration
        declaration-list declaration
declaration:
        declaration-specifiers init-declarator-list? ";"
init-declarator-list:
        init-declarator
        init-declarator-list "," init-declarator
init-declarator:
        declarator
        declarator "=" initializer
compound-statement:
        "{" block-item-list? "}"
block-item-list:
        block-item
        block-item-list block-item
block-item:
        declaration
        statement
statement:
        labeled-statement
        compound-statement
        expression-statement
        selection-statement
        iteration-statement
        jump-statement
labeled-statement:
        identifier ":" statement
        "case" constant-expression ":" statement
        "default" ":" statement
expression-statement:
        expression? ";"
selection-statement:
        "if" "(" expression ")" statement
        "if" "(" expression ")" statement "else" statement
        "switch" "(" expression ")" statement
iteration-statement:
        "while" "(" expression ")" statement
        "do" statement "while" "(" expression ")" ";"
        "for" "(" expression? ";" expression? ";" expression? ")" statement
        "for" "(" declaration expression? ";" expression? ")" statement
jump-statement:
        "goto" identifier ";"
        "continue" ";"
        "break" ";"
        "return" expression? ";"
		*/



========================================================================
    CONSOLE APPLICATION : logic Project Overview
========================================================================
0
0
0
par1
0
0
par2
begincnt
bp
pcret
bp 0 name
-1 type
-2 value
-3
-4
-5 value

Stack:
LBP VAL
LBP TYPE -1
LBP  NAME ""
LOVAL 1 VAL
LOCAL 1 TYPE
newBP: LOCAL 1 NAME 
CALL MAIN PC+1 => now a call to function is done by changeing PC to the location of BEGIN (because PC is incremented this BEGIN is not called by executer)
CALL MAIN BP
CALL MAIN begincnt
Global n VALUE
Global n TYPE
Global n NAME
...
Global 1 VALUE
Global 1 TYPE
Global 1 NAME
STACKEND

GLOBAL2
GLOBAL1 (ISP)
identinfovarend

global access points to value
stackend+(i->ISP-i->identinfovarend);

local access:
(i->ISP-i->IBP)*3+2
BSP
LOCAL 1
i->ISP-I->IBP=0
value is -2 and points to Local 1 val


	i->ISP->identptr.Int=(i->ISP-i->IBP)*3-2

u *a=(u*)(BP+PC[1].Int);
	_ cout <<*((char**)(((u*)a)+2))<<endl;
	
	
AppWizard has created this logic application for you.

This file contains a summary of what you will find in each of the files that
make up your logic application.


logic.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

logic.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named logic.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
