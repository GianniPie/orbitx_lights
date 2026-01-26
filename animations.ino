

void animationManager() {
  if (runQueue) executeQueue();

  //Executes blink when the timer set the flags
  if(blink.active) {
      FastLED.show();
  }

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
  Serial.print("Executing: ");
  Serial.println(CMDSTRINGS[cmd.type]);

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

}




void fillZone(Target t, uint8_t h, uint8_t s, uint8_t v) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;

  CHSV color(h, s, v);

  for (uint16_t i = first; i <= last; i++) {
    leds[i] = color;
  }

  FastLED.show();
}



void shadeZone(Target t,
  uint8_t h1, uint8_t s1, uint8_t v1,
  uint8_t h2, uint8_t s2, uint8_t v2
) {
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

  FastLED.show();
}



void waitSeconds(uint16_t sec){ 
  waitSecondsCounter = sec;
  waitForTimer = true;
}



void blinkZone(Target t, uint16_t ms_on, uint16_t ms_off) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;


  //led strip backup
  for (uint16_t i = first; i <= last; i++) {
    animBackup[i] = leds[i];
  }

  waitForTimer = true;

  blink.first   = first;
  blink.last    = last;
  blink.msOn    = ms_on / 10;
  blink.msOff   = ms_off /10;
  blink.stateOn = true;
  blink.stateOff= true;
  blink.active  = true;

}


void repeatLast(uint16_t repeat){ 
  if(repeat > 0) {
    cmdQueue[commandsIndex].repeat--;
    commandsIndex = commandsIndex-2;
  }
}




void runAnim(Target t, uint8_t num, uint8_t sp, uint8_t h, uint8_t s, uint8_t v){ 
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;
  
  currentAnim.number = num;
  currentAnim.first = first;
  currentAnim.last = last;
  currentAnim.h = h;
  currentAnim.s = s;
  currentAnim.v = v;
  currentAnim.speed = sp;

 switch(num) {
    case 0:
      break;

    case 20:  // HYPERSPACE
      if (!animActive) {
        //init stars
        for (uint8_t i = 0; i < MAX_STARS; i++) {
          stars[i].active = false;
        }
        animActive = true;
      }
      break;

    case 21:   // ALTERNATE COLOR
      // snapshot prev color
      for (uint16_t i = first; i <= last; i++) {
        animBackup[i] = leds[i];
      }
      animStep    = 0; 
      animCounter = 0;
      animPeriod = sp;

      waitForTimer = true; 
      animActive = true;
      break;

    default:
      Serial.println("The animation does't exist");
      break;
  }

}


void showAnimation() {

  if (!tickAnim) return;
  tickAnim = false;
  if (!animActive) return;

  // ---------- DIM ANIMATION - ONCE ----------
  if (dim.active) {
    dim.counter++;

    uint8_t amount;
    if (dim.counter >= dim.duration) {
      amount = dim.up ? 255 : 0;
      dim.active = false;
      waitForTimer = false; 
    } else {
      amount = (dim.counter * 255UL) / dim.duration;
      if (!dim.up) amount = 255 - amount;
    }

    for (uint16_t i = dim.first; i <= dim.last; i++) {
      leds[i] = animBackup[i];
      leds[i].nscale8_video(amount);
    }

    FastLED.show();
    return;
  }


  // ---------- ANM 20 : STARS (infinite) ----------
  if (currentAnim.number == 20) {
    updateStars();
    FastLED.show();
    return;
  }

  // ---------- ANM 21 : ALTERNATE - ONCE ----------
  if (currentAnim.number == 21) {

    animCounter++;

    if (animCounter < animPeriod) return;
    animCounter = 0;

    if (animStep == 0) {
      // A → B
      CHSV c(currentAnim.h, currentAnim.s, currentAnim.v);
      for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
        leds[i] = c;
      }
      FastLED.show();

      animStep = 1;
      return;
    }

    if (animStep == 1) {
      // B → A
      for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
        leds[i] = animBackup[i];
      }
      FastLED.show();

      animStep = 2;
      return;
    }

    animActive = false;
    waitForTimer = false; 
    animStep = 0;
  }
}




void updateStars() {

  // background
  CHSV base(currentAnim.h, currentAnim.s, currentAnim.v);
  for (uint16_t i = currentAnim.first; i <= currentAnim.last; i++) {
    leds[i] = base;
  }


  // spawn new stars
  if (random8() < 30) {
    for (uint8_t i = 0; i < MAX_STARS; i++) {
      if (!stars[i].active) {
        stars[i].active = true;
        stars[i].pos = currentAnim.first;
        stars[i].speed = random8(1, 4);
        stars[i].bright = 20;
        break;
      }
    }
  }

  // stars update
  for (uint8_t i = 0; i < MAX_STARS; i++) {
    if (!stars[i].active) continue;

    stars[i].pos += stars[i].speed;
    stars[i].bright = qadd8(stars[i].bright, 20);

    if (stars[i].pos > currentAnim.last) {
      stars[i].active = false;
      continue;
    }

    // overlay stars on background
    leds[stars[i].pos] += CHSV(0, 0, stars[i].bright);
  }
}




void dimZone(Target t, Dir d, uint16_t ms) {
  uint16_t first, last;
  if (!getTargetRange(t, first, last)) return;

  // backup stato corrente
  for (uint16_t i = first; i <= last; i++) {
    animBackup[i] = leds[i];
  }

  dim.first = first;
  dim.last = last;
  dim.duration = ms; 
  dim.counter = 0;
  dim.up = (d == DIR_UP);
  dim.active = true;

  waitForTimer = true;  
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