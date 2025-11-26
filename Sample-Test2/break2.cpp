#include "pch.h"
namespace break2 {

	TESTCOMPILER(
		void fn() {

		int i = 0;
		goto lab2;
		print(i);
	lab2:;
	}
	void main() {

		int a = 1;
		int b = 18;
		int c = 0;
		for (a = 0; a < 12; a++) {

			if (a == 5) continue;
			if (a == 10) break; //this is broken due to second for loop we need to name them break$1 break$2...
			for (c = 0; c < 10; c++) {

				if (c == 5) break;
			}
			print(a);
		}
	ret:;
	ret2:;
		return;
		fn();
	lab0: print(a);
		print(b);
		{
			int a = 11;
			int b = 22;
			goto lab1;
			print(a);
		lab1: print(b);
			goto lab2;
		}
		print(a);
	lab2: print(b);
	})
		TEST(break2, break2) {
		dotest(fnname, main);
	}

}


