
//*******************************************************************************************************************
//                                 Declerations
//*******************************************************************************************************************
//seting up IO
int inClk = 1;
int inClkSupress = 2;
int inPolSig = 3;
int inReg1 = 4;
int inReg2 = 5;
int outClk = 6;
int outPolSig = 7;
int outRegRed = 8;
int outRegGreen = 9;
int outRegBlue = 10;
int outRegButtons1 = 16;
int outRegButtons2 = 14;
int wave = 0;
int waveLength = 100;
int inputLoopCount = 9;
int pulseWidthPos = 0;
int redLedWidths [8] = {-1, -1, -1, -1, -1, -1, -1, -1};
int greenLedWidths [8] = {-1, -1, -1, -1, -1, -1, -1, -1};
int blueLedWidths [8] = {-1, -1, -1, -1, -1, -1, -1, -1};
int buttonWidths1 [8] = {32, 32, 32, 32, 32, 32, 32, 32};
int buttonWidths2 [8] = {32, 32, 32, 32, 32, 32, 32, 32};
int topButtons [5] = {0, 0, 0, 0, 0};
int topButtonsEcho [5] = {0, 0, 0, 0, 0};
int topButtonsLights [5] = {32, 32, 32, 32, 32};
int midButtons [5] = {0, 0, 0, 0, 0};
int midButtonsEcho [5] = {0, 0, 0, 0, 0};
int midButtonsLights [5] = {32, 32, 32, 32, 32};
int botButtons [5] = {0, 0, 0, 0, 0};
int botButtonsEcho [5] = {0, 0, 0, 0, 0};
int botButtonsLights [5] = {32, 32, 32, 32, 32};
int inputReg1 [8] = {0, 0, 0, 0, 0, 0, 0, 0};
int inputReg2 [8] = {0, 0, 0, 0, 0, 0, 0, 0};
int selectedFunction = 0;
int redLightPulse [8] = {-1, -1, -1, -1, -1, -1, -1, -1};
int greenLightPulse [8] = {-1, -1, -1, -1, -1, -1, -1, -1};
int blueLightPulse [8] = {-1, -1, -1, -1, -1, -1, -1, -1};

int ledSelectEcho = -1;

