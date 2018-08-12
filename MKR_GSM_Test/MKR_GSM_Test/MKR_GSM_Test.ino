#include <MKRGSM.h>


// initialize the library instance
GSM gsmAccess;
GSMVoiceCall vcs;

char numtel[20];

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);


	//while (!Serial);
	Serial.println("MKR GSM 1400 DTMF");

	boolean notConnected = true;
	Serial.print("Setting up GSM connection...");
	// Start GSM
	// If your SIM has PIN, pass it as a parameter of begin() in quotes
	while (notConnected) {
		if (gsmAccess.begin() == GSM_READY) {
			notConnected = false;
			Serial.println(" Connected!");
		}
		else {
			Serial.println(" Not connected!");
			delay(1000);
		}
	}

	// This makes sure the modem correctly reports incoming events
	vcs.hangCall();
}

void loop() {

	switch (vcs.getvoiceCallStatus()) {

	case RECEIVINGCALL: // Yes! Someone is calling us
		Serial.println("Receiving call");
		vcs.retrieveCallingNumber(numtel, 20);
		Serial.print("Number:");
		Serial.println(numtel);
		vcs.answerCall();
		break;

	case TALKING:  // In this case the call would be established
		int dtmf = -1;
		uint32_t color = 0;
		do {
			if (vcs.getvoiceCallStatus() != TALKING) {
				vcs.hangCall();
				Serial.println("Hanging...");
			}
			else {
				dtmf = vcs.readDTMF(); // parse the pressed number
			}
		} while ((dtmf < 0) && (vcs.getvoiceCallStatus() == TALKING)); // read DTMF tones till a number is pressed or the call is hanged

		if (dtmf > 0)
		{
			Serial.println("Pressed button: ");
			Serial.println(dtmf);
		}

		break;
	}
}