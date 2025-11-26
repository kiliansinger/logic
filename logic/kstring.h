#ifndef KSTRING_H
#define KSTRING_H
#define S(s) s,s+sizeof(s)/sizeof(char)-1
typedef struct KStr{
	char **s;
	char *c;
} KStr;
typedef struct KStrRange{
	KStr begin;
	KStr end;
} KStrRange;
char getS(KStr *k);
KStrRange ksr(const char *s);
char incS(KStr *k);
int cmpS(KStr k1,KStr k2);
void printS(KStr k);
void printR(KStrRange r);
void copyStartStop(int *index,char **s,KStr start,KStr end);
void getR(KStrRange r,char *str,int maxlen,int *len);
#endif