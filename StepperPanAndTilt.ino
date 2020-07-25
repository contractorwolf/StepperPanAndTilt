
#define STEPDIRECTION                   4  // dir pin stepstick
#define STEPPIN                         3  // step pin stepstick
#define BTNPIN                         2  // step pin stepstick



#define STEPDIRECTIONLR                   4  // dir pin stepstick
#define STEPPINLR                         3  // step pin stepstick

#define STEPDIRECTIONUD                   12  // dir pin stepstick
#define STEPPINUD                         11  // step pin stepstick






//#define ONBOARDLED                      4  // wemos pin onboard led
bool triggered = false;


int buttonStatus = 0;

volatile byte stopState = false;
const byte interruptPin = 7;

int analogPinLR = A0; // potentiometer wiper (middle terminal) connected to analog pin 0
int analogPinUD = A1; // potentiometer wiper (middle terminal) connected to analog pin 0

int lastRead = 0;  // variable to store the value read



volatile byte state = LOW;

int fullSpin = 1600;// 1600 is 360 exactly
int chopDelay = 400;//200
int chopSplit = 400;//150


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

int currentPositionLR = 0;
int currentPositionUD = 0;
volatile int measureLeft = 0;
volatile int measureRight = 0;

byte calibrated = false;


byte leftRight = false;

int lastAvgLR = 0;
int lastAvgUD = 0;

static unsigned long last;
void setup() {
  // put your setup code here, to run once:

    Serial.println("STARTING");
    
    
    //pinMode(ONBOARDLED, OUTPUT);
    pinMode(STEPDIRECTIONUD, OUTPUT);
    pinMode(STEPPINUD, OUTPUT);
    pinMode(STEPDIRECTIONLR, OUTPUT);
    pinMode(STEPPINLR, OUTPUT);
    pinMode(BTNPIN, INPUT);


    

  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), stop, CHANGE);
  
    //digitalWrite(ONBOARDLED, HIGH);
    digitalWrite(STEPDIRECTIONLR, HIGH);
    digitalWrite(STEPPINLR, HIGH);
    digitalWrite(STEPDIRECTIONUD, HIGH);
    digitalWrite(STEPPINUD, HIGH);


    

    attachInterrupt(digitalPinToInterrupt(interruptPin), stop, FALLING );
    
}

void loop() {
    buttonStatus = digitalRead(BTNPIN);

    if(buttonStatus){

      
      stopState = false;
      Serial.println("button pressed");
      measureRight = measure(fullSpin, chopSplit, chopDelay, 1);
      move(5, chopSplit, chopDelay, 0);
      delay(10);   
      stopState = false;
      measureLeft = measure(fullSpin, chopSplit, chopDelay, 0);
      delay(10); 
      stopState = false;

      centerLR  = (measureLeft/2) + offset;
      currentPositionLR = centerLR;
      
      moveLR(centerLR, chopSplit, chopDelay, 1);
      currentPositionUD = 50;

      moveUD(currentPositionUD, chopSplit, chopDelay, 1);

      
      calibrated = true;
      

      //leftRight = !leftRight;
    }else{
      int currentReadLR = analogRead(analogPinLR);
      int currentReadUD = analogRead(analogPinUD);
      
      int avgLR = averageLastLR(currentReadLR);
      int mapLR = map(avgLR, 0, 1024, 906, 0);
      int diffLR = currentPositionLR - mapLR;    


      int avgUD = averageLastUD(currentReadUD);
      int mapUD = map(avgUD, 0, 1024, 500, 0);
      int diffUD = currentPositionUD - mapUD;    

       
//      
//      Serial.print(" currentPositionLR: ");
//      Serial.print(currentPositionLR);
//
//      Serial.print(" currentReadLR: ");
//      Serial.print(currentReadLR);
//      
//      Serial.print(" currentReadUD: ");
//      Serial.print(currentReadUD);
//
//      Serial.print(" avgLR: ");
//      Serial.print(avgLR);
//      
//      Serial.print(" mapLR: ");
//      Serial.print(mapLR);
//
//      Serial.print(" diffLR: ");
//      Serial.print(diffLR);
//      
//      Serial.print(" diffUD: ");
//      Serial.print(diffUD);

    
      int mvAmountLR = 0;
      int mvAmountUD = 0;
      
      int minChngLR = 50;
      int minChngUD = 30;

      
      int chopDelayPan = 1500;//200
      int chopSplitPan = 1500;//150
      
      if(calibrated){
        // left right movement
        if(diffLR > 0 && abs(diffLR)>minChngLR){
          mvAmountLR = moveLR(diffLR, chopSplitPan, chopDelayPan, 1);
          currentPositionLR = currentPositionLR - mvAmountLR;
        }else if(diffLR < 0 && abs(diffLR)>minChngLR){
          mvAmountLR = moveLR(abs(diffLR), chopSplitPan, chopDelayPan, 0);        
          currentPositionLR = currentPositionLR + mvAmountLR;
        }

        // updown stepper movement
        if(diffUD > 0 && abs(diffUD)>minChngUD){
          Serial.print("MOVE UD-----------------------------------");        
          mvAmountUD = moveUD(diffUD, chopSplitPan, chopDelayPan, 1);
          currentPositionUD = currentPositionUD - mvAmountUD;
        }else if(diffUD < 0 && abs(diffUD)>minChngUD){
          Serial.print("MOVE UD------------------------------------");    
          mvAmountUD = moveUD(abs(diffUD), chopSplitPan, chopDelayPan, 0);        
          currentPositionUD = currentPositionUD + mvAmountUD;
        }
      }

      Serial.print(" newCurrentLR: ");
      Serial.print(currentPositionLR);
      Serial.print(" newCurrentUD: ");
      Serial.println(currentPositionUD);


    }
}

