#include <elapsedMillis.h>

/* Status log for differnt phases
After each phase is done the PhaseLog number will increase by one.
BootIdle = 0
Feed1 PhaseLog = 1
Anaerobic1 PhaseLog = 2
Aerobic1 PhaseLog = 3
Anoxic1 PhaseLog = 4
Feed2 PhaseLog = 5
Anaerobic2 PhaseLog = 6
Aerobic2 PhaseLog = 7
Anoxic2 PhaseLog = 8
Settling PhaseLog = 9
Discharge PhaseLog = 10
Idle PhaseLog = 11
*/
int PhaseLog = 0; //set to start at BootIdle, will run only upon reset.

/*
ID for the different phases
Used for sending in signals through serial port
Signal is read in the void loop
*/
int ResetID = 48;
int Feed1ID = 49;
int Anaerobic1ID = 50;
int Aerobic1ID = 51;
int Anoxic1ID = 52;
int Feed2ID = 53;
int Anaerobic2ID = 54;
int Aerobic2ID = 55;
int Anoxic2ID = 56;
int SettleID = 57;
int DischargeID = 58;
int IdleID = 59;
int BootIdleID = 60;


// Set pins for actuators.
int FeedPump = 10;
int WastePump = 8;
int AirPump = 4;
int NitrogenValve1 = 2;
int NitrogenValve2 = 7;
int IndicatorRed = 3;
int IndicatorGreen = 5;
int IndicatorBlue = 6;
int Phase = BootIdleID; //set to start at BootIdle, will run only upon reset.

// Durations of each time variable in minutes.
long Feed1 = (0 * 3600000) + (8 * 60000);
long Anaerobic1 = (1 * 3600000) + (0 * 60000);
long Aerobic1 = (1 * 3600000) + (40 * 60000);
long Anoxic1 = (0 * 3600000) + (50 * 60000);
long Feed2 = (0 * 3600000) + (2 * 60000);
long Anaerobic2 = (0 * 3600000) + (30 * 60000);
long Aerobic2 = (0 * 3600000) + (50 * 60000);
long Anoxic2 = (0 * 3600000) + (45 * 60000);
long Settle = (0 * 3600000) + (5 * 60000);
long Discharge = (0 * 3600000) + (2 * 60000);
long Idle = (0 * 3600000) + (8 * 60000);
long BootIdle = (0 * 3600000) + (0.2 * 60000);
long DurationT2 = (Feed1 + Anaerobic1 + Aerobic1 + Anoxic1 + Feed2 + Anaerobic2 + Aerobic2 + Anoxic2);

// Variables for peristaltic pumps.
int FeedPumpState = LOW;
int WastePumpState = LOW;

// Variables for Nitrogen sparging.
long NitrogenOn = 5 *1000;
long NitrogenOff = 25 *1000;
int NitrogenValveState1 = LOW;
int NitrogenValveState2 = LOW;
long NitrogenOnOff = NitrogenOn + NitrogenOff;

// Variables for Air sparging.
long AirOn = 0.5 * 60000;
long AirOff = 0.5 * 60000;
int AirPumpState = LOW;
long AirOnOff = AirOn + AirOff;

//Set up timers.
elapsedMillis timer0; //Countdown for each phase.
elapsedMillis timer1; //Keeps track of Nitrogen and Air.
elapsedMillis timer2; //Keeps track of the phase duration.
elapsedMillis timer3; //Keeps track of time till end of Anoxic-2.

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  //Set all digital output pins to OUTPUT
  for (int x = 2; x < 11; x++) {
    pinMode(x , OUTPUT);
    digitalWrite(x, LOW);
  }
  pinMode(12 , OUTPUT);   // Turn ON LCD display
  digitalWrite(12, HIGH); // Turn ON LCD display
  timer0 = 0; // clear the timer at the end of startup
  timer1 = 0; // clear the timer at the end of startup
  timer2 = 0; // clear the timer at the end of startup
}

void FeedingPhase(long DurationF){
  if (timer2 <= DurationF){
    FeedPumpState = HIGH;
    digitalWrite(FeedPump, FeedPumpState);
  }
  else if  (timer2 > DurationF){
    FeedPumpState = LOW;
    digitalWrite(FeedPump, FeedPumpState);
    //Serial.println("Done Feeding.");
  }
}

