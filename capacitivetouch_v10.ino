// master


#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h> // library touchscreen 

// For the Adafruit shield, these are the default.
#define TFT_DC 9   
#define TFT_CS 10


#include <TFT_ILI9341.h> // Hardware-specific library
#include <SPI.h>

#define TFT_GREY 0x5AEB // kleur code

TFT_ILI9341 tft = TFT_ILI9341();     // library TFT scherm

#include "smoothJoystick.h";


// kleuren
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define DARKCYAN  0x03EF


// calibratie van touchscreen
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

/*
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
*/
// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ts = Adafruit_FT6206();

int valYaw = 0;
int newvalYaw  = 0;  // zal een procent waarde worden 

 // Variable om de stand van potmeter 2 op te slaan
int valPitch = 0;
int newvalPitch = 0; // zal een procent waarde worden 

int optellen = 1;  // snelheid motoren
boolean opstartSchermTouch =1;
boolean beginSchermTouch = 0; // zet het touchscreen van het beginshcerm aan en uit
boolean beginPuntTouch =0; // zet het touchscreen van het beginpuntscherm aan en uit
boolean eindPuntTouch =0; //zet het touchscreen van het eind punt scherm aan en uit
boolean tijdTouch = 0; // zet het tijd instel touch scherm aan en uit
boolean timelapseTouch = 0; // zet het touch screen van de timelapse aan en uit
boolean tijdTimelapse =0; // dit zorgt ervoor dat de tijd loopt als het touchscreen niet wordt aangeraakt 

boolean backgroundD = false; //zet de achtergrond aan en uit 

int secondes =0;
int minuten =0;
int uren =0;

int ledRood =44;
int ledBlauw = 45;
int ledGroen = 46;


int seconde =0; 
int minuut =0;
int uur =0;
int fotos = 0;
int delays = 0;
int gemaakteFotos =0;
int vertraging =0;

int driefps =0;
int zesfps =0;
int totaleTijd =0;


// batterij intern
int analogInput = A12;
float vout = 0.0;
float vin = 0.0;
float R1 = 97200.0; // resistance of R1 (10K) -see text!
float R2 = 9690.0; // resistance of R2 (1K) - see text!
int value = 0;
int percentage =0;
unsigned long previousMillis = 0; // seconde die aftellen tot de volgende foto
const long interval = 1000; // dit is 1 seconden 

// dit om te zorgen dat hij om de 5 seconde de batterij status meet
unsigned long previousMillis1 = 0; // batterij status
const long interval1 = 10000; // meet om de 5 seconde



  void opstartScherm(){
     tft.fillRect(0, 0, 240, 320,BLACK);


     unsigned int rgbColour[3];

  // Start off with green.
  rgbColour[0] = 0;
  rgbColour[1] = 255;
  rgbColour[2] = 0;  

  // Choose the colours to increment and decrement.
  for (int decColour = 0; decColour < 3; decColour += 1) {
    int incColour = decColour == 2 ? 0 : decColour + 1;

    // cross-fade the two colours.
    for(int i = 0; i < 255; i += 1) {
      rgbColour[decColour] -= 1;
      rgbColour[incColour] += 1;
      
      setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
      delay(5);
    }
  }
      tft.setCursor(25,150);
        tft.setTextColor(WHITE,BLACK);
        tft.setTextFont(4);
        tft.setTextSize(1);
        tft.print("F O X    F L I G H T");
        tft.setCursor(55,175);
        delay(2000);
        tft.setTextFont(2);
        tft.setTextSize(1);
        tft.print("p r o d u c t i o n s");
        delay(1000);
          tft.setCursor(65,290);
         tft.setTextFont(2);
        tft.setTextSize(1);
        tft.print("press to continue");


        
        
  }

  void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(ledRood, red);
  analogWrite(ledGroen, green);
  analogWrite(ledBlauw, blue);
 }

/*//////////////////////////////////////////////////////////////////////////////////////////////////
                            beginscherm
 ///////////////////////////////////////////////////////////////////////////////////////////////*/
