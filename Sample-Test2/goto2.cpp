#include "pch.h"
namespace goto2 {

	TESTCOMPILER(
		void printf(int a) { print(a); }
	void main() {

		int a = 0;
		{
			goto lab1;
			printf(a++);
		lab1:	printf(a++);
			goto lab2;
		}
		printf(a++);
	lab2: printf(a++);
	})
		TEST(goto2, goto2) {
		dotest(fnname, main);
	}

}