void NitrogenSparge(long DurationNS){
  if (timer2 <= DurationNS){
    if ((timer1 < NitrogenOnOff - NitrogenOn) && (NitrogenValveState1 == HIGH)) {
      NitrogenValveState1 = LOW;
      digitalWrite(NitrogenValve1, NitrogenValveState1);
    }
    else if ((timer1 > NitrogenOnOff - NitrogenOn) && (timer1 < NitrogenOnOff) && (NitrogenValveState1 == LOW)) {
      NitrogenValveState1 = HIGH;
      digitalWrite(NitrogenValve1, NitrogenValveState1);
    }
    else if ((timer1 > NitrogenOnOff) && (NitrogenValveState1 == HIGH)) {
      NitrogenValveState1 = LOW;
      digitalWrite(NitrogenValve1, NitrogenValveState1);
      timer1 -= NitrogenOnOff; //reset the timer
    }
  }
  else if (timer2 > DurationNS){
    NitrogenValveState1 = LOW;
    digitalWrite(NitrogenValve1, NitrogenValveState1);
    //Serial.println("Done Nitrogen Sparging.");
  }
}

void AirSparge(long DurationAS){
  if (timer2 <= DurationAS){
    if ((timer1 < AirOnOff - AirOn) && (AirPumpState == HIGH)) { 
      AirPumpState = LOW;
      digitalWrite(AirPump, AirPumpState);
    }
    else if ((timer1 > AirOnOff - AirOn) && (timer1 < AirOnOff) && (AirPumpState == LOW)) { 
      AirPumpState = HIGH;
      digitalWrite(AirPump, AirPumpState);
    }
    else if ((timer1 > AirOnOff) && (AirPumpState == HIGH)) { 
      AirPumpState = LOW;
      digitalWrite(AirPump, AirPumpState);
      timer1 -= AirOnOff; //reset the timer
    }
  }
  else if (timer2 > DurationAS){
    AirPumpState = LOW;
    digitalWrite(AirPump, AirPumpState);
    //Serial.println("Done Air Sparging.");
  }
}

void SettlingPhase(long DurationS){
  if  (timer2 > DurationS){
    //Serial.println("Done Settling.");
  }
}

void DischargingPhase(long DurationD){
  if (timer2 <= DurationD){
    WastePumpState = HIGH;
    digitalWrite(WastePump, WastePumpState);
  }
  else if  (timer2 > DurationD){
    WastePumpState = LOW;
    digitalWrite(WastePump, WastePumpState);
    //Serial.println("Done Discharging.");
  }
}

void IdlePhase(long DurationI){
  if (timer2 <= DurationI){
  }
  else if  (timer2 > DurationI){
    //Serial.println("Done Idling.");
  }
}

void TimerDisplay(long DurationT){
  if (timer2 <= DurationT){
    if (timer0 > 1000){
      selectLineThree();
      TimePrinter(DurationT - timer2);
      //Serial.print(" remaining");
      timer0 = 0;
    }
  }
  else if  (timer2 > DurationT){
    Phase = ResetID; //function to reset timer
  }
  if (timer3 <= DurationT2 && PhaseLog > 0){
    if (timer0 > 250 && timer0 < 600){
      selectLineFour();
      TimePrinter(DurationT2 - timer3);
    }
  }
  if  (timer3 > DurationT2){
    selectLineFour();
    Serial.print("Cycle Over");
  }
}

