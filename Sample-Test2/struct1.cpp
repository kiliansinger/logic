#include "pch.h"
namespace struct1 {

	TESTCOMPILER(
		//struct A;
				struct A{
					double f;
				};
				struct B{
					char c;
					int d;
					struct C{
						int iii;
						int iiii;//works now
						struct A aaa;
					} cc;
					double ddd;
				};
				void main() {
					int i=0;
					int ii[5];
					struct B b;
	//struct C c;
	struct A a;
					b.d = 0; b.d += 1;
					b.c='a';
					b.d++;
					b.c++;
					b.cc.iii = 0; b.cc.iii++;
					b.cc.iii+=9;
					b.cc.aaa.f=1.0;//We do not have float double support for setting values 1.2 it somehow affects b.cc.iii
					++b.cc.aaa.f;
					//print(b.cc.aaa.f);
					print(' ');
					print(b.cc.iii);
					print(' ');
					print(b.c);
					print(' ');
					print(b.d++);print(' ');print(b.d);print(' ');
				})
		TEST(struct1, struct1) {
		dotest(fnname, main);
	}

}
