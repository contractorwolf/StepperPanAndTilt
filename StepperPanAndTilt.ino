#define BTNPIN                            8  // step pin stepstick
#define STEPDIRECTIONLR                   4  // dir pin stepstick
#define STEPPINLR                         3  // step pin stepstick
#define STEPDIRECTIONUD                   12  // dir pin stepstick
#define STEPPINUD                         11  // step pin stepstick

const byte interruptPinLR = 7;
const byte interruptPinUD = 2;


const int analogPinLR = A0; // potentiometer wiper (middle terminal) connected to analog pin 0
const int analogPinUD = A1; // potentiometer wiper (middle terminal) connected to analog pin 0

volatile bool stopStateLR = false;
volatile bool stopStateUD = false;
volatile byte state = LOW;

int buttonStatus = 0;
int lastRead = 0;  // variable to store the value read


int fullSpin = 1600;// 1600 is 360 exactly


int offset = 30;
int centerLR = 0;
int centerUD = 0;

int last5LR = 0;
int last4LR = 0;
int last3LR = 0;
int last2LR = 0;
int last1LR = 0;
int last6LR = 0;
int last7LR = 0;
int last8LR = 0; 
int last9LR = 0;
int last10LR = 0;
int last11LR = 0;

int last5UD = 0;
int last4UD = 0;
int last3UD = 0;
int last2UD = 0;
int last1UD = 0;
int last6UD = 0;
int last7UD = 0;
int last8UD = 0; 
int last9UD = 0;
int last10UD = 0;
int last11UD = 0;

volatile int currentPositionLR = 0;
volatile int currentPositionUD = 0;

volatile int measureLeft = 0;
volatile int measureRight = 0;

volatile int measureUp = 0;
volatile int measureDown = 0;


volatile int directionLR = 0; //left
volatile int directionUD = 0; //up


byte calibrated = false;
byte leftRight = false;

int lastAvgLR = 0;
int lastAvgUD = 0;

int chopDelayPan = 1500;//200
int chopSplitPan = 1500;//150
int chopDelay = 400;//200
int chopSplit = 400;//150



static unsigned long last;

void setup() {
  // put your setup code here, to run once:

    Serial.println("STARTING");

    //pin setup
    pinMode(STEPDIRECTIONUD, OUTPUT);
    pinMode(STEPPINUD, OUTPUT);
    pinMode(STEPDIRECTIONLR, OUTPUT);
    pinMode(STEPPINLR, OUTPUT);
    pinMode(BTNPIN, INPUT);

    pinMode(interruptPinLR, INPUT_PULLUP);
    pinMode(interruptPinUD, INPUT_PULLUP);
    
    
    //pin initialize
    digitalWrite(STEPDIRECTIONLR, HIGH);
    digitalWrite(STEPPINLR, HIGH);
    digitalWrite(STEPDIRECTIONUD, HIGH);
    digitalWrite(STEPPINUD, HIGH);

    attachInterrupt(digitalPinToInterrupt(interruptPinLR), stopLR, FALLING );
    attachInterrupt(digitalPinToInterrupt(interruptPinUD), stopUD, FALLING );
    
}

