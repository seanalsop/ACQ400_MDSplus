/* ------------------------------------------------------------------------- */
/* file test-seg-pull.cpp                                                    */
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

/** @file test-seg-pull.cpp exercise MDSplus segmented pull
 *
 * test-seg-pull [opts] TREE FIELD
 *
 * --nchan	channels [96]
 * --c1		channel1 [1]
 * --c2		channel1 [nchan]
 * --len	length   [1024]
 * --nsegs	number of segments [1]
 * --prf	pulse repetition frequency Hz [10]
 * --isr	input sample rate kHz [500]
 */

#include <iostream>


#include <math.h>
#include <popt.h>


#include <mdsobjects.h>
using namespace MDSplus;

int acq200_debug;
int verbose = 0;

struct Globs {
	int c1;
	int c2;
	int nchan;
	int len;
	int nsegs;
	int prf;
	int isr;
	const char *tree;
	const char *field;
	int shot;

};

struct Globs globs = {
	1, 96,
	96,
	1024,
	10,
	10,
	500	
};


static poptContext processOpts(int argc, const char** argv)
{
	static struct poptOption opt_table[] = {
        { "debug",              'd', POPT_ARG_INT, &acq200_debug,       0 },
	{ "nchan",		'n', POPT_ARG_INT, &globs.nchan,	0 },
	{ "len",		'l', POPT_ARG_INT, &globs.len,          0 },
	{ "nsegs",		's', POPT_ARG_INT, &globs.nsegs,        0 },
	{ "prf",                'P', POPT_ARG_INT, &globs.prf,		0 },
	{ "isr",		'I', POPT_ARG_INT, &globs.isr,		0 },
	{ "shot",		'S', POPT_ARG_INT, &globs.shot,		0 },
	{ "c1",			'c', POPT_ARG_INT, &globs.c1,		0 },
	{ "c2",			'C', POPT_ARG_INT, &globs.c2,		0 },
        POPT_AUTOHELP
        POPT_TABLEEND
        };

	poptContext opt_context =
                poptGetContext(argv[0], argc, argv, opt_table, 0);
	int rc;

	while((rc = poptGetNextOpt(opt_context)) > 0){
		switch(rc){
			;
		}
	}

	globs.tree = poptGetArg(opt_context);
	if (globs.tree == 0){
		cerr << "ERROR: no tree" << endl;
		exit(1);
	}
	globs.field = poptGetArg(opt_context);
	if (globs.field == 0){
		cerr << "ERROR: no field" << endl;
		exit(1);
	}

	if (globs.c2){
		globs.nchan = globs.c2 - globs.c1 + 1; // inclusive
	}else{
		globs.c2 = globs.nchan;
	}

	if (getenv("VERBOSE")) verbose = atoi(getenv("VERBOSE"));	
}

int channel_segs[100];

static bool getChan(Tree *tree, int iseg, int ch, FILE* fp)
{
	char node_name[128];
	snprintf(node_name, 128, globs.field, ch);
	bool fault = false;

	try {	
		TreeNode *node = tree->getNode(node_name);

		try {
			if (channel_segs[ch] == 0){
				channel_segs[ch] = node->getNumSegments();
				if (globs.nsegs == 0){
					if (acq200_debug){
						printf("set nsegs %d\n", 
							      channel_segs[ch]);
					}
					globs.nsegs = channel_segs[ch];
				}
			}
			if (iseg >= channel_segs[ch]){
				if (acq200_debug) printf("past end\n");
				fault = true;
				goto cleanup;
			}
			
			Array *data = node->getSegment(iseg);
			int numElements;

			short *shorts = data->getShortArray(&numElements);

			if (verbose){
				cout << "numElements:" << numElements << endl;
			}

			fwrite(shorts, sizeof(short), numElements, fp);
			deleteData(data);
		} catch(...){
			cerr << "ERROR node->getSegment:" << iseg << endl;
			throw 37;
		}
	cleanup:
		deleteData(node);
	} catch(...){
		cerr << "ERROR tree->getNode:" <<node_name << endl;
		exit(1);
	}
	return fault;
}

static bool getSeg(Tree *tree, int iseg, FILE** fpa)
{
	if (acq200_debug) printf("getSeg(%d)\n", iseg);
	try {
		bool fault;
		for (int ch = globs.c1; ch <= globs.c2; ++ch){
			fault = getChan(tree, iseg, ch, fpa[ch]);
			if (fault){
				return true;
			}		
		}			
		return false;
	}catch(...){
		 cerr << "getSeg() dropping out at:" << iseg << endl;
	}
	return true;
}

static FILE** createOutputFiles(void)
{
	FILE **fpa = new FILE* [globs.nchan+1];	/* index from 1 */

	for (int ch = globs.c1; ch <= globs.c2; ++ch){
		char outname[80];
		sprintf(outname, "%02d.raw", ch);

		fpa[ch] = fopen(outname, "w");
		if (fpa[ch] == 0){
			perror(outname);
			exit(errno);
		}
	}

	return fpa;
}

static void closeOutputFiles(FILE **fpa)
{
	for (int ch = globs.c1; ch <= globs.c2; ++ch){
		fclose(fpa[ch]);
	}
	delete [] fpa;
}

static int pullData() {
	try {
		/* Tree signature should be const char* */
		char *tree_tmp = new char[strlen(globs.tree)+1];
		strcpy(tree_tmp, globs.tree);
		Tree *tree = new Tree(tree_tmp, globs.shot);

		FILE **fpa = createOutputFiles();

		for (int iseg = 0; iseg <= globs.nsegs; ++iseg){
			if (getSeg(tree, iseg, fpa)){
				break;
			}
		}
	} catch(...) {
		cerr << "ERROR failed to open Tree \"" << globs.tree <<
			"\"" << endl;
		return 1;
	}
}
int main(int argc, const char **argv)
{
	processOpts(argc, argv);

	return pullData();
}
