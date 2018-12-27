/* 
RMCIOS - Reactive Multipurpose Control Input Output System
Copyright (c) 2018 Frans Korhonen

RMCIOS was originally developed at Institute for Atmospheric 
and Earth System Research / Physics, Faculty of Science, 
University of Helsinki, Finland

Assistance, experience and feedback from following persons have been 
critical for development of RMCIOS: Erkki Siivola, Juha Kangasluoma, 
Lauri Ahonen, Ella Häkkinen, Pasi Aalto, Joonas Enroth, Runlong Cai, 
Markku Kulmala and Tuukka Petäjä.

This file is part of RMCIOS. This notice was encoded using utf-8.

RMCIOS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RMCIOS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public Licenses
along with RMCIOS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "RMCIOS-functions.h"

// Channel function for allocating and freeing memory
void stdout_func (void *data,
               const struct context_rmcios *context, int id,
               enum function_rmcios function,
               enum type_rmcios paramtype,
               union param_rmcios returnv,
               int num_params, const union param_rmcios param)
{
    
}

// Channel function for allocating and freeing memory
void mem_func (void *data,
               const struct context_rmcios *context, int id,
               enum function_rmcios function,
               enum type_rmcios paramtype,
               union param_rmcios returnv,
               int num_params, const union param_rmcios param)
{
   switch (function)
   {
   case help_rmcios:
      // MEMORY INTERFACE: 
      return_string (context, paramtype, returnv,
                     " read mem \r\n "
                     "   -read ammount of free memory\r\n"
                     " write mem \r\n "
                     "   -read memory allocation block size\r\n"
                     " write mem n_bytes \r\n "
                     "   -Allocate n bytes of memory\r\n"
                     "   -Returns address of the allocated memory\r\n"
                     "   -On n_bytes < 0 allocates complete allocation blocks\r\n"
                     "   -returns 0 length on failure\r\n"
                     " write mem (empty) addr(buffer/id)\r\n"
                     "   -free memory pointed by addr in buffer\r\n"
                     );
      break;

   case write_rmcios:
      if (num_params == 0)
      {
      } // Read memory allocation block size
      if (num_params == 1)      // Allocate n bytes of memory
      {
         void *ptr = malloc (param_to_integer (context, paramtype,
                                               (const union param_rmcios)
                                               param, 0));
         //printf("allocated %x\n",ptr) ;
         return_binary (context, paramtype, returnv, (char *) &ptr,
                        sizeof (ptr));
      }
      if (num_params > 1)
      {
      } // Write data to memory by access id
      if (num_params > 2)
      {
      } // +max size in bytes
      if (num_params > 3)
      {
      } // +starting at offset
      if (num_params == 2)      // Free 
      {
         if (param_to_integer
             (context, paramtype, (const union param_rmcios) param, 0) == 0)
         {
            char *ptr = 0;
            param_to_binary (context, paramtype, param, 1,
                             sizeof (ptr), (char *) &ptr);
            //printf("freeing: %x\n",ptr) ;
            if (ptr != 0)
               free (ptr);
         }
      }
      break;
   }
}

