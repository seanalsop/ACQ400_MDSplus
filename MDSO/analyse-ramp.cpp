/* ------------------------------------------------------------------------- */
/* file analyse-ramp.cpp                                                     */
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

/** @file analyse-ramp.cpp validate ACQ196 test ramp on 2 channels.
 *
 * analyse-ramp left right
 * where left, right are raw binary data, left is MSW, right is LSW
 */

#include "Chdata.h"

unsigned makeword(Chdata& left, Chdata&right, int ii)
{
	unsigned xx = (left.buffer[ii] << 16) | right.buffer[ii];

	if (verbose > 1){
		printf("%10d %08x\n", ii, xx);
	}		
	return xx;
}

int analyse_ramp(const char* left_name, const char* right_name)
{
	Chdata left(left_name);
	Chdata right(right_name);
	int lwords, rwords;
	unsigned x1;
	bool first_time = 1;
	unsigned long long offset = 0;
	int errors = 0;
	unsigned x2;	

	if (verbose) left.print();
	if (verbose) right.print();

	if (left.length != right.length){
		fprintf(stderr, 
			"lengths are different, go with shorter %d %d\n",
			left.length, right.length);
	}

	while((lwords = left.readbuf()) > 0 &&
              (rwords = right.readbuf()) > 0  &&
	      lwords == rwords			){
		int ii = 0;

		if (first_time){
			x1 = makeword(left, right, 0);

			if (verbose){
				printf("start value: %10lld: 0x%08x\n", 
				       offset, x1);
			}
			ii = 1;
			first_time = 0;
			offset++;
		}
		for ( ; ii < lwords; ++ii){
			x2 = makeword(left, right, ii);
			if (x2 != x1 + STEP){
				if (verbose && errors < MAXERR){
					printf("ERROR at 0x%08llx %10lld: "
						"%08x => %08x expected: %08x\n",
					       offset*sizeof(short), 
					       offset*sizeof(short), 
						x1, x2, x1+STEP);
				}
				++errors;
				if (errors == MAXERR){
					fprintf(stderr, "too many errors\n");
				}
			}
			x1 = x2;
			offset++;
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

	if (argc == 3){
		return analyse_ramp(argv[1], argv[2]);
	}else{
		fprintf(stderr, "usage: analyse-ramp LEFT RIGHT\n");
		return -1;
	}
}
