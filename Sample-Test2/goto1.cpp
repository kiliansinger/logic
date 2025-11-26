#include "pch.h"
namespace goto1 {

	TESTCOMPILER(
		void printf(int a) { print(a); print('\n'); }
	void main() {

		int a = 0;
		for (a = 0; a < 2; ++a) {
			if (!(a == 0)) goto lab1;
			printf(a++);
		lab1:	printf(a++);
			goto lab2;
		}
		printf(a++);
	lab2: printf(a++);
	})
		TEST(goto1, goto1) {
		dotest(fnname, main);
	}

}
