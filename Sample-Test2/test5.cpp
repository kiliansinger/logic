#include "pch.h"
namespace test5 {

	TESTCOMPILER(
		void printf(int a) { print(a); }//we have problem with function definitions leading to seh exceptions if we do not zero in pushISP but when we zero we get problem with big code in main.

	void main() {
		int j = 1;//this works
		int i = 0;
		
		char aa = 'a'; 
		int a = 42; 
			int b = 24; 
			int c[10 + 20]; 
			int* p = &a; 
			int jj;
			for (jj = 0; jj < 30; ++jj) c[jj] = 32 + jj;
			for (jj = 0; jj < 30; ++jj) print(c[jj]);
		
			print(*(p--)); 
			* (p + 1) = 123; 
			//print(*p); 
		for (aa = 'a'; aa < 'z'; aa++)print(aa);
		
		while (j < 5) {

			i = 0;
			while (i < 10) {

				++i;
				int u = 9;
				print(u); print('\n');
				if (i >= 5) break;
				print(i); print('\n');
				print(j); print('\n');
			}
			if (j == 2) break;
			++j;
		}
	})

		TEST(test5, test5) {
		dotest(fnname, main);
	}

}
