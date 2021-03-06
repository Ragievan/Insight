// Insight 2016
// HackJob Mafia :)
// @RedLoop

//-------------------------------

#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

float a;
float b;
float c;

float acck;

Adafruit_BNO055 bno = Adafruit_BNO055();

File myFile;

const int  cs = 8; //rtc chip select
const int cssd = 10; //sd chip select
const int beat = 6; //Polar heart beat pin LOW on pulse

float res;

int beatstate = 0; //state of pulse train
int beatState = 0; //variable for the beatState to be stored in

long beatTime; //long integer for heart beat millis
long myBeatMillis[1]; //data array for beat samples



void setup() {
  digitalWrite(cssd, HIGH);

  pinMode(10, OUTPUT);

  Serial.begin(19200);   //Serial begin at 19200, but opens monitor at 9600  :|



  digitalWrite(cssd, LOW);
  delay(10);

  //------------------------------

  
  SPI.setDataMode(SPI_MODE0);  // switch mode to SD

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }


  Serial.println("Card Accepted...");

  myFile = SD.open("HBlog.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to HBlog.txt...");
    // close the file:
    myFile.println("Restart");
    myFile.close();
    Serial.println("Please Wait");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening HBlog.txt");
  }


  digitalWrite(cssd, HIGH);

  SPI.setDataMode(SPI_MODE3);  // switch mode to clock
  delay(10);
  // end SD card insert




}

void loop() {

  // Serial.println(ReadTimeDate());  // Included as a trial, need to configure correct date and time


  //create the counter for the data array
  for (int sampleNumber = 0; sampleNumber < 1; sampleNumber++) {

    //wait here until a beat(LOW) is received
    beatState = digitalRead(beat);
    while (beatState == HIGH) {
      beatState = digitalRead(beat);
    }

    beatTime = millis(); //log the number of millis
    myBeatMillis[sampleNumber] = beatTime;

    while (beatState == LOW) {
      beatState = digitalRead(beat);


    }
  }



  // write millis to the SD card
  myFile = SD.open("HBlog.txt", FILE_WRITE);

  if (myFile) {



    //read the data out of the array
    for (int sampleNumber = 0; sampleNumber < 1; sampleNumber++) {
      myFile.print(myBeatMillis[sampleNumber]);

      myFile.print("         ");
      imu::Vector<3> lineaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

      a = abs(lineaccel.x());
      b = abs(lineaccel.y());
      c = abs(lineaccel.z());

      acck = a + b + c;       //Resolving absolute XYZ values into a single vector

      myFile.println(acck);  // Resolved absolute XYZ values .. marco altini

    }
  }

  // close the file:
  myFile.close();

}








