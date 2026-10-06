#include "EmbeddedFunctions.h"

int main(void) {
	// Start galil
	EmbeddedFunctions Galil;
	Galil.GOpen("192.168.0.120 - d", 23);

	// Send commands
	String^ command;
	String^ respond;
	Galil.GCommand("OP 255,255");
	System::Threading::Thread::Sleep(2000);
	Galil.GCommand("OP 0,0");

	System::Threading::Thread::Sleep(500);

	respond = Galil.GCommand("MG @AN[0]");
	Console::WriteLine(respond);
	command = respond;
	Galil.GCommand("AO 0," + command);
	System::Threading::Thread::Sleep(2000);
	Galil.GCommand("AO 0,0");

	// End program
	Galil.GClose();
	return 0;
}