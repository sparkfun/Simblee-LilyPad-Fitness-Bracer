//Simblee Lilypad Fitness Bracer Code
//Sarah Al-Mutlaq 
//March 8th 2016
//Written in Arduino 1.6.7
//SparkFun Electronics

// To use the SimbleeForMobile library, you must include this file at the top
// of your sketch. **DO NOT** include the SimbleeBLE.h file, as it will cause
// the library to silently break.
#include<SimbleeForMobile.h>

//Define pins:

const uint32_t LEDPIN = 9; 
const int pulsePin = 6;
const int buttonPin = 15;
const int vibePin = 12;
const int vibePin2 = 11;
const int accXPin = 5;
const int accYPin = 3;
//const int tempPin = 4;


//Floats for calculating accelorometer changes to get steps:
float threshhold=300.0;

float xval[100]={0};
float yval[100]={0};

float xavg = 173.10;
float yavg = 0.00;

int flag=0;


//Names and identifiers for the drawn stuff on the app:
uint8_t rectangle1ID;
uint8_t rectangle2ID;
uint8_t button1ID;
uint8_t addCalBtnID;
uint8_t button3ID;
uint8_t LEDswitchID;
uint8_t text1ID;
uint8_t text2ID;
uint8_t text3ID;
uint8_t text3unitID;
uint8_t text4ID;
uint8_t text5ID;
uint8_t text6ID;
uint8_t text7ID;
uint8_t lastActTextID;
uint8_t activeUnitID;
uint8_t text9ID;
uint8_t text10ID;
uint8_t text10UnitID;
uint8_t text11ID;
uint8_t text12ID;
uint8_t text12UnitID;
uint8_t text13ID;
uint8_t calTextFldID;
uint8_t text15ID;
uint8_t text16ID;
uint8_t text16UnitID;
uint8_t segment1ID;
uint8_t InputTextFldID;

//Place holders for values we get/calculate:
int16_t STEPS = 0;
float steps = STEPS;
int16_t STRIPCHOICE = 0;
const char* stripChoiceUnit = " ";
int16_t LASTACTIVITY = 0;
const char* activityUnit = "mins";
int16_t CURRENTTEMP = 0;
const char* tempUnit = "F";
int16_t LASTHEARTRATE = 95;
const char* heartUnit = "bpm";
int16_t Add = 0;
int16_t caloriesTotal = 0;
int16_t currentScreen = -1;
int16_t stripSelection = 4;
float percentSteps = (steps/stepGoal);

//Some variables to keep track of time and states (since you can't have blocking code):
int buttonState = HIGH;
long lastStepDebounceTime = 0;
long stepDebounceDelay = 30;
long lastTimer = 0;
long newTimer = 0;

//Values for the red, green, and blue parts of the RGB LED:
int redVal;
int greenVal;
int blueVal;

//Variable for your daily step goal (for the LED indicator):
float stepGoal = 10000;

//Indicator for whether the LED should be on or off (1 = on):
int leds = 1;


//Variables to get and calculate heart rate from the pulse sensor:
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

volatile int rate[10];                    // used to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find the inter beat interval
volatile int P =512;                      // used to find peak in pulse wave
volatile int T = 512;                     // used to find trough in pulse wave
volatile int thresh = 512;                // used to find instant moment of heart beat
volatile int amp = 100;                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = true;       // used to seed rate array so we startup with reasonable BPM

//Setup function (only runs once at the start):
void setup()
{      
  //Define our pins as inputs of outputs, and set ones that are outputs:
        pinMode(LEDPIN, OUTPUT);
        pinMode(buttonPin, INPUT_PULLUP);           
        pinMode(pulsePin, INPUT);
        pinMode(vibePin, OUTPUT);          
        pinMode(vibePin2, OUTPUT);           
        digitalWrite(vibePin, LOW);         
        digitalWrite(vibePin2, LOW);        
        pinMode(accXPin, INPUT);
        pinMode(accYPin, INPUT);

//        pinMode(tempPin, INPUT); // if I had more anologe pins I would add a temp sensor

        Serial.begin(9600); //to be able to use the arduino serial monitor terminal to debug

//        calibrate(); //Function you can call to set the average X and Y from the accelorometer 
                       //so that it is more accurate about your steps (uncomment function to run).


        // advertisementData shows up in the app as a line under deviceName. Note
        // that the length of these two fields combined must be less than 16
        // characters!
        SimbleeForMobile.advertisementData = "FitnessTracker";
        SimbleeForMobile.deviceName = "5";
        // txPowerLevel can be any multiple of 4 between -20 and +4, inclusive. The
        // default value is +4; at -20 range is only a few feet.
        SimbleeForMobile.txPowerLevel = -4;

        // This must be called *after* you've set up the variables above, as those
        // variables are only written during this function and changing them later
        // won't actually propagate the settings to the device.
        SimbleeForMobile.begin();
}


