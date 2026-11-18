#include "WindowPeek.h"
#include <math.h>
#include <stdio.h>

int32 slideshowThread(void*data) {
  ViewFile *parent = (ViewFile*)data;
  bool firstRun = true;
  while ( true ) {

    if (firstRun == false) snooze( parent->setup->SlideshowDelay() );
                      else firstRun = false;

    if (parent->setup->Sliding()) parent->Slideshow();

  }
  return B_OK;
}

// END OF THREAD
