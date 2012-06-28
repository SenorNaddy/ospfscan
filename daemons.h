/*
 * This file is part of libwandipfix
 *
 * Copyright (c) 2008 The University of Waikato, Hamilton, New Zealand.
 * Author: Shane Alcock
 *
 * Contributors: Lance Fitz-Herbert
 *               Refael Whyte
 *               Perry Lorier
 *
 * All rights reserved.
 *
 * This code has been developed by the University of Waikato WAND research
 * group. For further information, please see http://www.wand.net.nz/
 *
 * libwandipfix and maji are free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * libwandipfix is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with libwandipfix; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  0211-1307  USA
 *
 * Any feedback (bug reports, suggestions, complaints) should be sent to
 * contact@wand.net.nz
 *
 * $Id: daemons.h 171 2008-10-23 03:09:41Z salcock $
 *
 */

    
#ifndef DAEMONS_H
#define DAEMONS_H

#ifdef __cplusplus
extern "C" {
#endif


void put_pid( char *fname );
void daemonise( char *name ) ;

extern int daemonised;

#ifdef __cplusplus
}
#endif

#endif /* DAEMONS_H */
    