void beginscherm(){
      
    tft.setCursor(3,3);
    tft.setTextColor(WHITE);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.print("FOX FLIGHT");


      //////// Begin scherm///////////////////
      tft.drawRect(10,25,120,35,WHITE);  // (y,x,grote vorm kleur)
      tft.setCursor(15,30);
      tft.setTextColor(RED,BLACK); // kleur teskt,achgrond tekst
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print("timelapse");
      

      
    // Yaw text vlak//
    tft.drawRect(0,150,120,170,WHITE);  // (y,x,grote vorm kleur) Snelheid yaw
    tft.drawRect(120,150,120,170,WHITE);  // (y,x,grote vorm kleur) + knop
    tft.setCursor(35,165); // yaw
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(1);
    tft.print("Yaw");

      // Pitch text vlak//
    tft.drawRect(0,150,120,170,WHITE);  // (y,x,grote vorm kleur) Snelheid yaw
    tft.drawRect(120,150,120,170,WHITE);  // (y,x,grote vorm kleur) + knop
    tft.setCursor(145,165); // yaw
    tft.setTextColor(WHITE,BLACK);
    tft.setTextSize(1);
    tft.print("Pitch");

   
   
}
/*//////////////////////////////////////////////////////////////////////////////////////////////////
                           begin punt van de motoren instellen 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/
void beginpunt(){

      //////// gaat terug naar het begin scherm///////////////////
      tft.drawRect(10,25,120,35,WHITE);  // (y,x,grote vorm kleur)
      tft.setCursor(15,30);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print("home         ");

      
     ///// zet het begin punt waar de camera begind///
     
      tft.drawRect(0,150,240,50,WHITE);  // (y,x,grote,vorm kleur)
      tft.setCursor(110,178);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextSize(2);
      tft.println(" ");// zorgt ervoor dat het scherm helemaal leeg is
      
   
      tft.drawRect(0,150,240,50,WHITE);  // (y,x,grote,vorm kleur)
      tft.setCursor(45,165);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.println("set start point");
      
    // vorige en volgende knop//
    tft.drawRect(0,200,120,120,WHITE);  // (y,x,grote vorm kleur) vorige knop
    tft.drawRect(120,200,120,120,WHITE);  // (y,x,grote vorm kleur) volgende knop
    
    tft.setCursor(35,245); // - knop
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("back");

   
    tft.setCursor(150,245); // + knop
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("next"); 
}


/*//////////////////////////////////////////////////////////////////////////////////////////////////
                           eind punt van de motoren instellen 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/
void eindpunt(){

      //////// Begin punt///////////////////
      tft.drawRect(10,25,120,35,WHITE);  // (y,x,grote vorm kleur)
      tft.setCursor(15,30);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print("home       ");

      
     ///// zet het eind punt waar de camera begind///
     
      tft.fillRect(120, 180, 20, 7,BLACK); // maakt de tekst niet meer zichtbaar
      tft.fillRect(196, 155, 20, 30,BLACK); // maakt de tekst niet meer zichtbaar
      tft.drawRect(0,150,240,50,WHITE);  // (y,x,grote,vorm kleur)
      tft.setCursor(45,165);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.println("set end point");
      
    // vorige en volgende knop//
    tft.drawRect(0,200,120,120,WHITE);  // (y,x,grote vorm kleur) vorige knop
    tft.drawRect(120,200,120,120,WHITE);  // (y,x,grote vorm kleur) volgende knop
    
    tft.setCursor(35,245); // - knop
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("back");

   
    tft.setCursor(150,245); // + knop
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("next"); 
}

