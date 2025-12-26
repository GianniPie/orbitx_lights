#ifndef DEFINES_H
#define DEFINES_H


//Strips definitions
//------------------------------------------
#define NUM_STRIPS 3
#define LEDS_STRIP_LEFT 250
#define LEDS_STRIP_RIGHT 250
#define LEDS_STRIP_OCT 200
#define NUM_LEDS (LEDS_STRIP_LEFT + LEDS_STRIP_RIGHT + LEDS_STRIP_OCT)


//Zones definitions
//------------------------------------------
# define LFT_LENGHT LEDS_STRIP_LEFT
# define RGT_LENGHT LEDS_STRIP_RIGHT
# define OCT_LENGHT LEDS_STRIP_OCT
# define TOP_LENGHT (LEDS_STRIP_LEFT + LEDS_STRIP_RIGHT)
# define ALL_LENGHT (LEDS_STRIP_LEFT + LEDS_STRIP_RIGHT + LEDS_STRIP_OCT)
# define PFL_LENGHT 64
# define PFR_LENGHT 64
# define PRL_LENGHT 64
# define PRR_LENGHT 64
# define FNT_LENGHT 50
# define RAR_LENGHT 50


# define LFT_FIRST 0
# define RGT_FIRST LEDS_STRIP_LEFT
# define OCT_FIRST (LEDS_STRIP_LEFT + LEDS_STRIP_RIGHT)
# define TOP_FIRST LFT_FIRST
# define ALL_FIRST 0
# define PFL_FIRST LFT_FIRST + 70
# define PFR_FIRST RGT_FIRST + 70
# define PRL_FIRST LFT_FIRST + 160
# define PRR_FIRST RGT_FIRST + 160
# define FNT_FIRST (LEDS_STRIP_LEFT - FNT_LENGHT)
# define RAR_FIRST (LEDS_STRIP_LEFT - RAR_LENGHT)



//HW Inputs
//------------------------------------------
#define PIN_DOOR         2
#define PIN_SEAT_FL      3
#define PIN_SEAT_FR      4
#define PIN_SEAT_RL      5
#define PIN_SEAT_RR      6


//HW Outputs
//------------------------------------------
#define PIN_STRIP_LEFT      6
#define PIN_STROP_RIGHT     7
#define PIN_STRIP_OCT       8
#define PIN_WHITE_LIGHT     11


//Default colors (not used in production)
//------------------------------------------
#define RED         0x00FF00
#define ORANGE      0x15FF00
#define YELLOW      0xFFFF00
#define GREEN       0xFF0000
#define LIGHTBLUE   0xFF00FF
#define BLUE        0x0000FF
#define MAGENTA     0x00FFFF
#define WHITE       0xFFFFFF


#define INPUT_DOOR         2
#define INPUT_SEAT_FL      3
#define INPUT_SEAT_FR      4
#define INPUT_SEAT_RL      5
#define INPUT_SEAT_RR      6


#define LIGHT_OFF       0
#define LIGHT_LOW       10
#define LIGHT_MID       50
#define LIGHT_HIGH      130
#define LIGHT_MAX       255
#define SATURATION_MAX  255



//Serial commands
//------------------------------------------
enum Cmd : uint8_t {
  CMD_ERR,

  CMD_FIX,
  CMD_DIM,
  CMD_SHD,
  CMD_ANM,
  CMD_BLK,
  CMD_WIT,
  CMD_REP,

  CMD_START,
  CMD_STOP,
  CMD_ON,
  CMD_OFF,
  CMD_RESET
};


enum Target : uint8_t {
  T_ERR,
  
  T_LFT,
  T_RGT,
  T_OCT,
  T_TOP,
  T_ALL,
  T_PFL,
  T_PFR,
  T_PRL,
  T_PRR,
  T_FNT,
  T_RAR,
  T_C01,
  T_C02,
  T_C03,
  T_C04,
  T_C05,
  T_C06,
  T_C07,
  T_C08,
  T_C09,
  T_C10
};


enum Dir : uint8_t {
  DIR_ERR,

  DIR_UP,
  DIR_DW
};


//struct custom zones
struct LedRange {
  uint16_t first;
  uint16_t last;   // inclusiv
};



/*
	•	sec → solo WIT
	•	ms_on → DIM e BLK
	•	ms_off → solo BLK
	•	direction → solo DIM
	•	anim_number, anim_speed → solo ANM
	•	repeat → solo REP
	•	campi non usati non vanno letti
*/
struct Command {
  Cmd       type;         // all commands
  Target    target;       // commands with a target
  Dir       direction;    // DIM → UP / DW

  uint8_t   h, s, v;      // color 1 FIX, ANM, SHD
  uint8_t   h2, s2, v2;   // color 2 SHD

  uint16_t  sec;          // WIT → seconds (0–999)
  uint16_t  ms_on;        // DIM, BLK → milliseconds (0-65535)
  uint16_t  ms_off;       // BLK → milliseconds (0-65535)

  uint8_t   anim_number;  // ANM → animation ID (0–99)
  uint8_t   anim_speed;   // ANM → speed preset (0–9)
  uint16_t  repeat;       // REP → 0–999 
};


#endif