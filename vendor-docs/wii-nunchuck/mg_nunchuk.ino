


/*************************************************************************
    iTapStick Project:
    Wii Nunchuk-to-Mouse USB Controller Stick
    Copyright (c) 2015 iTapArcade, LLC
    
    Date Created: 9 March 2015
    Date Modified: 10 March 2015
    Version: 1.0
    Visit us at iTapArcade.com
    Follow us on Twitter @iTapArcade
    
    Function: Wired and Wireless mouse emulation (mouse cursor, left click, and right click) 
    from Wii Nunchuk to support Tap and Mouse control based Games for PC, Mac, and Android based games
    
    Support our open source projects by getting an iTapStick Kit Available:
    https://www.tindie.com/products/itaparcade/itapstick-wii-nunchuck-to-mouse-usb-stick/?pt=ac_prod_search
    
    Microcontroller: Pro Trinket (3 or 5 volt) from Adafruit.com
    Note: Need 5 volt Pro Trinket for Wireless Wii Nunchuks!
 
    Wii Nunchuck Library Code from: 
    Author: Tim Teatro
    Date  : Feb 2012
     http://www.timteatro.net/2012/02/10/a-library-for-using-the-wii-nunchuk-in-arduino-sketches/
 ************************/
 
#include <Wire.h> 
#include <EEPROM.h>
//#include "wiinunchuk.h"  // Wii Nunchuk Library
//#include <ProTrinketMouse.h>    // Pro Trinket V-USB mouse emulator
#include <wiinunchuck.h>
#include <ProTrinketMouse.h>


int led = 13;    // Use LED on Pro Trinket for Status and Button Press Indicator
int loop_cnt=0;   
int mode = 0;     // Defines mode of the iTapStick: left mouse click is C or Z button
 
// parameters for reading the joystick:
int range = 256; //40                // output range of X or Y movement
int threshold = range/10;      // resting threshold
int center = range/2;         // resting position value
int fsample = 3; // poll every 1 ms
 
void setup() {
  pinMode(led, OUTPUT); 

  nunchuk_setpowerpins();    // set power pins for Wii Nunchuk
  nunchuk_init();            // initilization for the Wii Nunchuk
  delay(250);
  
  while(!nunchuk_get_data()){ // loop until Wii Nunchuk is connected to Nunchucky
  nunchuk_init();
  delay(250);
  nunchuk_get_data();
  digitalWrite(led, HIGH); 
  delay(250);
  digitalWrite(led, LOW); 
  delay(500);
  }

  /*
  nunchuk_init(); 
  delay(250);           // Once connected lets get additional data to determine if C or Z
  nunchuk_get_data();  // button is being held down for programming mode
  delay(10);           
  nunchuk_get_data();
  delay(10);           
  nunchuk_get_data();
  */
  
  digitalWrite(led, LOW);  // get LED indicator ready to show when buttons are being pressed    
 
 // Start Pro Trinket into Mouse mode
  TrinketMouse.begin();               // Initialize mouse library
}
 
void loop() {
  if( loop_cnt > fsample ) { 
    loop_cnt = 0;
    
   
    if(nunchuk_get_data())   // only check for data if data is available from Wii Nunchuk
    {

    // right and left click control
    int leftState = nunchuk_zbutton();
    int rightState = nunchuk_cbutton();

    if (leftState) // if button is pressed update
    {
      TrinketMouse.move(0,0,0,MOUSEBTN_LEFT_MASK);

      digitalWrite(led, HIGH); // show button is pressed by LED
    }else if (rightState) // if button is pressed update
    {
      TrinketMouse.move(0,0,0,MOUSEBTN_RIGHT_MASK);
       digitalWrite(led, HIGH);// show button is pressed by LED  
    }
     else
    {
    TrinketMouse.move(0,0,0,0); // if no button is pressed update
     digitalWrite(led, LOW); // show no button is pressed by LED
    }
    
    int xReading = nunchuk_joy_x();    // read the x axis
    //xReading = map(xReading, 38, 232, 0, range); // map accordingly
    //xReading = constrain(xReading,0,range);
    int xDistance = xReading - center;
    if (abs(xDistance) < threshold) {
      xDistance = 0;
    } 
 
    
    int yReading = nunchuk_joy_y();   // read the y axis
    //yReading = map(yReading, 38, 232, 0, range); // map accordingly
    //yReading = constrain(yReading,0,range);
    int yDistance = yReading - center;
    if (abs(yDistance) < threshold) {
      yDistance = 0;
    } 
 
    if ((xDistance != 0) || (yDistance != 0)) { // move the mouse based on x and y
      TrinketMouse.move(xDistance, -yDistance, 0, 0); 
    } 
    }
  }
  
  loop_cnt++;
  delay(1);
}
