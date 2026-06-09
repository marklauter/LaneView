#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
//#include <string.h>
//#include <math.h>
//#include <conio.h>
//#include <bios.h>
//#include <malloc.h>

#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "msgbox.h"

// function_not_supported

void function_not_supported(void) {
  msgbox("The function you have selected is not|supported by this release of InterLane.", "Information", MB_OK);
}
