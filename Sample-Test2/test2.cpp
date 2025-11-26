#include "pch.h"
namespace test2 {

	TESTCOMPILER(
		int i = 9;
	int a = i + 3 * 3;
	int fn(int p) { int ux = 12; int gn = p; {int ux = 0; }ux; return p + 1; }
	int fn2(int a, int b) { return a + b; }
	int larissi = fn2(2, 3);
	int kili = a;
	void main(void) {

		int i = 2;
		int n = 123;
		int m;
		while (i) {

			i = i - 1;
			if (0) { 33; }
			else { 44; }
			if (1) { 33; }
			else { 44; }
		}
		fn(42);
		m = i = fn(43) * fn(43);
		m;
		a = 123;
		a++;
		print(a);
		{
			int j = 0;
			j = a;
			print(a);
			m = a;
		}
	})
		TEST(test2, test2) {
		dotest(fnname, main);
	}

}