void ResetTimer(){
  clearLCD();
  //Serial.print("timer2: ");
  //Serial.println(timer2/1000);
  timer0 = 0; // clear the timer at the end of startup
  timer1 = 0; // clear the timer at the end of startup
  timer2 = 0; // clear the timer at the end of startup
  if (PhaseLog == 0){
    timer3 = 0; // reset timer after boot idle phase.
  }
  for (int x = 2; x < 11; x++) {
    pinMode(x , OUTPUT);
    digitalWrite(x, LOW);
  }
  // Read the PhaseLog to determined what was the previous phase.
  if (PhaseLog == 1){
    PhaseLog +=1;
    Phase = Feed1ID;
    selectLineOne();
    Serial.print("Feeding(1)");
    selectLineTwo();
    Serial.print(Feed1 / 3600000);
    Serial.print("h ");
    Serial.print((Feed1 % 3600000) / 60000);
    Serial.print("m ");
  }
  if (PhaseLog == 2){
    PhaseLog +=1;
    Phase = Anaerobic1ID;
    selectLineOne();
    Serial.print("Anaerobic (1)");
    selectLineTwo();
    Serial.print(Anaerobic1 / 3600000);
    Serial.print("h ");
    Serial.print((Anaerobic1 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 3){
    PhaseLog +=1;
    Phase = Aerobic1ID;
    selectLineOne();
    Serial.print("Aerobic (1)");
    selectLineTwo();
    Serial.print(Aerobic1 / 3600000);
    Serial.print("h ");
    Serial.print((Aerobic1 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 4){
    PhaseLog +=1;
    Phase = Anoxic1ID;
    selectLineOne();
    Serial.print("Anoxic (1)");
    selectLineTwo();
    Serial.print(Anoxic1 / 3600000);
    Serial.print("h ");
    Serial.print((Anoxic1 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 5){
    PhaseLog +=1;
    Phase = Feed2ID;
    selectLineOne();
    Serial.print("Feeding (2)");
    selectLineTwo();
    Serial.print(Feed2 / 3600000);
    Serial.print("h ");
    Serial.print((Feed2 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 6){
    PhaseLog +=1;
    Phase = Anaerobic2ID;
    selectLineOne();
    Serial.print("Anaerobic (2)");
    selectLineTwo();
    Serial.print(Anaerobic2 / 3600000);
    Serial.print("h ");
    Serial.print((Anaerobic2 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 7){
    PhaseLog +=1;
    Phase = Aerobic2ID;
    selectLineOne();
    Serial.print("Aerobic (2)");
    selectLineTwo();
    Serial.print(Aerobic2 / 3600000);
    Serial.print("h ");
    Serial.print((Aerobic2 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 8){
    PhaseLog +=1;
    Phase = Anoxic2ID;
    selectLineOne();
    Serial.print("Anoxic (2)");
    selectLineTwo();
    Serial.print(Anoxic2 / 3600000);
    Serial.print("h ");
    Serial.print((Anoxic2 % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 9){
    PhaseLog +=1;
    Phase = SettleID;
    selectLineOne();
    Serial.print("Settle");
    selectLineTwo();
    Serial.print(Settle / 3600000);
    Serial.print("h ");
    Serial.print((Settle % 3600000) / 60000);
    Serial.print("m ");
  }
  else if (PhaseLog == 10){
    PhaseLog +=1;
    Phase = DischargeID;
    selectLineOne();
    Serial.print("Discharge");
    selectLineTwo();
    Serial.print(Discharge / 3600000);
    Serial.print("h ");
    Serial.print((Discharge % 3600000) / 60000);
    Serial.print("m ");
 }
  else if (PhaseLog == 11){
    PhaseLog +=1;
    Phase = IdleID;
    selectLineOne();
    Serial.print("Idle");
    selectLineTwo();
    Serial.print(Idle / 3600000);
    Serial.print("h ");
    Serial.print((Idle % 3600000) / 60000);
    Serial.print("m ");
  }
  else {
    PhaseLog = 1;
    Phase = Feed1ID;
    selectLineOne();
    Serial.print("Feeding (1)");
    selectLineTwo();
    Serial.print(Feed1 / 3600000);
    Serial.print("h ");
    Serial.print((Feed1 % 3600000) / 60000);
    Serial.print("m ");
    timer3 = 0; // reset timer after idle phase.
  }
}

void selectLineOne(){  //puts the cursor at line 0 char 0.
   Serial.write(0xFE);   //command flag
   Serial.write(128);    //position
   Serial.print("l1 ");
}
void selectLineTwo(){  //puts the cursor at line 2 char 0.
   Serial.write(0xFE);   //command flag
   Serial.write(192);    //position
   Serial.print("l2 ");
}
void selectLineThree(){  //puts the cursor at line 3 char 0.
   Serial.write(0xFE);   //command flag
   Serial.write(148);    //position
   Serial.print("l3 ");
}
void selectLineFour(){  //puts the cursor at line 4 char 0.
   Serial.write(0xFE);   //command flag
   Serial.write(212);    //position
   Serial.print("l4 ");
}
void clearLCD(){
   Serial.write(0xFE);   //command flag
   Serial.write(0x01);   //clear command.
}

void TimePrinter (long TimeToPrint){
  int HourDisplay = (TimeToPrint / 3600000);
  int MinuteDisplay = ((TimeToPrint % 3600000) / 60000);
  int SecondDisplay = ((TimeToPrint % 60000) / 1000);
  Serial.print(HourDisplay);
  if (MinuteDisplay > 9){
    Serial.print(":");
    Serial.print(MinuteDisplay);
  }
  else{
    Serial.print(":0");
    Serial.print(MinuteDisplay);
  }
  if (SecondDisplay > 9){
    Serial.print(":");
    Serial.print(SecondDisplay);
  }
  else{
    Serial.print(":0");
    Serial.print(SecondDisplay);
  }
}

void loop() {
  if (Serial.available() > 0) {
    Phase = Serial.read();
    //Serial.print("Order received: ");
    //Serial.println(Phase);
    if (Phase == Feed1ID){
      PhaseLog = 2;
      //Serial.println("Begin Feeding 1.");
    }
    else if (Phase == Anaerobic1ID){
      PhaseLog = 3;
      //Serial.println("Begin Anaerobic Phase 1.");
    }
    else if (Phase == Aerobic1ID){
      PhaseLog = 4;
      //Serial.println("Begin Aerobic Phase 1.");
    }
    else if (Phase == Anoxic1ID){
      PhaseLog = 5;
      //Serial.println("Begin Anoxic Phase 1.");
    }
    else if (Phase == Feed2ID){
      PhaseLog = 6;
      //Serial.println("Begin Fedding 2.");
    }
    else if (Phase == Anaerobic2ID){
      PhaseLog = 7;
      //Serial.println("Begin Anaerobic Phase 2.");
    }
    else if (Phase == Aerobic2ID){
      PhaseLog = 8;
      //Serial.println("Begin Aerobic Phase 2.");
    }
    else if (Phase == Anoxic2ID){
      PhaseLog = 9;
      //Serial.println("Begin Anoxic Phase 2.");
    }
    else if (Phase == SettleID){
      PhaseLog = 10;
      //Serial.println("Begin Settling.");
    }
    else if (Phase == DischargeID){
      PhaseLog = 11;
      //Serial.println("Begin Discharging.");
    }
    else if (Phase == IdleID){
      PhaseLog = 12;
      //Serial.println("Begin Idling.");
    }
    else if (Phase == ResetID){
      //Serial.println("Reseting timer for next phase.");
    }
    
  }

  switch (Phase){
    /* Remember to change the values for the case if the
    ID for the different phases have been changed. */
    case 49:
    NitrogenSparge(Feed1);
    FeedingPhase(Feed1);
    TimerDisplay(Feed1);
    break;
    case 50:
    NitrogenSparge(Anaerobic1);
    TimerDisplay(Anaerobic1);
    break;
    case 51:
    AirSparge(Aerobic1);
    TimerDisplay(Aerobic1);
    break;
    case 52:
    NitrogenSparge(Anoxic1);
    TimerDisplay(Anoxic1);
    break;
    case 53:
    NitrogenSparge(Feed2);
    FeedingPhase(Feed2);
    TimerDisplay(Feed2);
    case 54:
    NitrogenSparge(Anaerobic2);
    TimerDisplay(Anaerobic2);
    break;
    case 55:
    AirSparge(Aerobic2);
    TimerDisplay(Aerobic2);
    break;
    case 56:
    NitrogenSparge(Anoxic2);
    TimerDisplay(Anoxic2);
    break;
    case 57:
    SettlingPhase(Settle);
    TimerDisplay(Settle);
    break;
    case 58:
    DischargingPhase(Discharge);
    TimerDisplay(Discharge);
    break;
    case 59:
    IdlePhase(Idle);
    TimerDisplay(Idle);
    break;
    case 60:
    IdlePhase(BootIdle);
    TimerDisplay(BootIdle);
    break;
    case 48:
    ResetTimer();
    break; 
  }

   
}    


