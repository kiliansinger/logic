#include "pch.h"
namespace array2 {
	TESTCOMPILER(
		void main() {
		int i[4];
		int* ii=i;
		i[0] = 1;
		i[1] = 1;
		i[2] = 1;
		i[3] = 1;
		print(i[2]++);
		print((*(ii + 3))++);
		print(++i[2]);
		print(++(*(3+ii)));
		print(i[2]+=2);
		print((*(ii + 3))+=2);
	})	
	TEST(array2, array2) {
		dotest(fnname, main);
	}

}



