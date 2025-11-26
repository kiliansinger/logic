//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"
void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}
void print(int *i) {
	std::cout << (int)i<<":"<<*i;
}
void print(int i) {
	std::cout << i;
}
void print(char c) {
	std::cout << c;
}
void print(float f) {
	std::cout << f;
}
void print(double d) {
	std::cout << d;
}
void dotest(char* fnname, void (*main)()) {
	gentokenlookup();
	info i;
	pos p;
	i.mode = kvm;////here we set the mode: kvm,printit
#ifdef DODEBUG //attention do not set debugmode=1 in if DEDEBUG is not defined as we emit mnemonics with debugging info
	i.debugmode = 1;
#else
	i.debugmode = 0;
#endif
	std::string in(fnname);
	ReplaceStringInPlace(in, "\\n", "\n");
	inittables(&i, (char*)in.c_str(), stack, &p);
	printf("\n");
	int compiled = 0;
	
	if (translation_unit(&p)) {
		compiled = 1;
		testing::internal::CaptureStdout();
		run(i.mode, i.debugmode);
	}
	else {
		printf("\n");
		printS(p.k);
		printf("%s ", i.err);
		printR(i.krerrtoken);
		printf(" POS");
		printS(i.kerrpos);
	}
	//due to some error in 64 bit mode of the google testing we have to erase all zeros from string
	std::string output = testing::internal::GetCapturedStdout(), output0 = "";
	for (auto it = output.begin(); it < output.end(); ++it) if (*it) output0 += *it;
	testing::internal::CaptureStdout();
	main();
	std::string output2 = testing::internal::GetCapturedStdout();
	EXPECT_EQ(1, compiled);
	EXPECT_EQ(output0, output2);
}

