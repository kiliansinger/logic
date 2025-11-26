#include "pch.h"
namespace charasarrayindex1 {

	//!!!also if fn(int a,int b) then compiler fails to find error of multiple definition

	TESTCOMPILER(
		void main() {
		char c[40];
		c[0] = 'a';
		c[1] = 'b';
		c[2] = 'c';
		c[32] = 'X';
		print(c[' ']);//does work
		print(c[(int)' ']);//does  work but we have no typecast op implemented
	})
		TEST(charasarrayindex1, charasarrayindex1) {
		dotest(fnname, main);
	}

}



