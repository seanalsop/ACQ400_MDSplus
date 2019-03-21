/* ------------------------------------------------------------------------- */
/* file report-flats.cpp                                                     */
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

/** @file report-flats.cpp A run of duplicate values (eg 0) indicates a fault.
 *
 */

#include "Chdata.h"

int MAXDUPS = 8;

static void print_err(const char* id, unsigned long long offset, unsigned xx)
{
	printf("%30s: 0x%08llx %10lld : %04x\n",
	       id, offset, offset, xx);
}
int search_flats(const char* chx_name)
{
	Chdata chx(chx_name);
	int lwords;
	unsigned x1;
	bool first_time = 1;
	unsigned long long offset = 0;
	unsigned long long offset_first_dup;
	int errors = 0;
	unsigned x2;	
	int ndups = 0;

	printf("%s\n", chx_name);
	if (verbose) chx.print();

	while((lwords = chx.readbuf()) > 0){
		int ii = 0;

		if (first_time){
			x1 = chx.buffer[0];

			if (verbose){
				printf("start value: %10lld: 0x%08x\n", 
				       offset, x1);
			}
			ii = 1;
			first_time = 0;
			offset += sizeof(short);
		}
		for ( ; ii < lwords; ++ii){
			x2 = chx.buffer[ii];

			if (x1 == x2){
				if (ndups == 0){
					offset_first_dup = offset;
				}
				if (++ndups == MAXDUPS){
					print_err("MAXDUPS exceeded from:",
						  offset_first_dup, x1);
				}
			}else{
				if (ndups > MAXDUPS){
					print_err("DUPS OK at:", offset, x2);
				}
				ndups = 0;
			}

			x1 = x2;
			offset += sizeof(short);
		}
	}

	if (verbose) printf("  end value: %10lld: 0x%08x\n", 
					offset*sizeof(short), x2);

	printf("total words:  %lld errors:%d\n", offset, errors);
	return errors;
}

int main(int argc, char *argv[])
{
	if (getenv("VERBOSE")) verbose = atoi(getenv("VERBOSE"));
	if (getenv("MAXERR")) MAXERR = atoi(getenv("MAXERR"));
	if (getenv("MAXDUPS")) MAXDUPS = atoi(getenv("MAXDUPS"));

	if (argc > 1){
		for (int ii = 1; ii < argc; ++ii){
			if (search_flats(argv[ii])){
				return 1;
			}
		}
	}else{
		fprintf(stderr, "usage: report-flats CHX [CHX2]\n");
		return -1;
	}
}
