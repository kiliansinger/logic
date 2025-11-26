#include "pch.h"
namespace break1 {

	TESTCOMPILER(
		void main() {
		int a = 1;
		int b = 18;
		for (a = 0; a < 12; a++) {
			int uu = 22;
			if (a == 5) continue;
			if (a == 10) break;
			print(uu++); print('\n');
			print(a); print('\n');
		}
	})
		TEST(break1, break1) {
		dotest(fnname, main);
	}

}