void loop() {
    buttonStatus = digitalRead(BTNPIN);

    if(buttonStatus){
      Serial.println("button pressed");
      calibrated = false;

      calibrated = calibrate();

    }else{
      int currentReadLR = analogRead(analogPinLR);
      int currentReadUD = analogRead(analogPinUD);
      
      int avgLR = averageLastLR(currentReadLR);
      int mapLR = map(avgLR, 0, 1024, 850, 150);
      int diffLR = currentPositionLR - mapLR;    


      int avgUD = averageLastUD(currentReadUD);
      int mapUD = map(avgUD, 0, 1024, 350, 30);
      int diffUD = currentPositionUD - mapUD;    

      int mvAmountLR = 0;
      int mvAmountUD = 0;
      
      int minChngLR = 50;
      int minChngUD = 30;

      if(calibrated){
        // left right movement
        if(diffLR > 0 && abs(diffLR)>minChngLR){
          mvAmountLR = moveLR(diffLR, 1);
          currentPositionLR = currentPositionLR - mvAmountLR;
          Serial.print(" newCurrentLR: ");      
          Serial.println(currentPositionLR);
        }else if(diffLR < 0 && abs(diffLR)>minChngLR){
          mvAmountLR = moveLR(abs(diffLR), 0);        
          currentPositionLR = currentPositionLR + mvAmountLR;
          Serial.print(" newCurrentLR: ");      
          Serial.println(currentPositionLR);
        }

        // updown stepper movement
        if(diffUD > 0 && abs(diffUD)>minChngUD){
          //Serial.print("MOVE UD-----------------------------------");      
          mvAmountUD = moveUD(diffUD, 1);
          currentPositionUD = currentPositionUD - mvAmountUD;
          Serial.print(" newCurrentUD: ");      
          Serial.println(currentPositionUD);
        }else if(diffUD < 0 && abs(diffUD)>minChngUD){
          //Serial.print("MOVE UD------------------------------------");    
          mvAmountUD = moveUD(abs(diffUD), 0);        
          currentPositionUD = currentPositionUD + mvAmountUD;
          Serial.print(" newCurrentUD: ");      
          Serial.println(currentPositionUD);
        }
      }
//
//      Serial.print(" newCurrentLR: ");
//      Serial.print(currentPositionLR);
//      Serial.print(" newCurrentUD: ");      
//      Serial.println(currentPositionUD);


    }
}

bool calibrate(){

  calibrateLR();
  
  calibrateUD();
  
  return true;
}


void calibrateLR(){
  //measure LR side to side 
  stopStateLR = false;      
  measureRight = measureLR(fullSpin, 1, STEPDIRECTIONLR, STEPPINLR);
  moveLR(5, 0);
  
  delay(10);   
  stopStateLR = false;
  measureLeft = measureLR(fullSpin, 0, STEPDIRECTIONLR, STEPPINLR);
  
  delay(10); 
  
  centerLR  = (measureLeft/2) + offset;
  stopStateLR = false;
  moveLR(centerLR, 1);
  currentPositionLR = centerLR;
}

void calibrateUD(){
    //measure UD top to bottom
  stopStateUD = false;      
  measureUp = measureUD(fullSpin, 1, STEPDIRECTIONUD, STEPPINUD);
  moveUD(5, 0);
  
  delay(10);   
  stopStateUD = false;
  measureDown = measureUD(fullSpin, 0, STEPDIRECTIONUD, STEPPINUD);
  
  delay(10); 
  
  centerUD  = (measureDown/2) + 0; //offset
  stopStateUD = false;
  moveUD(centerUD, 1);
  currentPositionUD = centerUD;
}



void stopLR(){
  stopStateLR = true;

  // look at current direction L or R
  // if L then set current position to max left
  // if R then set current poistion to max right 
  
  Serial.println("stop LR");
}
void stopUD(){
  stopStateUD = true;

  Serial.println("stop UD");
  Serial.print("directionUD: ");
  Serial.print(directionUD);
  Serial.print(" currentPositionUD");
  Serial.println(currentPositionUD);

  

//// 1 is down, 0 is up
//  if(directionUD == 1){
//    moveUD(5, 1);
//    currentPositionUD = 0;
//  }else if(directionUD == 0){
//    moveUD(5, 0);
//    currentPositionUD = 500;
//  }
//


//  Serial.print(" new currentPositionUD");
//  Serial.println(currentPositionUD);
//
//  // look at current direction U or D
//  // if U then set current position to max up
//  // if D then set current poistion to max down 
//  Serial.println("stop UD");
}

int measureLR(int times, int dir, int dirPin, int stepPin){
  int index = 0;

//  Serial.print("direction: ");// 1 is right, 0 is left
//  Serial.println(dir);

  directionLR = dir;
  
  if(dir){
    digitalWrite(dirPin, HIGH);
  }else{
    digitalWrite(dirPin, LOW);
  }
  
  while(index<times && stopStateLR == false){
    digitalWrite(stepPin, LOW);
    delayMicroseconds(chopSplit);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(chopDelay);
    index = index + 1;
  }
  return index;
}

