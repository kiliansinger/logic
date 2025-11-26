#include "pch.h"
namespace test4{

	//!!!also if fn(int a,int b) then compiler fails to find error of multiple definition

	TESTCOMPILER(
		int sam = 36;
	int kilian = 39;
	int sum = 0;
	int fn(int a, int b) {

		int c;
		print(a);
		print(b);
		for (c = a; c <= b; c++) {

			print(c * 2);
		}
		return (a + b) * 3;
	}
	void main(void) {

		int m;
		print(fn(1, 3));
		for (m = 0; m < 2000; ++m) {

			print(m * 3);
		}
		m = 10;
		m *= 10;
		print(5 - 1 - 2);
	}
	)
		TEST(test4, test4) {
		dotest(fnname, main);
	}

}