//Function that runs continuously looping:
void loop()
{ 
  //Check the button pin to see if it has been switched:
  int buttonRead = digitalRead(buttonPin);
  
  Serial.println(percentSteps); //for debugging

//These if functions set the LED different colors based on how close you are to your step goal: 
   if ((percentSteps <= 0.25) && (leds == 1))
        {
            redVal = 0xff;
            greenVal = 0x00;
            blueVal = 0x00;
        }
   else if ((percentSteps <= 0.5) && (leds == 1))
        {
            redVal = 0xff;
            greenVal = 0xb3;
            blueVal = 0x00;
        }       
   else if ((percentSteps <= 0.75) && (leds == 1))
        {
            redVal = 0xff;
            greenVal = 0xff;
            blueVal = 0x00;
        }
   else if ((percentSteps <= 1) && (leds == 1))
        {
            redVal = 0x00;
            greenVal = 0xff;
            blueVal = 0x00;
        }
   else if ((percentSteps > 1) && (leds == 1))
        {
            redVal = 0x00;
            greenVal = 0xff;
            blueVal = 0xff;
        }
    if (leds == 0)
        {
            redVal = 0x00;
            greenVal = 0x00;
            blueVal = 0x00;
        }

     RGB_Show(redVal,greenVal,blueVal, LEDPIN);


//This a timer so that it can keep track of the last time you were active:  
        static uint32_t timer = 0;
        long timerMins = 0; 
        timer = millis()/1000; 
        timerMins = timer/60;
        if (SimbleeForMobile.updatable)
        { 
          newTimer = timerMins - lastTimer;
          if(currentScreen == 2){
            SimbleeForMobile.updateValue(lastActTextID, newTimer);
          }
          if(currentScreen == 1){
              LASTACTIVITY = newTimer;
              if (stripSelection == 0){
                STRIPCHOICE = LASTACTIVITY;
              SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);
              }
          }
        }

//This is the code that caluclates when a step has been taken:
  int accelerationX = analogRead(accXPin);
  int accelerationY = analogRead(accYPin);

  int acc=0;
  float totvect[100]={0};
  float totave[100]={0};
  float xaccl[100]={0};
  float yaccl[100]={0};   
      
    for (int i=0;i<100;i++)
    {
    xaccl[i]=float(analogRead(accXPin));
    yaccl[i]=float(analogRead(accYPin));
    
     totvect[i] = sqrt(((xaccl[i]-xavg)* (xaccl[i]-xavg))+ ((yaccl[i] - yavg)*(yaccl[i] - yavg)));
     totave[i] = (totvect[i] + totvect[i-1]) / 2 ;
     if ((millis() - lastStepDebounceTime) > stepDebounceDelay)
     {
            if (totave[i]>threshhold && flag==0)
            {
              STEPS = STEPS + 1;
              flag = 1;
              lastTimer = timerMins;
              
              if (currentScreen == 1)
              {
                SimbleeForMobile.updateValue(text2ID, STEPS);
              }
              if (currentScreen == 2)
              {
                SimbleeForMobile.updateValue(text6ID, STEPS);
              }
            }
             else if (totave[i] > threshhold && flag == 1)
            {
             //do nothing 
            }
     }
      if (totave[i] < threshhold  && flag == 1)
      {
        flag=0;
        lastStepDebounceTime = millis();
      }
   }



   
