//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"
#include <string.h>//for memset
#include <stdio.h>//printf sprintf getchar
#include <iostream>
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>

	extern "C" {
#include "kstring.h"
#include "compiler.h"
#include "interpreter.h"
	}

extern void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

extern void print(int i);
extern void print(char c);
extern void print(float f);
extern void print(double d);
extern void dotest(char* fnname, void (*main)());
#define TESTCOMPILER(string) char fnname[]=#string;\
string

#define TESTCOMPILERONLY(stringvm) char fnname[]=#stringvm;