//*******************************************************************************************************************
//                                 Setup
//*******************************************************************************************************************
void setup() {
  // pin mode setup
  pinMode(inClk, OUTPUT);
  pinMode(inClkSupress, OUTPUT);
  pinMode(inPolSig, OUTPUT);
  pinMode(inReg1, INPUT);
  pinMode(inReg2, INPUT);
  pinMode(outClk, OUTPUT);
  pinMode(outPolSig, OUTPUT);
  pinMode(outRegRed, OUTPUT);
  pinMode(outRegGreen, OUTPUT);
  pinMode(outRegBlue, OUTPUT);
  pinMode(outRegButtons1, OUTPUT);
  pinMode(outRegButtons2, OUTPUT);

  // timer interrupt setup
  cli();
  TCCR1A = 0; // 0b00000000  - TOP is 0xFFFF
  // ICNC1 | ICES1 | - | WGM13 | WGM12 | CS12 | CS11 | CS10
  TCCR1B = 0x09; //0x0C; // 0b00001XXX  // last 3 bits specify clock source: 0 = no clock source (stopped); 1 = no prescale; 2 = clock / 8; 3 = clock / 64
                  // 4 = clock / 256; 5 = clock / 1024; 6 = ext clock on T0 pin falling edge; 7 = ext clock on T0 pin rising edge
  TCCR1C = 0; // not forcing output compare
  TCNT1 = 0; // set timer counter initial value (16 bit value)

  // Interrupts on overflow (every ~1 second)
  //OCR1A = 0xF424;
  OCR1A = 0x2000; // closer to one second than value above

  //TIMSK1 = 1; // enable timer overflow interrupt 
  TIMSK1 = 2; // enable timer compare match 1A interrupt 

  sei(); // enable interrupts
}
//*******************************************************************************************************************
//                                 Functions
//*******************************************************************************************************************
void functionSelect(){
  if(topButtons[0] == 1 && topButtons[4] == 1 && botButtons[0] == 1 && botButtons[4] == 1){
    selectedFunction = 0;
    buttonLightsOff();
    lightsOff();
  }
  switch(selectedFunction){
    case 1: //RGB Wave
      buttonLightNormalAction();
      waveSin();
     break;
    case 2: //LED Brightness Step
      buttonLightNormalAction();
      stepConnect();
      break;
    case 3: //Button Toggle
      buttonLightToggle();
      break;
    case 4: //Brightness Set By Address
      buttonLightToggle();
      binaryLightSet();
      break;
    case 5: //light Pulse
      buttonLightNormalAction();
      lightPulse();
      break;
    default:
      menue();
  }
}
void menue(){
  topButtonsLights[0] = 32;
  topButtonsLights[1] = 32;
  topButtonsLights[2] = 32;
  topButtonsLights[3] = 32;
  topButtonsLights[4] = 32;
  midButtonsLights[0] = -1;
  midButtonsLights[1] = -1;
  midButtonsLights[2] = -1;
  midButtonsLights[3] = -1;
  midButtonsLights[4] = -1;
  botButtonsLights[0] = -1;
  botButtonsLights[1] = -1;
  botButtonsLights[2] = -1;
  botButtonsLights[3] = -1;
  botButtonsLights[4] = -1;
  if(topButtons[0] == 1 && topButtonsEcho[0] == 0){selectedFunction = 1;}
  if(topButtons[1] == 1 && topButtonsEcho[1] == 0){selectedFunction = 2;}
  if(topButtons[2] == 1 && topButtonsEcho[2] == 0){selectedFunction = 3; buttonLightsOn();}
  if(topButtons[3] == 1 && topButtonsEcho[3] == 0){selectedFunction = 4; buttonLightsOff(); lightsOff();}
  if(topButtons[4] == 1 && topButtonsEcho[4] == 0){selectedFunction = 5;}
  //if(midButtons[0] == 1 && midButtonsEcho[0] == 0){selectedFunction = 6;}
  //if(midButtons[1] == 1 && midButtonsEcho[1] == 0){selectedFunction = 7;}
  //if(midButtons[2] == 1 && midButtonsEcho[2] == 0){selectedFunction = 8;}
  //if(midButtons[3] == 1 && midButtonsEcho[3] == 0){selectedFunction = 9;}
  //if(midButtons[4] == 1 && midButtonsEcho[4] == 0){selectedFunction = 10;}
  //if(botButtons[0] == 1 && botButtonsEcho[0] == 0){selectedFunction = 11;}
  //if(botButtons[1] == 1 && botButtonsEcho[1] == 0){selectedFunction = 12;}
  //if(botButtons[2] == 1 && botButtonsEcho[2] == 0){selectedFunction = 13;}
  //if(botButtons[3] == 1 && botButtonsEcho[3] == 0){selectedFunction = 14;}
  //if(botButtons[4] == 1 && botButtonsEcho[4] == 0){selectedFunction = 15;}
}
void buttonLightsOff(){
  topButtonsLights[0] = -1;
  topButtonsLights[1] = -1;
  topButtonsLights[2] = -1;
  topButtonsLights[3] = -1;
  topButtonsLights[4] = -1;
  midButtonsLights[0] = -1;
  midButtonsLights[1] = -1;
  midButtonsLights[2] = -1;
  midButtonsLights[3] = -1;
  midButtonsLights[4] = -1;
  botButtonsLights[0] = -1;
  botButtonsLights[1] = -1;
  botButtonsLights[2] = -1;
  botButtonsLights[3] = -1;
  botButtonsLights[4] = -1;
}
void buttonLightsOn(){
  topButtonsLights[0] = 32;
  topButtonsLights[1] = 32;
  topButtonsLights[2] = 32;
  topButtonsLights[3] = 32;
  topButtonsLights[4] = 32;
  midButtonsLights[0] = 32;
  midButtonsLights[1] = 32;
  midButtonsLights[2] = 32;
  midButtonsLights[3] = 32;
  midButtonsLights[4] = 32;
  botButtonsLights[0] = 32;
  botButtonsLights[1] = 32;
  botButtonsLights[2] = 32;
  botButtonsLights[3] = 32;
  botButtonsLights[4] = 32;
}
void lightsOff(){
  redLedWidths[0] = -1;
  redLedWidths[1] = -1;
  redLedWidths[2] = -1;
  redLedWidths[3] = -1;
  redLedWidths[4] = -1;
  redLedWidths[5] = -1;
  redLedWidths[6] = -1;
  greenLedWidths[0] = -1;
  greenLedWidths[1] = -1;
  greenLedWidths[2] = -1;
  greenLedWidths[3] = -1;
  greenLedWidths[4] = -1;
  greenLedWidths[5] = -1;
  greenLedWidths[6] = -1;
  blueLedWidths[0] = -1;
  blueLedWidths[1] = -1;
  blueLedWidths[2] = -1;
  blueLedWidths[3] = -1;
  blueLedWidths[4] = -1;
  blueLedWidths[5] = -1;
  blueLedWidths[6] = -1;
}
void lightsOn(){
  redLedWidths[0] = 32;
  redLedWidths[1] = 32;
  redLedWidths[2] = 32;
  redLedWidths[3] = 32;
  redLedWidths[4] = 32;
  redLedWidths[5] = 32;
  redLedWidths[6] = 32;
  greenLedWidths[0] = 32;
  greenLedWidths[1] = 32;
  greenLedWidths[2] = 32;
  greenLedWidths[3] = 32;
  greenLedWidths[4] = 32;
  greenLedWidths[5] = 32;
  greenLedWidths[6] = 32;
  blueLedWidths[0] = 32;
  blueLedWidths[1] = 32;
  blueLedWidths[2] = 32;
  blueLedWidths[3] = 32;
  blueLedWidths[4] = 32;
  blueLedWidths[5] = 32;
  blueLedWidths[6] = 32;
}
void buttonLightToggle(){
   if(inputLoopCount == 0){
    if(topButtons[0] == 1 && topButtonsEcho[0] == 0){if(topButtonsLights[0] > -1){topButtonsLights[0] = -1;}else{topButtonsLights[0] = 32;}}
    if(topButtons[1] == 1 && topButtonsEcho[1] == 0){if(topButtonsLights[1] > -1){topButtonsLights[1] = -1;}else{topButtonsLights[1] = 32;}}
    if(topButtons[2] == 1 && topButtonsEcho[2] == 0){if(topButtonsLights[2] > -1){topButtonsLights[2] = -1;}else{topButtonsLights[2] = 32;}}
    if(topButtons[3] == 1 && topButtonsEcho[3] == 0){if(topButtonsLights[3] > -1){topButtonsLights[3] = -1;}else{topButtonsLights[3] = 32;}}
    if(topButtons[4] == 1 && topButtonsEcho[4] == 0){if(topButtonsLights[4] > -1){topButtonsLights[4] = -1;}else{topButtonsLights[4] = 32;}}
    if(midButtons[0] == 1 && midButtonsEcho[0] == 0){if(midButtonsLights[0] > -1){midButtonsLights[0] = -1;}else{midButtonsLights[0] = 32;}}
    if(midButtons[1] == 1 && midButtonsEcho[1] == 0){if(midButtonsLights[1] > -1){midButtonsLights[1] = -1;}else{midButtonsLights[1] = 32;}}
    if(midButtons[2] == 1 && midButtonsEcho[2] == 0){if(midButtonsLights[2] > -1){midButtonsLights[2] = -1;}else{midButtonsLights[2] = 32;}}
    if(midButtons[3] == 1 && midButtonsEcho[3] == 0){if(midButtonsLights[3] > -1){midButtonsLights[3] = -1;}else{midButtonsLights[3] = 32;}}
    if(midButtons[4] == 1 && midButtonsEcho[4] == 0){if(midButtonsLights[4] > -1){midButtonsLights[4] = -1;}else{midButtonsLights[4] = 32;}}
    if(botButtons[0] == 1 && botButtonsEcho[0] == 0){if(botButtonsLights[0] > -1){botButtonsLights[0] = -1;}else{botButtonsLights[0] = 32;}}
    if(botButtons[1] == 1 && botButtonsEcho[1] == 0){if(botButtonsLights[1] > -1){botButtonsLights[1] = -1;}else{botButtonsLights[1] = 32;}}
    if(botButtons[2] == 1 && botButtonsEcho[2] == 0){if(botButtonsLights[2] > -1){botButtonsLights[2] = -1;}else{botButtonsLights[2] = 32;}}
    if(botButtons[3] == 1 && botButtonsEcho[3] == 0){if(botButtonsLights[3] > -1){botButtonsLights[3] = -1;}else{botButtonsLights[3] = 32;}}
    if(botButtons[4] == 1 && botButtonsEcho[4] == 0){if(botButtonsLights[4] > -1){botButtonsLights[4] = -1;}else{botButtonsLights[4] = 32;}}
  }
}