//this code would be used if you had an extra analog pin for a temp sensor:
/*
    float tempVoltage;

  // First we'll measure the voltage at the temperature analog pin. Normally
  // we'd use analogRead(), which returns a number from 0 to 1023.
  // Here we've written a function (further down) called
  // getVoltage() that returns the true voltage (0 to 5 Volts)
  // present on an analog input pin.

//  tempVoltage = getVoltage(tempPin);

  // Now we'll convert the voltage to degrees Celsius.
  // This formula comes from the temperature sensor datasheet:
  
  float tempC = (tempVoltage - 0.5) * 100.0;
  
   // While we're at it, let's convert degrees Celsius to Fahrenheit.
  // This is the classic C to F conversion formula:
  
  float tempF = (tempC * (9.0/5.0)) + 32.0;
  CURRENTTEMP = tempF;
  
  if (currentScreen == 1)
  {
    if (stripSelection == 1){
              STRIPCHOICE = CURRENTTEMP;
              SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);
              
    }
  }
  if (currentScreen == 2)
  {
              SimbleeForMobile.updateValue(text10ID, CURRENTTEMP);
  }
  */


//This is seeing if the button is switched and if it is looks for your pulse and calculates your 
//heart rate:
        if (buttonRead == LOW)
        {
          findPulseStuff();
          LASTHEARTRATE = BPM;
        }
        
        Serial.println(BPM); //for debugging
        delay(1);
        

//This code checks what screen you are on the app, and if it is the first screen it makes sure to 
//put the right text into the middle based on what button you have pressed last:
  if (currentScreen == 1)
  {
    if (stripSelection == 1){
              STRIPCHOICE = caloriesTotal;
              SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);
              
    }
  }
  

       if (currentScreen == 1)
       {
           if (stripSelection == 2)
           {
              STRIPCHOICE = LASTHEARTRATE;
              SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);        
           }
                if (stripSelection != (0 || 1 || 2))
             {
               STRIPCHOICE = 0;
             }
       }


//This last function turns on the vibe motors if you haven't been active for more than 60 mins, 
//and won't stop until you active again:
        if (LASTACTIVITY >= 60)  //if you have been inactive for an hour
          {
            digitalWrite(vibePin, HIGH); //set off the vibe motor
            digitalWrite(vibePin2, HIGH); //set off the vibe motor
          }
        else if (LASTACTIVITY < 60)
          {
            digitalWrite(vibePin, LOW);
            digitalWrite(vibePin2, LOW);
          }
        
        
        // This function must be called regularly to process UI events.      
        SimbleeForMobile.process();
}



//Funtion to change the RGB LED to a specific color:
void RGB_Show(uint8_t r, uint8_t g, uint8_t b, uint32_t ledPin)
{
    uint8_t rgb[3] = {g, r, b};
    uint8_t *p = rgb;
    uint8_t *end = p + 3;

    while (p < end)
    {
        uint8_t pix = *p++;
        for (uint8_t mask = 0x80; mask; mask >>= 1)
        {
            if (pix & mask)
            {
                // T1H 760ns
                NRF_GPIO->OUTSET = (1UL << ledPin);
                NRF_GPIO->OUTSET = (1UL << ledPin);
                NRF_GPIO->OUTSET = (1UL << ledPin);

                // T1L 660ns
                NRF_GPIO->OUTCLR = (1UL << ledPin);
            }
            else
            { 
                // T0H 380ns
                NRF_GPIO->OUTSET = (1UL << ledPin);

                // T0L 840ns
                NRF_GPIO->OUTCLR = (1UL << ledPin);
                NRF_GPIO->OUTCLR = (1UL << ledPin);
                NRF_GPIO->OUTCLR = (1UL << ledPin);
                NRF_GPIO->OUTCLR = (1UL << ledPin);
            }
        }
    } 
    NRF_GPIO->OUTCLR = (1UL << ledPin);
}