/*//////////////////////////////////////////////////////////////////////////////////////////////////
                          Hier stel je de tijd in 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

void tijdinstellen(){
       //////// Begin punt///////////////////
      tft.drawRect(10,25,120,35,WHITE);  // (y,x,grote vorm kleur)
      tft.setCursor(15,30);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print("home         ");

///////////// ingestelde tijd////////////////

     secondes = fotos*delays;  // totaal aantal seconde dat hij bezig is
     totaleTijd = secondes;
      uren = ((fotos*delays)/3600); //rekent het aantal uren uit 
       
      if ( minuten < 59){ // dit voor de eerste minuut 
         minuten = ((fotos*delays)/60); // dit is het aantal minuten
         seconde = ((secondes)-(60*minuten)); // dit is het aantalseconde 
      }
      if( minuten >59){ // dit is voor alles boven de 1 minuut 
        uren = ((fotos*delays)/3600);
        minuten = (((secondes)-(3600*uren))/60);
        seconde = (((secondes)-(3600*uren))-(60*minuten)); // dit is het aantalseconde
        Serial.println(minuten); 
      }
     

 
     /////ingestelde de tijd  ///
      tft.drawRect(0,100,240,50,WHITE);  // (y,x,grote,vorm kleur)
      tft.setCursor(80,115);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print(uren);
      tft.print(":");
      tft.print(minuten);
      tft.print(":");
      tft.print(seconde);
      

 ///////////// foto's optellen en aftelllen///////////////////////
    tft.drawRect(0,150,120,130,WHITE);  // (y,x,grote vorm kleur) vorige knop
    
     /////foto's ///
      tft.setCursor(20,250);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.println("images");

       ///// + foto's optellen ///
      tft.setCursor(10,195);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(2);
      tft.println("+");

       ///// - foto's optellen ///
      tft.setCursor(90,195);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(2);
      tft.println("-");

       ///// ingestelde foto's ///
      tft.setCursor(30,165);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.println(fotos);

      
/////////////vertraging tussen de foto's(delay)///////////////////////
   tft.drawRect(120,150,120,130,WHITE);  // (y,x,grote vorm kleur) volgende knop
   
       /////delay ///
      tft.setCursor(150,250);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.println("delay");

        ///// + delay in seconde ///
      tft.setCursor(130,195);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(2);
      tft.println("+");

       ///// - delay in seconde ///
      tft.setCursor(200,195);
      tft.setTextColor(RED,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(2);
      tft.println("-");

      ///// ingestelde delay ///
      tft.setCursor(170,165);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.println(delays);

  ///////// duur van de video in fps //////////////////////////
    driefps = fotos/30;
    zesfps = fotos/60;
  
    tft.drawRect(140,60,100,40,WHITE);  // (y,x,grote vorm kleur) // duur van de video in fps
      tft.setCursor(145,65);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(2);
      tft.setTextSize(1);
      tft.print("30fps  sec");
      tft.setTextColor(RED,BLACK);
      tft.print(driefps);

      tft.setCursor(145,80);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(2);
      tft.setTextSize(1);;
      tft.print("60fps  sec");
      tft.setTextColor(RED,BLACK);
      tft.print(zesfps);
      
      
    // vorige en volgende knop//
    tft.drawRect(0,280,120,40,WHITE);  // (y,x,grote vorm kleur) rand vorige knop
    tft.drawRect(120,280,120,40,WHITE);  // (y,x,grote vorm kleur) rand volgende knop
    
    tft.setCursor(30,290); //  vorige
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("back");
    
    tft.setCursor(150,290); // volgende
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("start");
  
}

  /*//////////////////////////////////////////////////////////////////////////////////////////////////
                           HIer is de timelapse bezig
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

