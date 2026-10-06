#include "Galil.h"

// Default constructor. Initialize variables, open Galil connection and allocate memory.
// Should assign a default embedded functions that works with physical hardware and a 
// default Galil address as described in the assignment spec.
Galil::Galil() {
	Functions = new EmbeddedFunctions;
	FunctionOwnership = true;
	gRet = Functions->GOpen("192.168.0.120 -d", &g);
	if (gRet != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
	ControlParameters[0] = 1.0;
	ControlParameters[1] = 1.0;
	ControlParameters[2] = 1.0;
	setPoint = 0;
	return;
}

// Constructor with EmbeddedFunciton pre-initialised and passed in.
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) : Functions(Funcs) {
	FunctionOwnership = false;
	gRet = Functions->GOpen(address, &g);
	if (gRet != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
	ControlParameters[0] = 1.0;
	ControlParameters[1] = 1.0;
	ControlParameters[2] = 1.0;
	setPoint = 0;
	return;
}

// Copy constructor to copy the state of all elements within the object other.
// It should construct a new EmbeddedFunctions object and open a separate connection
// (i.e., each class will have a unique value of the GCon g). All other data members
// should be transferred.
Galil::Galil(const Galil& other) {
	Functions = new EmbeddedFunctions;
	FunctionOwnership = true;
	gRet = Functions->GOpen("192.168.0.120 -d", &g);
	if (gRet != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
	ControlParameters[0] = other.ControlParameters[0];
	ControlParameters[1] = other.ControlParameters[1];
	ControlParameters[2] = other.ControlParameters[2];
	setPoint = other.setPoint;
	BOUTBank1 = other.BOUTBank1;
	BOUTBank2 = other.BOUTBank2;
	encoderValA = other.encoderValA;
	encoderValB = other.encoderValB;
	return;
}

// Default destructor. Deallocate memory and close Galil connection.
Galil::~Galil() {
	if (g != nullptr && g != (GCon)0x1) GClose(g);
	else std::cout << "G failed to be found when disconnet was tried!" << std::endl;
	if (FunctionOwnership) delete Functions;
	return;
}

// DIGITAL OUTPUTS
// Write to all 16 bits of digital output, 1 command to the Galil
// Loop code inspired by bitwise practice example
void Galil::DigitalOutput(uint16_t value) {
	BOUTBank1 = value & 0xFF;
	BOUTBank2 = (value >> 8) & 0xFF;
	std::string command = "OP " + std::to_string(BOUTBank1) + ", " + std::to_string(BOUTBank2) + ";";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	return;
}
	
// Write to one byte, either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	if (bank == 0) BOUTBank1 = value;
	else BOUTBank2 = value;
	std::string command = "OP " + std::to_string(BOUTBank1) + ", " + std::to_string(BOUTBank2) + ";";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	return;
}

// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	int no = static_cast<int>(bit);
	if (no > 15) return;
	uint16_t mask = 0b0;
	if (val) mask = 0b1 << no;
	else mask = 0xFFFF ^ (0b1 << no);
	if (val) {
		BOUTBank1 = BOUTBank1 | (mask & 0xFF);
		BOUTBank2 = BOUTBank2 | ((mask >> 8) & 0xFF);
	}
	else {
		BOUTBank1 = BOUTBank1 & (mask & 0xFF);
		BOUTBank2 = BOUTBank2 & ((mask >> 8) & 0xFF);
	}
	std::string command = "OP " + std::to_string(BOUTBank1) + ", " + std::to_string(BOUTBank2) + ";";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	return;
}


// DIGITAL INPUTS
// Return the 16 bits of input data
// Query the digital inputs of the GALIL, See Galil command library @IN
uint16_t Galil::DigitalInput() {
	// use IQ 65535 if the config is wrong on the Galil
	uint16_t digitalIn= 0b0;
	for (int i = 0; i < 16; i++) {
		std::string command = "MG @IN[" + std::to_string(i) + "];";
		gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
		if (gRet == G_NO_ERROR) {
			std::string output = buff;
			int value = std::stoi(output);
			if (value) {
				uint16_t mask = 0b1 << i;
				digitalIn = digitalIn | mask;
			}
		}
	}
	return digitalIn;
}

// Read either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
// A bank is one byte (8 bits). The low bank (0) is the first 8
// bits (DI0-DI7) and the high bank (1) is the upper 8 bits
// (DI8-DI15).
uint8_t Galil::DigitalByteInput(bool bank) {
	// use IQ 65535 if the config is wrong on the Galil
	uint8_t offset = 0;
	uint8_t digitalIn = 0b0;
	if (bank) offset = 8;
	for (int i = 0; i < 8; i++) {
		std::string command = "MG @IN[" + std::to_string(i + offset) + "];";
		gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
		if (gRet == G_NO_ERROR) {
			std::string output = buff;
			int value = std::stoi(output);
			if (value) {
				uint8_t mask = 0b1 << i;
				digitalIn = digitalIn | mask;
			}
		}
	}
	return digitalIn;
}

// Read single bit from current digital inputs. Above functions
// may use this function
bool Galil::DigitalBitInput(uint8_t bit) {
	uint8_t digitalIn = 0b0;
	std::string command = "MG @IN[" + std::to_string(bit) + "];";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	if (gRet == G_NO_ERROR) {
		std::string output = buff;
		int value = std::stoi(output);
		if (value) return true;
		else return false;
	};
	return 0;
}

