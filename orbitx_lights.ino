#define DEBUG
#define DEVELOP

#include <FastLED.h>
#include "defines.h"

//It create the working arrray (only one for all the 3 strips)
//strip leds in array are LEFT, RIGHT, OCTAGON in sequence
CRGB leds[NUM_LEDS];

#include <TimerOne.h>
uint8_t everyXmsCounter, every1msCounter, every10msCounter, every100msCounter, every1sCounter, every1mCounter, every1hCounter = 0;


//Seria config
#define SERIAL_BUF_LEN 64
char rxBuf[SERIAL_BUF_LEN];
uint8_t rxPos = 0;

//buffer 
#define CMD_QUEUE_SIZE 20
Command cmdQueue[CMD_QUEUE_SIZE];
uint8_t cmdCount = 0;
uint8_t cmdIndex = 0;

//Custom zones
LedRange customZones[10];
bool customZoneDefined[10];




void setup() {

  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(callback);

  FastLED.addLeds<WS2812B, PIN_STRIP_LEFT,  GRB>(leds + LFT_FIRST, LFT_LENGHT);
  FastLED.addLeds<WS2812B, PIN_STROP_RIGHT, GRB>(leds + RGT_FIRST, RGT_LENGHT);
  FastLED.addLeds<WS2812B, PIN_STRIP_OCT,   GRB>(leds + OCT_FIRST, OCT_LENGHT);

  pinMode(LED_BUILTIN,      OUTPUT);
  pinMode(PIN_WHITE_LIGHT,  OUTPUT); 

  pinMode(INPUT_DOOR   , INPUT_PULLUP);
  pinMode(INPUT_SEAT_FL, INPUT_PULLUP);
  pinMode(INPUT_SEAT_FR, INPUT_PULLUP);
  pinMode(INPUT_SEAT_RL, INPUT_PULLUP);
  pinMode(INPUT_SEAT_RR, INPUT_PULLUP);

  analogWrite(PIN_WHITE_LIGHT, LIGHT_LOW);


  //Init custom zones
  for (uint8_t i = 0; i < 10; i++) {
    customZoneDefined[i] = false;
  }
}



void loop() {

  inputManager();
  serialManager();
  executeQueue();
  animationManager();
  showAnimation();

}





void serialManager() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\r' || c == '\n') {
      if (rxPos > 0) {
        rxBuf[rxPos] = '\0';
        processLine(rxBuf);
        rxPos = 0;
      }
    }
    else {
      if (rxPos < SERIAL_BUF_LEN - 1) {
        rxBuf[rxPos++] = c;
      }
      else {
        // overflow → scarta linea
        Serial.println("Overflow");
        rxPos = 0;
      }
    }
  }
}




void processLine(const char* line) {

  // ---- special commands ----
  if (!strcmp(line, "CMDSTART")) {
    cmdIndex = 0
    //executeQueue();
    return;
  }

  if (!strcmp(line, "CMDSTOP")) {
    cmdIndex = cmdCount;
    //stopExecution();
    return;
  }

  if (!strcmp(line, "RESET")) {
    cmdCount = 0; 
    cmdIndex = 0;
    //resetSystem();
    return;
  }

  if (!strcmp(line, "CMDON")) {
    //powerOn();
    return;
  }

  if (!strcmp(line, "CMDOFF")) {
    //powerOff();
    return;
  }

  // ---- custom zone commands ----
  if (!strncmp(line, "SETC", 4)) {
    handleSetZone(line);
    return;
  }

  if (!strncmp(line, "GETC", 4)) {
    handleGetZone(line);
    return;
  }

  // ---- bufferizable commands ----
  if (cmdCount >= CMD_QUEUE_SIZE) {
    // coda piena → scarta
    Serial.println("Buffer full");
    return;
  }

Command cmd;
  if (parseCommand(line, cmd)) {
    cmdQueue[cmdCount++] = cmd;
  }
}



bool parseCommand(const char* s, Command& cmd) {

  // FIX
  if (!strncmp(s, "FIX", 3)) {
    cmd.type   = CMD_FIX;
    cmd.target = decodeTarget(s + 3);
    cmd.h = hexByte(s + 6);
    cmd.s = hexByte(s + 8);
    cmd.v = hexByte(s + 10);
    return true;
  }
  // DIM
  if (!strncmp(s, "DIM", 3)) {
    cmd.type      = CMD_DIM;
    cmd.target    = decodeTarget(s + 3);
    cmd.direction = decodeDir(s + 6);
    cmd.ms_on     = decWord(s + 8);
    return true;
  }
  // SHD
  if (!strncmp(s, "SHD", 3)) {
    cmd.type   = CMD_SHD;
    cmd.target = decodeTarget(s + 3);
    cmd.h  = hexByte(s + 6);
    cmd.s  = hexByte(s + 8);
    cmd.v  = hexByte(s + 10);
    cmd.h2 = hexByte(s + 12);
    cmd.s2 = hexByte(s + 14);
    cmd.v2 = hexByte(s + 16);
    return true;
  }
  // BLK
  if (!strncmp(s, "BLK", 3)) {
    cmd.type   = CMD_BLK;
    cmd.target = decodeTarget(s + 3);
    cmd.ms_on  = decWord(s + 6);
    cmd.ms_off = decWord(s + 10);
    return true;
  }
  // ANM
  if (!strncmp(s, "ANM", 3)) {
    cmd.type        = CMD_ANM;
    cmd.target      = decodeTarget(s + 3);
    cmd.anim_number = decByte(s + 6);
    cmd.anim_speed  = decByte(s + 8);
    cmd.h = hexByte(s + 10);
    cmd.s = hexByte(s + 12);
    cmd.v = hexByte(s + 14);
    return true;
  }
  // WIT
  if (!strncmp(s, "WIT", 3)) {
    cmd.type = CMD_WIT;
    cmd.sec  = decWord(s + 3);
    return true;
  }
  // REP
  if (!strncmp(s, "REP", 3)) {
    cmd.type   = CMD_REP;
    cmd.repeat = decWord(s + 3);
    return true;
  }
  Serial.print("Unknown command: "); Serial.println(s);
  return false;
}



