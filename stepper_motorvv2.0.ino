/* slave
  deze arduino ontvangt data via serial van de bluetooth scherm arduino
  deze data gebruikt hij om de motoren aan te sturen.
*/


#include <AccelStepper.h>
#include "MultiStepper.h"

#include "smoothJoystick.h";
#define joystickPin A0 // analog pin A1
#define joystickPin1 A3// analog pin A1
#define deadband 20 // de joustick word niet aangeraakt
#define deadband1 20 // de joustick word niet aangeraakt

#define JoyX A1  // Joystick X pin connected to A1 on the UNO
#define JoyY1 A2  // Joystick Y pin connected to A2 on the UNO


// Define a stepper and the pins it will use
AccelStepper stepperYaw(AccelStepper::DRIVER, 2, 3); // stappen motor step and dir pins
AccelStepper stepperPitch(AccelStepper::DRIVER, 5, 6); // stappen motor step and dir pins

MultiStepper StepperControl;  // Create instance of MultiStepper

long joystepX=0; // Used to move steppers when using the Joystick
long joystepY1=0; // Used to move steppers when using the Joystick

volatile long XInPoint=0;  // motor 0 begin punt
volatile long ZInPoint1=0;  // motor 1 begin punt
volatile long XOutPoint=0;  // mototr 0 eind punt
volatile long ZOutPoint1=0;  // motor 1 begin punt

int InandOut=0;  // Used to detect if IN and OUT points have been set

long gotoposition[2];  // Used to move steppers using MultiStepper Control

joystick joy(joystickPin, 258);
joystick joy1(joystickPin1, 258);

int sign = 0;  // als sign negatief is draaid hij de ander kant op
int sign1 = 0;  // als sign negatief is draaid hij de ander kant op

#include <SoftwareSerial.h>
SoftwareSerial BTserial(10, 11); // RX/TX serial comicatie bluetooth

int optellen =1;
int beginScherm =1;
int beginPuntScherm =0;
int eindPuntScherm =0;
int tijdInstelScherm =0;
int timeLapseBezig =0;

/////////////////// nieuwe code/////////////
int signYaw =0;  // links of rechts om draaien 
int signPitch =0;                               

// Variable om de stand van potmeter 1 op te slaan
int valYaw = 0;
int long newvalYaw  = 0;  // waarde zal worden aangepast aan motor snelheid

 // Variable om de stand van potmeter 2 op te slaan
int valPitch = 0;
int long newvalPitch = 0; // waarde zal worden aangepast aan motor snelheid

int  switchYaw = 8;  // waarde zal worden aangepast aan motor snelheid
int switchPitch =9;  // waarde zal worden aangepast aan motor snelheid

int cameraOut = 7; // dit is de uitgang die naar de camera gaat en de camera een foto laat maken
int voltMeter = A5; // meet hoeveel de accu nog is
int ledGroen =12; // gaat aan als je de kop aan zet
int ledRood = 13; // gaat aan als de spanning onder de 10volt komt 

float vPow = 4.7; // voltage arduino
float r1 = 100000; // weestand 1
float r2 = 10000; // weestand 2
 



////////////////////////////////////////// hieronder volgs oude code die waarschijnlijk weg kan

int snelheidmotoren =0; // is de snelheid die je insteld in het beginscherm dit kan 1,2 of 3 zijn

int ontvangenBestanden =0; // als er op + of - images      of +0f - delay wordt gedrukt wordt dat nu ontavgen via de BTserial

int totaalfotos =0;  // wordt opgehoogd of verlaagd met 50 als bestand wordt ontvangen via de BTserial
int totaaldelay =0; // wordt opgehoogd of verlaagd met 1 als bestand wordt ontvangen via de BTserial
int totaaldelayAftellen =0; // hier telt hij de delay af

int stappen =0; // is het totlaal aantal stappen to dat  motor 0 bij het eindpunt is;
int stappen1 =0;  // is het totlaal aantal stappen to dat  motor 1 bij het eindpunt is;

int stappenPerInterval =0; // is het aantal stappen tot de volgende delay/foto voor motor 0
int stappenPerInterval1 =0; // is het aantal stappen tot de volgende delay/foto voor motor 1

int stepper0 = 0; //is het aantal stpappen dat hij al heeft gemaakt 
int stepperOne = 0; //is het aantal stpappen dat hij al heeft gemaakt 


unsigned long previousMillis = 0; // seconde die aftellen tot de volgende foto
const long interval = 1000; // dit is 1 seconden

