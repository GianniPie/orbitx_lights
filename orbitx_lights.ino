//Vr  1.5


#define DEBUG
//#define INPUTS_MANAGED

#if defined(ARDUINO_AVR_UNO)
  #define DEVELOP
#else
  #define PRODUCTION
#endif


#include "defines.h"
#include <FastLED.h>
#include <avr/wdt.h>

//It create the working arrray (only one for all the 3 strips)
//strip leds in array are LEFT, RIGHT, OCTAGON in sequence
CRGB leds[NUM_LEDS];

#include <TimerOne.h>
uint8_t everyXmsCounter, every1msCounter, every10msCounter, every100msCounter, every1sCounter, every1mCounter, every1hCounter = 0;

//Serial config
#define SERIAL_BUF_LEN 255
char rxBuffer[SERIAL_BUF_LEN];
uint8_t rxPos = 0;

//Queue 
#define CMD_QUEUE_SIZE 20
Command cmdQueue[CMD_QUEUE_SIZE];
uint8_t commandsCount = 0;  //number of commands in the Queue
uint8_t commandsIndex = 0;  //Index of the command executed

//Custom zones
// LedRange customZones[10];
// bool customZoneDefined[10];

bool runQueue = false;
bool skipTimer = false;
bool waitForTimer = false;
uint16_t waitSecondsCounter = 0;  //WIT

//Animations
//-------------------------
CRGB animBackup[NUM_LEDS];   // backup strip

//Blink
BlinkState blink;

//Alternate
uint8_t animStep = 0;   // 0 = A→B, 1 = B→A, 2 = DONEuint8_t animStep = 0;   // 0 = A→B, 1 = B→A, 2 = DONE
uint16_t animCounter = 0;
uint16_t animPeriod = 0;

//hyperspace
#define MAX_STARS 5
Star stars[MAX_STARS];
volatile bool tickAnim = false;
volatile uint8_t animTickCounter = 0;
bool animActive = false;
Anim currentAnim;

//Dim
Dim dim;


void setup() {
  wdt_disable(); 

  // Initialize serial communication
  Serial.begin(38400);
  while (!Serial);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(callback);

#ifdef DEVELOP
  FastLED.addLeds<WS2812B, PIN_STRIP_LEFT,  GRB>(leds + LFT_FIRST, LFT_LENGHT);
  FastLED.addLeds<WS2812B, PIN_STROP_RIGHT, GRB>(leds + RGT_FIRST, RGT_LENGHT);
  FastLED.addLeds<WS2812B, PIN_STRIP_OCT,   GRB>(leds + OCT_FIRST, OCT_LENGHT);
#endif

#ifdef PRODUCTION
  FastLED.addLeds<WS2812B, PIN_STRIP_LEFT,  RGB>(leds + LFT_FIRST, LFT_LENGHT);
  FastLED.addLeds<WS2812B, PIN_STROP_RIGHT, RGB>(leds + RGT_FIRST, RGT_LENGHT);
  FastLED.addLeds<WS2812B, PIN_STRIP_OCT,   RGB>(leds + OCT_FIRST, OCT_LENGHT);
#endif

  pinMode(LED_BUILTIN,      OUTPUT);
  pinMode(PIN_STRIP_LEFT,   OUTPUT); 
  pinMode(PIN_STROP_RIGHT,  OUTPUT); 
  pinMode(PIN_STRIP_OCT,    OUTPUT); 

  //Init custom zones
  // for (uint8_t i = 0; i < 10; i++) {
  //   customZoneDefined[i] = false;
  // }

  processLine("CMDON");
  Serial.println("Device ready");
}



void loop() {

  //inputManager();
  serialManager();
  animationManager();
  showAnimation();

}





void serialManager() {
  while (Serial.available()) {
    char c = Serial.read();
    
    //every CR 
    if (c == '\r') {
      rxBuffer[rxPos] = '\0';
      //The shortest command is 5 charatters
      if (rxPos >= 5) {
        //It cleans the string
        trim(rxBuffer);
        
        if (strchr(rxBuffer, ';') == NULL) {
          // Single command
          processLine(rxBuffer);
        } else {
          //Multiple commands in one single string
          char* p = rxBuffer;

          while (*p != '\0') {

            char* sep = strchr(p, ';');
            if (sep != NULL) {
              *sep = '\0';  // closes the command
            }
            if (strlen(p) > 0) { // filters empty commands
              processLine(p);
            }
            if (sep == NULL) { // last command
              break;               
            }
            p = sep + 1;  // next command
          }
        }
        
        rxPos = 0;
      } else {
        rxPos = 0;
      }

    } else {
      //Build the buffer
      if (rxPos < SERIAL_BUF_LEN - 1) {
        //fills the buffer
        rxBuffer[rxPos++] = c;
      } else {
        // overflow → scarta linea
        Serial.println("Overflow");
        rxPos = 0;
      }
    }
  }
}




