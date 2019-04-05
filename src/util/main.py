import argparse
from datetime import datetime
import os

parser = argparse.ArgumentParser()
parser.add_argument('-dataset', required=True)
parser.add_argument('-block', type=int)
opt = parser.parse_args()

dataset = opt.dataset
blocks = opt.block

start = datetime.now()
for block in range(1, blocks+1):
    os.system("g++ -fopenmp %s_matrix_block.cpp -o %s_matrix_block" % (dataset, dataset))
    os.system("./%s_matrix_block %d %d" % (dataset, block, blocks))
end = datetime.now()
total_seconds = (end - start).seconds

hours = total_seconds // 3600
total_seconds = total_seconds % 3600
minutes = total_seconds // 60
seconds = total_seconds % 60

print("Time: %d : %d : %d" % (hours, minutes, seconds))
