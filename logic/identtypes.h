#ifdef IDENTTYPES_ENUM
ignore = 0,NOCHANGE = 0,
#else
DUMP,
#endif
#ifdef IDENTTYPES_ENUM
VOID,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
POINTER,
#else
GLOBALLIFE, 
#endif
#ifdef IDENTTYPES_ENUM
INT,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
FLOAT,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
DOUBLE,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
CHAR,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
fn,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
fnforward,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
fnforwardpatch,
#else
GLOBALLIFE, 
#endif
#ifdef IDENTTYPES_ENUM
varglob,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
varloc,
#else
SCOPE, 
#endif
#ifdef IDENTTYPES_ENUM
varstatic,
#else
GLOBANONYM, 
#endif
#ifdef IDENTTYPES_ENUM
ilbp,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
label,
#else
FUNC, 
#endif
#ifdef IDENTTYPES_ENUM
labelpatch,
#else
FUNC,
#endif
#ifdef IDENTTYPES_ENUM
structdeflocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
structdefforwardlocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
structdefglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
structforwardglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
structvarlocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
structmembervarlocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
structvarglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
structmembervarglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
uniondeflocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
unionforwardlocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
uniondefglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
unionforwardglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
unionvarlocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
unionvarglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
unionmembervarglobal,
#else
GLOBALLIFE,
#endif
#ifdef IDENTTYPES_ENUM
unionmembervarlocal,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
localtypedef,
#else
SCOPE,
#endif
#ifdef IDENTTYPES_ENUM
globaltypedef
#else
GLOBALLIFE
#endif