// Check the string response from the Galil to check that the last
// command executed correctly. 1 = succesful.
// A successful write indicates that a write command (sending a 
// message to the Galil that does not have a response -- e.g., 
// digitalOutput, analogOutput) has completed without errors.
// This should validate some part of the Galil's response (it is 
// up to you how this is completed) but should validly
// differentiate when a write command has been completed 
// successfully.
// This will be called from your main function (do not call it
// within your implementation functions of this Galil class.
bool Galil::CheckSuccessfulWrite() {
	if (gRet != G_NO_ERROR) return false;
	std::string buffS = buff;
	if (buffS.empty() || buffS.find('?') != std::string::npos) return false;
	return true;
}

// ANALOG FUNCITONS
// Read Analog channel and return voltage
float Galil::AnalogInput(uint8_t channel) {
	float volt = 0;
	std::string command = "MG @AN[" + std::to_string(channel) + "];";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	if (gRet == G_NO_ERROR) {
		std::string output = buff;
		volt = std::stof(output);
	}
	return volt;
}

// Write to any channel of the Galil, send voltages as
// 2 decimal place in the command string
void Galil::AnalogOutput(uint8_t channel, double voltage) {
	if (voltage > 1000 || voltage < -1000) return;
	char volt[12];
	sprintf(volt, "%.2f", voltage);
	std::string command = "AO " + std::to_string(channel) + "," + volt + ";";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	return;
}

// Configure the range of the input channel with
// the desired range code
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	std::string command = "AQ " + std::to_string(channel) + "," + std::to_string(range) + ";";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	return;
}

// ENCODER
// Manually Set the motor encoder value to zero (encoder channel 0)
void Galil::WriteEncoder() {
	encoderValA = 0;
	std::string command = "QE 1;";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	if (gRet == G_NO_ERROR) {
		std::string output = buff;
		encoderValB = std::stoi(output);
	}
	command = "WE " + std::to_string(encoderValA) + "," + std::to_string(encoderValB) + ";";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	return;
}

// Read from motor Encoder (encoder channel 0)
int Galil::ReadEncoder() {
	std::string command = "QE 0;";
	gRet = Functions->GCommand(g, command.c_str(), buff, sizeof(buff), &n);
	if (gRet == G_NO_ERROR) {
		std::string output = buff;
		encoderValA = std::stoi(output);
	}
	return encoderValA;
}

// CONTROL FUNCTIONS
// Set the desired setpoint for control loops, counts or counts/sec
// This should set it within the class not on the actual Galil.
void Galil::setSetPoint(int s) {
	setPoint = s;
	return;
}

// Gets the current setpoint stored in the class
double Galil::getSetPoint() {
	return setPoint;
}

// Set the proportional gain of the controller used in controlLoop() of Position/SpeedControl
// This should set it within the class not on the actual Galil.
void Galil::setKp(double gain) {
	ControlParameters[0] = gain;
	return;
}

// Gets the current proportional gain stored in the class
double Galil::getKp() {
	return ControlParameters[0];
}

// Set the integral gain of the controller used in controlLoop()  of Position / SpeedControl
// This should set it within the class not on the actual Galil.
void Galil::setKi(double gain) {
	ControlParameters[1] = gain;
	return;
}

// Gets the current integral gain stored in the class
double Galil::getKi() {
	return ControlParameters[1];
}

// Set the derivative gain of the controller used in controlLoop()  of Position/SpeedControl
// This should set it within the class not on the actual Galil.
void Galil::setKd(double gain) {
	ControlParameters[2] = gain;
	return;
}

// Gets the current derivative gain stored in the class
double Galil::getKd() {
	return ControlParameters[2];
}

// OPERATOR OVERLOADS
// Operator overload for '<<' operator. So the user can say cout << Galil;
// This function should print out the output of GInfo and GVersion, with
// two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil) {
	std::string info;
	std::string version;
	if (galil.g == nullptr || galil.g == (GCon)0x1) {
		output << "G connection cannot be found (simulator)" << std::endl;
		return output;
	}
	galil.gRet = GInfo(galil.g, galil.buff, sizeof(galil.buff));
	if (galil.gRet == G_NO_ERROR) info = galil.buff;
	galil.gRet = GVersion(galil.buff, sizeof(galil.buff));
	if (galil.gRet == G_NO_ERROR) version = galil.buff;
	output << info << "\n\n" << version << "\n\n";
	return output;
}

// Copy assignment operator. This acts in the same way as the copy constructor
// (refer above for details).
Galil& Galil::operator=(const Galil& other) {
	// Check if self
	if (this == &other) return *this;
	// Check if own connecton exist
	if (!FunctionOwnership) {
		Functions = new EmbeddedFunctions;
		FunctionOwnership = true;
		gRet = Functions->GOpen("192.168.0.120 -d", &g);
		if (gRet != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
	}
	ControlParameters[0] = other.ControlParameters[0];
	ControlParameters[1] = other.ControlParameters[1];
	ControlParameters[2] = other.ControlParameters[2];
	setPoint = other.setPoint;
	BOUTBank1 = other.BOUTBank1;
	BOUTBank2 = other.BOUTBank2;
	encoderValA = other.encoderValA;
	encoderValB = other.encoderValB;
	return *this;
}