void timelapseloopt(){


   // STOP knop//
    tft.drawRect(0,270,240,50,WHITE);  // (y,x,grote vorm kleur) de rand van de stop knop
    tft.setCursor(90,285); // stop knop
    tft.setTextColor(RED,BLACK);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.print("STOP");


    // gemaakte foto's

     tft.drawRect(0,200,120,70,WHITE);  // (y,x,grote vorm kleur) de rand van de gemaakte foto's
     tft.setCursor(40,210); // al gemaakte foto's
     tft.setTextColor(RED,BLACK);
     tft.setTextFont(4);
      tft.setTextSize(1);
     tft.print(gemaakteFotos);
     
     
     tft.setCursor(20,240); // al gemaakte foto's
     tft.setTextColor(WHITE,BLACK);
     tft.setTextFont(4);
      tft.setTextSize(1);
     tft.print("images");

     tft.drawRect(120,200,120,70,WHITE);  // (y,x,grote vorm kleur) de rand van aflopende tijd
     tft.setCursor(170,210); // delay in seconde die afteld tot de volgende foto
     tft.setTextColor(RED,BLACK);
     tft.setTextFont(4);
      tft.setTextSize(1);
     tft.print(vertraging);

     tft.setCursor(150,240); // al gemaakte foto's
     tft.setTextColor(WHITE,BLACK);
     tft.setTextFont(4);
     tft.setTextSize(1);
     tft.print("delay");


     //// zolang duurt de timelapse al bij 30 of 60 fps////
      tft.drawRect(140,60,100,40,WHITE);  // (y,x,grote vorm kleur) // duur van de video in fps

       driefps = gemaakteFotos/30;
       zesfps = gemaakteFotos/60;
      
       // duur van de timelapse 
      tft.drawRect(140,60,100,40,WHITE);  // (y,x,grote vorm kleur) // duur van de video in fps
      tft.setCursor(145,65);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(2);
      tft.setTextSize(1);
      tft.print("30fps  sec");
      tft.setTextColor(RED,BLACK);
      tft.print(driefps);

      tft.setCursor(145,80);
      tft.setTextColor(WHITE,BLACK);
      tft.setTextFont(2);
      tft.setTextSize(1);;
      tft.print("60fps  sec");
      tft.setTextColor(RED,BLACK);
      tft.print(zesfps);
      
       // aflopende tijd//
        tft.drawRect(0,100,240,100,WHITE);  // (y,x,grote vorm kleur) de rand van de aflopende tijd
        tft.setCursor(35,130);
        tft.setTextColor(WHITE,BLACK);
        tft.setTextFont(6);
        tft.setTextSize(1);
        tft.print(uren);
        tft.print(":");
        tft.print(minuten);
        tft.print(":");
        tft.print(seconde);
    
}

void setup(void) 
{
  pinMode(ledRood, OUTPUT);
  pinMode(ledBlauw, OUTPUT);
  pinMode(ledGroen, OUTPUT);

   setColourRgb(0,0,0);

   Serial.begin(9600);  // serial naar pc
  Serial1.begin(38400); // serial naar andere arduino
  tft.begin();
  if (!ts.begin(40)) { 
    Serial.println("Unable to start touchscreen.");
  } 
  else { 
    Serial.println("Touchscreen started."); 
  }

  //////// standaard waardeen scherm///////////////////
  tft.fillScreen(ILI9341_BLACK);
 

    
 opstartScherm();
}



 /*//////////////////////////////////////////////////////////////////////////////////////////////////
                            hier begint de loop
 ///////////////////////////////////////////////////////////////////////////////////////////////*/
