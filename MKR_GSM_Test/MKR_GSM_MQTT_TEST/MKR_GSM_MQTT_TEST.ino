// This example uses an Arduino MKR GSM 1400 board
// to connect to shiftr.io.
//
// IMPORTANT: This example uses the new MKRGSM library.
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Sandeep Mistry
// https://github.com/256dpi/arduino-mqtt

#include <MKRGSM.h>
#include <MQTT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char apn[] = "telia";
const char login[] = "";
const char password[] = "";


GSMClient net;
GPRS gprs;
GSM gsmAccess;
MQTTClient client;

static int ONE_WIRE_BUS = 2;
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature TemperatureSensors(&oneWire);

unsigned long lastMillis = 0;

void connect() {
	// connection state
	bool connected = false;

	Serial.print("connecting to cellular network ...");

	// After starting the modem with gsmAccess.begin()
	// attach to the GPRS network with the APN, login and password
	while (!connected) {
		if ((gsmAccess.begin() == GSM_READY) &&
			(gprs.attachGPRS(apn, login, password) == GPRS_READY)) {
			connected = true;
		}
		else {
			Serial.print(".");
			delay(1000);
		}
	}

	Serial.print("\nconnecting...");
	while (!client.connect("arduino", "try", "try")) {
		Serial.print(".");
		delay(1000);
	}

	Serial.println("\nconnected!");

	client.subscribe("/hello");
	// client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
	Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {
	Serial.begin(115200);
	TemperatureSensors.begin();
	// Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
	// You need to set the IP address directly.
	client.begin("broker.hivemq.com", net);
	client.onMessage(messageReceived);
	

	connect();
	gsmAccess.lowPowerMode();
}

void loop() {
	client.loop();
	
	if (!client.connected()) {
		connect();
	}

	// publish a message roughly every second.
	if (millis() - lastMillis > 30000) {
		lastMillis = millis();
		// Getting Temperatures
		TemperatureSensors.requestTemperatures();
		float temerature = TemperatureSensors.getTempCByIndex(0);
		client.publish("/Temp/Test", String(temerature), true, 0);
		client.publish("/hello", String(temerature));
	}
}