#include "pch.h"

namespace stringtest1 {

	TESTCOMPILER(
		void printstr(char* s) {
			while (*s) {//!!!todo we need to typecast to int\n\
				print(*s);
				++s;
			}
		}
		void main() {
					int a = 42;
					int b = 24;
					char* s;
					char aa;
					int i;
					//print((int)'a');
					for (aa = 'a'; aa < 'z'; aa++)print(aa);
					s = "hello";
					printstr(s);
					print(a);
					print('a');//we have problem with char in 64 bit mode because we print it too many times\n\
					print(b);
				}
				
	)
		TEST(stringtest1, stringtest1) {
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