void trim(char* s) { 
  char* src = s;
  char* dst = s;

  while (*src) {
    if (*src != '\n' && *src != '\r' && *src != '\t' && *src != '"'  && *src != ' ') {
      *dst++ = *src;
    }
    src++;
  }

  *dst = '\0';
  return s;
}



void processLine(const char* line) {
  Command cmd;

  // -------- special commands --------
  if (!strcmp(line, "RESET")) {
    softwareReset();
    return;
  }

  if (!strcmp(line, "CMDSTART")) {
    commandsIndex = 0;
    skipTimer = true;
    runQueue = true;
    return;
  }

  if (!strcmp(line, "CMDSTOP")) {
    commandsCount = 0; 
    commandsIndex = 0;
    runQueue = false;
    clearForCmd();
    return;
  }

  if (!strcmp(line, "CMDON")) {
    commandsCount = 1; 
    commandsIndex = 0;
    skipTimer = true;
    if (parseCommand("FIXTOP0000FF",  cmd)) {
      cmdQueue[0] = cmd;
    }
    runQueue = true;
    clearForCmd();
    return;
  }

  if (!strcmp(line, "CMDOFF")) {
    commandsCount = 1; 
    commandsIndex = 0;
    skipTimer = true;
    if (parseCommand("FIXALL000000",  cmd)) {
      cmdQueue[0] = cmd;
    }
    runQueue = true;
    clearForCmd();
    return;
  }

  // -------- custom zone commands --------
  // if (!strncmp(line, "SETC", 4)) {
  //   handleSetZone(line);
  //   return;
  // }

  // if (!strncmp(line, "GETC", 4)) {
  //   handleGetZone(line);
  //   return;
  // }

  // -------- bufferizable commands --------
  if (commandsCount >= CMD_QUEUE_SIZE) {
    // Full queue - it does nothing
    Serial.println("Commands queue is full"); 
    return;
  }

  if (parseCommand(line,  cmd)) {
    clearForNextCommand();
    cmdQueue[commandsCount++] = cmd;
    Serial.print(commandsCount);
    Serial.print('\t');
    printCommand(cmd);  
  }
}



void clearForCmd () {
  skipTimer = false;
  waitForTimer = false;
  blink.active = false;
  blink.stateOn = false;
  blink.stateOff = false;
  animActive = false;
  tickAnim = false;
}

void clearForNextCommand () {
  skipTimer = false;
  blink.active = false;
}



bool parseCommand(const char* s, Command& cmd) {
  memset(&cmd, 0, sizeof(Command)); //resets the cmd variable

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
    cmd.ms_off = decWord(s + 11);
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
    cmd.sec  = decByte3(s + 3);
    return true;
  }
  // REP
  if (!strncmp(s, "REP", 3)) {
    cmd.type   = CMD_REP;
    cmd.repeat = decByte3(s + 3);
    return true;
  }

  #ifdef DEVELOP
  Serial.print("Unknown command: "); 
  Serial.println(s);
  #endif
  return false;
}



Target decodeTarget(const char* s) {
  
  if (!strncmp(s, TARGETSTRING[T_LFT], 3)) return T_LFT;
  if (!strncmp(s, TARGETSTRING[T_RGT], 3)) return T_RGT;
  if (!strncmp(s, TARGETSTRING[T_OCT], 3)) return T_OCT;
  if (!strncmp(s, TARGETSTRING[T_TOP], 3)) return T_TOP;
  if (!strncmp(s, TARGETSTRING[T_ALL], 3)) return T_ALL;
  if (!strncmp(s, TARGETSTRING[T_PFL], 3)) return T_PFL;
  if (!strncmp(s, TARGETSTRING[T_PFR], 3)) return T_PFR;
  if (!strncmp(s, TARGETSTRING[T_PRL], 3)) return T_PRL;
  if (!strncmp(s, TARGETSTRING[T_PRR], 3)) return T_PRR;
  if (!strncmp(s, TARGETSTRING[T_FNT], 3)) return T_FNT;
  if (!strncmp(s, TARGETSTRING[T_RAR], 3)) return T_RAR;

  // Custom zones C01–C10
  if (!strncmp(s, TARGETSTRING[T_C01], 3)) return T_C01;
  if (!strncmp(s, TARGETSTRING[T_C02], 3)) return T_C02;
  if (!strncmp(s, TARGETSTRING[T_C03], 3)) return T_C03;
  if (!strncmp(s, TARGETSTRING[T_C04], 3)) return T_C04;
  if (!strncmp(s, TARGETSTRING[T_C05], 3)) return T_C05;
  if (!strncmp(s, TARGETSTRING[T_C06], 3)) return T_C06;
  if (!strncmp(s, TARGETSTRING[T_C07], 3)) return T_C07;
  if (!strncmp(s, TARGETSTRING[T_C08], 3)) return T_C08;
  if (!strncmp(s, TARGETSTRING[T_C09], 3)) return T_C09;
  if (!strncmp(s, TARGETSTRING[T_C10], 3)) return T_C10;

  // Default
  Serial.print("Unknown target: "); Serial.println(s);
  return T_NOT; 
}



