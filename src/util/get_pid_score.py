import os
import argparse
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

print("[Info] create pid_score file")

for block in range(1, blocks + 1):
    block_dir = os.path.join("../..", "data_block", dataset, str(blocks), str(block))
    if os.path.exists(os.path.join(block_dir, "pid_score.txt")):
        print("[Info] pid_score ", block, " is exist")
        continue
    print("[Info] block ", block)

    source_1 = read_txt(os.path.join(block_dir, "source_1.txt"))
    source_2 = read_txt(os.path.join(block_dir, "source_2.txt"))
    source = source_1 + source_2

    N1 = len(source_1)
    N2 = len(source_2)
    N = N1 + N2

    tfidf_vectorizer = TfidfVectorizer(token_pattern=r"[a-zA-Z0-9\-\.\'\*\#\$\%\&]+")
    transformer = TfidfTransformer()
    tfidf = transformer.fit_transform(tfidf_vectorizer.fit_transform(source))
    weight = tfidf.toarray()
    print("vocabulary: ", len(tfidf_vectorizer.vocabulary_))

    simMatrix = (tfidf * tfidf.T).A

    ret_list = []

    for i in range(N1):
        for j in range(N1):
            simMatrix[i, j] = 0

    for i in range(N1, N1+N2):
        for j in range(N1, N1+N2):
            simMatrix[i, j] = 0

    for i in range(N):
        for j in range(N):
            pid = i * N + j
            ret_list.append("%d %f" % (pid, simMatrix[i, j]))

    with open(os.path.join(block_dir, "pid_score.txt"), "w+", encoding="utf-8") as f:
        f.write("\n".join(ret_list) + "\n")