void buttonLightLoadState(int topButtonsLoad[5], int midButtonsLoad[5], int botButtonsLoad[5]){
  topButtonsLights[0] = topButtonsLoad[0];
  topButtonsLights[1] = topButtonsLoad[1];
  topButtonsLights[2] = topButtonsLoad[2];
  topButtonsLights[3] = topButtonsLoad[3];
  topButtonsLights[4] = topButtonsLoad[4];
  midButtonsLights[0] = midButtonsLoad[0];
  midButtonsLights[1] = midButtonsLoad[1];
  midButtonsLights[2] = midButtonsLoad[2];
  midButtonsLights[3] = midButtonsLoad[3];
  midButtonsLights[4] = midButtonsLoad[4];
  botButtonsLights[0] = botButtonsLoad[0];
  botButtonsLights[1] = botButtonsLoad[1];
  botButtonsLights[2] = botButtonsLoad[2];
  botButtonsLights[3] = botButtonsLoad[3];
  botButtonsLights[4] = botButtonsLoad[4];
}

void binaryLightSet(){
    int ledSelect = -1;
  if(topButtonsLights[0] > 0){
      ledSelect = ledSelect + 4;
  }
  if(midButtonsLights[0] > 0){
      ledSelect = ledSelect + 2;
  }
  if(botButtonsLights[0] > 0){
      ledSelect = ledSelect + 1;
  }
  //load buttons on led select change
  if(ledSelect != ledSelectEcho && ledSelect > -1){
      if(bitRead(byte(redLedWidths[ledSelect] + 1),0) == 1){topButtonsLights[1] = 32;}else{topButtonsLights[1] = -1;}
    if(bitRead(byte(redLedWidths[ledSelect] + 1),1) == 1){topButtonsLights[2] = 32;}else{topButtonsLights[2] = -1;}
    if(bitRead(byte(redLedWidths[ledSelect] + 1),2) == 1){topButtonsLights[3] = 32;}else{topButtonsLights[3] = -1;}
    if(bitRead(byte(redLedWidths[ledSelect] + 1),3) == 1){topButtonsLights[4] = 32;}else{topButtonsLights[4] = -1;}
    if(bitRead(byte(greenLedWidths[ledSelect] + 1),0) == 1){midButtonsLights[1] = 32;}else{midButtonsLights[1] = -1;}
    if(bitRead(byte(greenLedWidths[ledSelect] + 1),1) == 1){midButtonsLights[2] = 32;}else{midButtonsLights[2] = -1;}
    if(bitRead(byte(greenLedWidths[ledSelect] + 1),2) == 1){midButtonsLights[3] = 32;}else{midButtonsLights[3] = -1;}
    if(bitRead(byte(greenLedWidths[ledSelect] + 1),3) == 1){midButtonsLights[4] = 32;}else{midButtonsLights[4] = -1;}
    if(bitRead(byte(blueLedWidths[ledSelect] + 1),0) == 1){botButtonsLights[1] = 32;}else{botButtonsLights[1] = -1;}
    if(bitRead(byte(blueLedWidths[ledSelect] + 1),1) == 1){botButtonsLights[2] = 32;}else{botButtonsLights[2] = -1;}
    if(bitRead(byte(blueLedWidths[ledSelect] + 1),2) == 1){botButtonsLights[3] = 32;}else{botButtonsLights[3] = -1;}
    if(bitRead(byte(blueLedWidths[ledSelect] + 1),3) == 1){botButtonsLights[4] = 32;}else{botButtonsLights[4] = -1;}
  }
  ledSelectEcho = ledSelect;
  //read button input into brightness value
  if(ledSelect > -1){
      int redWidth = 0;
    int greenWidth = 0;
    int blueWidth = 0;
    if(topButtonsLights[1] > 0){redWidth += 1;}
    if(topButtonsLights[2] > 0){redWidth += 2;}
    if(topButtonsLights[3] > 0){redWidth += 4;}
    if(topButtonsLights[4] > 0){redWidth += 8;}
    if(midButtonsLights[1] > 0){greenWidth += 1;}
    if(midButtonsLights[2] > 0){greenWidth += 2;}
    if(midButtonsLights[3] > 0){greenWidth += 4;}
    if(midButtonsLights[4] > 0){greenWidth += 8;}
    if(botButtonsLights[1] > 0){blueWidth += 1;}
    if(botButtonsLights[2] > 0){blueWidth += 2;}
    if(botButtonsLights[3] > 0){blueWidth += 4;}
    if(botButtonsLights[4] > 0){blueWidth += 8;}
    redLedWidths[ledSelect] = redWidth -1;
    greenLedWidths[ledSelect] = greenWidth -1;
    blueLedWidths[ledSelect] = blueWidth -1;
  }
}

