#include "Galil.h"
#using <system.dll>

using namespace System;

int main(void) {
	EmbeddedFunctions funcs;
	Galil myGalil(&funcs, "192.168.0.120 -d");

	Console::ReadKey();
	return 0;
}