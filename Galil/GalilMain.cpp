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
	EmbeddedFunctions funcs(true);
	Galil myGalil(&funcs, "192.168.0.120 -d");

	Console::WriteLine("This is the end This is the end This is the end This is the end This is the end ");
	Console::WriteLine("This is the end This is the end This is the end This is the end This is the end ");
	Console::WriteLine("This is the end This is the end This is the end This is the end This is the end ");
	Console::WriteLine("This is the end This is the end This is the end This is the end This is the end ");
	Console::WriteLine("This is the end This is the end This is the end This is the end This is the end ");

	myGalil.DigitalOutput(0b0000111100001111);

	Console::ReadKey();

	myGalil.DigitalByteOutput(1, 0b11110000);

	Console::ReadKey();

	myGalil.DigitalBitOutput(0, 3);
	myGalil.DigitalBitOutput(1, 4);

	Print test;
	test.print();

	Console::ReadKey();

	myGalil.DigitalOutput(0b0);

	Console::ReadKey();
	return 0;
}