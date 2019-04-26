import argparse
from tqdm import tqdm
import math
import os

parser = argparse.ArgumentParser()
parser.add_argument('-dataset', required=True)
opt = parser.parse_args()

dataset = opt.dataset

if os.path.exists("../../data/%s/idf_file.txt" % dataset):
    exit()


def is_price(word):
    a = word.split(".")
    if not len(a) == 2:
        return False
    else:
        try:
            int(a[0])
            int(a[1])
        except Exception:
            return False
    return True


with open("../../data/wiki_2016.txt", "r+", encoding="utf-8") as f:
    wiki_2016 = f.read()

with open("../../data/wiki_2012.txt", "r+", encoding="utf-8") as f:
    wiki_2012 = f.read()

with open("../../data/wiki_2010.txt", "r+", encoding="utf-8") as f:
    wiki_2010 = f.read()

with open("../../data/%s/source_1.txt" % dataset, "r+", encoding="utf-8") as f:
    s1 = f.read()

with open("../../data/%s/source_2.txt" % dataset, "r+", encoding="utf-8") as f:
    s2 = f.read()

corpus = wiki_2016 + wiki_2012 + wiki_2010
corpus = corpus.split("\n")

dataset_corpus = s1 + s2
dataset_corpus = dataset_corpus.split("\n")

vocabulary = dict()
dataset_vocabulary = dict()

for sentence in tqdm(corpus):
    words = set(sentence.split(" "))
    for word in words:
        if vocabulary.get(word):
            vocabulary[word] += 1
        else:
            vocabulary[word] = 1

for sentence in tqdm(dataset_corpus):
    words = set(sentence.split(" "))
    for word in words:
        if dataset_vocabulary.get(word):
            dataset_vocabulary[word] += 1
        else:
            dataset_vocabulary[word] = 1

print("corpus_word_num:", len(vocabulary))
print("dataset_word_num:", len(dataset_vocabulary))

length = len(corpus)
vocab_idf = dict()
dataset_vocab_idf = dict()

for (k, v) in vocabulary.items():
    vocab_idf[k] = round(math.log10(length / v), 6)

for (k, v) in dataset_vocabulary.items():
    if is_price(k):
        print(k)
        dataset_vocab_idf[k] = 3
    else:
        if vocab_idf.get(k):
            dataset_vocab_idf[k] = vocab_idf[k]
        else:
            dataset_vocab_idf[k] = 0
        # dataset_vocab_idf[k] = vocab_idf[k]

ret = ""

for (k, v) in dataset_vocab_idf.items():
    ret += "%s %s\n" % (k, str(v))

with open("../../data/%s/idf_file.txt" % dataset, "w+", encoding="utf-8") as f:
    f.write(ret)
