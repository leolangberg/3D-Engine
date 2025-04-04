#include "../model/object/polygon.h"

#ifndef PLG_READER_H
#define PLG_READER_H

// Reads line of PLG file and converts file text into string. 
char *PLG_Get_Line(char *string, int max_length, FILE *fp);

// Loads PLG files by reading from the text file and declaring variables accordingly.
// Also has the option to scale the object as it is being constructed.
int PLG_Load_Object(Object* object, char *filename, float scale);

int OBJ_Load_Object(Object* object, char *filename, float scale);

#endif
