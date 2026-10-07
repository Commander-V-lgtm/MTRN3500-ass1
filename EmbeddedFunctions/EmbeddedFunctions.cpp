#include "EmbeddedFunctions.h"

EmbeddedFunctions::EmbeddedFunctions() {
	SendData = gcnew array<uint8_t>(SendBufferSize);
	RecvData = gcnew array<uint8_t>(RecvBufferSize);
}

EmbeddedFunctions::~EmbeddedFunctions() {
	if (GalilMngHndl != nullptr || GalilStream != nullptr) GClose();
}

/**
* Open a connection to a Galil Controller.
*
* @param address Null-terminated address string. Use direct connection (-d) to connect to hardware or simulator (e.g., "192.168.0.120 -d").
* @param port Integer value for the port to connect to.
*
* @throws error if one occurs.
*/
void EmbeddedFunctions::GOpen(String^ address, const int port) {
	if (String::IsNullOrEmpty(address)) throw gcnew Exception("Address cannot be empty");
	if (GalilStream != nullptr || GalilMngHndl != nullptr) {
		throw gcnew InvalidOperationException("Connection exists, call GClose() first");
	}
	array<String^>^ input = address->Split(' ');
	IPAdress = input[0];
	Port = port;
	GalilMngHndl = gcnew TcpClient(IPAdress, Port);
	GalilMngHndl->SendBufferSize = SendBufferSize;
	GalilMngHndl->ReceiveBufferSize = RecvBufferSize;
	GalilMngHndl->SendTimeout = SendTimeout;
	GalilMngHndl->ReceiveTimeout = RecvTimeout;
	GalilMngHndl->NoDelay = NoDelay;
	GalilStream = GalilMngHndl->GetStream();
	if (GalilStream == nullptr) {
		GalilMngHndl->Close();
		GalilMngHndl = nullptr;
		throw gcnew Exception("GOpen failed to aquire Galil connection");
	}
}

/**
* Closes a connection to a Galil Controller.
 `GClose()` should be called whenever a program is finished with a controller. This includes when a program closes. A rule of thumb is that for every `GOpen()` call on a given connection, a `GClose()` call should be found on every code path. Failing to call GClose() may cause controller resources to not be released or can hang the process if there are outstanding asynchronous operations. The latter can occur, for example, if a call to GRead() times out and the process exits without calling GClose(). In this case, GRead() still has an outstanding asynchronous read pending. GClose() will terminate this operation allowing the process to exit correctly.
*
*
* @throws error if one occurs.
*/
void EmbeddedFunctions::GClose() {
	if (GalilStream != nullptr) {
		GalilStream->Close();
		GalilStream = nullptr;
	}
	if (GalilMngHndl != nullptr) {
		GalilMngHndl->Close();
		GalilMngHndl = nullptr;
	}
}

/**
* Performs a *command-and-response* transaction on the connection.
* IMPORTANT: Commands being sent to the galil should be in the form of strings based on the command reference (uploaded to Moodle). You should choose commands appropriate for a given task and avoid those that rely on logical expressions. Commands chosen should send data directly to the output or read directly from the inputs.
* IMPORTANT: If the command string supplied is not terminated by a semicolon, you should append one.
*
* @param command Null-terminated command string to send to the controller.
*
* @return The reponse from the Galil.
* @throws error if one occurs.
*/
String^ EmbeddedFunctions::GCommand(String^ command) {
	if (String::IsNullOrEmpty(command)) throw gcnew Exception("Command cannot be empty");
	if (GalilStream == nullptr) throw gcnew InvalidOperationException("GConnection not found");
	
	Command = command;
	if (!Command->EndsWith(";")) Command += ";";
	Command += "\r";
	
	Array::Clear(RecvData, 0, RecvData->Length);
	SendData = Text::Encoding::ASCII->GetBytes(Command);
	GalilStream->Write(SendData, 0, SendData->Length);
	Threading::Thread::Sleep(10);

	int GRead = GalilStream->Read(RecvData, 0, RecvData->Length);
	Threading::Thread::Sleep(25);

	if (GRead == 0) throw gcnew Exception("No response from controller");
	Response = Text::Encoding::ASCII->GetString(RecvData, 0, GRead);
	if (Response->Contains("?")) throw gcnew Exception("Unknown Galil command: " + Command);

	array<wchar_t>^ delims = { L':', L'\n', L'\r'};
	array<String^>^ output = Response->Split(delims);

	return output[0];
}