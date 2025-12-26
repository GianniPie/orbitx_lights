

void callback() {
  //***************************************************************************
  //Code here for events every 1ms



  //***************************************************************************
  if (every10msCounter == 10) {
    every10msCounter = 0;
    //Code here for events every 10ms




    //***************************************************************************
    if (every100msCounter == 10) {
      every100msCounter = 0;
      //Code here for events every 100ms



 
      //***************************************************************************
      if (every1sCounter == 10) {
        every1sCounter = 0;
        //Code here for events every 1 second


      

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