Target decodeTarget(const char* s) {
  
  if (!strncmp(s, "LFT", 3)) return T_LFT;
  if (!strncmp(s, "RGT", 3)) return T_RGT;
  if (!strncmp(s, "OCT", 3)) return T_OCT;
  if (!strncmp(s, "TOP", 3)) return T_TOP;
  if (!strncmp(s, "ALL", 3)) return T_ALL;
  if (!strncmp(s, "PFL", 3)) return T_PFL;
  if (!strncmp(s, "PFR", 3)) return T_PFR;
  if (!strncmp(s, "PRL", 3)) return T_PRL;
  if (!strncmp(s, "PRR", 3)) return T_PRR;
  if (!strncmp(s, "FNT", 3)) return T_FNT;
  if (!strncmp(s, "RAR", 3)) return T_RAR;

  // Custom zones C01–C10
  if (!strncmp(s, "C01", 3)) return T_C01;
  if (!strncmp(s, "C02", 3)) return T_C02;
  if (!strncmp(s, "C03", 3)) return T_C03;
  if (!strncmp(s, "C04", 3)) return T_C04;
  if (!strncmp(s, "C05", 3)) return T_C05;
  if (!strncmp(s, "C06", 3)) return T_C06;
  if (!strncmp(s, "C07", 3)) return T_C07;
  if (!strncmp(s, "C08", 3)) return T_C08;
  if (!strncmp(s, "C09", 3)) return T_C09;
  if (!strncmp(s, "C10", 3)) return T_C10;

  // Default
  Serial.print("Unknown target: "); Serial.println(s);
  return T_ERR; 
}



Dir decodeDir(const char* s) {
  // s punta subito dopo il target, es. "UP003000" o "DW003000"
  if (!strncmp(s, "UP", 2)) return DIR_UP;
  if (!strncmp(s, "DW", 2)) return DIR_DW;

  // Default in caso di errore
  Serial.print("Unknown direction: "); Serial.println(s);
  return DIR_ERR;
}




void handleSetZone(const char* cmd) {
  uint8_t idx = (cmd[4] - '0') * 10 + (cmd[5] - '0');
  if (idx < 1 || idx > 10) return;

  uint16_t first = atoi(cmd + 6);
  uint16_t last  = atoi(cmd + 9);

  if (first >= NUM_LEDS || last >= NUM_LEDS || first > last) return;

  customZones[idx - 1].first = first;
  customZones[idx - 1].last  = last;
  customZoneDefined[idx - 1] = true;
}



void handleGetZone(const char* cmd) {
  uint8_t idx = (cmd[4] - '0') * 10 + (cmd[5] - '0');
  if (idx < 1 || idx > 10) return;

  Serial.print("ZONE C");
  Serial.print(idx);
  Serial.print(":");

  if (!customZoneDefined[idx - 1]) {
    Serial.println("UNDEF");
    return;
  }

  Serial.print(customZones[idx - 1].first);
  Serial.print("-");
  Serial.println(customZones[idx - 1].last);
}




uint8_t hexByte(const char* p) {
  return (hexNibble(p[0]) << 4) | hexNibble(p[1]);
}

uint8_t hexNibble(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

uint16_t decWord(const char* p) {
  uint16_t v = 0;
  for (uint8_t i = 0; i < 4; i++) {
    v = v * 10 + (p[i] - '0');
  }
  return v;
}

uint8_t decByte(const char* p) {
  return (p[0] - '0') * 10 + (p[1] - '0');
}




// ------------------- EXECUTION -------------------
void executeQueue() {
  if (cmdIndex >= cmdCount) return;
  Command& cmd = cmdQueue[cmdIndex];

  switch(cmd.type) {
    case CMD_FIX:
      // esempio: color fill
      fillZone(cmd.target, cmd.h, cmd.s, cmd.v);
      break;
    case CMD_DIM:
      // esempio: fade
      dimZone(cmd.target, cmd.direction, cmd.ms_on);
      break;
    case CMD_BLK:
      blinkZone(cmd.target, cmd.ms_on, cmd.ms_off);
      break;
    case CMD_SHD:
      shadeZone(cmd.target, cmd.h, cmd.s, cmd.v, cmd.h2, cmd.s2, cmd.v2);
      break;
    case CMD_ANM:
      runAnim(cmd.target, cmd.anim_number, cmd.anim_speed, cmd.h, cmd.s, cmd.v);
      break;
    case CMD_WIT:
      waitSeconds(cmd.sec);
      break;
    case CMD_REP:
      repeatLast(cmd.repeat);
      break;
    default:
      break;
  }
  cmdIndex++; // next
}


// ------------------- PLACEHOLDER FUNCTIONS -------------------
void fillZone(Target t, uint8_t h, uint8_t s, uint8_t v) { /* riempi la zona con colore */ }
void dimZone(Target t, Dir d, uint16_t ms) { /* fade UP/DW */ }
void blinkZone(Target t, uint16_t ms_on, uint16_t ms_off) { /* blink */ }
void shadeZone(Target t, uint8_t h1,uint8_t s1,uint8_t v1,uint8_t h2,uint8_t s2,uint8_t v2){ }
void runAnim(Target t,uint8_t num,uint8_t sp,uint8_t h,uint8_t s,uint8_t v){ }
void waitSeconds(uint16_t sec){ }
void repeatLast(uint16_t repeat){ }