void buttonLightNormalAction(){
    if(topButtons[0] == 1){topButtonsLights[0] = -1;}else{topButtonsLights[0] = 32;}
  if(topButtons[1] == 1){topButtonsLights[1] = -1;}else{topButtonsLights[1] = 32;}
  if(topButtons[2] == 1){topButtonsLights[2] = -1;}else{topButtonsLights[2] = 32;}
  if(topButtons[3] == 1){topButtonsLights[3] = -1;}else{topButtonsLights[3] = 32;}
  if(topButtons[4] == 1){topButtonsLights[4] = -1;}else{topButtonsLights[4] = 32;}
  if(midButtons[0] == 1){midButtonsLights[0] = -1;}else{midButtonsLights[0] = 32;}
  if(midButtons[1] == 1){midButtonsLights[1] = -1;}else{midButtonsLights[1] = 32;}
  if(midButtons[2] == 1){midButtonsLights[2] = -1;}else{midButtonsLights[2] = 32;}
  if(midButtons[3] == 1){midButtonsLights[3] = -1;}else{midButtonsLights[3] = 32;}
  if(midButtons[4] == 1){midButtonsLights[4] = -1;}else{midButtonsLights[4] = 32;}
  if(botButtons[0] == 1){botButtonsLights[0] = -1;}else{botButtonsLights[0] = 32;}
  if(botButtons[1] == 1){botButtonsLights[1] = -1;}else{botButtonsLights[1] = 32;}
  if(botButtons[2] == 1){botButtonsLights[2] = -1;}else{botButtonsLights[2] = 32;}
  if(botButtons[3] == 1){botButtonsLights[3] = -1;}else{botButtonsLights[3] = 32;}
  if(botButtons[4] == 1){botButtonsLights[4] = -1;}else{botButtonsLights[4] = 32;}
}
void buttonLightReverseAction(){
    if(topButtons[0] == 0){topButtonsLights[0] = -1;}else{topButtonsLights[0] = 32;}
  if(topButtons[1] == 0){topButtonsLights[1] = -1;}else{topButtonsLights[1] = 32;}
  if(topButtons[2] == 0){topButtonsLights[2] = -1;}else{topButtonsLights[2] = 32;}
  if(topButtons[3] == 0){topButtonsLights[3] = -1;}else{topButtonsLights[3] = 32;}
  if(topButtons[4] == 0){topButtonsLights[4] = -1;}else{topButtonsLights[4] = 32;}
  if(midButtons[0] == 0){midButtonsLights[0] = -1;}else{midButtonsLights[0] = 32;}
  if(midButtons[1] == 0){midButtonsLights[1] = -1;}else{midButtonsLights[1] = 32;}
  if(midButtons[2] == 0){midButtonsLights[2] = -1;}else{midButtonsLights[2] = 32;}
  if(midButtons[3] == 0){midButtonsLights[3] = -1;}else{midButtonsLights[3] = 32;}
  if(midButtons[4] == 0){midButtonsLights[4] = -1;}else{midButtonsLights[4] = 32;}
  if(botButtons[0] == 0){botButtonsLights[0] = -1;}else{botButtonsLights[0] = 32;}
  if(botButtons[1] == 0){botButtonsLights[1] = -1;}else{botButtonsLights[1] = 32;}
  if(botButtons[2] == 0){botButtonsLights[2] = -1;}else{botButtonsLights[2] = 32;}
  if(botButtons[3] == 0){botButtonsLights[3] = -1;}else{botButtonsLights[3] = 32;}
  if(botButtons[4] == 0){botButtonsLights[4] = -1;}else{botButtonsLights[4] = 32;}
}
void directConnect(){
      //if(digitalRead(switch1) == HIGH){redLedWidths[0] = 32;}else{redLedWidths[0] = -1;}
  if(topButtons[0] == 1){redLedWidths[1] = 32;}else{redLedWidths[1] = -1;}
  if(topButtons[1] == 1){redLedWidths[2] = 32;}else{redLedWidths[2] = -1;}
  if(topButtons[2] == 1){redLedWidths[3] = 32;}else{redLedWidths[3] = -1;}
  if(topButtons[3] == 1){redLedWidths[4] = 32;}else{redLedWidths[4] = -1;}
  if(topButtons[4] == 1){redLedWidths[5] = 32;}else{redLedWidths[5] = -1;}
//if(digitalRead(switch1) == HIGH){redLedWidths[6] = 32;}else{redLedWidths[6] = -1;}
  //if(digitalRead(switch2) == HIGH){redLedWidths[0] = 32;}else{redLedWidths[0] = -1;}
  if(midButtons[0] == 1){greenLedWidths[1] = 32;}else{greenLedWidths[1] = -1;}
  if(midButtons[1] == 1){greenLedWidths[2] = 32;}else{greenLedWidths[2] = -1;}
  if(midButtons[2] == 1){greenLedWidths[3] = 32;}else{greenLedWidths[3] = -1;}
  if(midButtons[3] == 1){greenLedWidths[4] = 32;}else{greenLedWidths[4] = -1;}
  if(midButtons[4] == 1){greenLedWidths[5] = 32;}else{greenLedWidths[5] = -1;}
  //if(digitalRead(switch2) == HIGH){redLedWidths[6] = 32;}else{redLedWidths[6] = -1;}
  //if(digitalRead(switch3) == HIGH){redLedWidths[0] = 32;}else{redLedWidths[0] = -1;}
  if(botButtons[0] == 1){blueLedWidths[1] = 32;}else{blueLedWidths[1] = -1;}
  if(botButtons[1] == 1){blueLedWidths[2] = 32;}else{blueLedWidths[2] = -1;}
  if(botButtons[2] == 1){blueLedWidths[3] = 32;}else{blueLedWidths[3] = -1;}
  if(botButtons[3] == 1){blueLedWidths[4] = 32;}else{blueLedWidths[4] = -1;}
  if(botButtons[4] == 1){blueLedWidths[5] = 32;}else{blueLedWidths[5] = -1;}
  //if(digitalRead(switch3) == HIGH){redLedWidths[6] = 32;}else{redLedWidths[6] = -1;}
  //setLights();
}
void stepConnect(){
    if(inputLoopCount == 0){
      if(topButtons[0] == 1 && topButtonsEcho[0] == 0){redLedWidths[0] = redLedWidths[0] + 4; if(redLedWidths[0] > 35){redLedWidths[0] = -1;}}
    if(topButtons[0] == 1 && topButtonsEcho[0] == 0){redLedWidths[1] = redLedWidths[1] + 4; if(redLedWidths[1] > 35){redLedWidths[1] = -1;}}
    if(topButtons[1] == 1 && topButtonsEcho[1] == 0){redLedWidths[2] = redLedWidths[2] + 4; if(redLedWidths[2] > 35){redLedWidths[2] = -1;}}
    if(topButtons[2] == 1 && topButtonsEcho[2] == 0){redLedWidths[3] = redLedWidths[3] + 4; if(redLedWidths[3] > 35){redLedWidths[3] = -1;}}
    if(topButtons[3] == 1 && topButtonsEcho[3] == 0){redLedWidths[4] = redLedWidths[4] + 4; if(redLedWidths[4] > 35){redLedWidths[4] = -1;}}
    if(topButtons[4] == 1 && topButtonsEcho[4] == 0){redLedWidths[5] = redLedWidths[5] + 4; if(redLedWidths[5] > 35){redLedWidths[5] = -1;}}
    if(topButtons[4] == 1 && topButtonsEcho[4] == 0){redLedWidths[6] = redLedWidths[6] + 4; if(redLedWidths[6] > 35){redLedWidths[6] = -1;}}
    if(midButtons[0] == 1 && midButtonsEcho[0] == 0){greenLedWidths[0] = greenLedWidths[0] + 4; if(greenLedWidths[0] > 35){greenLedWidths[0] = -1;}}
    if(midButtons[0] == 1 && midButtonsEcho[0] == 0){greenLedWidths[1] = greenLedWidths[1] + 4; if(greenLedWidths[1] > 35){greenLedWidths[1] = -1;}}
    if(midButtons[1] == 1 && midButtonsEcho[1] == 0){greenLedWidths[2] = greenLedWidths[2] + 4; if(greenLedWidths[2] > 35){greenLedWidths[2] = -1;}}
    if(midButtons[2] == 1 && midButtonsEcho[2] == 0){greenLedWidths[3] = greenLedWidths[3] + 4; if(greenLedWidths[3] > 35){greenLedWidths[3] = -1;}}
    if(midButtons[3] == 1 && midButtonsEcho[3] == 0){greenLedWidths[4] = greenLedWidths[4] + 4; if(greenLedWidths[4] > 35){greenLedWidths[4] = -1;}}
    if(midButtons[4] == 1 && midButtonsEcho[4] == 0){greenLedWidths[5] = greenLedWidths[5] + 4; if(greenLedWidths[5] > 35){greenLedWidths[5] = -1;}}
    if(midButtons[4] == 1 && midButtonsEcho[4] == 0){greenLedWidths[6] = greenLedWidths[6] + 4; if(greenLedWidths[6] > 35){greenLedWidths[6] = -1;}}
    if(botButtons[0] == 1 && botButtonsEcho[0] == 0){blueLedWidths[0] = blueLedWidths[0] + 4; if(blueLedWidths[0] > 35){blueLedWidths[0] = -1;}}
    if(botButtons[0] == 1 && botButtonsEcho[0] == 0){blueLedWidths[1] = blueLedWidths[1] + 4; if(blueLedWidths[1] > 35){blueLedWidths[1] = -1;}}
    if(botButtons[1] == 1 && botButtonsEcho[1] == 0){blueLedWidths[2] = blueLedWidths[2] + 4; if(blueLedWidths[2] > 35){blueLedWidths[2] = -1;}}
    if(botButtons[2] == 1 && botButtonsEcho[2] == 0){blueLedWidths[3] = blueLedWidths[3] + 4; if(blueLedWidths[3] > 35){blueLedWidths[3] = -1;}}
    if(botButtons[3] == 1 && botButtonsEcho[3] == 0){blueLedWidths[4] = blueLedWidths[4] + 4; if(blueLedWidths[4] > 35){blueLedWidths[4] = -1;}}
    if(botButtons[4] == 1 && botButtonsEcho[4] == 0){blueLedWidths[5] = blueLedWidths[5] + 4; if(blueLedWidths[5] > 35){blueLedWidths[5] = -1;}}
    if(botButtons[4] == 1 && botButtonsEcho[4] == 0){blueLedWidths[6] = blueLedWidths[6] + 4; if(blueLedWidths[6] > 35){blueLedWidths[6] = -1;}}
  }
  //setLights();
}
void waveSin(){
      if(pulseWidthPos == 31){
        if( wave > waveLength){
          wave = 0;
    }
    redLedWidths[0] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 0/44))));
    redLedWidths[1] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 4/44))));
    redLedWidths[2] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 8/44))));
    redLedWidths[3] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 12/44))));
    redLedWidths[4] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 16/44))));
    redLedWidths[5] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 20/44))));
    redLedWidths[6] = 25*((sin(3.14*2*wave/waveLength + (3.14 * 2* 24/44))));
    greenLedWidths[0] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 16/44))));
    greenLedWidths[1] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 20/44))));
    greenLedWidths[2] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 24/44))));
    greenLedWidths[3] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 28/44))));
    greenLedWidths[4] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 32/44))));
    greenLedWidths[5] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 36/44))));
    greenLedWidths[6] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 40/44))));
    blueLedWidths[0] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 32/44))));
    blueLedWidths[1] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 36/44))));
    blueLedWidths[2] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 40/44))));
    blueLedWidths[3] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 0/44))));
    blueLedWidths[4] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 4/44))));
    blueLedWidths[5] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 8/44))));
    blueLedWidths[6] = 25*(0.25+(sin(3.14*2*wave/waveLength + (3.14 * 2* 12/44))));
    wave++;
  }
}

