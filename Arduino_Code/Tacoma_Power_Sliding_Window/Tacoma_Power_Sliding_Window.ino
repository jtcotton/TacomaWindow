/*
Title:		Toyota Tacoma Power Sliding Window Arduino Code
Author:		Jonathan Cotton (aka TXCotton)
Date:		01/31/2015
Attribution:
		Original idea and R&D by "scratch" on tacomaworld.com
		http://www.tacomaworld.com/forum/2nd-gen-tacomas/356628-power-sliding-window-again.html
Description:	
		This code reads a momentary ON-OFF-ON rocker switch & 2 limit switches.  Using these
		inputs, it controls a DC Motor to open / close the rear sliding window.  The rocker
		switch acts as an automatic open / close switch, allowing the user to press and release,
		while the window opens or closes completely without having to hold down the switch for
		the entire duration of travel, much like modern auto down power windows.  Pressing the
		opposite rocker switch direction during movement will stop the movement completely but
		will not change direction (e.g.: while closing, if open is pressed, the window will stop).

		This assumes the following hardware:
		*  3D Printed Mounting Brackets & Leadscrew (https://github.com/scratchhax/TacomaWindow)
		*  Momentary ON-OFF-ON switch (ex: http://www.amazon.com/gp/product/B00H8RD000)
		*  L293D H-Bridge Motor Driver (or compatible such as SN754410 from https://www.sparkfun.com/products/315)
		*  Arduino Pro Mini 5V (https://www.sparkfun.com/products/11113)
		*  L7805 5V Regulator (https://www.sparkfun.com/products/107)
		*  2 x D2F-L-D3 or similar Limit Switches (http://www.digikey.com/product-detail/en/D2F-L-D3/D2F-L-D3-ND)
		*  300RPM DC Motor (http://www.amazon.com/gp/product/B0080DL25Q)
		*  3mm to 5mm Motor Shaft Coupler (http://www.amazon.com/gp/product/B00KHTVZ1M)
		*  10-24 threaded rod, nylon lock nuts, washers
		*  Misc wire, protoboard, screw terminals, screws, solder, etc...
*/

/**********************************************************************************************
Type Definitions
**********************************************************************************************/
enum dir {
  OFF,
  OPEN,
  CLOSE
};


/**********************************************************************************************
Variable Declarations
**********************************************************************************************/
int enaPin = 2;
int in1Pin = 3;
int in2Pin = 8;
int switchClosePin = 10;
int switchOpenPin = 11;
int limitClosePin = 12;
int limitOpenPin = 9;
dir state = OFF;


/**********************************************************************************************
Function Declarations
**********************************************************************************************/
void setup();
void loop();
void setState(dir newState);


/**********************************************************************************************
Configure I/O Pins when powered on.

Note:	All Input Pins are pulled up to VCC, so we are using inverse logic by pulling all Inputs
	to GND when the are activated.
**********************************************************************************************/
void setup()
{
  /********************************************************************************************
	L293D Connections:		Arduino Pro Mini Connections:	L7805 Connections:

	1	ENA			GND	GND			IN	Truck +12V
	2	In1			RAW	+5V			GND	GND
	3	Motor +			2	ENA			OUT	Arduino RAW
	4	GND			3	In1
	6	Motor -			8	In2
	7	In2			10	SwitchClose
	8	Truck +12V		11	SwitchOpen
	16	Truck +12V		12	LimitClose
					9	LimitOpen

	Switch Connections:		Limit Open Connections:		Limit Close Connections:

	Open	SwitchOpen(11)		NO	LimitOpen(9)		NO	LimitClose(12)
	COM	GND			COM	GND			COM	GND
	Close	SwitchClose(10)

  ********************************************************************************************/

  pinMode(enaPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(switchClosePin, INPUT_PULLUP);
  pinMode(switchOpenPin, INPUT_PULLUP);
  pinMode(limitClosePin, INPUT_PULLUP);
  pinMode(limitOpenPin, INPUT_PULLUP);
}


/**********************************************************************************************
Detect state changes in a loop
**********************************************************************************************/
void loop()
{
  /********************************************************************************************
	State Change Scenarios:

	LastState	LimitClose	LimitOpen	SwitchClose	SwitchOpen	NewState
	OFF		X		HIGH		X		LOW		OPEN		If Stopped & Open Button pressed & Not @ Open Limit, start opening
	OFF		HIGH		X		LOW		X		CLOSE		If Stopped & Close Button pressed & Not @ Close Limit, start Closing
	OPEN		X		X		LOW		X		OFF		If Opening & Close Button pressed, stop
	OPEN		X		LOW		X		X		OFF		If Opening & Opened limit reached, stop
	CLOSE		X		X		X		LOW		OFF		If Closing & Open Button pressed, stop
	CLOSE		LOW		X		X		X		OFF		If Closing & Closed limit reached, stop


	Note:	All Input Pins are pulled up to VCC (normally HIGH), so we are using inverse logic by connecting the Input
		to GND when activated so they show up as LOW.
  ********************************************************************************************/

  if (state == OFF && digitalRead(limitOpenPin) == HIGH && digitalRead(switchOpenPin) == LOW )
  {
    setState(OPEN);
  }
  else if (state == OFF && digitalRead(limitClosePin) == HIGH && digitalRead(switchClosePin) == LOW )
  {
    setState(CLOSE);
  }
  else if (state == OPEN && digitalRead(switchClosePin) == LOW )
  {
    setState(OFF);
  }
  else if (state == OPEN && digitalRead(limitOpenPin) == LOW )
  {
    setState(OFF);
  }
  else if (state == CLOSE && digitalRead(switchOpenPin) == LOW )
  {
    setState(OFF);
  }
  else if (state == CLOSE && digitalRead(limitClosePin) == LOW )
  {
    setState(OFF);
  }
}


/**********************************************************************************************
Set outputs based on new state
**********************************************************************************************/
void setState(dir newState)
{
  /********************************************************************************************
	State Output Definitions:

	ENA	In1	In2
	1	0	0	OFF
	1	1	0	OPEN
	1	0	1	CLOSE
  ********************************************************************************************/
  state = newState;
  if (newState == OFF)
  {
    digitalWrite(enaPin, HIGH);
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);
    delay(500);
  }
  else if (newState == OPEN)
  {
    digitalWrite(enaPin, HIGH);
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  }
  else if (newState == CLOSE)
  {
    digitalWrite(enaPin, HIGH);
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  }
}
