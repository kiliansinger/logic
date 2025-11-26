#include "pch.h"
namespace bigcodetest1 {

	TESTCOMPILER(
void mainx() {
	
		int a = 42; 
		int b = 24; 
		int* p = &a; 
		int* pp = &b; 
		int xx = 888; 
		//int c[a - 42 + 3];//causes problem in 64bit
	    int c[3];//causes no problem in 64bit
		int d[1];
		int x=1;
		*p=1;
		//*(p-x)=99;//this is different in vm and real pc due to different stack layout
		*(c)=111;
		c[x]=222;
		c[2*x]=333;
		*d=777;
		print(a);
		print(b);
		//print(*(c-2));
		//print(*(c-1));
		//print(*d);
		//print(*c);
		//print(c[1]);
		//print(*(c+2));
		//print(*(c+3));
		//print(*(c+4));
		//print(*(c+5));
		//print(*(c+6));
		//print(*(c+7));
		{
			int a=42;
			int b=24;
			int *p=&a;
			int *pb=&b;
			int **pp=&p;
			**pp=22;
			**pp=33;
			*p=44; 
			print(**pp);
			pp=&pb;//*pp=&pb is not recognized as error in our compiler!
			**pp=123;
			print(b);
			print(a);
		}
	}
	void main0(){
		int a=1;
		int b=18;
		for(a=0;a<12;a++){
			int uu=22;
			if(a==5) continue;
			if(a==10) break;
			print(uu++);
			print(a);
		}
	}
	void printf(int a){print(a);}
	void main1(){
		int a=0;
		{
			if(!(a==0)) goto lab1;
			printf(a++);
	lab1:	printf(a++);
			goto lab2;
		}
		printf(a++);
	lab2: printf(a++);
	}
	int fib4(int, int, int ) ;
	int i=1;
	int j=2;
	int jjjj=333;
	double d=3;
	int fn(int i,double j){
		int uuu;//!!we have issues with int uuu,ooo,ppp; I think
		int ooo;
		int ppp;
		if(1) fib4(i,i,i);
		return i+j;
	}
	int fib3(int n) {
		int a;
		int b;
		int neu;
		int zaehler;
		if (n<=2) return 1;
		a = 1;
		b =		1;
		for (zaehler=3; zaehler<=n; zaehler++)
		{
			neu = a+b;
			a = b;
			b = neu;
		}
		return neu;
	}
	int fib4(int n, int a, int b) {
		if (n==1) return a;
		if (n==2) return b;
		return fib4(n-1,b,a+b);
	}
	void main(){
		mainx();
		main0();
		main1();
		char cc='a';
		char end='z';
		int m;
		int u=0;
		double f=123.67e-3+11.9;
		print(end);
		for(cc='a';cc<end;cc++) print(cc);
		//print(f);//float have different outputs due to other decimal settings
		print(fn(i+9,100));
		print(110);
		for(m=0;m < 10;++m) u+=fib3(1234567+m);
		print(fib3(1234567));
		print(u);
		print(fib3(f));
		print(fib4(12,1,1));
		m=0;
		while(m<5) print(m++);
		do{print(--m);} while(m>0);
	})


	

	TEST(bigcodetest1, bigcodetest1) {
		dotest(fnname, main);
	}

}