void lightPulse(){
  int pulseLength = 100;
  //detecting button press and seting light to be pulsed
  if(topButtons[0] == 1 && topButtonsEcho[0] == 0){redLightPulse[0] = pulseLength; redLightPulse[2] = pulseLength;}
  if(topButtons[1] == 1 && topButtonsEcho[1] == 0){redLightPulse[1] = pulseLength; redLightPulse[3] = pulseLength;}
  if(topButtons[2] == 1 && topButtonsEcho[2] == 0){redLightPulse[2] = pulseLength; redLightPulse[4] = pulseLength;}
  if(topButtons[3] == 1 && topButtonsEcho[3] == 0){redLightPulse[3] = pulseLength; redLightPulse[5] = pulseLength;}
  if(topButtons[4] == 1 && topButtonsEcho[4] == 0){redLightPulse[4] = pulseLength; redLightPulse[6] = pulseLength;}
  if(midButtons[0] == 1 && midButtonsEcho[0] == 0){greenLightPulse[0] = pulseLength; greenLightPulse[2] = pulseLength;}
  if(midButtons[1] == 1 && midButtonsEcho[1] == 0){greenLightPulse[1] = pulseLength; greenLightPulse[3] = pulseLength;}
  if(midButtons[2] == 1 && midButtonsEcho[2] == 0){greenLightPulse[2] = pulseLength; greenLightPulse[4] = pulseLength;}
  if(midButtons[3] == 1 && midButtonsEcho[3] == 0){greenLightPulse[3] = pulseLength; greenLightPulse[5] = pulseLength;}
  if(midButtons[4] == 1 && midButtonsEcho[4] == 0){greenLightPulse[4] = pulseLength; greenLightPulse[6] = pulseLength;}
  if(botButtons[0] == 1 && botButtonsEcho[0] == 0){blueLightPulse[0] = pulseLength; blueLightPulse[2] = pulseLength;}
  if(botButtons[1] == 1 && botButtonsEcho[1] == 0){blueLightPulse[1] = pulseLength; blueLightPulse[3] = pulseLength;}
  if(botButtons[2] == 1 && botButtonsEcho[2] == 0){blueLightPulse[2] = pulseLength; blueLightPulse[4] = pulseLength;}
  if(botButtons[3] == 1 && botButtonsEcho[3] == 0){blueLightPulse[3] = pulseLength; blueLightPulse[5] = pulseLength;}
  if(botButtons[4] == 1 && botButtonsEcho[4] == 0){blueLightPulse[4] = pulseLength; blueLightPulse[6] = pulseLength;}
  //pullsing light
  if(redLightPulse[0] >= 0){redLedWidths[0] = 25 * ((sin(3.14159 * redLightPulse[0]/pulseLength))); redLightPulse[0]--;}else{redLedWidths[0] = -1;}
  if(redLightPulse[1] >= 0){redLedWidths[1] = 25 * ((sin(3.14159 * redLightPulse[1]/pulseLength))); redLightPulse[1]--;}else{redLedWidths[1] = -1;}
  if(redLightPulse[2] >= 0){redLedWidths[2] = 25 * ((sin(3.14159 * redLightPulse[2]/pulseLength))); redLightPulse[2]--;}else{redLedWidths[2] = -1;}
  if(redLightPulse[3] >= 0){redLedWidths[3] = 25 * ((sin(3.14159 * redLightPulse[3]/pulseLength))); redLightPulse[3]--;}else{redLedWidths[3] = -1;}
  if(redLightPulse[4] >= 0){redLedWidths[4] = 25 * ((sin(3.14159 * redLightPulse[4]/pulseLength))); redLightPulse[4]--;}else{redLedWidths[4] = -1;}
  if(redLightPulse[5] >= 0){redLedWidths[5] = 25 * ((sin(3.14159 * redLightPulse[5]/pulseLength))); redLightPulse[5]--;}else{redLedWidths[5] = -1;}
  if(redLightPulse[6] >= 0){redLedWidths[6] = 25 * ((sin(3.14159 * redLightPulse[6]/pulseLength))); redLightPulse[6]--;}else{redLedWidths[6] = -1;}
  if(greenLightPulse[0] >= 0){greenLedWidths[0] = 25 * ((sin(3.14159 * greenLightPulse[0]/pulseLength))); greenLightPulse[0]--;}else{greenLedWidths[0] = -1;}
  if(greenLightPulse[1] >= 0){greenLedWidths[1] = 25 * ((sin(3.14159 * greenLightPulse[1]/pulseLength))); greenLightPulse[1]--;}else{greenLedWidths[1] = -1;}
  if(greenLightPulse[2] >= 0){greenLedWidths[2] = 25 * ((sin(3.14159 * greenLightPulse[2]/pulseLength))); greenLightPulse[2]--;}else{greenLedWidths[2] = -1;}
  if(greenLightPulse[3] >= 0){greenLedWidths[3] = 25 * ((sin(3.14159 * greenLightPulse[3]/pulseLength))); greenLightPulse[3]--;}else{greenLedWidths[3] = -1;}
  if(greenLightPulse[4] >= 0){greenLedWidths[4] = 25 * ((sin(3.14159 * greenLightPulse[4]/pulseLength))); greenLightPulse[4]--;}else{greenLedWidths[4] = -1;}
  if(greenLightPulse[5] >= 0){greenLedWidths[5] = 25 * ((sin(3.14159 * greenLightPulse[5]/pulseLength))); greenLightPulse[5]--;}else{greenLedWidths[5] = -1;}
  if(greenLightPulse[6] >= 0){greenLedWidths[6] = 25 * ((sin(3.14159 * greenLightPulse[6]/pulseLength))); greenLightPulse[6]--;}else{greenLedWidths[6] = -1;}
  if(blueLightPulse[0] >= 0){blueLedWidths[0] = 25 * ((sin(3.14159 * blueLightPulse[0]/pulseLength))); blueLightPulse[0]--;}else{blueLedWidths[0] = -1;}
  if(blueLightPulse[1] >= 0){blueLedWidths[1] = 25 * ((sin(3.14159 * blueLightPulse[1]/pulseLength))); blueLightPulse[1]--;}else{blueLedWidths[1] = -1;}
  if(blueLightPulse[2] >= 0){blueLedWidths[2] = 25 * ((sin(3.14159 * blueLightPulse[2]/pulseLength))); blueLightPulse[2]--;}else{blueLedWidths[2] = -1;}
  if(blueLightPulse[3] >= 0){blueLedWidths[3] = 25 * ((sin(3.14159 * blueLightPulse[3]/pulseLength))); blueLightPulse[3]--;}else{blueLedWidths[3] = -1;}
  if(blueLightPulse[4] >= 0){blueLedWidths[4] = 25 * ((sin(3.14159 * blueLightPulse[4]/pulseLength))); blueLightPulse[4]--;}else{blueLedWidths[4] = -1;}
  if(blueLightPulse[5] >= 0){blueLedWidths[5] = 25 * ((sin(3.14159 * blueLightPulse[5]/pulseLength))); blueLightPulse[5]--;}else{blueLedWidths[5] = -1;}
  if(blueLightPulse[6] >= 0){blueLedWidths[6] = 25 * ((sin(3.14159 * blueLightPulse[6]/pulseLength))); blueLightPulse[6]--;}else{blueLedWidths[6] = -1;}
}
void waveTri(){
    for(int wave = 0; wave < 64; wave++){
        redLedWidths[0] = 31*(abs(wave/100 - 0.5));
    redLedWidths[1] = 31*(abs(wave/100 - 0.5))*2;
    redLedWidths[2] = 31*(abs(wave/100 - 0.5))*2;
    redLedWidths[3] = 31*(abs(wave/100 - 0.5))*2;
    redLedWidths[4] = 31*(abs(wave/100 - 0.5))*2;
    redLedWidths[5] = 31*(abs(wave/100 - 0.5))*2;
    redLedWidths[6] = 31*(abs(wave/100 - 0.5))*2;
    //frameLoop();
  }
}
//*******************************************************************************************************************
//                                 Output
//*******************************************************************************************************************
void outputUpdate() {
      for(int pos = 7; pos >= 0; pos--){
        if(pulseWidthPos <= redLedWidths[pos]){
          digitalWrite(outRegRed, HIGH);
    }
    else{
          digitalWrite(outRegRed, LOW);
    }
    if(pulseWidthPos <= greenLedWidths[pos]){
          digitalWrite(outRegGreen, HIGH);
    }
    else{
          digitalWrite(outRegGreen, LOW);
    }
    if(pulseWidthPos <= blueLedWidths[pos]){
          digitalWrite(outRegBlue, HIGH);
    }
    else{
          digitalWrite(outRegBlue, LOW);
    }
    if(pulseWidthPos <= buttonWidths1[pos]){
          digitalWrite(outRegButtons1, HIGH);
    }
    else{
          digitalWrite(outRegButtons1, LOW);
    }
    if(pulseWidthPos <= buttonWidths2[pos]){
          digitalWrite(outRegButtons2, HIGH);
    }
    else{
          digitalWrite(outRegButtons2, LOW);
    }
    digitalWrite(outClk, HIGH);
    //delayMicroseconds(20);
    digitalWrite(outClk, LOW);
  }
}
void frameLoop() {
    if(pulseWidthPos >= 32){
      pulseWidthPos = 0;
  }
  outputUpdate();
  digitalWrite(outPolSig, HIGH);
  digitalWrite(outPolSig, LOW);
  pulseWidthPos++;
}
void setLights() {
    outputUpdate();
  digitalWrite(outPolSig, HIGH);
  digitalWrite(outPolSig, LOW);
}
void buttonUpdate(){
    buttonWidths1[0] = topButtonsLights[0];
  buttonWidths1[1] = topButtonsLights[1];
  buttonWidths1[2] = topButtonsLights[2];
  buttonWidths1[3] = topButtonsLights[3];
  buttonWidths1[4] = topButtonsLights[4];
  buttonWidths1[5] = midButtonsLights[0];
  buttonWidths1[6] = midButtonsLights[1];
  buttonWidths1[7] = midButtonsLights[2];
  buttonWidths2[0] = midButtonsLights[3];
  buttonWidths2[1] = midButtonsLights[4];
  buttonWidths2[2] = botButtonsLights[0];
  buttonWidths2[3] = botButtonsLights[1];
  buttonWidths2[4] = botButtonsLights[2];
  buttonWidths2[5] = botButtonsLights[3];
  buttonWidths2[6] = botButtonsLights[4];
}
//*******************************************************************************************************************
//                                 Input
//*******************************************************************************************************************
void readIntoBuffer1(int pos){
    if(digitalRead(inReg1) == HIGH) {
      inputReg1[pos] = 1;
  }
  else{
        inputReg1[pos] = 0;
  }
}
void readIntoBuffer2(int pos){
    if(digitalRead(inReg2) == HIGH) {
      inputReg2[pos] = 1;
  }
  else{
        inputReg2[pos] = 0;
  }
}
void clockPulse (){
    //delayMicroseconds(clockPulseLength);
  digitalWrite(inClk, LOW);
  //delayMicroseconds(clockPulseLength * 2);
  digitalWrite(inClk, HIGH);
  //delayMicroseconds(clockPulseLength);
}
void updateInputData(){
    //Red Buttons
  topButtonsEcho[0] = topButtons[0];
  topButtonsEcho[1] = topButtons[1];
  topButtonsEcho[2] = topButtons[2];
  topButtonsEcho[3] = topButtons[3];
  topButtonsEcho[4] = topButtons[4];
  topButtons[0] = inputReg1[0];
  topButtons[1] = inputReg1[1];
  topButtons[2] = inputReg1[2];
  topButtons[3] = inputReg1[3];
  topButtons[4] = inputReg1[4];
  //Green Buttons
  midButtonsEcho[0] = midButtons[0];
  midButtonsEcho[1] = midButtons[1];
  midButtonsEcho[2] = midButtons[2];
  midButtonsEcho[3] = midButtons[3];
  midButtonsEcho[4] = midButtons[4];
  midButtons[0] = inputReg1[5];
  midButtons[1] = inputReg1[6];
  midButtons[2] = inputReg1[7];
  midButtons[3] = inputReg2[0];
  midButtons[4] = inputReg2[1];
  //Black Buttons
  botButtonsEcho[0] =  botButtons[0];
  botButtonsEcho[1] =  botButtons[1];
  botButtonsEcho[2] =  botButtons[2];
  botButtonsEcho[3] =  botButtons[3];
  botButtonsEcho[4] =  botButtons[4];
  botButtons[0] = inputReg2[2];
  botButtons[1] = inputReg2[3];
  botButtons[2] = inputReg2[4];
  botButtons[3] = inputReg2[5];
  botButtons[4] = inputReg2[6];
}
void inputLoop (int pollLoop) {
    readIntoBuffer1(pollLoop);
  readIntoBuffer2(pollLoop);
  clockPulse();
  if(pollLoop == 0) {
      digitalWrite(inClkSupress, HIGH);
    digitalWrite(inPolSig, LOW);
    updateInputData();
    digitalWrite(inPolSig, HIGH);
    digitalWrite(inClkSupress, LOW);
  }
}
void inputRun() {
    if(inputLoopCount < 0){
      inputLoopCount = 7;
  }
  inputLoop(inputLoopCount);
  inputLoopCount--;
}

//*******************************************************************************************************************
//                                 Interrupt
//*******************************************************************************************************************

ISR(TIMER1_COMPA_vect) // 16 bit timer 1 compare 1A match
{
    frameLoop();
}
//*******************************************************************************************************************
//                                 Main Loop
//*******************************************************************************************************************
void loop() {
    functionSelect();
  buttonUpdate();
  inputRun();
}