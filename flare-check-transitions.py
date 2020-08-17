#!/usr/bin/python3


import MDSplus
import argparse
import numpy as np
import matplotlib.pyplot as plt
import os


def get_args():
    parser = argparse.ArgumentParser(description="jScope file creator.")
    parser.add_argument('--shot', default=0, type=int,
                        help='specify shot number')
    parser.add_argument('uuts', nargs='+', help="uut list")
    return parser.parse_args()


def get_mdsplus_data(uuts, shot):
    uut_data = []
    for uut in uuts:
        data = MDSplus.Tree(uut, shot)
        data = data.TRANSIENT1.INPUT_001.CAL_INPUT.data()
        uut_data.append(data)
    return np.array(uut_data)


def compare_data(data, uuts):
    transition_locations = []
    for num, channel in enumerate(data):

        diffs = np.diff(channel)
        where = np.where(diffs < -0.04, 1, 0)
        for index, num in enumerate(where):
            if where[index-1] == 1:
                where[index] = 0
        transition_locations.append(where)

    for index, array in enumerate(transition_locations[1:]):
        if not np.array_equal(transition_locations[0], array):
            print("Found an error comparing uut: {} to {}".format(
                uuts[0], uuts[index+1]))
            exit(0)

    print("\nNo errors found.")
    return None


def main():
    args = get_args()
    data = get_mdsplus_data(args.uuts, args.shot)
    compare_data(data, args.uuts)


if __name__ == '__main__':
    main()
