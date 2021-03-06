#!/usr/bin/env python

import argparse
import os
from MDSplus import *

idnames = ("MAG_%d", "phi_%d", "PWR_%d")
idunits = ("V", "rad", "W")
idcal = ("7.109e-8", "1.8626e-9", "4.550e-6")


def add_ohmic_heating(module, modpath, tree):
    # for num, ch in enumerate(range(3, 24+1, 3)):
    for bc in range(1, 8+1):  # bolo channel
        rc = 3*bc            # raw channel
        # add the Ioh and Voh data.
        cooked = module.addNode("IOH_%d" % bc, "SIGNAL")
        expr = "(%s.CH%02d & 65535) * 2.604e-6" % (modpath, rc)
        print expr
        cooked.putData(tree.tdiCompile(expr))
        cooked = module.addNode("VOH_%d" % bc, "SIGNAL")
        expr = "%s.CH%02d / 65536 * 3.05e-5" % (modpath, rc)
        print expr
        cooked.putData(tree.tdiCompile(expr))
    return None


def run_make_acqtree(args):
    return_val = os.system('python make_acqtree.py {}'.format(args.tree[0]))
    if return_val != 0:
        exit()
    else:
        return None


def run_new_shot(args):
    return_val = os.system('python new_shot {}'.format(args.tree[0]))
    if return_val != 0:
        exit()
    else:
        return None


def make_bolo_tree(args):
    run_make_acqtree(args)

    tree = Tree(args.tree[0], -1, "NEW")

    for site in range(1, args.bolo8_count+1):
        bname = "BOLO%d" % (site)
        module = tree.addNode(".%s" % (bname))
        modpath = "\\%s::TOP.%s" % (args.tree[0], bname)

        for ch in range(1, 24+1):
            rawname = "CH%02d" % (ch)
            raw = module.addNode(rawname, "SIGNAL")
            bchan = 1 + (ch - 1)/3
            id = (ch - 1) % 3
            cooked = module.addNode(idnames[id] % (bchan), "SIGNAL")
            expr = "%s.%s * %s" % (modpath, rawname, idcal[id])
            print(expr)
            cooked = tree.getNode(cooked)
            expr = tree.tdiCompile(expr)
            cooked.putData(expr)
            cooked.setUnits(idunits[id])
        add_ohmic_heating(module, modpath, tree)
    tree.write()
    run_new_shot(args)
    return None


def run_main():
    parser = argparse.ArgumentParser(description="make_bolo_tree")
    parser.add_argument('--bolo8_count', default=1,
                        type=int, help="number of bolo8 modules")
    parser.add_argument('tree', nargs=1, help="tree name")
    make_bolo_tree(parser.parse_args())
# execution starts here


if __name__ == '__main__':
    run_main()
