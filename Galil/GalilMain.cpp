#include "Galil.h"
#using <system.dll>

using namespace System;

ref class Print {
public:
	void print() {
		Console::WriteLine("Test test test");
	}
};

int main(void) {
	EmbeddedFunctions funcs;
	Galil myGalil(&funcs, "192.168.0.120 -d");

	Print test;
	test.print();

	Console::WriteLine("Successful connection!");

	myGalil.DigitalOutput(0b0000111100001111);

	Console::ReadKey();

	myGalil.DigitalByteOutput(1, 0b11110000);

	Console::ReadKey();

	myGalil.DigitalBitOutput(0, 3);
	myGalil.DigitalBitOutput(1, 4);

	Console::ReadKey();

	// myGalil.DigitalOutput(0b0);

	Console::ReadKey();

	myGalil.DigitalInput();

	Console::ReadKey();
	return 0;
}