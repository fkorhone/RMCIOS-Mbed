#include "string-conversion.h"

#include <stdlib.h>
#include <stdio.h>

/* String to float converter 
 * @param str String to convert from
 * returns the converted value. Returns 0 on failure. */
int string_to_integer (const char *str)
{
   return strtol (str, NULL, 0);
}

/* String to float converter 
 * @param str String to convert from
 * returns the converted value. Returns 0 on failure. */
double string_to_float (const char *str)
{
   return strtof (str, NULL);
}

/* Integer to string converter 
 * @param buffer buffer to write to
 * @param len size of buffer
 * @param value number to convert
 * @return number of characters the full string representation needs. */
int integer_to_string (char *buffer, int len, int value)
{
   return snprintf (buffer, len, "%d", value);
}

/* Float to string converter 
 * @param buffer buffer to write to
 * @param len size of buffer
 * @param value Number to convert
 * @return number of characters the full string representation needs.*/
int float_to_string (char *buffer, int len, double value)
{
   return snprintf (buffer, len, "%g", value);
}


