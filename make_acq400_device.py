#!/usr/bin/python


import MDSplus
import make_acqtree # want to use path_check function.
import acq400_hapi
import argparse


def get_args():
    parser = argparse.ArgumentParser(description="make_acqtree with MDSplus device support.")
    parser.add_argument('--model', default='tr', type=str, help='tr, st or mr.')
    parser.add_argument('--name', default='default', type=str, help='Name of device.')
    parser.add_argument('tree', nargs=1, help="tree name (UUT name)")
    return parser.parse_args()


def make_device(tname, args):
    tree = MDSplus.Tree(tname, -1, "NEW")
    uut = acq400_hapi.Acq400(args.tree[0])
    nchan = uut.nchan()
    carrier_type = args.tree[0][0:7]

    model = "{}_{}_{}".format(carrier_type, args.model, nchan) # e.g. acq2106_32_tr
    tree.addDevice(args.name, model)
    tree.write()
    
    tree = MDSplus.Tree(tname, -1, "EDIT")

    # Change node name to tree name
    node = tree.getNode(args.name).getNode("node")

    node.putData(str(args.tree[0]))

    # Create pulse 0.
    tree.createPulse(1)
    tree.setCurrent(1)
    return None


def main():
    args = get_args()
    tname = args.tree[0]
    make_acqtree.path_check(tname)
    make_device(tname, args)
    print("Device created.")
    return None


if __name__ == '__main__':
    main()