int measureUD(int times, int dir, int dirPin, int stepPin){
  int index = 0;

//  
//  Serial.print("direction: ");// 1 is down, 0 is up
//  Serial.println(dir);

  directionUD = dir;


  if(dir){
    digitalWrite(dirPin, HIGH);
  }else{
    digitalWrite(dirPin, LOW);
  }
  
  while(index<times && stopStateUD == false){
    digitalWrite(stepPin, LOW);
    delayMicroseconds(chopSplit);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(chopDelay);
    index = index + 1;
  }
  return index;
}

// need to combine moveUD and moveLR into a single method that looks at the difference between the current position and suggested position for each 
// stepper and moves in the suggested direction to achieve that position on each iteration in order to not waste and cycles
int moveUD(int times, int dir){
  stopStateUD = false;
  
  int index = 0;
  if(dir){
    digitalWrite(STEPDIRECTIONUD, HIGH);
  }else{
    digitalWrite(STEPDIRECTIONUD, LOW);
  }

  while(index<times && stopStateUD == false){
    digitalWrite(STEPPINUD, LOW);
    delayMicroseconds(chopSplitPan);
    digitalWrite(STEPPINUD, HIGH);
    delayMicroseconds(chopDelayPan);
    index = index + 1;
  }
  return index;
}

int moveLR(int times, int dir){
  int index = 0;

  if(dir){
    digitalWrite(STEPDIRECTIONLR, HIGH);
  }else{
    digitalWrite(STEPDIRECTIONLR, LOW);
  }
  
  while(index<times){
    digitalWrite(STEPPINLR, LOW);
    delayMicroseconds(chopSplitPan);
    digitalWrite(STEPPINLR, HIGH);
    delayMicroseconds(chopDelayPan);
    index = index + 1;
  }
  return index;
}


//******************************************************************************************************

int averageLastLR(int incoming){
  if(last10LR == 0){last10LR = incoming;}
  if(last9LR == 0){last9LR = incoming;}
  if(last8LR == 0){last8LR = incoming;}
  if(last7LR == 0){last7LR = incoming;}
  if(last6LR == 0){last6LR = incoming;}
  
  if(last5LR == 0){last5LR = incoming;}
  if(last4LR == 0){last4LR = incoming;}
  if(last3LR == 0){last3LR = incoming;}
  if(last2LR == 0){last2LR = incoming;}
  if(last1LR == 0){last1LR = incoming;}

  last1LR = last2LR;
  last2LR = last3LR;
  last3LR = last4LR;  
  last4LR = last5LR;
  last5LR = last6LR;
  last6LR = last7LR;
  last7LR = last8LR;
  last8LR = last9LR;  
  last9LR = last10LR;
  last10LR = last11LR;
  last11LR = incoming;

  return (last6LR +last5LR +last4LR +last3LR +last2LR + last1LR +last7LR +last8LR +last9LR +last10LR +last11LR)/11;
}


int averageLastUD(int incoming){
  if(last10UD == 0){last10UD = incoming;}
  if(last9UD == 0){last9UD = incoming;}
  if(last8UD == 0){last8UD = incoming;}
  if(last7UD == 0){last7UD = incoming;}
  if(last6UD == 0){last6UD = incoming;}
  
  if(last5UD == 0){last5UD = incoming;}
  if(last4UD == 0){last4UD = incoming;}
  if(last3UD == 0){last3UD = incoming;}
  if(last2UD == 0){last2UD = incoming;}
  if(last1UD == 0){last1UD = incoming;}

  last1UD = last2UD;
  last2UD = last3UD;
  last3UD = last4UD;  
  last4UD = last5UD;
  last5UD = last6UD;
  last6UD = last7UD;
  last7UD = last8UD;
  last8UD = last9UD;  
  last9UD = last10UD;
  last10UD = last11UD;
  last11UD = incoming;

  return (last6UD +last5UD +last4UD +last3UD +last2UD + last1UD +last7UD +last8UD +last9UD +last10UD +last11UD)/11;
}




void flash_leds(int times, int flash_delay){
  int index = 0;
  while(index<times){
    digitalWrite(LED_BUILTIN, LOW);
    delay(flash_delay);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(flash_delay);
    index = index + 1;
  }
}