void stop(){
  stopState = true;
  Serial.println("stop");
}


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
  while(index<times && stopState == false){
    digitalWrite(LED_BUILTIN, LOW);
    delay(flash_delay);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(flash_delay);
    index = index + 1;
  }
}





void simple_wave(int times, int flash_delay, int off_delay){
  int index = 0;
  while(index<times && stopState == false){
    digitalWrite(STEPPIN, LOW);
    delayMicroseconds(flash_delay);
    digitalWrite(STEPPIN, HIGH);
    delayMicroseconds(off_delay);
    index = index + 1;
  }
}



int measure(int times, int flash_delay, int off_delay, int dir){
  int index = 0;


  if(dir){
    digitalWrite(STEPDIRECTION, HIGH);
  }else{
    digitalWrite(STEPDIRECTION, LOW);
  }
  
  while(index<times && stopState == false){

      digitalWrite(STEPPIN, LOW);
      delayMicroseconds(flash_delay);
      digitalWrite(STEPPIN, HIGH);
      delayMicroseconds(off_delay);

    index = index + 1;
  }
  return index;
}

int move(int times, int flash_delay, int off_delay, int dir){
  int index = 0;
  if(dir){
    digitalWrite(STEPDIRECTION, HIGH);
  }else{
    digitalWrite(STEPDIRECTION, LOW);
  }
  
  while(index<times){
    digitalWrite(STEPPIN, LOW);
    delayMicroseconds(flash_delay);
    digitalWrite(STEPPIN, HIGH);
    delayMicroseconds(off_delay);
    index = index + 1;
  }
  return index;
}

int moveUD(int times, int flash_delay, int off_delay, int dir){
  int index = 0;
  if(dir){
    digitalWrite(STEPDIRECTIONUD, HIGH);
  }else{
    digitalWrite(STEPDIRECTIONUD, LOW);
  }
  
  while(index<times){
    digitalWrite(STEPPINUD, LOW);
    delayMicroseconds(flash_delay);
    digitalWrite(STEPPINUD, HIGH);
    delayMicroseconds(off_delay);
    index = index + 1;
  }
  return index;
}

int moveLR(int times, int flash_delay, int off_delay, int dir){
  int index = 0;
  if(dir){
    digitalWrite(STEPDIRECTIONLR, HIGH);
  }else{
    digitalWrite(STEPDIRECTIONLR, LOW);
  }
  
  while(index<times){
    digitalWrite(STEPPINLR, LOW);
    delayMicroseconds(flash_delay);
    digitalWrite(STEPPINLR, HIGH);
    delayMicroseconds(off_delay);
    index = index + 1;
  }
  return index;
}


// 3240, 200, 200
//(1/(3240/4))+1
void step_wave(int times, int flash_delay, int off_delay){
  int index = 0;
  int quad = 1;
  int on_t;

  // min: 200--800
  while(index<times && stopState == false){
    quad = (index/(times/10)) + 1;

    if(quad==1){
        on_t = flash_delay * 3;
    }else if(quad==2){
        on_t = flash_delay * 2;
    }else if(quad==3){
        on_t = flash_delay;
    }else if(quad==4){
        on_t = flash_delay;
    }else if(quad==5){
        on_t = flash_delay;
    }else if(quad==6){
        on_t = flash_delay;
    }else if(quad==7){
        on_t = flash_delay;
    }else if(quad==8){
        on_t = flash_delay;
    }else if(quad==9){
        on_t = flash_delay;
    }else if(quad==10){
        on_t = flash_delay * 2;
    }

    digitalWrite(STEPPIN, LOW);
    delayMicroseconds(on_t);
    digitalWrite(STEPPIN, HIGH);
    delayMicroseconds(on_t);
    
    index = index + 1;
    
    yield();

  }
}
