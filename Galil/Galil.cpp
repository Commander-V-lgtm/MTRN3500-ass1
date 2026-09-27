#include "Galil.h"

// Default constructor. Initialize variables, open Galil connection and allocate memory.
// Should assign a default embedded functions that works with physical hardware and a 
// default Galil address as described in the assignment spec.
Galil::Galil() {
	Functions = new EmbeddedFunctions;
	FunctionOwnership = true;
	if (Functions->GOpen("192.168.0.120 -d", &g) != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
	ControlParameters[0] = 1.0;
	ControlParameters[1] = 1.0;
	ControlParameters[2] = 1.0;
	setPoint = 0;
	return;
}

// Constructor with EmbeddedFunciton pre-initialised and passed in.
Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address) : Functions(Funcs) {
	FunctionOwnership = false;
	if (Functions->GOpen(address, &g) != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
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
	if (Functions->GOpen("192.168.0.120 -d", &g) != G_NO_ERROR) std::cout << "G connection failed!" << std::endl;
	ControlParameters[0] = other.ControlParameters[0];
	ControlParameters[1] = other.ControlParameters[1];
	ControlParameters[2] = other.ControlParameters[2];
	setPoint = other.setPoint;
	return;
}

// Default destructor. Deallocate memory and close Galil connection.
Galil::~Galil() {
	if (g != nullptr && g != (GCon)0x1) GClose(g);
	else std::cout << "G failed to be found!" << std::endl;
	if (FunctionOwnership) delete Functions;
	return;
}

// DIGITAL OUTPUTS
// Write to all 16 bits of digital output, 1 command to the Galil
// Loop code inspired by bitwise practice example
void Galil::DigitalOutput(uint16_t value) {
	int N_BITS = 16;
	for (int i = 0; i < N_BITS; i++) {
		int16_t bit_mask = 1 << i;
		if (value & bit_mask) {
			std::string command = "SB ";
			command += std::to_string(i) + ";";
			Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytes);
		}
		else {
			std::string command = "CB ";
			command += std::to_string(i) + ";";
			Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytes);
		}
	}
	return;
}
	
// Write to one byte, either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
void Galil::DigitalByteOutput(bool bank, uint8_t value) {
	int offset = 0;
	if (bank) offset = 8;
	int N_BITS = 8;
	for (int i = 0; i < N_BITS; i++) {
		int8_t bit_mask = 1 << i;
		if (value & bit_mask) {
			std::string command = "SB ";
			command += std::to_string(i + offset) + ";";
			Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytes);
		}
		else {
			std::string command = "CB ";
			command += std::to_string(i + offset) + ";";
			Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytes);
		}
	}
	return;
}

// Write single bit to digital outputs. 'bit' specifies which bit
void Galil::DigitalBitOutput(bool val, uint8_t bit) {
	int no = static_cast<int>(bit);
	if (no > 16) return;
	if (val) {
		std::string command = "SB ";
		command += std::to_string(no) + ";";
		Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytes);
	}
	else {
		std::string command = "CB ";
		command += std::to_string(no) + ";";
		Functions->GCommand(g, command.c_str(), buffer, sizeof(buffer), &bytes);
	}
	return;
}


// DIGITAL INPUTS
// TODO: complete this function.
// Return the 16 bits of input data
// Query the digital inputs of the GALIL, See Galil command library @IN
uint16_t Galil::DigitalInput() {
	// All in loop
	// if check
	// read buffer
	// use postive mask + | to add no; use negative mask + & to remove no
	return static_cast<uint16_t>(0);
}

// TODO: complete this function.
// Read either high or low byte, as specified by user in 'bank'
// 0 = low, 1 = high
// A bank is one byte (8 bits). The low bank (0) is the first 8
// bits (DI0-DI7) and the high bank (1) is the upper 8 bits
// (DI8-DI15).
uint8_t Galil::DigitalByteInput(bool bank) {
	return static_cast<uint8_t>(0);
}

// TODO: complete this function.
// Read single bit from current digital inputs. Above functions
// may use this function
bool Galil::DigitalBitInput(uint8_t bit) {
	return false;
}

// TODO: complete this function.
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
	return false;
}

// ANALOG FUNCITONS
// TODO: complete this function.
// Read Analog channel and return voltage
float Galil::AnalogInput(uint8_t channel) {
	return NAN;
}

// TODO: complete this function.
// Write to any channel of the Galil, send voltages as
// 2 decimal place in the command string
void Galil::AnalogOutput(uint8_t channel, double voltage) {
	return;
}

// TODO: complete this function.
// Configure the range of the input channel with
// the desired range code
void Galil::AnalogInputRange(uint8_t channel, uint8_t range) {
	return;
}

// ENCODER
// TODO: complete this function.
// Manually Set the motor encoder value to zero (encoder channel 0)
void Galil::WriteEncoder() {
	return;
}

// TODO: complete this function.
// Read from motor Encoder (encoder channel 0)
int Galil::ReadEncoder() {
	return 0;
}

// CONTROL FUNCTIONS
// TODO: complete this function.
// Set the desired setpoint for control loops, counts or counts/sec
// This should set it within the class not on the actual Galil.
void Galil::setSetPoint(int s) {
	return;
}

// TODO: complete this function.
// Gets the current setpoint stored in the class
double Galil::getSetPoint() {
	return NAN;
}

// TODO: complete this function.
// Set the proportional gain of the controller used in controlLoop() of Position/SpeedControl
// This should set it within the class not on the actual Galil.
void Galil::setKp(double gain) {
	return;
}

// TODO: complete this function.
// Gets the current proportional gain stored in the class
double Galil::getKp() {
	return NAN;
}

// TODO: complete this function.
// Set the integral gain of the controller used in controlLoop()  of Position / SpeedControl
// This should set it within the class not on the actual Galil.
void Galil::setKi(double gain) {
	return;
}

// TODO: complete this function.
// Gets the current integral gain stored in the class
double Galil::getKi() {
	return NAN;
}

// TODO: complete this function.
// Set the derivative gain of the controller used in controlLoop()  of Position/SpeedControl
// This should set it within the class not on the actual Galil.
void Galil::setKd(double gain) {
	return;
}

// TODO: complete this function.
// Gets the current derivative gain stored in the class
double Galil::getKd() {
	return NAN;
}

// Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.
// The loop will run using the PID values specified in the data of this object, and has an 
// automatic timeout of 10s. You do NOT need to implement this function, it is defined in
// GalilControl.lib
void Galil::PositionControl(bool debug, int Motorchannel) {
	return;
}

// same as above. Setpoint interpreted as counts per second							
void Galil::SpeedControl(bool debug, int Motorchannel) {
	return;
}

// OPERATOR OVERLOADS
// TODO: complete this function.
// Operator overload for '<<' operator. So the user can say cout << Galil;
// This function should print out the output of GInfo and GVersion, with
// two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil) {
	return output;
}

// Copy assignment operator. This acts in the same way as the copy constructor
// (refer above for details).
Galil& Galil::operator=(const Galil& other) {
	return *this;
}