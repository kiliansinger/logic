//kstring is used to recycle strings inside another string
//by pointing to beginning end.
#include "kstring.h"
#include <string.h>
#include <stdio.h>//printf sprintf getchar
//KStr
//usage:
//char *kstring[]={S(hallo),S(_),S("larissa"),S(_),S(wie),S(_),S(gehts),S(qm),0};
//char *kstring[]={hallo,hallo+strlen(hallo)-1,0};
//char *kstring[]={hallo,-1,0};
//char *kstring[]={hallo,0};

//char *s equivalent:
//Kstr k;
//k.s=0;
//k.c=".lk;lk;lk;l";



//end of KStr



char getS(KStr *k){
	static char zero=0;
	if(k->s==0){//we have string equivalent where KStr.s=0, and Kstr.c="hello"
		if(k->c) return *k->c;
		else {
			k->c=&zero;
			return 0;
		}
	}

	if(k->c==0) k->c=*k->s;
	while(*k->s){
		if(k->c<(k->s)[1] || (k->s)[1]==0 ){
			if(*k->c) {
				return *k->c;
			}else{//if string has zero we get to next string part
				if((k->s)[1]==0) {
					k->c=&zero;
					return 0;
				}
				k->s+=2;
				k->c=*k->s;
			}
		}else{//now we are at the end of string (not neccesarily zero)
			k->s+=2;
			k->c=*k->s;
		}
	}
	k->c=&zero;
	return 0;
}


KStrRange ksr(const char *s){
	KStrRange ret;

	ret.begin.s=0;
	ret.begin.c=(char*)s;;//getS(&ret.begin);
	ret.end.s=0;
	ret.end.c=ret.begin.c+strlen(s);
	return ret;
}

char incS(KStr *k){
	static char zero=0;
	if(k->s==0){//we have string equivalent where KStr.s=0, and Kstr.c="hello"
		if(k->c){
			++k->c;
			return *k->c;
		}
		else {
			k->c=&zero;
			return 0;
		}
	}

	if(k->c==0) k->c=*k->s;
	++k->c;
	while(*k->s){
		if(k->c<(k->s)[1] || (k->s)[1]==0 ){
			if(*k->c) {
				return *k->c;
			}else{//if string has zero we get to next string part
				if((k->s)[1]==0) {
					k->c=&zero;
					return 0;
				}
				k->s+=2;
				k->c=*k->s;
			}
		}else{//now we are at the end of string (not neccesarily zero)
			k->s+=2;
			k->c=*k->s;
		}
	}
	k->c=&zero;
	return 0;
}

int cmpS(KStr k1,KStr k2){
	if(k1.s>k2.s) return 1;
	else if(k1.s<k2.s) return -1;
	else{
		if(k1.c>k2.c) return 1;
		else if(k1.c<k2.c) return -1;
		else return 0;
	}
}
void printS(KStr k){
	
	char **s=k.s;
	char *c=k.c;
	if(k.s==0){//char *s, equiv
		for(;*c;++c) putchar(*c);
		return;
	}
	else if (c == 0) c = *s;
	while(*s){
		if(c<s[1] || s[1]==0){
			if(*c) {
				putchar(*c);
				++c;
			}else{//if string has zero we get to next string part
				if(s[1]==0) return;
				s+=2;
				c=*s;
			}
		}else{//now we are at the end of string (not neccesarily zero)
			s+=2;
			c=*s;
		}
	}
}

void printR(KStrRange r){
	if(r.begin.s==0 || r.end.s==0){
			for(;*r.begin.c && r.begin.c<r.end.c ;++r.begin.c) putchar(*r.begin.c);
	}
	else while(*r.begin.s && cmpS(r.begin,r.end)<0){
		if(r.begin.c<r.begin.s[1] || r.begin.s[1]==0){
			if(*r.begin.c) {
				putchar(*r.begin.c);
				++r.begin.c;
			}else{//if string has zero we get to next string part
				if(r.begin.s[1]==0) return;
				r.begin.s+=2;
				r.begin.c=*r.begin.s;
			}
		}else{//now we are at the end of string (not neccesarily zero)
			r.begin.s+=2;
			r.begin.c=*r.begin.s;
		}
	}
}
void copyStartStop(int *index,char **s,KStr start,KStr end){
    char **i;
	s[(*index)++]=start.c;
	if(start.s) for(i=start.s+1;i<=end.s;++i){
		s[(*index)++]=*i;
	}
	s[(*index)++]=end.c;
}
//todo //if Range.end.c=0 and Range.end.s=0 then we use full range
void getR(KStrRange r,char *str,int maxlen,int *len){
	int cnt=0;
	if(r.begin.s==0 || r.end.s==0){
			for(;*r.begin.c && r.begin.c<r.end.c ;++r.begin.c,++cnt){
				if(cnt<maxlen) str[cnt]=*r.begin.c;
				else { str[cnt - 1] = 0; *len = cnt; return; }
			}
	}
	else while(*r.begin.s && cmpS(r.begin,r.end)<0){
		if(r.begin.c<r.begin.s[1] || r.begin.s[1]==0){
			if(*r.begin.c) {
				if(cnt<maxlen)	str[cnt]=*r.begin.c;
				else {str[cnt-1]=0; *len = cnt; return;}
				cnt++;
				++r.begin.c;
			}else{//if string has zero we get to next string part
				if(r.begin.s[1]==0){str[cnt]=0; *len = cnt+1; return;}
				r.begin.s+=2;
				r.begin.c=*r.begin.s;
			}
		}else{//now we are at the end of string (not neccesarily zero)
			r.begin.s+=2;
			r.begin.c=*r.begin.s;
		}
	}
	str[cnt]=0;
	*len = cnt+1;
}
