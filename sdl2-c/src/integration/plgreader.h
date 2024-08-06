#include "../model/polygon.h"
#include "../model/sector.h"

#ifndef PLG_READER_H
#define PLG_READER_H

/**
* Loads PLG files by reading from the text file and declaring variables accordingly.
* Also has the option to scale the object as it is being constructed.
*/
int PLG_Load_Object(Object* object, char *filename, float scale);

int PLG_Load_Sector(Sector* sector, char *filename);

#endif
