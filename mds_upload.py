"""
mds_upload.py is a utility for uploading bulk raw data to an MDSplus server.

Some tips for usage:

Need to define <UUT name>_path=<server where data is stored>:://<path to UUT tree>
e.g. seg_tree_path=andros::///home/dt100/TREES/seg_tree

You must also pass the directory containing data to --data_dir at the
command line.
e.g. python mds_upload.py --data_dir=/data/<UUT name> <UUT name>
This mirrors the intended use of acq400_stream2.py (streaming to
/data/<UUT_name>).

This will upload segments of size 1kb to the specified node in the tree.

To upload to MDSplus on windows then use this to set path variable:
    $env:mds_test_path="andros:://home/dt100/TREES/mds_test"
"""


import numpy as np
import argparse
from MDSplus import *
import os


def upload_segment(segment, node, segID):
    start_time = Float64(segID)
    end_time = Float64(segID+1)
    delta = 1 / float((len(segment)))
    segment = Float32Array(segment)
    segDimension = Range(start_time, end_time, delta)
    node.makeSegment(start_time, end_time, segDimension, segment)


def upload_ch(data, node):
    data = Float32Array(data)
    node.putData(data)
    return None


def upload_data(args):
    # upload data to the MDSplus tree.
    segID = 0
    data = []
    tree = Tree(args.uuts[0], Tree.getCurrent(args.uuts[0]))


    if args.store_seg == 1 and args.store_chs == 1:
        print "This is an incompatible argument selection. Please either choose \n"
        "to store data in segments or to store by channel. To store by channel \n"
        "the data must first be stored to disk by channel by host_demux.py \n"
        return None

    if args.store_seg == 1:
        node = tree.getNode(args.node)
        directories = [name for name in os.listdir(args.data_dir) if os.path.isdir(os.path.join(args.data_dir, name))]
        for dir1 in directories:
            for file in os.listdir(args.data_dir + dir1):
                data = np.fromfile(args.data_dir + dir1 + "/" + file, dtype=np.int16)
                upload_segment(data, node, segID)
                segID += 1
                print "file uploaded"

    if args.store_chs == 1:
        args.data_dir = r"D:\{}".format(args.data_dir)
        list_of_dirs = os.listdir(args.data_dir)

        for enum, channel in enumerate(list_of_dirs): # channel == file.
            if channel == "format":
                continue
            print "loop"
            data = np.fromfile(str(args.data_dir) + "/" + channel, dtype=np.int16)

            if len(list_of_dirs) > 99:
                node = args.node + ":" + "CH" + "{:03d}".format(enum + 1)
            else:
                node = args.node + ":" + "CH" + "{:02d}".format(enum + 1)
            print node
            node = tree.getNode(node)
            upload_ch(data, node)
            print "file uploaded"


def run_upload(args):
    print "uut = ", args.uuts[0]
    upload_data(args)


def run_main():
    parser = argparse.ArgumentParser(description='acq400 MDSplus interface')
    parser.add_argument('--node', default="AI", type=str, help="Which node to pull data from")
    parser.add_argument('--store_seg', default=0, type=int, help="Whether to upload data as a segment or not.")
    parser.add_argument('--data_dir', default='/data/', type=str, help="")
    parser.add_argument('--verbose', default=0, type=int, help='Prints status messages as the data is being pulled.')
    # parser.add_argument('--data_dir', default="AI", type=str, help="Which node to pull data from")
    parser.add_argument('--store_chs', default=0, type=int, help="Whether to store channelized data to MDSplus.")
    parser.add_argument('uuts', nargs='+', help="uuts")
    run_upload(parser.parse_args())


if __name__ == '__main__':
    run_main()

