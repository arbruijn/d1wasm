/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.  
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/
/*
 * $Source: f:/miner/source/main/rcs/cdrom.h $
 * $Revision: 1.2 $
 * $Author: john $
 * $Date: 1995/03/20 15:01:12 $
 * 
 * Prototypes for cdrom.c
 * 
 * $Log: cdrom.h $
 * Revision 1.2  1995/03/20  15:01:12  john
 * Initial revision.
 * 
 * Revision 1.1  1995/03/14  17:01:37  john
 * Initial revision
 * 
 * 
 */



#ifndef _CDROM_H
#define _CDROM_H

// Returns -1 if no cdrom drive, -2 if no memory
// 0 if no descent cd, else 1-26 where descent cd found.
int find_descent_cd();

#endif
