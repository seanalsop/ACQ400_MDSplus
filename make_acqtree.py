#!/usr/bin/env python

import argparse
import MDSplus
import os

def make_chan(tree, nchan, id):
    subdir = tree.addNode(".{}".format(id))
    chfmt = "CH{:0" + "{}".format('3' if nchan > 99 else '2') + "}"

    for ch in range(1, nchan+1):
        subdir.addNode(chfmt.format(ch), "SIGNAL")

def path_check(tname):
    root = os.getenv("MDS_TREE_ROOT", "{}/TREES".format(os.environ['HOME']))
    key = "{}_path".format(tname)
    tpath = "{}/{}".format(root, tname)
    mpath = os.getenv(key, "notfound")
    if mpath == "notfound":
        print("run as root:")
        print('echo "{} {}" >> {}'.
		format(key, tpath, "/usr/local/mdsplus/local/envsyms"))
	exit(1)
    
    if not os.path.exists(root):
        print('mkdir {}'.format(root))
	exit(1)

    if os.path.exists(tpath):
        print('existing tree {} may already exist. Delete it'.format(tpath))
	exit(1)
    else:
        os.mkdir(tpath)
    

def make_acqtree(args):
    tname = args.tree[0]
    path_check(tname)
    tree = MDSplus.Tree(tname, -1, "NEW")
    
    if args.aichan > 0:
	make_chan(tree, args.aichan, "AI")
    if args.aochan > 0:
        make_chan(tree, args.aochan, "AO")
    if args.dio > 0:
        make_chan(tree, args.dio, "DIO")
    if args.stat > 0:
        make_chan(tree, args.stat, "ST")
    tree.write()


def run_main():
    parser = argparse.ArgumentParser(description="make_acqtree")
    parser.add_argument('--aichan', default=0, type=int, help='ai channel count')
    parser.add_argument('--aochan', default=0, type=int, help='ao channel count')
    parser.add_argument('--dio', default=0, type=int, help='dio, words')
    parser.add_argument('--stat', default=0, type=int, help='status, words')
    parser.add_argument('tree', nargs=1, help="tree name, ideally UUT name")
    make_acqtree(parser.parse_args())


if __name__ == '__main__':
    run_main()

