import random
import argparse
import numpy
import math
import os
import json
from tqdm import tqdm
from sklearn.feature_extraction.text import TfidfTransformer, TfidfVectorizer


def read_txt(filename):
    with open(filename, "r+", encoding="utf-8") as f:
        a = f.read()
    a_list = a.split("\n")
    if not a_list[-1]:
        a_list = a_list[:-1]
    return a_list


parser = argparse.ArgumentParser()
parser.add_argument('-dataset', required=True)
parser.add_argument('-block', type=int)
opt = parser.parse_args()

dataset = opt.dataset
blocks = opt.block

source_1 = read_txt("../../data/%s/source_1.txt" % dataset)
source_2 = read_txt("../../data/%s/source_2.txt" % dataset)

N1 = len(source_1)
N2 = len(source_2)

source = source_1 + source_2

tfidf_vectorizer = TfidfVectorizer(token_pattern=r"[a-zA-Z0-9\-\.\'\*\#\$\%\&]+")
transformer = TfidfTransformer()
tfidf = transformer.fit_transform(tfidf_vectorizer.fit_transform(source))
weight = tfidf.toarray()
print("vocabulary: ", len(tfidf_vectorizer.vocabulary_))

simMatrix = (tfidf * tfidf.T).A

candidate_list = []
TbTaMatrix = simMatrix[N1: (N1+N2), :N1]
TbMatrix = simMatrix[:N1, N1: (N1+N2)]
TaMatrix = simMatrix[:N1, :N1]

for i in range(N1):
    TaMatrix[i, i] = 0

remain_s1_list = [i for i in range(N1)]

s1tos2 = dict()
s2tos1 = dict()

remain_N1 = N1
remain_block = blocks

s1_block_dict = dict()

for i in tqdm(range(blocks)):
    per_block = math.ceil(remain_N1 / remain_block)
    if not remain_s1_list:
        continue
    seed = random.sample(remain_s1_list, 1)[0]
    block_s1_list = set()
    block_s2_list = set()
    remain_s1_list.remove(seed)
    block_s1_list.add(seed)
    TaMatrix[:, seed] = -1
    for j in range(int(per_block)-1):
        tempAList = TaMatrix[seed, :]
        idx = numpy.where(tempAList == numpy.max(tempAList))[0][0]
        idx = int(idx)
        if tempAList[idx] == -1:
            continue
        remain_s1_list.remove(idx)
        block_s1_list.add(idx)
        TaMatrix[:, idx] = -1
        seed = idx
    candidate_list.append([block_s1_list, []])
    for j in block_s1_list:
        s1_block_dict[j] = i

    remain_block = blocks - i - 1
    remain_N1 -= per_block


for i in range(N2):
    tempAList = TbTaMatrix[i, :]
    idx = numpy.where(tempAList == numpy.max(tempAList))[0][0]
    idx = int(idx)
    block_idx = s1_block_dict[idx]
    candidate_list[block_idx][1].append(i)

try:
    os.system("rm -r -f ../../data_block/%s/%d" % (dataset, blocks))
except Exception:
    pass

try:
    os.mkdir("../../data_block/%s/%d" % (dataset, blocks))

except Exception:
    pass

for i, candidate in enumerate(candidate_list):
    os.mkdir("../../data_block/%s/%d/%d" % (dataset, blocks, i+1))
    block_map = []
    src1_list = candidate[0]
    src2_list = candidate[1]

    print("===============================")
    print(len(src1_list))
    print(len(src2_list))

    block_map.append(dict(enumerate(src1_list)))
    block_map.append(dict(enumerate(src2_list)))

    src1 = [source[k] for k in src1_list]
    src2 = [source[N1+k] for k in src2_list]

    with open("../../data_block/%s/%d/%d/source_1.txt" % (dataset, blocks, i+1), "w+", encoding="utf-8") as f:
        f.write("\n".join(src1) + "\n")

    with open("../../data_block/%s/%d/%d/source_2.txt" % (dataset, blocks, i+1), "w+", encoding="utf-8") as f:
        f.write("\n".join(src2) + "\n")

    with open("../../data_block/%s/%d/%d/block_map.json" % (dataset, blocks, i+1), "w+", encoding="utf-8") as f:
        f.write(json.dumps(block_map))
