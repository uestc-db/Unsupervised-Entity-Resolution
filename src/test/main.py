import argparse
from datetime import datetime
import os
import sys
sys.path.append("..")
from util.calculate_precision import cal_prec
from util.cal_score import cal_sco
from util.cal_result import cal_res

parser = argparse.ArgumentParser()
parser.add_argument('-dataset', required=True)
parser.add_argument('-block', type=int)
opt = parser.parse_args()

dataset = opt.dataset
blocks = opt.block
start = datetime.now()

os.system("python3 ../util/corpus_get_idf.py -dataset %s" % dataset)
os.system("python3 ../util/get_pid_score.py -dataset %s -block %d" % (dataset, blocks))

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

cal_prec(dataset, blocks)
cal_sco(dataset, blocks)
cal_res(dataset, blocks)
