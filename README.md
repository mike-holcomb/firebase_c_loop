#Firebase C Loop
A test C program to use the JSON-C and libCurl libraries for interacting with the Firebase API.

##Purpose
This program is the starting point for a program that runs on a relay hub for remote sensors.  Specifically, the target is for a Raspberry Pi that will also eventually listen on a RF24 compatible radio for incoming sensor data from Arduino transmitting captured data on an RF24 compatible radio.

##References
This example borrows heavily from leprechau's [curltest.c](https://gist.github.com/leprechau/e6b8fef41a153218e1f4)for building a curl request and json payload.
