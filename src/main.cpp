#include <WiFi.h>
#include <ESPmDNS.h>

const char* ssid = "PKC Security";
const char* password = "dam2ranch2comet2gist2slay2kept";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server{80, 4};

class Signal
{
public:
	Signal(int pin, unsigned long pulseDuration): pin{pin}, pulseDuration{pulseDuration}, isOn{false}, pulseStartTime{0UL}
	{
	}

	void setup(void)
	{
		Serial.println("setting pin to OUTPUT");
		pinMode(pin, OUTPUT);
		Serial.println("setting pin to HIGH");
		digitalWrite(pin, HIGH);
	}

	void pulse(void)
	{
		if(isOn) return;

		Serial.println("setting pin to LOW");
		digitalWrite(pin, LOW);
		isOn = true;
		pulseStartTime = millis();
	}

	void loop(void)
	{
		if(!isOn) return;

		unsigned long millisElapsed = millis() - pulseStartTime;

		if(millisElapsed >= pulseDuration)
		{
			Serial.println("setting pin to HIGH");
			digitalWrite(pin, HIGH);
			isOn = false;
			pulseStartTime = 0UL;
		}
	}

private:
	int pin;
	unsigned long pulseDuration;
	bool isOn;
	unsigned long pulseStartTime;
};

Signal signal{15, 2000UL/*ms*/};

void setup(void)
{
	Serial.begin(115200);

	signal.setup();

	// Connect to WiFi network
	Serial.print("Connecting to ");
	Serial.print(ssid);
	WiFi.begin(ssid, password);

	// Wait for connection
	while(WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("Connected!");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	// Set up mDNS responder
	if(!MDNS.begin("zappy"))
	{
		Serial.println("Error setting up MDNS responder!");
		while(true)
		{
			delay(1000);
		}
	}
	Serial.println("mDNS responder started");

	// Start TCP (HTTP) server
	server.begin();
	Serial.println("TCP server started");

	// Add service to MDNS-SD
	MDNS.addService("http", "tcp", 80);
}

void loop(void)
{
	signal.loop();

	// Check if a client has connected
	WiFiClient client = server.available();
	if(!client)
	{
		return;
	}

	Serial.println(">>> Client Connected");

	// Wait for data from client to become available
	while(client.connected() && !client.available())
	{
		delay(1);
	}

	// Read the first line of HTTP request
	// NOTE: potential DDoS point
	// replace with timed and buffered read so only
	// GET / HTTP/1.1 is a valid request that fits
	// in the buffer
	String req = client.readStringUntil('\r');

	// Extract path part of HTTP GET request
	int pathStartIndex = req.indexOf(' ');
	int pathEndIndex = req.indexOf(' ', pathStartIndex + 1);

	if(pathStartIndex == -1 || pathEndIndex == -1)
	{
		Serial.print("Invalid Request: ");
		// print the first 64 characters of the request
		String shortReq = req.substring(0, min(req.length(), 64U));
		Serial.println(shortReq);
		return;
	}

	req = req.substring(pathStartIndex + 1, pathEndIndex);
	Serial.print("Request: ");
	// print the first 64 characters of the request
	String shortReq = req.substring(0, min(req.length(), 64U));
	Serial.println(shortReq);

	String resp;

	if (req == "/")
	{
		resp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Zapping...</html>\r\n\r\n";
		Serial.println("Zapping...");
		signal.pulse();
		Serial.println("Sending 200");
	}
	else
	{
		resp = "HTTP/1.1 404 Not Found\r\n\r\n";
		Serial.println("Sending 404");
	}

	// Send HTTP response
	client.print(resp);

	client.stop();
	Serial.println("<<< Client Disconnected");
}