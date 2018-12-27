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

/*
 * Changelog: (date,who,description)
 */
#define VERSION_STR "Mbed" 
 
#include "mbed.h"
#include "RMCIOS-system.h"
#include "std_channels.h"
#include "base_channels.h"
#include "mbed_channels.h"

#include <stdio.h>

LocalFileSystem local("local") ;
// data_handle_name,MAX_CLASSES,MAX_CHANNELS
CREATE_STATIC_CHANNEL_SYSTEM_DATA (ch_sys_dat, 80, 180);  

int main (void)
{
   printf ("\nRMCIOS - Reactive Multipurpose Control Input Output Systen\r\n["
           "] \r\n");
   printf ("Copyright (c) 2018 Frans Korhonen\n");
   printf ("\nInitializing system:\r\n");
   ////////////////////////////////////////////////////////////////////////
   // Init channel system
   ////////////////////////////////////////////////////////////////////////
   const struct context_rmcios *context;
   // init channel api system
   set_channel_system_data ((struct ch_system_data *) &ch_sys_dat);     
   context = get_rmios_context();

   // Init channel modules:
   init_base_channels(context) ;
   init_std_channels(context) ;
   init_mbed_channels(context) ;
   init_mbed_platform_channels(context) ; 
   
   write_str(context, context->control, 
             "read as control file /local/conf.ini\n", 0);
   
   ///////////////////////////////////////////////////////////////////////
   // initial configuration 
   ///////////////////////////////////////////////////////////////////////
   printf ("\r\nSystem initialized!\r\n");

   /////////////////////////////////////////////////////////////////////////
   // reception loop
   /////////////////////////////////////////////////////////////////////////
   while (1)
   {
        wait(1) ;
   }
}

