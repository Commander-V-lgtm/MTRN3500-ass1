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

	Print test;
	test.print();

	if(myGalil.CheckSuccessfulWrite()) Console::WriteLine("Successful connection!");

	myGalil.DigitalOutput(0b0000111100001111);

	Console::ReadKey();

	myGalil.DigitalByteOutput(1, 0b11110000);

	Console::ReadKey();

	myGalil.DigitalBitOutput(0, 3);
	myGalil.DigitalBitOutput(1, 4);

	Console::ReadKey();

	myGalil.DigitalOutput(0b0);

	Console::ReadKey();

	myGalil.DigitalByteOutput(0, 0b10110100);
	Console::WriteLine(myGalil.DigitalInput());
	Console::WriteLine(myGalil.DigitalByteInput(0));
	Console::WriteLine(myGalil.DigitalByteInput(1));
	Console::Write(static_cast<int>(myGalil.DigitalBitInput(0)));
	Console::Write(static_cast<int>(myGalil.DigitalBitInput(1)));
	Console::Write(static_cast<int>(myGalil.DigitalBitInput(2)));
	Console::Write(static_cast<int>(myGalil.DigitalBitInput(3)));
	Console::WriteLine(static_cast<int>(myGalil.DigitalBitInput(4)));

	Console::ReadKey();

	double ao = myGalil.AnalogInput(0);
	myGalil.AnalogOutput(0, ao);

	Console::ReadKey();

	myGalil.AnalogInputRange(0, 1);
	ao = myGalil.AnalogInput(0);
	myGalil.AnalogOutput(0, ao);

	Console::ReadKey();

	myGalil.AnalogInputRange(0, 2);
	ao = myGalil.AnalogInput(0);
	myGalil.AnalogOutput(0, ao);

	Console::ReadKey();

	myGalil.AnalogOutput(0, 0);
	Console::WriteLine("Awaiting input: wait for wheel to stop.");

	Console::ReadKey();

	Console::WriteLine(myGalil.ReadEncoder());
	myGalil.WriteEncoder();

	Console::ReadKey();

	myGalil.setSetPoint(1000);
	myGalil.setKp(0.001);
	myGalil.setKi(0.0001);
	myGalil.setKd(0.0005);
	myGalil.SpeedControl(false, 0);

	Console::ReadKey();
	myGalil.DigitalOutput(0b0);
	myGalil.AnalogOutput(0, 0);
	myGalil.AnalogInputRange(0, 2);
	return 0;
}