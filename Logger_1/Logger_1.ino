//implemented SD card and RTC with Arduino uno
//on boot opens new file on SD card called HBlog.txt - **will need to change this to a CSV**
//logs time and date to the file
//implemented same level of functionality on arduino mini
//implemented incorporate Polar chip
//implemented log beat millis
//implemented log beats as an array of 100 samples
// csv file not implemented -- not needed txt satis
//Implemented 3.3v and polar board change sd slave select to 10 because of new cct 
//board construction.

//Version 5_1

//strip out all serial comms after card initialisation

//final
//strip out all excess code
// plus check all functionality at 3.3v

/*
wires:

Red 3.7v VCC
Black GND

<SPI Bus>
Green 11
Yellow 12
Orange 13
<SPI Bus>

Brown 4 SD Slave Select
White 6 HR i/p
Grey 8 RTC Slave Select

*/

#include <SPI.h>
#include <SD.h>

File myFile;

const int  cs=8; //rtc chip select 
const int cssd=10; //sd chip select
const int beat = 6; //Polar heart beat pin LOW on pulse

int beatstate = 0; //state of pulse train
int beatState = 0; //variable for the beatState to be stored in

long beatTime; //long integer for heart beat millis
long myBeatMillis[100]; //data array for beat samples



void setup() {
  digitalWrite(cssd, HIGH);

  pinMode(10, OUTPUT);
  
  Serial.begin(9600);
  
   RTC_init();
  //day(1-31), month(1-12), year(0-99), hour(0-23), minute(0-59), second(0-59)
  //SetTimeDate(06,04,14,14,38,00); 
  
  
  
  
  
  // INsert SD card stuff
  digitalWrite(cssd, LOW);
  delay(10);
  



//SPI.setDataMode(SPI_MODE3);  // switch mode to clock

  //code

SPI.setDataMode(SPI_MODE0);  // switch mode to SD



if (!SD.begin(10)) {
Serial.println("initialization failed!");
return;
 }


  Serial.println("initialization done.");
  
  myFile = SD.open("HBlog.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to HBlog.txt...");
    myFile.println("Hello World");
	// close the file:
    myFile.close();
    Serial.println("done.");
    } else {
    // if the file didn't open, print an error:
    Serial.println("error opening HBlog.txt");
  }
  
  
  digitalWrite(cssd, HIGH);
 
  SPI.setDataMode(SPI_MODE3);  // switch mode to clock
  delay(10);
  // end SD card insert
  
  
 String NOW = ReadTimeDate();
 //Serial.println("going to Print the string");
 //Serial.println(NOW);
 //Serial.println("Printed the string?");
 
 SPI.setDataMode(SPI_MODE0);  // switch mode to SD
 
 myFile = SD.open("HBlog.txt", FILE_WRITE);
  if (myFile) {
    //Serial.print("Writing to HBlog.txt...");
    myFile.println(NOW);
	// close the file:
    myFile.close();
    //Serial.println("done.");
    } 
    
    //else {
    // if the file didn't open, print an error:
   // Serial.println("error opening HBlog.txt");
  //}
  
  
  //SPI.setDataMode(SPI_MODE3);  // switch mode to clock
  //delay(10);
  
}






void loop() {
  
  //create the counter for the data array
   for (int sampleNumber = 0; sampleNumber < 100; sampleNumber++) { 
  
  //wait here until a beat(LOW) is received
  beatState = digitalRead(beat);
  while(beatState == HIGH){
  beatState = digitalRead(beat);
}


  beatTime = millis(); //log the number of millis
 // Serial.println(beatTime);
 // Serial.println(sampleNumber);
  
  //add the beat millis to a data array
myBeatMillis[sampleNumber] = beatTime;
  
  
  
  //wait until the beat pulse ends  
while(beatState == LOW){
beatState = digitalRead(beat);
}

   }
   
// write millis to the SD card
myFile = SD.open("HBlog.txt", FILE_WRITE);
  if (myFile) {
   
   //read the data out of the array
  for (int sampleNumber = 0; sampleNumber < 100; sampleNumber++) {

    //Serial.print("Writing to HBlog.txt...");
    myFile.println(myBeatMillis[sampleNumber]);
  //  Serial.println(myBeatMillis[sampleNumber]);
	
    } 
    
  }
  // close the file:
  myFile.close();
  
}



















//=====================================
int RTC_init(){ 
	  pinMode(cs,OUTPUT); // chip select
	  // start the SPI library:
	  SPI.begin();
	  SPI.setBitOrder(MSBFIRST); 
	  SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work 
	  //set control register 
	  digitalWrite(cs, LOW);  
	  SPI.transfer(0x8E);
	  SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
	  digitalWrite(cs, HIGH);
	  delay(10);
}
//=====================================
int SetTimeDate(int d, int mo, int y, int h, int mi, int s){ 
	int TimeDate [7]={s,mi,h,0,d,mo,y};
	for(int i=0; i<=6;i++){
		if(i==3)
			i++;
		int b= TimeDate[i]/10;
		int a= TimeDate[i]-b*10;
		if(i==2){
			if (b==2)
				b=B00000010;
			else if (b==1)
				b=B00000001;
		}	
		TimeDate[i]= a+(b<<4);
		  
		digitalWrite(cs, LOW);
		SPI.transfer(i+0x80); 
		SPI.transfer(TimeDate[i]);        
		digitalWrite(cs, HIGH);
  }
}
//=====================================
String ReadTimeDate(){
	String temp;
	int TimeDate [7]; //second,minute,hour,null,day,month,year		
	for(int i=0; i<=6;i++){
		if(i==3)
			i++;
		digitalWrite(cs, LOW);
		SPI.transfer(i+0x00); 
		unsigned int n = SPI.transfer(0x00);        
		digitalWrite(cs, HIGH);
		int a=n & B00001111;    
		if(i==2){	
			int b=(n & B00110000)>>4; //24 hour mode
			if(b==B00000010)
				b=20;        
			else if(b==B00000001)
				b=10;
			TimeDate[i]=a+b;
		}
		else if(i==4){
			int b=(n & B00110000)>>4;
			TimeDate[i]=a+b*10;
		}
		else if(i==5){
			int b=(n & B00010000)>>4;
			TimeDate[i]=a+b*10;
		}
		else if(i==6){
			int b=(n & B11110000)>>4;
			TimeDate[i]=a+b*10;
		}
		else{	
			int b=(n & B01110000)>>4;
			TimeDate[i]=a+b*10;	
			}
	}
	temp.concat(TimeDate[4]);
	temp.concat("/") ;
	temp.concat(TimeDate[5]);
	temp.concat("/") ;
	temp.concat(TimeDate[6]);
	temp.concat("     ") ;
	temp.concat(TimeDate[2]);
	temp.concat(":") ;
	temp.concat(TimeDate[1]);
	temp.concat(":") ;
	temp.concat(TimeDate[0]);
  return(temp);
}


