#!/usr/bin/python3

"""
Example:
./jscope-flare-gen.py --LHS="$LHS" --RHS="$RHS" $UUTS > ~/jScope/configurations/FLARE_2COL.jscp
"""

import MDSplus
import argparse
import os


def get_args():
    parser = argparse.ArgumentParser(description="jScope file creator.")
    parser.add_argument('--LHS', default="default", type=str, help="")
    parser.add_argument('--RHS', default="default", type=str, help="")
    parser.add_argument('uuts', nargs='+', help="uut list")
    return parser.parse_args()


def get_default_jscp():
    """
    Retrieves a default jscp file to use as a template.
    """
    user = os.getlogin()
    with open('/home/{}/jScope/configurations/default.jscp'.format(user)) as f:
        text = f.read()
    return text


def create_new_jscp(args, text):
    col = 1
    new_text = ""

    LHS = len(args.LHS) if args.LHS != "default" else len(args.uuts) - len(args.uuts)//2
    RHS = len(args.RHS) if args.RHS != "default" else len(args.uuts)//2

    text += "Scope.plot_1_1.x_expr_1: \TOP:TRANSIENT1:TB_NS\n"
    text += "Scope.plot_1_1.y_expr_1: \TOP:TRANSIENT1:INPUT_001:CAL_INPUT\n"
    text += "Scope.plot_1_1.num_shot: 1\n"

    text = text.replace("Scope.rows_in_column_2: 1",
                        "Scope.rows_in_column_2: {}".format(LHS))
    text = text.replace("Scope.rows_in_column_1: 1",
                        "Scope.rows_in_column_1: {}".format(RHS))
    text = text.replace("Scope.plot_1_1.num_expr: 0",
                        "Scope.plot_1_1.num_expr: 1")
    text = text.replace("Scope.plot_1_1.global_defaults: -1",
                        "Scope.plot_1_1.global_defaults: 196353")

    for line in text.split("\n"):
        if line.startswith("Scope.plot_1_1"):
            line_to_add = line.replace("1_1", "{index}_{column}")
            new_text += line_to_add + "\n"

    row = 0
    for num, uut in enumerate(args.uuts):
        if num == 8:
            col = 2
            row = 0

        if num != 0:
            text += new_text.format(index=row+1, column=col)
        text += "Scope.plot_{}_{}.title: '{} shot: '//$SHOT\n".format(
            row+1, col, uut.upper())
        text += "Scope.plot_{}_{}.experiment: {}\n".format(row+1, col, uut.upper())
        #text += "Scope.plot_{}_1.num_expr: {}\n".format(row+1, num+1)
        text += "Scope.plot_{}_{}.mode_1D_1_1: Line\n".format(row+1, col)
        text += "Scope.plot_{}_{}.mode_2D_1_1: xz(y)\n".format(row+1, col)
        text += "Scope.plot_{}_{}.color_1_1: 0\n".format(row+1, col)
        text += "Scope.plot_{}_{}.marker_1_1: 0\n".format(row+1, col)
        text += "Scope.plot_{}_{}.step_marker_1_1: 1\n\n".format(row+1, col)
        row += 1
    print(text)
    return None


def main():
    args = get_args()
    text = get_default_jscp()
    if not args.LHS == "default" and not args.RHS == "default":
        args.LHS = args.LHS.split(" ")
        args.RHS = args.RHS.split(" ")
    create_new_jscp(args, text)


if __name__ == '__main__':
    main()
