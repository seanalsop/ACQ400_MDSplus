/* ------------------------------------------------------------------------- */
/* file test-seg-push.cpp                                                    */
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

/** @file test-seg-push.cpp exercise MDSplus segmented push
 *
 * test-seg-push [opts] TREE FIELD
 *
 * --nchan	channels [96]
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

struct Globs {
	int nchan;
	int len;
	int nsegs;
	int prf;
	int isr;
	const char *tree;
	const char *field;
};

struct Globs globs = {
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
}

short *test_data;

static void getData() {
	test_data = new short[globs.len];
	
	double W = 2*M_PI * 10/globs.len;

	for (int ii = 0; ii != globs.len; ++ii){
		double volts = sin(W*ii);
		test_data[ii] = volts * 32760;
	}	
}

static void storeChan(
	Tree *tree, Data* start, Data* end, Data* dimension, int ch)
{
	char node_name[128];
	snprintf(node_name, 128, globs.field, ch);
	
	TreeNode *node = tree->getNode(node_name);
	Array *data = new Int16Array(test_data, globs.len);

	node->beginSegment(start, end, dimension, data);

	deleteData(data);
	deleteData(node);
}

static void storeSeg(Tree *tree, int iseg)
{
	double startsec = (double)iseg / globs.prf;
	Data* start = new Float64(startsec);
	double isr = (double)globs.isr * 1000;
	double lensec = (double)globs.len / isr;
	Data* end   = new Float64(startsec + lensec);
	Data* dimension = new Range(start, end, new Float64(1.0/isr));

	for (int ch = 1; ch < globs.nchan; ++ch){
		storeChan(tree, start, end, dimension, ch);
	}		
	
	deleteData(dimension);
}
static int storeData() {
	try {
		/* Tree signature should be const char* */
		char *tree_tmp = new char[strlen(globs.tree)+1];
		strcpy(tree_tmp, globs.tree);
		Tree *tree = new Tree(tree_tmp, 0, "EDIT");

		for (int iseg = 0; iseg < globs.nsegs; ++iseg){
			storeSeg(tree, iseg);
		}
		tree->write();
	} catch(...) {
		cerr << "ERROR failed to open Tree \"" << globs.tree <<
			"\"" << endl;
		return 1;
	}
}
int main(int argc, const char **argv)
{
	processOpts(argc, argv);
	getData();

	return storeData();
}
