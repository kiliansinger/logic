#include "pch.h"

namespace test1 {

	TESTCOMPILER(
		void main() {
		int aa = 42;
		int b = 24;
		char* c;
		c = "hello";

		print(41);
		print('a');//we have problem with char in 64 bit mode because we print it too many times
		print(b);
		print('\n');
		print(0 ? 0?3:4 : 3);
		print(1 ? 0 ? 3 : 4 : 3);
		print(0 ? 1 ? 3 : 4 : 3);
		print(1 ? 1 ? 3 : 4 : 3);
		print(0 ? 3 : 1? 4 : 5);
		print(1 ? 3 : 0 ? 4 : 5);
		print(1 ? 3 : 1 ? 4 : 5);
		print(0 ? 3 : 0 ? 4 : 5);
		print('\n');
	}
	)
	TEST(printint, printint1) {
		dotest(fnname, main);
	}

}
/*
//KSKSK so this works with nested loop but ignores break
				//if you comment out && (id->parsing != looplabel)
				//then break works but not nested loops
				int cnt=0;
				while (ii /*&& (id->parsing != looplabel)//!!!!!we patch the calls to forward defined functions//todo we should when compiler is finished check that there are no fnforwardpatches available
				
				*/