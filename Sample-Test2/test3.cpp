#include "pch.h"
namespace test3 {

	//!!!also if fn(int a,int b) then compiler fails to find error of multiple definition

	TESTCOMPILER(
		int fn(int a2, int b2) {
		int a = 1;
		int b = 2;
		int c = 0;
		if (a == b) c = 1;
		else c = 2;
		return c;
	}
	int i = fn(1, 2);//we have trouble with global functions leading to seh exception
	void main(void) {

		int a;
		for (a = 0; a < 10; ++ ++a) {
			int i;i = a;
			print(a);
//			int i = a;//!!!also here we expect compiler error message
		}
	})
		TEST(test3, test3) {
		dotest(fnname, main);
	}

}

