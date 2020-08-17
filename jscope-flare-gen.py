#!/usr/bin/python3


import MDSplus
import argparse
import os


def get_args():
    parser = argparse.ArgumentParser(description="jScope file creator.")
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
    new_text = ""
    text += "Scope.plot_1_1.x_expr_1: \TOP:TRANSIENT1:TB_NS\n"
    text += "Scope.plot_1_1.y_expr_1: \TOP:TRANSIENT1:INPUT_001:CAL_INPUT\n"
    text += "Scope.plot_1_1.num_shot: 1\n"

    text = text.replace("Scope.rows_in_column_1: 1",
                        "Scope.rows_in_column_1: {}".format(len(args.uuts)))
    text = text.replace("Scope.plot_1_1.num_expr: 0",
                        "Scope.plot_1_1.num_expr: 1")
    text = text.replace("Scope.plot_1_1.global_defaults: -1",
                        "Scope.plot_1_1.global_defaults: 196353")

    for line in text.split("\n"):
        if line.startswith("Scope.plot_1_1"):
            line_to_add = line.replace("1_1", "{index}_1")
            new_text += line_to_add + "\n"

    for num, uut in enumerate(args.uuts):
        if num != 0:
            text += new_text.format(index=num+1)
        text += "Scope.plot_{}_1.title: '{} shot: '//$SHOT\n".format(
            num+1, uut.upper())
        text += "Scope.plot_{}_1.experiment: {}\n".format(num+1, uut.upper())
        #text += "Scope.plot_{}_1.num_expr: {}\n".format(num+1, num+1)
        text += "Scope.plot_{}_1.mode_1D_1_1: Line\n".format(num+1)
        text += "Scope.plot_{}_1.mode_2D_1_1: xz(y)\n".format(num+1)
        text += "Scope.plot_{}_1.color_1_1: 0\n".format(num+1)
        text += "Scope.plot_{}_1.marker_1_1: 0\n".format(num+1)
        text += "Scope.plot_{}_1.step_marker_1_1: 1\n\n".format(num+1)

    print(text)
    return None


def main():
    args = get_args()
    text = get_default_jscp()
    create_new_jscp(args, text)


if __name__ == '__main__':
    main()