void loop ()
{

   value = analogRead(analogInput);
   vout = (value * 5.0) / 1024.0; // see text
   vin = vout / (R2/(R1+R2)); 
   if (vin<0.09) {
   vin=0.0;//statement to quash undesired reading 
   }


// batterij is leeg
if (vin < 10){
  digitalWrite(ledRood,HIGH);
  digitalWrite(ledGroen,LOW);
}
if (beginSchermTouch == 1 && beginPuntTouch == 0){ // dit voor de snelheid van de motoren voor het beginscherm
  
 valYaw  = analogRead(A8);  //  leest de potmeter voor Yaw
 newvalYaw = map(valYaw , 0, 1023, 0, 100);


 valPitch  = analogRead(A9);  //  leest de potmeter voor Yaw
 newvalPitch = map(valPitch , 0, 1023, 0, 100);

   ///// snelheid motoren////
      tft.drawRect(0,150,240,50,WHITE);  // (y,x,grote vorm kleur) YAW tekst
      tft.setCursor(45,245);
      tft.setTextColor(RED,BLACK); // yaw
      tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print( newvalYaw);

     if (newvalYaw<10){   // als yaw kleiner is dan 10 komt er een zwarte balk over de 0
     tft.fillRect(58, 235, 18, 37,BLACK);}

      tft.setCursor(165,245); // Pitch
      tft.setTextColor(RED,BLACK);
       tft.setTextFont(4);
      tft.setTextSize(1);
      tft.print(newvalPitch);

      if (newvalPitch<10){   // als pitch kleiner is dan 10 komt er een zwarte balk over de 0
     tft.fillRect(178, 235, 18, 37,BLACK);}
} 
 unsigned long currentMillis1 = millis();
   if (currentMillis1 - previousMillis1 >= interval1) {
                
                

    // batterij indicator van de controller zelf
    tft.setCursor(160,3);
    tft.setTextColor(WHITE,BLACK);
    tft.setTextFont(2);
    tft.setTextSize(1);
    percentage = (100-((12.5-vin)/2.5)*100);
    tft.print(percentage); 
     
     tft.setCursor(178,3);
    tft.setTextColor(WHITE);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.print("% battery");

    previousMillis1 = currentMillis1;
   }
  if (ts.touched())
  {   
    // Retrieve a point  
    TS_Point p = ts.getPoint();  // standaard code voor touchscreen 
   
    p.x = map(p.x, 0, 240, 240, 0); // standaard code voor touschreen 
    p.y = map(p.y, 0, 320, 320, 0);

if (opstartSchermTouch ==1){
        if (ts.touched()){   // als het scherm gedrukt is gaat hij verder
            
    // Retrieve a point  
    TS_Point p = ts.getPoint();  // standaard code voor touchscreen 
   
    p.x = map(p.x, 0, 240, 240, 0); // standaard code voor touschreen 
    p.y = map(p.y, 0, 320, 320, 0);

          // gaat weer terug naar home
       if((p.x > 0) && (p.x <=240)) {  // coordinaten van de timelapse knop
     if ((p.y > 0) && (p.y <=320)) {
      tft.fillRect(0,60, 240, 320,BLACK);
       opstartSchermTouch =0;
       beginSchermTouch =1;
       beginscherm();
     }
    }
  }
}
    
  /*//////////////////////////////////////////////////////////////////////////////////////////////////
                            beginscherm
 ///////////////////////////////////////////////////////////////////////////////////////////////*/
if (beginSchermTouch == 1 && beginPuntTouch == 0){ // kijkt of het begintouchreen aan staat
  
 
  beginPuntTouch = 0;
  beginscherm();

  
    //// snelheid  motoren  beginscherm optellen ///
    if((p.x > 120) && (p.x <=240)) {    // coordinaten van het + vak 
     if ((p.y > 200) && (p.y <=320)) {
      Serial.println("+ gedrukt");

      if (optellen<3){ // zorgt ervoor dat de snelheid niet boven de 10 komt 
        delay(15);
       optellen ++;
       Serial1.write(optellen); //stuurt de waarde naar de motor
      } 
      
       beginscherm(); 
     }
     
    }

    //// snelheid motoren beginscherm  aftellen///
    if((p.x > 0) && (p.x <=120)) {  // coordinaten van het - vak 
     if ((p.y > 200) && (p.y <=320)) {
      Serial.println("- gedrukt");
      if (optellen>1){ // zorgt ervoor dat de snelheid niet onder de 0 komt 
         delay(15);
         optellen --;
         Serial1.write(optellen); //stuurt de waarde naar de motor
      }
       
       beginscherm();
     }
    } 
         // zet de timelapse mode aan
  if((p.x > 20) && (p.x <=135)) {  // coordinaten van de timelapse knop
     if ((p.y > 20) && (p.y <=50)) {
      Serial.println("timelapse knop is gedrukt");
      Serial1.write(5); // zet de motor uit en gaat verder om het begin punt inteslteen 
      beginPuntTouch = 1; // zet het beginpunt scherm aan
      beginSchermTouch = 0;
      tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
      beginpunt();
      delay(500);
     
     }
  }   
} 
      /*//////////////////////////////////////////////////////////////////////////////////////////////////
                          zet begin  begin punt waar de timelapse begint 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

   else if(  beginSchermTouch == 0 && beginPuntTouch ==1){
     beginSchermTouch =0;
      beginpunt();

          //// gaat terug naar het beginscherm///
    if((p.x > 0) && (p.x <=120)) {  // coordinaten van het - vak 
     if ((p.y > 200) && (p.y <=320)) {
      Serial.println("vorige gedrukt");
      beginSchermTouch = 1; // zethet beginscherm aan
      beginPuntTouch =0;
      tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
       beginscherm();
      }
     }

       // gaat weer terug naar het beginscherm (home)
      if((p.x > 20) && (p.x <=135)) {  // coordinaten van de timelapse knop
     if ((p.y > 20) && (p.y <=50)) {
      Serial.println("home knop is gedrukt");
      Serial1.write(199); // nu is stuurt hij 199 en gaan de motoren terug naar het begin scherm
      beginSchermTouch = 1; // zethet beginscherm aan
      beginPuntTouch =0;
      tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
       beginscherm();
      delay(500);
     }
      }
 
       // gaat naar het volgende scherm om eind punt intestellen///
    if((p.x > 120) && (p.x <=240)) {    // coordinaten van het + vak 
     if ((p.y > 200) && (p.y <=320)) {
      Serial.println("volgende ingedrukt");
      Serial1.write(6); // nu is het begin punt gezet en gaat verder naar het eind punt
        beginPuntTouch =0; // zethet beginpunt scherm uit
        eindPuntTouch =1;
        tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
        eindpunt();
        } 
       }  
 }
 
 /*//////////////////////////////////////////////////////////////////////////////////////////////////
                          zet eindpunt waar de timelapse eindigd
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

   else if( beginPuntTouch ==0 && eindPuntTouch ==1){
     beginSchermTouch =0;
      eindpunt();

          //// gaat terug om  het beginpunt intestellen///
    if((p.x > 0) && (p.x <=120)) {  // coordinaten van het - vak 
     if ((p.y > 200) && (p.y <=320)) {
      Serial.println("vorige gedrukt");
      beginSchermTouch = 0; 
      beginPuntTouch =1;
       tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
       beginpunt(); // gaat terug om het begin punt intestellen
      }
     }

       // gaat weer terug naar home
      if((p.x > 20) && (p.x <=135)) {  // coordinaten van de timelapse knop
     if ((p.y > 20) && (p.y <=50)) {
      Serial.println("home knop is gedrukt");
      Serial1.write(199); // nu is stuurt hij 199 en gaan de motoren terug naar het begin scherm
      beginSchermTouch = 1; // zet het beginscherm aan
      beginPuntTouch =0;
       tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
       beginscherm();
      delay(500);
      }
     }

        // gaat naar het volgende scherm de tijd intestellen///
    if((p.x > 120) && (p.x <=240)) {    // coordinaten van het + vak 
     if ((p.y > 200) && (p.y <=320)) {
      Serial.println("volgende ingedrukt");
      Serial1.write(7); // het eind punt is gezet de motoren gaan nu terug naar het begin punt
        beginPuntTouch =0; // zethet beginpunt scherm uit
        eindPuntTouch =0;
        tijdTouch =1;
         tft.fillRect(0, 60, 240, 350,BLACK); // reset de background 
       tijdinstellen();
        } 
       } 
 }

  /*//////////////////////////////////////////////////////////////////////////////////////////////////
                         Hier stel je de tijd van de motoren in 
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

   else if( eindPuntTouch ==0 && tijdTouch ==1){
     eindPuntTouch=0;

      
          //// gaat terug om  het beginpunt intestellen///
    if((p.x > 0) && (p.x <=115)) {  // coordinaten van het - vak 
     if ((p.y > 290) && (p.y <=320)) {
      Serial.println("vorige gedrukt");
      eindPuntTouch = 1; 
      beginPuntTouch =0;
      delays=0;
      fotos=0;
      seconde =0;
      minuten =0;
      uren =0;
      fotos = 0;
        tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
       eindpunt(); // gaat terug om het eind punt in te stellen
      }
     }

       // gaat weer terug naar home
      if((p.x > 20) && (p.x <=135)) {  // coordinaten van de timelapse knop
     if ((p.y > 20) && (p.y <=50)) {
      Serial.println("home knop is gedrukt");
      Serial1.write(199); // nu is stuurt hij 25 en gaan de motoren terug naar het begin scherm
      beginSchermTouch = 1; // zet het beginscherm aan
      beginPuntTouch =0;
      delays=0;
      fotos=0;
        tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
       beginscherm();
      delay(500);
      }
     }

        // gaat naar het volgende scherm de timelapse begint eindelijk///
    if((p.x > 125) && (p.x <=239)) {    // volgende is ingedrukt
     if ((p.y > 290) && (p.y <=319)) {
      Serial.println("volgende ingedrukt");
      Serial1.write(8); //de motoren zijn in hun begin positie en gaan beginnen aan de timelapse
       tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
        beginPuntTouch =0; // zethet beginpunt scherm uit
        eindPuntTouch =0;
        tijdTouch =0;
        timelapseTouch =1; // gaat nar het timelapse scherm
        tijdTimelapse =1; // nu begint de tijd afftellen
        timelapseloopt();
        } 
       } 


///////////////////////foto's///////////////////////////////////
           // optellen van de foto's///
    if((p.x > 10) && (p.x <=35)) {    // coordinaten van het + vak 
     if ((p.y > 185) && (p.y <=230)) {
      Serial.println("+50 foto's");
       if (fotos<9950){
         fotos = fotos + 50;
          Serial1.write(15); // hij stuurt de waarde 15 naar de motor arduino waar +50 fotos woordt gedaan
         tijdinstellen();}
        } 
       } 

             // aftellen van de foto's///
    if((p.x > 75) && (p.x <=120)) {    // coordinaten van het - vak 
     if ((p.y > 185) && (p.y <=230)) {
      Serial.println("-50 foto's");
        if (fotos>0){
         fotos = fotos - 50;
         Serial1.write(16); // hij stuurt de waarde 16 naar de motor arduino waar -50 fotos woordt gedaan
         tijdinstellen();}

         if (fotos<1000){  
           tft.fillRect(73, 160, 18, 37,BLACK);
         }
        } 
       } 
/////////////////////////////// delay/////////////////////////////////
      if((p.x > 120) && (p.x <=170)) {    // coordinaten van het + vak delay
     if ((p.y > 185) && (p.y <=230)) {
      Serial.println("+ 1 seconde delay");
       if (delays<1000){
         delays = delays +1;
         Serial1.write(18); // hij stuurt de waarde 17 naar de motor arduino waar + 1 seconde delay wordt gedaan
         vertraging++;
         tijdinstellen();
         }
        } 
       } 

       if((p.x > 190) && (p.x <=240)) {    // coordinaten van het - vak delay
     if ((p.y > 185) && (p.y <=230)) {
      Serial.println("-1 seconde delay");
       if (delays>0){
         delays = delays -1;
          Serial1.write(19); // hij stuurt de waarde 18 naar de motor arduino waar - 1 seconde delay wordt gedaan
         vertraging--;
         tijdinstellen();

           if (delays<10){  
           tft.fillRect(184, 160, 18, 37,BLACK);
         }
         }
        } 
       } 


 }
}  // eind van het touchscreen 

  /*//////////////////////////////////////////////////////////////////////////////////////////////////
                         Hier is de timelapse bezig en telt hij af
 ///////////////////////////////////////////////////////////////////////////////////////////////*/

/// code staat buiten het touchscreen anders telt hij alleen af als je het touch aanraakt//
  if( tijdTimelapse == 1){

        // hier telt hij steeds het aantal ingestelde secondes af tot de volgende foto//
        unsigned long currentMillis = millis();

          if(vertraging <11){// zorgt ervoor dat de 0 van 10 niet meer zichtbaar is
             tft.fillRect(184, 205, 30, 35,BLACK); // reset de background
        }
          
          if (currentMillis - previousMillis >= interval) {
                
                previousMillis = currentMillis;

                 if( vertraging ==1){
                      gemaakteFotos++;
                  }
               
                vertraging--; // hier haalt hij er steeds 1 seconde van af 
                 
                if (vertraging <1){
                   vertraging =delays; // als de vertraging kleiner is dan 0 zet hij terug naar de ingestelde delay
                    
                }
                Serial1.write(vertraging); // nu schrijft hij de vertraging naar de motoren
                timelapseloopt();
              }
    
      if (secondes >1){ // secondes is de totale ingestelde tijd in seconden


      // zorgt ervoor dat de tijd altijd goed word weergeven
        if(seconde <10 && minuten<10){// zorgt ervoor dat de 0 van 10 niet meer zichtbaar is
        tft.fillRect(145, 120, 50, 50,BLACK); // reset de background
        }
        if(seconde <10 && minuten>10){// zorgt ervoor dat de 0 van 10 niet meer zichtbaar is
        tft.fillRect(171, 120, 50, 50,BLACK); // reset de background
        }
      if( totaleTijd >1){
        totaleTijd = totaleTijd-1;
      Serial.println("timeloopt !!!!");
      seconde = seconde-1;
      delay(1000);


      if ( minuten <1 && seconde <1){   // uren -1
        uren = uren -1;
         minuten =59;
         seconde =59;
      }
      if (seconde <1){  // minuten -1
        minuten = minuten -1;
      }
        if(seconde <=0){  //seconde -1
        seconde =59;
          }
        timelapseloopt();
        }  if (totaleTijd <=2 ){  // timelapse is afgelopen gaat terug naar begin scherm 
          tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
           Serial1.write(199); // nu is stuurt hij 199 en gaan de motoren terug naar het begin scherm
          uren=0;
          minuten=0;
          seconde=0;
          vertraging =0;
          gemaakteFotos =0;
          delays=0;
          fotos=0;
          beginSchermTouch = 1; // zethet beginscherm aan
          beginPuntTouch =0;
          tijdTimelapse =0;
          tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
          beginscherm();
        }}

               
// zet het touchscreen hier aan om te zorgen dat hij terug naar home kan en kan stoppen
       if (ts.touched()){   
    // Retrieve a point  
    TS_Point p = ts.getPoint();  // standaard code voor touchscreen 
   
    p.x = map(p.x, 0, 240, 240, 0); // standaard code voor touschreen 
    p.y = map(p.y, 0, 320, 320, 0);

          // gaat weer terug naar home
       if((p.x > 0) && (p.x <=240)) {  // coordinaten van de timelapse knop
     if ((p.y > 270) && (p.y <=320)) {
      Serial.println("STOP");
      Serial1.write(199); // nu is stuurt hij 199 en gaan de motoren terug naar het begin scherm
      beginSchermTouch = 1; // zet het beginscherm aan
      beginPuntTouch =0;
      delays=0;
      fotos=0;
      tijdTimelapse =0;
      vertraging =0;
      gemaakteFotos =0;
      tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
      beginscherm();
      delay(500);
      }
     }

          // gaat weer terug naar home
      if((p.x > 20) && (p.x <=135)) {  // coordinaten van de timelapse knop
     if ((p.y > 20) && (p.y <=50)) {
      Serial.println("home knop is gedrukt");
      Serial1.write(199); // nu is stuurt hij 199 en gaan de motoren terug naar het begin scherm
      beginSchermTouch = 1; // zet het beginscherm aan
      beginPuntTouch =0;
      delays=0;
      fotos=0;
      tijdTimelapse =0;
      vertraging =0;
      gemaakteFotos =0;
      tft.fillRect(0, 60, 240, 350,BLACK); // reset de background
      beginscherm();
      delay(500);
      }
     }

   
     
    }
  } 
} // eind van de loop
