import os
import sys
sys.path.append("..")
import argparse
from util.upper_bound import upper_bou


def blo_main(dataset, blocks):
    os.system("python3 ../util/tfidf-blocking.py -dataset %s -block %d" % (dataset, blocks))
    os.system("python3 ../util/find_block_match.py -dataset %s -block %d" % (dataset, blocks))
    result = upper_bou(dataset, blocks)
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-dataset', required=True)
    parser.add_argument('-block', required=True, type=int)
    opt = parser.parse_args()

    dataset = opt.dataset
    blocks = opt.block

    blo_main(dataset, blocks)
