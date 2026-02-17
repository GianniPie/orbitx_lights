

void animationManager() {
  if (runQueue) executeQueue();

  //End of the queue
  if (commandsIndex == commandsCount) {
    commandsCount = 0; 
    commandsIndex = 0;
    runQueue = false;
  }
  return;
}


// ------------------- EXECUTION -------------------
void executeQueue() {
  if(commandsIndex >= commandsCount) return;   
  if(!skipTimer) {            //Skip Timper for CMD commands and RESET
    if(waitForTimer) return;  //Used in BLK and WIT
  }
  skipTimer = false;

  Command& cmd = cmdQueue[commandsIndex];
  executeCommand(cmd);
  commandsIndex++; // next
}



void executeCommand(Command cmd) {
  Serial.print(CMDSTRINGS[cmd.type]);
  Serial.print(" ");
  Serial.println(commandsIndex);

  // Stop infinite anumatio 
  //if(cmd.type != CMD_ANM || cmd.type != CMD_WIT ) {stopCurrentAnim();}

  switch(cmd.type) {
    case CMD_FIX:
      // esempio: color fill
      fillZone(cmd.target, cmd.h, cmd.s, cmd.v, cmd.show);
      break;
    case CMD_DIM:
      // esempio: fade
      dimZone(cmd.target, cmd.direction, cmd.ms_on);
      break;
    case CMD_BLK:
      blinkZone(cmd.target, cmd.ms_on, cmd.ms_off);
      break;
    case CMD_SHD:
      shadeZone(cmd.target, cmd.h, cmd.s, cmd.v, cmd.h2, cmd.s2, cmd.v2, cmd.show);
      break;
    case CMD_ANM:
      runAnim(cmd.target, cmd.anim_number, cmd.anim_speed, cmd.h, cmd.s, cmd.v, cmd.show);
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

}



void fillZone(Target t, uint8_t h, uint8_t s, uint8_t v, Show sh) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;

  CHSV color(h, s, v);

  for (uint16_t i = first; i <= last; i++) {
    leds[i] = color;
  }

  if(sh == SHOW_Y) FastLED.show();
}



void dimZone(Target t, Dir d, uint16_t ms) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;

  // backup stato corrente
  for (uint16_t i = first; i <= last; i++) {
    animBackup[i] = leds[i];
  }

  if (ms < 10) ms = 10;
  dim.first = first;
  dim.last = last;
  dim.duration = ms / 20;
  if (dim.duration == 0) dim.duration = 1;
  dim.counter = 0;
  dim.up = (d == DIR_UP);
  dim.active = true;
  waitForTimer = true;
  animActive = false;
}



void shadeZone(Target t, uint8_t h1, uint8_t s1, uint8_t v1, uint8_t h2, uint8_t s2, uint8_t v2, Show sh) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;

  CRGB c1, c2;
  hsv2rgb_rainbow(CHSV(h1, s1, v1), c1);
  hsv2rgb_rainbow(CHSV(h2, s2, v2), c2);

  uint16_t len = last - first;
  if (len == 0) {
    leds[first] = c1;
    return;
  }

  for (uint16_t i = 0; i <= len; i++) {
    uint8_t amount = (i * 255) / len;
    leds[first + i] = blend(c1, c2, amount);
  }

  if(sh == SHOW_Y) FastLED.show();
}



void waitSeconds(uint16_t sec){ 
  waitSecondsCounter = sec;
  waitForTimer = true;
  witActive = true;
}



void blinkZone(Target t, uint16_t ms_on, uint16_t ms_off) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;

  //led strip backup
  for (uint16_t i = first; i <= last; i++) {
    animBackup[i] = leds[i];
  }

  //init
  waitForTimer = true;
  blink.first   = first;
  blink.last    = last;
  blink.msOn    = ms_on; // 20;
  blink.msOff   = ms_off; // 20;
  blink.stateOn = true;
  blink.stateOff= false;
  blink.active  = true;

}


void repeatLast(uint16_t repeat){ 
  if(repeat > 0) {
    cmdQueue[commandsIndex].repeat--;
    commandsIndex = commandsIndex-2;
  }
}




void runAnim(Target t, uint8_t num, uint8_t sp, uint8_t h, uint8_t s, uint8_t v, Show sh){ 
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;
  
  currentAnim.number = num;
  currentAnim.first = first;
  currentAnim.last = last;
  currentAnim.h = h;
  currentAnim.s = s;
  currentAnim.v = v;
  currentAnim.speed = sp;
  currentAnim.show = sh;

 switch(num) {
    case 0:
      break;

    case 20:   // ALTERNATE COLOR
      //led strip backup
      for (uint16_t i = first; i <= last; i++) {
        animBackup[i] = leds[i];
      }

      //init
      waitForTimer      = true;
      alternate.msA     = sp * 5;
      alternate.msB     = sp * 5;
      alternate.stateA  = true;
      alternate.stateB  = false;
      alternate.active  = true;
      animActive        = false; //not infinite animation
      break;


    case 21:  // HYPERSPACE
      if (!animActive) {
        // Salva il background iniziale
        for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
          animBackup[i] = leds[i];  //  usato come background
          leds[i] = CHSV(currentAnim.h, currentAnim.s, currentAnim.v);
        }
        for (uint8_t i = 0; i < MAX_STARS; i++) {
          stars[i].active = false;
        }
        animActive = true;
      }
      break;


    default:
      Serial.println("The animation does't exist");
      break;
  }

}


