

void callback() {
  //***************************************************************************
  //Code here for events every 1ms

  animTickCounter++;
  if (animTickCounter >= currentAnim.speed) {
    animTickCounter = 0;
    tickAnim = true;   // da il consenso all'update
  }

  //***************************************************************************
  if (every10msCounter == 10) {
    every10msCounter = 0;
    //Code here for events every 10ms

    tickAnim = true;

  
    //BLK timer
    if(blink.active) {
      if(blink.msOn == 0) {
        blink.stateOn = false;
        //Time off: it turn off the selected zone
        for (uint16_t i = blink.first; i <= blink.last; i++) {
          leds[i] = CRGB::Black;
        }

        //When timeOn is over, it executes timeOff
        if(blink.active) {
          
        }
        if(blink.msOff == 0) {
          blink.stateOff = false;
          
          //When the time off it's over it show the previews backuped configuration
          for (uint16_t i = blink.first; i <= blink.last; i++) {
            leds[i] = animBackup[i];
          }
          blink.active = false;  //Timer unloked, the queue can go on 
          waitForTimer = false;
          
        } else {
          blink.msOff--;
        }

      } else {
        blink.msOn--;
        //Time on: it doesn nothing because it shows the previws color
      }
    }


    //***************************************************************************
    if (every100msCounter == 10) {
      every100msCounter = 0;
      //Code here for events every 100ms



 
      //***************************************************************************
      if (every1sCounter == 10) {
        every1sCounter = 0;
        //Code here for events every 1 second

        //WIT timer
        if(waitForTimer) {
          if (waitSecondsCounter == 0) {
            waitForTimer = false;  //Timer unloked
          } else {
            waitSecondsCounter--;
          }
        }
        

      

        //***************************************************************************
        if (every1mCounter == 60) {
          every1mCounter = 0;
          //Code here for events every 1 minute




          //***************************************************************************
          if (every1hCounter == 60) {
            every1hCounter = 0;
            //Code here for events every 1 hour




            //***************************************************************************
          } else {
            every1hCounter++;
          }
        } else {
          every1mCounter++;
        }
      } else {
        every1sCounter++;
      }
    } else {
      every100msCounter++;
    }
  } else {
    every10msCounter++;
  }
}