//Function that takes analog values from a temp sensor and calculates the actual voltage output:
float getVoltage(int pin)
{
  // This function has one input parameter, the analog pin number
  // to read. You might notice that this function does not have
  // "void" in front of it; this is because it returns a floating-
  // point value, which is the true voltage on that pin (0 to 5V).
  
  // You can write your own functions that take in parameters
  // and return values. Here's how:
  
    // To take in parameters, put their type and name in the
    // parenthesis after the function name (see above). You can
    // have multiple parameters, separated with commas.
    
    // To return a value, put the type BEFORE the function name
    // (see "float", above), and use a return() statement in your code
    // to actually return the value (see below).
  
    // If you don't need to get any parameters, you can just put
    // "()" after the function name.
  
    // If you don't need to return a value, just write "void" before
    // the function name.

  // Here's the return statement for this function. We're doing
  // all the math we need to do within this statement:

   
  return ((analogRead(pin) * (3.3/1023))); 
  
  // This equation converts the 0 to 1023 value that analogRead()
  // returns, into a 0.0 to 5 value that is the true voltage
  // being read at that pin.
}


//Function that gets your heart rate from the pulse sensor:
void findPulseStuff ()
{
  for (int pulseFlag = 0; pulseFlag < 10; pulseFlag++)
    {
  
            Signal = analogRead(pulsePin);              // read the Pulse Sensor 
            sampleCounter += 1;                         // keep track of the time in mS with this variable
            int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
        
        //  find the peak and trough of the pulse wave
            if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
                if (Signal < T){                        // T is the trough
                    T = Signal;                         // keep track of lowest point in pulse wave 
                 }
               }
              
            if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
                P = Signal;                             // P is the peak
               }                                        // keep track of highest point in pulse wave
            
          //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
          // signal surges up in value every time there is a pulse
        if (N > 250){                                   // avoid high frequency noise
          if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
            Pulse = true;                               // set the Pulse flag when we think there is a pulse
            RGB_Show(0x00, 0x00, 0x00, LEDPIN);
            IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
            lastBeatTime = sampleCounter;               // keep track of time for next pulse
                 
                 if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
                     firstBeat = false;                 // clear firstBeat flag
                     return;                            // IBI value is unreliable so discard it
                    }   
                 if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
                    secondBeat = false;                 // clear secondBeat flag
                       for(int i=0; i<=9; i++){         // seed the running total to get a realisitic BPM at startup
                            rate[i] = IBI;                      
                            }
                    }
                  
            // keep a running total of the last 10 IBI values
            word runningTotal = 0;                   // clear the runningTotal variable    
        
            for(int i=0; i<=8; i++){                // shift data in the rate array
                  rate[i] = rate[i+1];              // and drop the oldest IBI value 
                  runningTotal += rate[i];          // add up the 9 oldest IBI values
                }
                
            rate[9] = IBI;                          // add the latest IBI to the rate array
            runningTotal += rate[9];                // add the latest IBI to runningTotal
            runningTotal /= 10;                     // average the last 10 IBI values 
            BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
            QS = true;                              // set Quantified Self flag 
            // QS FLAG IS NOT CLEARED INSIDE THIS ISR
            }                       
        }
        
          if (Signal < thresh && Pulse == true){     // when the values are going down, the beat is over
              RGB_Show(redVal, greenVal, blueVal, LEDPIN);
              Pulse = false;                         // reset the Pulse flag so we can do it again
              amp = P - T;                           // get amplitude of the pulse wave
              thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
              P = thresh;                            // reset these for next time
              T = thresh;
             }
          
          if (N > 2500){                             // if 2.5 seconds go by without a beat
              thresh = 512;                          // set thresh default
              P = 512;                               // set P default
              T = 512;                               // set T default
              lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
              firstBeat = true;                      // set these to avoid noise
              secondBeat = true;                     // when we get the heartbeat back
             }
         }
    
}



//Function you can run to find that average X and Y vallues from your accelorometer, to make 
//calculating your steps easier and more accurate:
void calibrate()
{  
      float sum=0;
      float sum1=0;
    
    for (int i=0;i<100;i++)
    {
    xval[i]=float(analogRead(accXPin));
    
    sum=xval[i]+sum;
    }
    xavg=sum/100.0;
    
    Serial.println(xavg);
    
    for (int j=0;j<100;j++)
    {
    yval[j]=float(analogRead(accYPin));
    
    }
    yavg=sum1/100.0;
    
    Serial.println(yavg);

}


