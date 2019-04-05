import json
import argparse


def read_txt(sentences):
    sentences = sentences.split("\n")
    pairs = dict()
    for pair in sentences:
        if pair:
            rec = pair.split("\t")
            if pairs.get(int(rec[0])):
                pairs[int(rec[0])].append(int(rec[1]))
            else:
                pairs[int(rec[0])] = [int(rec[1])]
    return pairs


def inv(map_dict):
    s1_dict = {}
    s2_dict = {}
    for (key, value) in map_dict[0].items():
        s1_dict[value] = int(key)
    for (key, value) in map_dict[1].items():
        s2_dict[value] = int(key)
    return [s1_dict, s2_dict]


parser = argparse.ArgumentParser()
parser.add_argument('-dataset', required=True)
parser.add_argument('-block', type=int, default=5000)
opt = parser.parse_args()

dataset = opt.dataset
block = opt.block

with open("../../data/%s/match.txt" % dataset, "r+", encoding="utf-8") as f:
    match = f.read()
    pairs = read_txt(match)

for i in range(block):
    i += 1
    with open("../../data_block/%s/%d/%d/block_map.json" % (dataset, block, i), "r+", encoding="utf-8") as f:
        map_list = json.loads(f.read())
        inv_map_list = inv(map_list)

    new_match = []
    new_match_ret = ""
    ret_str = ""

    for (id1, total_id1) in map_list[0].items():
        total_id2_list = pairs.get(total_id1)
        if total_id2_list:
            for total_id2 in total_id2_list:
                new_match.append([total_id1, total_id2])

    for mat in new_match:
        total_id1 = mat[0]
        total_id2 = mat[1]
        id1 = inv_map_list[0].get(total_id1)
        id2 = inv_map_list[1].get(total_id2)

        if (id1 != None) and (id2 != None):
            new_match_ret += (str(id1) + "\t" + str(id2) + "\n")
            ret_str += (str(total_id1) + "\t" + str(total_id2) + "\n")

    with open("../../data_block/%s/%d/%d/matcher.txt" % (dataset, block, i), "w+", encoding="utf-8") as f:
        f.write(new_match_ret)

    with open("../../data_block/%s/%d/%d/origin_matcher.txt" % (dataset, block, i), "w+", encoding="utf-8") as f:
        f.write(ret_str)
