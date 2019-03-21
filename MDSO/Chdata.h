/* ------------------------------------------------------------------------- */
/* file Chdata.h	                                                     */
/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2011 Peter Milne, D-TACQ Solutions Ltd
 *                      <Peter dot Milne at D hyphen TACQ dot com>
    This program is free software; you can redistribute it and/or modify
    it under the terms of Version 2 of the GNU General Public License
    as published by the Free Software Foundation;

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/* ------------------------------------------------------------------------- */

/** @file Chdata.h. Chdata class manages the data for one channel.
 *
 */

#ifndef __CHDATA_H__
#define __CHDATA_H__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define BUFLEN	0x100000	/* seems a useful size */
#define STEP	0x10		/* pattern characteristic */

int verbose = 0;
int MAXERR = 10;

struct Chdata {
	const char* file_name;
	FILE* fp;
	int length;
	unsigned short* buffer;

public:
	Chdata(const char* fname);
	~Chdata();

	void print() {
		printf("Chdata:\"%s\" length %d shorts\n", 
				file_name, length);
	}

	int readbuf() {
		return fread(buffer, sizeof(short), BUFLEN, fp);
	}

};

Chdata::Chdata(const char* fname) : file_name(fname)
{
	fp = fopen(fname, "r");
	if (fp == 0){
		perror(fname);
		exit(errno);
	}
	struct stat sb;
	if (stat(fname, &sb) != 0){
		perror("stat failed");
		exit(errno);
	}	
	length = sb.st_size/sizeof(short);
	buffer = new unsigned short[BUFLEN];
}
Chdata::~Chdata()
{
	fclose(fp);
	delete [] buffer;
}

#endif // __CHDATA_H__