//Function for the first screen of the phone app:
void screen1()
{

        uint16_t height = SimbleeForMobile.screenHeight;
        uint16_t width = SimbleeForMobile.screenWidth;
        // These variable names are long...let's shorten them. They allow us to make
        // an interface that scales and scoots appropriately regardless of the screen
        // orientation or resolution.

        color_t lightBlue = rgb(0,85,250);

        SimbleeForMobile.beginScreen(lightBlue, PORTRAIT);

        rectangle1ID = SimbleeForMobile.drawRect(
                        (width/2) - 100,      // x location
                        (height/4) - 10,      // y location
                        200,                  // width of rectangle
                        50,                   // height of rectangle
                        WHITE);               // color of rectangle 

        rectangle2ID = SimbleeForMobile.drawRect(
                        (width/2) - 100,      // x location
                        (height/2),           // y location
                        200,                  // width of rectangle
                        50,                   // height of rectangle
                        WHITE);               // color of rectangle 

        text1ID = SimbleeForMobile.drawText(
                        (width/2) - 100 ,      // x location
                        (height/4) - 75,       // y location
                        "Steps Today:",              // shown text
                        WHITE,                  // color of text
                        36);                  // size of text from 8 to 36

        text2ID = SimbleeForMobile.drawText(
                        (width/2) - 30 ,      // x location
                        (height/4),       // y location
                        STEPS,              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        text3ID = SimbleeForMobile.drawText(
                        (width/2) - 20 ,      // x location
                        (height/2) + 10,       // y location
                        STRIPCHOICE,              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        text3unitID = SimbleeForMobile.drawText(
                        (width/2) + 30 ,      // x location
                        (height/2) + 10,      // y location
                        stripChoiceUnit,      // shown text
                        BLACK,                // color of text
                        30);                  // size of text from 8 to 36


        text4ID = SimbleeForMobile.drawText(
                        (width/2) - 150,             // x location
                        3*(height/4),       // y location
                        "LEDs On/Off:",              // shown text
                        WHITE,                  // color of text
                        22);                  // size of text from 8 to 36

        button1ID = SimbleeForMobile.drawButton(
                        (width/2) - 75,      // x location
                        (height/2) - 75,      // y location
                        150,                  // width of button
                        "All Info",                // text shown on button
                        BLACK,                 // color of button
                        BOX_TYPE);            // type of button

        // Buttons, by default, produce only EVENT_PRESS type events. We want to also
        // do something when the user releases the button, so we need to invoke the
        // setEvents() function. Note that, even though EVENT_PRESS is default, we
        // need to include it in setEvents() to avoid accidentally disabling it.
        SimbleeForMobile.setEvents(button1ID, EVENT_PRESS | EVENT_RELEASE);

        addCalBtnID = SimbleeForMobile.drawButton(
                        3*(width/4) - 60,      // x location
                        (height) - 75,      // y location
                        120,                  // width of button
                        "Add Calories",                // text shown on button
                        BLACK,                 // color of button
                        BOX_TYPE);            // type of button

        SimbleeForMobile.setEvents(addCalBtnID, EVENT_PRESS | EVENT_RELEASE);

        LEDswitchID = SimbleeForMobile.drawSwitch(
                        (width/2) - 110,       // x location
                        3*(height/4) + 50,      // y location
                        GREEN);                // color of switch

        // Syntax for specifying segment titles; odd names and values used to show
        //  that segment text and title variable names are completely arbitrary.
        const char* activity = "Last Activity";
        const char* cals = "Calories";
        const char* heart = "Heart Rate";

        const char* const strip[3] = {activity, cals, heart};
        segment1ID = SimbleeForMobile.drawSegment(
                        0,                          // x location 
                        3*(height/4) - 60,          // y location
                        width,                      // width of segment
                        strip,                      // segment context
                        3,                          // number of segments
                        BLACK);                      // color of segment

        SimbleeForMobile.setEvents(segment1ID, 0 | 1 | 2);

        int16_t CalorieAdd = 0;

        InputTextFldID = SimbleeForMobile.drawTextField(
                        3*(width/4) - 45,                 // x location
                        (height) - 120,                 // y location
                        90,                    // text field width
                        CalorieAdd,              // value entered
                        "Enter Value",       // placeholder text
                        BLACK,               // text color
                        WHITE);   // background color

        SimbleeForMobile.setEvents(InputTextFldID, EVENT_PRESS | EVENT_RELEASE);

        SimbleeForMobile.endScreen();

        //These are updated outside of the screen in case anything changes while you are looking 
        //at the screen:
        SimbleeForMobile.updateValue(text2ID, STEPS);
        SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);  
        SimbleeForMobile.updateText(text4ID, "LEDs On/Off:"); 
}


//Function for the second screen of the phone app:
void screen2()
{
        uint16_t height = SimbleeForMobile.screenHeight;
        uint16_t width = SimbleeForMobile.screenWidth;
        color_t lightBlue = rgb(0,85,250);

        Serial.print("SCRN2 cal: ");
        Serial.println(caloriesTotal);

        SimbleeForMobile.beginScreen(lightBlue, PORTRAIT);

        text5ID = SimbleeForMobile.drawText(
                        10,      // x location
                        (height/14),       // y location
                        "Steps Today:",              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        text6ID = SimbleeForMobile.drawText(
                        20,      // x location
                        2*(height/14),       // y location
                        STEPS,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36

        text7ID = SimbleeForMobile.drawText(
                        10,             // x location
                        3*(height/14),                // y location
                        "Last Activity:",              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        lastActTextID = SimbleeForMobile.drawText(
                        20,             // x location
                        4*(height/14),       // y location
                        LASTACTIVITY,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36

        activeUnitID = SimbleeForMobile.drawText(
                        60,             // x location
                        4*(height/14),       // y location
                        activityUnit,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36



        //If you wanted to show temp sensor info (if you had it):
/*
        text9ID = SimbleeForMobile.drawText(
                        10,             // x location
                        9*(height/14),       // y location
                        "Current Temperature:",              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        text10ID = SimbleeForMobile.drawText(
                        20,             // x location
                        10*(height/14),       // y location
                        CURRENTTEMP,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36'

                       

        text10UnitID = SimbleeForMobile.drawText(
                        50,             // x location
                        10*(height/14),       // y location
                        tempUnit,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36
   */

        text11ID = SimbleeForMobile.drawText(
                        10,             // x location
                        5*(height/14),       // y location
                        "Last Heart Rate",              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        text12ID = SimbleeForMobile.drawText(
                        20,             // x location
                        6*(height/14),       // y location
                        LASTHEARTRATE,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36

        text12UnitID = SimbleeForMobile.drawText(
                        75,             // x location
                        6*(height/14),       // y location
                        heartUnit,              // shown text
                        WHITE,                  // color of text
                        26);                  // size of text from 8 to 36


        text13ID = SimbleeForMobile.drawText(
                        10,             // x location
                        7*(height/14),       // y location
                        "Total Calories Today:",              // shown text
                        BLACK,                  // color of text
                        30);                  // size of text from 8 to 36

        calTextFldID = SimbleeForMobile.drawText(
                        20,               // x location
                        8*(height/14),   // y location
                        caloriesTotal,    // shown text
                        WHITE,            // color of text
                        26);   


        button3ID = SimbleeForMobile.drawButton(
                        (width/2) - 75,                 // x location
                        13*(height/14),      // y location
                        150,                  // width of button
                        "Back",                // text shown on button
                        BLACK,                 // color of button
                        BOX_TYPE);            // type of button

        SimbleeForMobile.setEvents(button3ID, EVENT_PRESS | EVENT_RELEASE);

        SimbleeForMobile.endScreen();

        //These are updated outside of the screen in case anything changes while you are looking 
        //at the screen:
        SimbleeForMobile.updateValue(lastActTextID, LASTACTIVITY);
        SimbleeForMobile.updateText(activeUnitID, "mins");
        SimbleeForMobile.updateValue(calTextFldID, caloriesTotal);
        SimbleeForMobile.updateValue(text6ID, STEPS);
 //       SimbleeForMobile.updateValue(text10ID, CURRENTTEMP);
        
}


// ui() is a SimbleeForMobile specific function which handles the specification
// of the GUI on the mobile device the Simblee connects to.
void ui()
{
        // color_t is a special type which contains red, green, blue, and alpha 
        // (transparency) information packed into a 32-bit value. The functions rgb()
        // and rgba() can be used to create a packed value.
        color_t lightBlue = rgb(0,85,250);
        Serial.println("UICALLED");


        //The ui() fuction is called each time you want to change screens, and these are 
        //the statements that see which screen to be on and to draw that screen:
        if (currentScreen == SimbleeForMobile.screen) return;

        currentScreen = SimbleeForMobile.screen;

        if (currentScreen ==  1)
                screen1();
        else
                screen2();

}



//This is another function you need when making a SimbleeForMobile project, it is the 
//function that gets called each time an event from the app happens (such as a button push):
void ui_event(event_t &event)
{
        printEvent(event); //for debugging

        //Each statement specifies what to do when something specific happens from the app:

        if (event.id == InputTextFldID)
        {
                Add = event.value;
        }

        else if (event.id == button1ID)
        {
                if (event.type == EVENT_RELEASE)
                {
                        SimbleeForMobile.showScreen(2);      
                }    
        }

        else if (event.id == addCalBtnID)
        {
                if (event.type == EVENT_PRESS)
                {
                        caloriesTotal = caloriesTotal + Add;  
                        Serial.print("EVENT cal: ");
                        Serial.println(caloriesTotal);
                }  
                if (event.type == EVENT_RELEASE)
                {
                        SimbleeForMobile.updateValue(InputTextFldID, 0);
                        
                }  
        }

        else if (event.id == button3ID)
        {
                if (event.type == EVENT_RELEASE)
                {
                        SimbleeForMobile.showScreen(1); 
                        stripSelection = 4;
                }  
        }

        else if (event.id == LEDswitchID)
        {
          if (event.value == 1)
          {
            leds = 0;
          }
          if (event.value == 0)
          {
            leds = 1;
          }

        }

        else if (event.id == segment1ID)
        {
          stripSelection = event.value;
                uint16_t width = SimbleeForMobile.screenWidth;
                if (event.value == 0)
                {
                        STRIPCHOICE = LASTACTIVITY;
                        stripChoiceUnit = activityUnit;
                        SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);
                        SimbleeForMobile.updateX(text3ID, (width/2) - 10);
                        SimbleeForMobile.updateText(text3unitID, stripChoiceUnit);
                }
                if (event.value == 1)
                {
                        STRIPCHOICE = caloriesTotal;
                        stripChoiceUnit = " ";
                        SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);
                        SimbleeForMobile.updateX(text3ID, (width/2) - 20);
                        SimbleeForMobile.updateText(text3unitID, stripChoiceUnit);
                }
                if (event.value == 2)
                {
                        STRIPCHOICE = LASTHEARTRATE;
                        stripChoiceUnit = heartUnit;
                        SimbleeForMobile.updateValue(text3ID, STRIPCHOICE);
                        SimbleeForMobile.updateX(text3ID, (width/2) - 20);
                        SimbleeForMobile.updateText(text3unitID, stripChoiceUnit);
                }
        }

}



//Fuction that runs when you connect your phone to the projected app:
void SimbleeForMobile_onConnect()
{
        currentScreen = -1;
        RGB_Show(0x00,0xff,0x00, LEDPIN);
        delay(100);
}


//Function for debugging purposes:
void printEvent(event_t &event)
{
        Serial.println("Event:");
        Serial.print("  ID: ");
        Serial.println(event.id);

        Serial.print("   Type: ");
        Serial.println(event.type);  

        Serial.print("   Value: ");
        Serial.println(event.value);

        Serial.print("   Text: ");
        Serial.println(event.text);  

        Serial.print("   Coords: ");
        Serial.print(event.x);  
        Serial.print(",");  
        Serial.println(event.y);  

        Serial.println(STEPS);

//        Serial.print("Temp Read: ");
//        Serial.println(analogRead(tempPin));

//        Serial.print("Temp Display: ");
//        Serial.println(CURRENTTEMP);
}
