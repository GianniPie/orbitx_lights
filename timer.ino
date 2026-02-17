

void callback() {
  //***************************************************************************
  //Code here for events every 1ms
  every1msCounter++;
  if (every1msCounter >= 25) {
    every1msCounter = 0;
    tickAnim = true; //anum update
  }


    //BLK 
    if(blink.active) {
      if(blink.msOn == 0) {
        blink.stateOn = false;
        blink.stateOff = true;

        if(blink.msOff == 0) {
          blink.stateOff = false;
          blink.active = false;
          waitForTimer = false;
        } else {
          //Off fase
          blink.msOff--;
        }

      } else {
        blink.msOn--; 
        //On fase
      }
    }

    //ALTERNATE
    if(alternate.active) {
      if(alternate.msA == 0) {
        alternate.stateA = false;
        alternate.stateB = true;

        if(alternate.msB == 0) {
          alternate.stateB = false;
          alternate.active = false;
          waitForTimer = false;
        } else {
          //Off fase
          alternate.msB--;
        }

      } else {
        alternate.msA--; 
        //On fase
      }
    }


  //***************************************************************************
  if (every10msCounter == 10) {
    every10msCounter = 0;
    //Code here for events every 10ms

  
    animTickCounter++;
    if (animTickCounter >= currentAnim.speed) {
      animTickCounter = 0;
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
        if(witActive) {
          if(waitForTimer) {
            if (waitSecondsCounter == 0) {
              waitForTimer = false;  //Timer unloked
              witActive = false;
            } else {
              waitSecondsCounter--;
            }
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



