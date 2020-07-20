
#define STEPDIRECTION                   5  // dir pin stepstick
#define STEPPIN                         4  // step pin stepstick
#define BTNPIN                         3  // step pin stepstick



//#define ONBOARDLED                      4  // wemos pin onboard led
bool triggered = false;


int buttonStatus = 0;

volatile byte stopState = false;
const byte interruptPin = 7;
volatile byte state = LOW;

int fullSpin = 1600;// 1600 is 360 exactly
int chopDelay = 600;
int chopSplit = 150;

int lastLeft = 0;
int lastRight = 0;


byte leftRight = false;


static unsigned long last;
void setup() {
  // put your setup code here, to run once:

    Serial.println("STARTING");
    
    
    //pinMode(ONBOARDLED, OUTPUT);
    pinMode(STEPDIRECTION, OUTPUT);
    pinMode(STEPPIN, OUTPUT);
    pinMode(BTNPIN, INPUT);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), stop, CHANGE);
  
    //digitalWrite(ONBOARDLED, HIGH);
    digitalWrite(STEPDIRECTION, HIGH);
    digitalWrite(STEPPIN, HIGH);

    attachInterrupt(digitalPinToInterrupt(interruptPin), stop, FALLING );
    
}

void loop() {
    buttonStatus = digitalRead(BTNPIN);

    if(buttonStatus){
      stopState = false;
      Serial.println("button pressed");
      lastLeft = measure(fullSpin, chopSplit, chopDelay, 1);

      stopState = false;
      lastRight = measure(fullSpin, chopSplit, chopDelay, 0);

      stopState = false;
      lastRight = measure(lastRight/2, chopSplit, chopDelay, 1);
      

      //leftRight = !leftRight;
    }else{

      Serial.print("lastLeft: ");
      Serial.print(lastLeft);
      Serial.print(" lastRight: ");
      Serial.println(lastRight);
      
    }


    
}

void stop(){
  stopState = true;
  Serial.println("stop");
  
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
