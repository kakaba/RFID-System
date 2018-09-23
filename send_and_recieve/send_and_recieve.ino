#include <SPI.h>
#include <Ethernet.h>

String str;
String st;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Enter the IP address for Arduino, as mentioned we will use 192.168.0.16
// Be careful to use , insetead of . when you enter the address here
IPAddress ip(192, 168, 0, 102);

char server[] = "192.168.0.111"; // IMPORTANT: If you are using XAMPP you will have to find out the IP address of your computer and put it here (it is explained in previous article). If you have a web page, enter its address (ie. "www.yourwebpage.com")

// Initialize the Ethernet server library
EthernetClient client;


void setup() {

  // Serial.begin starts the serial connection between computer and Arduino
  Serial.begin(9600);

  // start the Ethernet connection
  Ethernet.begin(mac, ip);

  delay(3000);

  Serial.println("beggining the session");

}

void loop() {

  // Connect to the server (your computer or web page)
  sendData();
  delay(100);
  recieveData();
  delay(100);

}


void sendData() {

  client.connect(server, 80);

  if (client.connected()) {

    Serial.println("Send Data");

    client.print("GET http://127.0.0.1/write_data.php?"); // This
    client.print("mac=" );
    client.print("thisismac");
    client.print("&time=");
    client.print("thisistime");
    client.print("&date=");
    client.print("thisisdate");

    client.println(" HTTP/1.1"); // Part of the GET request
    client.println("Host: 192.168.0.103"); // IMPORTANT: If you are using XAMPP you will have to find out the IP address of your computer and put it here (it is explained in previous article). If you have a web page, enter its address (ie.Host: "www.yourwebpage.com")
    client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
    client.println(); // Empty line
    client.println(); // Empty line
    client.stop();    // Closing connection to server

  }

  else {
    // If Arduino can't connect to the server (your computer or web page)
    Serial.println("--> connection failed\n");
  }

}

void recieveData() {

  client.connect(server, 80);

  if (client.connected()) {

    Serial.println("Recieve Data");

    // Make a HTTP request:
    client.println("GET /access.php?password=arduino HTTP/1.1");
    client.println("Host: 192.168.0.103");
    client.println("Connection: close");
    client.println();

    delay(1000);

    while ( client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    // if the server's disconnected, stop the client:
    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();

      // do nothing forevermore
    }
  }

  else {
    // If Arduino can't connect to the server (your computer or web page)
    Serial.println("--> connection failed\n");
  }

}