void setup()
{ 
  Serial.begin(9600);  // serial naar pc
  BTserial.begin(38400); // serial naar andere arduino
  stepperYaw.setMaxSpeed(3000);
  stepperYaw.setAcceleration(500);
  stepperPitch.setMaxSpeed(3000);
  stepperPitch.setAcceleration(500);

  stepperYaw.moveTo(1500);
  StepperControl.addStepper(stepperYaw);  // Add Stepper #1 to MultiStepper Control
  StepperControl.addStepper(stepperPitch);

  pinMode(cameraOut,OUTPUT);
  pinMode(eindschakelaar,INPUT);
  pinMode(ledGroen, OUTPUT);
  pinMode(ledRood, OUTPUT);


  beginscherm(); // zorgt ervoor dat hij kan optellen
}

void loop()
{
     unsigned int analogVal = joy.smoothRead(); // dit is de smoothed out analog waarde
     unsigned int analogVal1 = joy1.smoothRead(); // dit is de smoothed out analog waarde
     digitalWrite(ledGroen,HIGH); // led die aan gaat bij de opstart

      float v = (analogRead(0) * vPow) / 1024.0;
      float v2 = v / (r2 / (r1 + r2));

       
    /*//////////////////////////////////////////////////////////////////////////////////////////////////
                            beginscherm  nu kan je vrij de motoren bewegen 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/
  
if (optellen >0 && beginScherm == 1){
   // nu kan je de motoren zelf handmatig bedienen

  valYaw  = analogRead(A0);  //  leest de potmeter voor Yaw
  valPitch = analogRead(A1);  //  leest de 2de potmeter voor Pitch
  
// yaw motor
  if ( digitalRead( switchYaw) == HIGH){  // kijkt of de yaw switch naar lins of rechts staat en zet zo de snelheid positief of negatief
    signYaw =1;
  }
  else if( digitalRead( switchYaw) == LOW){
    signYaw =-1;
  }
 // pitch motor
 if ( digitalRead( switchPitch) == HIGH){  // kijkt of de yaw switch naar lins of rechts staat en zet zo de snelheid positief of negatief
    signPitch =1;
  }
  else if( digitalRead( switchPitch) == LOW){
    signPitch =-1;
  }
 
         newvalYaw  = signYaw*(map(valYaw , 0, 1023, 0, 1600));  // Map value (1600 = 1 stepper shaft rotation)
         stepperYaw.setSpeed(newvalYaw); // snelheid
         stepperYaw.runSpeed();
         


        newvalPitch = signPitch*(map(valPitch, 0, 1023, 0, 1600));  // Map value (1600 = 1 stepper shaft rotation)
        stepperPitch.setSpeed(newvalPitch);  // Move stepper to new position
        stepperPitch.runSpeed();

         beginscherm();
      
     if (optellen ==5){ // nu is de timelapse knop ingedruk om het beginpunt intestellen
      beginScherm =0;
      optellen =0;
      beginPuntScherm = 1;
     }
   
  
   
}
      /*//////////////////////////////////////////////////////////////////////////////////////////////////
                          zet begin  begin punt waar de timelapse begint 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

 else if (optellen ==0 && beginPuntScherm ==1){
   valYaw  = analogRead(A0);  //  leest de potmeter voor Yaw
   valPitch = analogRead(A1);  //  leest de 2de potmeter voor Pitch
  
      // yaw motor
        if ( digitalRead( switchYaw) == HIGH){  // kijkt of de yaw switch naar lins of rechts staat en zet zo de snelheid positief of negatief
          signYaw =1;
        }
        else if( digitalRead( switchYaw) == LOW){
          signYaw =-1;
        }
       // pitch motor
       if ( digitalRead( switchPitch) == HIGH){  // kijkt of de yaw switch naar lins of rechts staat en zet zo de snelheid positief of negatief
          signPitch =1;
        }
        else if( digitalRead( switchPitch) == LOW){
          signPitch =-1;
        }
       
               newvalYaw  = signYaw*(map(valYaw , 0, 1023, 0, 1600));  // Map value (1600 = 1 stepper shaft rotation)
               stepperYaw.setSpeed(newvalYaw); // snelheid
               stepperYaw.runSpeed();
         
               newvalPitch = signPitch*(map(valPitch, 0, 1023, 0, 1600));  // Map value (1600 = 1 stepper shaft rotation)
               stepperPitch.setSpeed(newvalPitch);  // Move stepper to new position
               stepperPitch.runSpeed();
    
      XInPoint=stepperYaw.currentPosition(); // dit is het beginpunt van motor 0
      ZInPoint1=stepperPitch.currentPosition(); // dit is het beginpunt van motor 1
    
     Serial.println(XInPoint);
     
      beginpunt();   
  if (optellen ==6){ // nu is de volgende knop ingedrukt om het eind punt intestellen
       
      beginPuntScherm = 0;
      eindPuntScherm =1;
      optellen =0;
     }
  if(optellen ==199)  // hij gaat nu weer terug naar het begin scherm en reset alle waardes
  {  
   optellen=1; 
   beginScherm =1;
   beginPuntScherm =0;
  }   
 }

       /*//////////////////////////////////////////////////////////////////////////////////////////////////
                          zet het eind punt waar de timelapse eindigt
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

 else if( beginPuntScherm == 0 && eindPuntScherm == 1){
  
      valYaw  = analogRead(A0);  //  leest de potmeter voor Yaw
      valPitch = analogRead(A1);  //  leest de 2de potmeter voor Pitch
  
      // yaw motor
        if ( digitalRead( switchYaw) == HIGH){  // kijkt of de yaw switch naar lins of rechts staat en zet zo de snelheid positief of negatief
          signYaw =1;
        }
        else if( digitalRead( switchYaw) == LOW){
          signYaw =-1;
        }
       // pitch motor
       if ( digitalRead( switchPitch) == HIGH){  // kijkt of de yaw switch naar lins of rechts staat en zet zo de snelheid positief of negatief
          signPitch =1;
        }
        else if( digitalRead( switchPitch) == LOW){
          signPitch =-1;
        }
       
               newvalYaw  = signYaw*(map(valYaw , 0, 1023, 0, 1600));  // Map value (1600 = 1 stepper shaft rotation)
               stepperYaw.setSpeed(newvalYaw); // snelheid
               stepperYaw.runSpeed();
         
               newvalPitch = signPitch*(map(valPitch, 0, 1023, 0, 1600));  // Map value (1600 = 1 stepper shaft rotation)
               stepperPitch.setSpeed(newvalPitch);  // Move stepper to new position
               stepperPitch.runSpeed();
    
     XOutPoint=stepperYaw.currentPosition(); // dit is het eind punt van motor 0
     ZOutPoint1=stepperPitch.currentPosition(); // dit is het eind punt van motor 1

     Serial.println(XInPoint);
        eindpunt();
        
  if (optellen ==7){ // nu is de volgende knop ingegrukt om de tijd in te gaan instellen

      gotoposition[0]=XInPoint; //nu draaid motor 0 terug naar het beginpunt
      gotoposition[1]=ZInPoint1; // nu draaid motor 1 terug naar het beginpunt
      StepperControl.moveTo(gotoposition);  // deze code zorgt er volgens mij voor dat ze allebei tegelijk bij het eindpunt aankomen
      StepperControl.runSpeedToPosition();

     tijdInstelScherm =1;
     eindPuntScherm =0;
    
     Serial.println(XInPoint);
     }

  if(optellen ==199)  // hij gaat nu weer terug naar het begin scherm en reset alle waardes
     {  
         optellen=1; 
         beginScherm =1;
         beginPuntScherm =0;
         eindPuntScherm = 0;
      } 
   
 }

 /*//////////////////////////////////////////////////////////////////////////////////////////////////
                         nu wordt de tijd ingesteld
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

 else if( eindPuntScherm == 0 && tijdInstelScherm == 1){ 

    fotoinstelscherm();
   
 

  if (ontvangenBestanden ==8){  // nu is de start knop ingedrukt en gaat de timelapse beginnen

    stappen = (((XInPoint)-(XOutPoint))*-1); // dit zijn de totaal aantal stappen dat motor 0 moet maken
    stappen1= (((ZInPoint1)-(ZOutPoint1))*-1);  // dit zijn de totaal aantal stappen dat motor 1 moet maken
  
   stappenPerInterval = (stappen/totaalfotos);  // is het aantal stappen per interval voor motor 0
   stappenPerInterval1 = (stappen1/totaalfotos); // is het aantal stpapen per interval voor motor 1

   stepper0 = XInPoint;
   stepperOne= ZInPoint1;
   
    totaaldelayAftellen =  totaaldelay; // hier geeft hij de totaaldelay waarde aan toaaldelayaftellen zodat hij kan aftellen
    Serial.println(stappenPerInterval);
     tijdInstelScherm =0;
     timeLapseBezig =1;
  }

  
  if(ontvangenBestanden ==199)  // hij gaat nu weer terug naar het begin scherm en reset alle waardes
     {  
         optellen=1; 
         beginScherm =1;
         beginPuntScherm =0;
         eindPuntScherm = 0;
         tijdInstelScherm =0;
      } 
  
  
  
  
  
  }

 /*//////////////////////////////////////////////////////////////////////////////////////////////////
                     nu is de Timelapse daadwerkelijk bezig ( eindelijk :) )
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

else if( tijdInstelScherm == 0 && timeLapseBezig ==1){


///hier telt hij de delay af hierna gaan de motoren weer verder. Dit is de delay tijdens de foto
  unsigned long currentMillis = millis();
     if (currentMillis - previousMillis >= interval) {
                
                previousMillis = currentMillis;
             
                digitalWrite(cameraOut,HIGH); // nu maakt hij een foto
               
                
                
                if (totaaldelayAftellen ==1){
                     //  als de delay gelijk is aan 1 zal hij de motoren verder zetten.
                     digitalWrite(cameraOut,LOW); // hij zet de uitgang voor de foto's maken weer laag
                     gotoposition[0]= stepper0;  // is de nieuwe positie van de motoren 
                     gotoposition[1]= stepperOne;
                     StepperControl.moveTo(gotoposition);
                     StepperControl.runSpeedToPosition();

                     if ((stepperYaw.distanceToGo() == 0) && (stepperPitch.distanceToGo() == 0 )){ // nu telt de delay alleen af als de  motor bij zijn nieuw locatie is aangekomen
                      

                      stepper0 = (stepper0 + (stappenPerInterval)); // is de nieuwe locatie voor motor 0
                      stepperOne = (stepperOne + (stappenPerInterval1)); // is de nieuwe locatie voor motor 1
                     }
                }
                   
              }

           if (stepper0 == XOutPoint){  // nu zijn de motoren op hun eind punt en begint alles weer opnieuw
              tijdInstelScherm =0;
              timeLapseBezig =0;
              beginScherm = 1;
              totaaldelayAftellen =0;
              optellen =1;
              }


            if(totaaldelay  ==199)  // hij gaat nu weer terug naar het begin scherm en reset alle waardes
              {  
                   optellen=1; 
                   beginScherm =1;
                   beginPuntScherm =0;
                   eindPuntScherm = 0;
                   tijdInstelScherm =0;
                   timeLapseBezig =0;
                } 
   timelapseisbezig();
   
  }

 ///////////////// einde van de loop/////////////////////////






 
}


/*//////////////////////////////////////////////////////////////////////////////////////////////////
                            beginscherm
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

void beginscherm(){

  if (BTserial.available()) { // Checks whether data is comming from the serial port
     optellen = BTserial.read(); // Reads the data from the serial port

    if (optellen == 1){   
      snelheidmotoren = 1;}  // als de ontvangen snelheid 1 zal hij snelheid 1 instellen

    if (optellen == 2){
      snelheidmotoren = 2;}  // als de ontvangen snelheid 2 zal hij snelheid 2 instellen

     if (optellen == 3){
      snelheidmotoren = 3;}   // als de ontvangen snelheid 3 zal hij snelheid 3 instellen 
    }
   Serial.println(optellen);
}


void beginpunt(){

  if (BTserial.available()) { // Checks whether data is comming from the serial port
     optellen = BTserial.read(); // Reads the data from the serial port
    Serial.println(optellen);
  }
}


void eindpunt(){

  if (BTserial.available()) { // Checks whether data is comming from the serial port
     optellen = BTserial.read(); // Reads the data from the serial port
    Serial.println("test");
  }
}

void fotoinstelscherm(){
// hieronder ontvant hij de aantal foto's 
  if (BTserial.available()) { // Checks whether data is comming from the serial port
     ontvangenBestanden = BTserial.read(); // Reads the data from the serial port

  if (ontvangenBestanden ==8){  // nu is de start knop ingedrukt en gaat de timelapse beginnen
    
     tijdInstelScherm =0;
     timeLapseBezig =1;
     }
     
 if (ontvangenBestanden ==15){
    totaalfotos = totaalfotos +50;  // hij telt nu 50 op bij de foto's
   }

   if (ontvangenBestanden ==16){
    totaalfotos = totaalfotos -50; // hij trekt nu 50 af van de foto's
   }

   
// hieronder ontvangt hij de aantal delay
   if (ontvangenBestanden ==18){
    totaaldelay = totaaldelay +1;  // hij telt nu 1 op bij de delay
   }

   if (ontvangenBestanden ==19){
    totaaldelay = totaaldelay -1; // hij trekt nu 51 af van de delay
   }
    Serial.println( ontvangenBestanden);
}}


void timelapseisbezig(){

 if (BTserial.available()) { // Checks whether data is comming from the serial port
     totaaldelay = BTserial.read(); // Reads the data from the serial port
     totaaldelayAftellen = totaaldelay; // als de delay kleiner is dan 0 zet hij terug naar de ingestelde delay

     Serial.println(totaaldelayAftellen);
  }
  
}
