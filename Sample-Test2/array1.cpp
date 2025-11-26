#include "pch.h"
namespace array1 {

	TESTCOMPILERONLY(
	void main() {//problem the following code gives different result in 64 bit and 32 bit. we think it is related with the varsize array
		char s[10];
		s[0] = 'z';//we have trouble as char array is treated as integer array
		int a = 42;
		int b = 24;
		int* p = &a;
		int* pp = &b;
		int xx = 888;
		int c[a - 42 + 3];//causes problem in 64bit
		//int c[3];//causes no problem in 64bit
		int d[1];
		int x = 1;
		int xxx = 0;
		//(s[0])++;//this is broken
		//s[0] += 1;//this is broken
		xxx=s[0] = s[0] + 1;
		print(s[0]); print(xxx);
		*p = 1;
		//*(p - x) = 99;
		*(c) = 111;
		c[x] = 222;
		c[2 * x] = 333;
		*d = 777;
		print(a); print(' ');
		print(b); print(' ');
		print(*d); print(' ');
		print(*c); print(' ');
		print(c[1]); print(' ');
		print(*(c + 2)); print(' ');//333
		print(*(1 + c + 1)); print(' ');

		{
			int a = 42;
			int b = 24;
			int* p = &a;
			int* pb = &b;
			int** pp = &p;//here we have problem as the ptrcnt does not work
			**pp = 22;
			**pp = 33;
			*p = 44;
			print(**pp); print(' ');//here we have problem as the ptrcnt does not work
			pp = &pb;//error not detecte *pp=&pb;
			**pp = 123;
			print(b); print(' ');
			print(a); print(' ');
		}
	})


		void main() {//problem the following code gives different result in 64 bit and 32 bit. we think it is related with the varsize array
		char s[10];
		s[0] = 'z';//we have trouble as char array is treated as integer array
		int a = 42;
		int b = 24;
		int* p = &a;
		int* pp = &b;
		int xx = 888;
		int c[3];//causes problem in 64bit
		//int c[3];//causes no problem in 64bit
		int d[1];
		int x = 1;
		int xxx = 0;
		// (s[0])++;
		//s[0] += 1;
		xxx=s[0] = s[0] + 1;
		print(s[0]); print(xxx);//also we have problem with outputting string
		*p = 1;
		//*(p - x) = 99;
		*(c) = 111;
		c[x] = 222;
		c[2 * x] = 333;
		*d = 777;
		print(a); print(' ');
		print(b); print(' ');
		print(*d); print(' ');
		print(*c); print(' ');
		print(c[1]); print(' ');
		print(*(c + 2)); print(' ');
		print(*(1+c + 1)); print(' ');
	

		{
			int a = 42;
			int b = 24;
			int* p = &a;
			int* pb = &b;
			int** pp = &p;//here we have problem as the ptrcnt does not work
			**pp = 22;
			**pp = 33;
			*p = 44;
			print(**pp); print(' ');//here we have problem as the ptrcnt does not work
			pp = &pb;//error not detecte *pp=&pb;
			**pp = 123;
			print(b); print(' ');
			print(a); print(' ');
		}
	}

		TEST(array1, array1) {
		dotest(fnname, main);
	}

}