Dir decodeDir(const char* s) {
  // s punta subito dopo il target, es. "UP003000" o "DW003000"
  if (!strncmp(s, "UP", 2)) return DIR_UP;
  if (!strncmp(s, "DW", 2)) return DIR_DW;

  // Default in caso di errore
  Serial.print("Unknown direction: "); Serial.println(s);
  return DIR_NOD;
}




// void handleSetZone(const char* cmd) {
//   uint8_t idx = (cmd[4] - '0') * 10 + (cmd[5] - '0');
//   if (idx < 1 || idx > 10) return;

//   uint16_t first = atoi(cmd + 6);
//   uint16_t last  = atoi(cmd + 9);

//   if (first >= NUM_LEDS || last >= NUM_LEDS || first > last) return;

//   customZones[idx - 1].first = first;
//   customZones[idx - 1].last  = last;
//   customZoneDefined[idx - 1] = true;
// }



// void handleGetZone(const char* cmd) {
//   uint8_t idx = (cmd[4] - '0') * 10 + (cmd[5] - '0');
//   if (idx < 1 || idx > 10) return;

//   Serial.print("ZONE C");
//   Serial.print(idx);
//   Serial.print(":");

//   if (!customZoneDefined[idx - 1]) {
//     Serial.println("UNDEF");
//     return;
//   }

//   Serial.print(customZones[idx - 1].first);
//   Serial.print("-");
//   Serial.println(customZones[idx - 1].last);
// }




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
  for (uint8_t i = 0; i < 5; i++) {
    v = v * 10 + (p[i] - '0');
  }
  return v;
}

uint8_t decByte(const char* p) {
  return (p[0] - '0') * 10 + (p[1] - '0');
}

uint16_t decByte3(const char* p) {
  return (p[0] - '0') * 100 + (p[1] - '0') * 10 + (p[2] - '0');
}





void printCommand(const Command& c) {
  Serial.print(CMDSTRINGS[c.type]);
  Serial.print(" ");
  Serial.print(TARGETSTRING[c.target]);
  Serial.print(" ");
  Serial.print(DIRSTRINGS[c.direction]);

  Serial.print(" (");
  Serial.print(c.h);
  Serial.print(" ");
  Serial.print(c.s);
  Serial.print(" ");
  Serial.print(c.v);

  Serial.print(")");
  Serial.print('\t');
  Serial.print("(");
  Serial.print(c.h2);
  Serial.print(" ");
  Serial.print(c.s2);
  Serial.print(" ");
  Serial.print(c.v2);

  Serial.print(")");
  Serial.print('\t');
  Serial.print("sec ");
  Serial.print(c.sec);

  Serial.print(",");
  Serial.print('\t');
  Serial.print("ms_on ");
  Serial.print(c.ms_on);

  Serial.print(",");
  Serial.print('\t');
  Serial.print("ms_off ");
  Serial.print(c.ms_off);

  Serial.print(",");
  Serial.print('\t');
  Serial.print("anim ");
  Serial.print(c.anim_number);

  Serial.print(",");
  Serial.print('\t');
  Serial.print("speed ");
  Serial.print(c.anim_speed);

  Serial.print(",");
  Serial.print('\t');
  Serial.print("rep ");
  Serial.print(c.repeat);

  Serial.println("");
}



void softwareReset() {
    Serial.println("RESETTING...");
    Serial.flush();             // Assicurati che i dati seriali vengano inviati
    delay(10);                  // Piccolo ritardo per lasciare passare la seriale
    cli();                      // Disabilita interrupt
    wdt_enable(WDTO_15MS);      // Watchdog timer a 15 ms
    while (1) { }               // Blocca tutto finché il watchdog non resetta
}