void showAnimation() {  //chiamata nel loop

    if (!tickAnim) return;  // Aggiorna solo quando il timer lo permette (ogni 20 ms)
    tickAnim = false;


    // ---------- DIM ANIMATION - ONCE ----------
    if (dim.active) {
        dim.counter++;

        uint8_t scale;
        if (dim.counter >= dim.duration) { //3 sec = 150
            //reset
            scale = dim.up ? 255 : 0;
            dim.active = false;
            animActive = false;
            tickAnim = false;
            waitForTimer = false;
        } else {
            //Execute dim
            scale = (dim.counter * 255UL) / dim.duration; //duration 0-65535 / 20 -> 0 - 3276 
            if (!dim.up) scale = 255 - scale;
        }

        for (uint16_t i = dim.first; i <= dim.last; i++) {
            leds[i] = animBackup[i];
            leds[i].nscale8(scale);
        }
        FastLED.show();
    }


    // ---------- BLK ANIMATION - ONCE ----------
    if (blink.active) {

      if (blink.stateOn) {
        for (uint16_t i = blink.first; i <= blink.last; i++) {
          leds[i] = animBackup[i];
        }
        FastLED.show();
      }

      if (blink.stateOff) {
        for (uint16_t i = blink.first; i <= blink.last; i++) {
          leds[i] = CRGB::Black;
        }
        FastLED.show();
        //backup for the next repetition
        for (uint16_t i = blink.first; i <= blink.last; i++) {
          leds[i] = animBackup[i];
        }
      }
    }


    // ---------- ANM 20 : ALTERNATE - ONCE ----------
    if (alternate.active) {

      if (alternate.stateA) {
        CHSV color(currentAnim.h, currentAnim.s, currentAnim.v);
        for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
          leds[i] = color;
        }
      }

      if (alternate.stateB) {
        for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
          leds[i] = animBackup[i];
        }
      }

      FastLED.show();
    }



    // ---------- INFINITE ANIMATIONS ----------
    if (!animActive) return;


    // ---------- ANM 21 : STARS (infinite) ----------
    if (currentAnim.number == 21) {
        updateStars();   // Aggiorna la posizione delle stelle
    }

    FastLED.show();

}


void updateStars() {
    // Reset LED al background
    for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
        leds[i] = animBackup[i];
    }

    // Spawn nuove stelle casuali
    if (random8() < 100) { // più alto -> più stelle
        for (uint8_t i = 0; i < MAX_STARS; i++) {
            if (!stars[i].active) {
                stars[i].active = true;

                //divido tra destra e sinistra
                if(random(0,2))
                {
                  stars[i].pos =  random(currentAnim.first, currentAnim.last / 2); 
                } else {
                  stars[i].pos =  random(currentAnim.last / 2, currentAnim.last);
                }

                // Velocità variabile in base a currentAnim.speed
                stars[i].speed = (random8(5, 10) * currentAnim.speed) / 100 + 1;  //più alto -> più veloce

                stars[i].bright = 10;  // luminosità iniziale
                break;
            }
        }
    }

    // Aggiorna tutte le stelle attive
    for (uint8_t i = 0; i < MAX_STARS; i++) {
        if (!stars[i].active) continue;

        stars[i].pos += stars[i].speed;
        stars[i].bright = qadd8(stars[i].bright, 20); // aumenta luminosità

        if (stars[i].pos > currentAnim.last) {  
            stars[i].active = false;
            continue;
        }

        // Overlay sul LED attuale
        leds[stars[i].pos] += CHSV(0, 0, stars[i].bright);
    }
}



void stopCurrentAnim() {
    animActive = false;
    tickAnim = false;
    for (uint8_t i = 0; i < MAX_STARS; i++) 
      stars[i].active = false;
    //FastLED.show();
}



bool getTargetRange(Target t, uint16_t &first, uint16_t &last) {
  switch (t) {
    case T_LFT: first = LFT_FIRST; last = LFT_FIRST + LFT_LENGHT - 1; break;
    case T_RGT: first = RGT_FIRST; last = RGT_FIRST + RGT_LENGHT - 1; break;
    case T_OCT: first = OCT_FIRST; last = OCT_FIRST + OCT_LENGHT - 1; break;
    case T_TOP: first = TOP_FIRST; last = TOP_FIRST + TOP_LENGHT - 1; break;
    case T_ALL: first = ALL_FIRST; last = ALL_FIRST + ALL_LENGHT - 1; break;

    case T_PFL: first = PFL_FIRST; last = PFL_FIRST + PFL_LENGHT - 1; break;
    case T_PFR: first = PFR_FIRST; last = PFR_FIRST + PFR_LENGHT - 1; break;
    case T_PRL: first = PRL_FIRST; last = PRL_FIRST + PRL_LENGHT - 1; break;
    case T_PRR: first = PRR_FIRST; last = PRR_FIRST + PRR_LENGHT - 1; break;
    case T_FNT: first = FNT_FIRST; last = FNT_FIRST + FNT_LENGHT - 1; break;
    case T_RAR: first = RAR_FIRST; last = RAR_FIRST + RAR_LENGHT - 1; break;

    default:
      return false;
  }
  return true;
}