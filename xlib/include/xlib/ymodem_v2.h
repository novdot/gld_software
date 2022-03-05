/*
 * ESP32 YModem driver
 *
 * Copyright (C) LoBo 2017
 *
 * Author: Boris Lovosevic (loboris@gmail.com)
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */


#ifndef __YMODEM2_H__
#define __YMODEM2_H__

#include <stdint.h>
#include "xlib/ymodem.h"
typedef enum{
    _xlib_ymodem_abort_by_sender = -1
    , _xlib_ymodem_error_rec_packet = -2
}error_codes;

int Ymodem_Receive (x_ymodem_setups setups, unsigned int maxsize, char* getname);
int Ymodem_Transmit (x_ymodem_setups setups, char* sendFileName, unsigned int sizeFile);


#endif