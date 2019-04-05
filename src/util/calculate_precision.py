import json
import os


def read_txt(sentences):
    sentences = sentences.split("\n")
    pairs = []
    for pair in sentences:
        if pair:
            rec = pair.split("\t")
            pairs.append([int(rec[0]), int(rec[1])])
    return pairs


def read_precision(sentences, map_dict):
    sentences = sentences.split("\n")
    pairs = []
    all_precision = []
    for pair in sentences:
        if pair:
            rec = pair.split("\t")
            if rec:
                r1 = map_dict[0][rec[0]]
                r2 = map_dict[1][rec[1]]
                item = [r1, r2]
                if item not in all_precision:
                    all_precision.append(item)
    return pairs, all_precision


def read_dir(path):
    block_file_path = os.path.join(path, "block_map.json")
    prec_file_path = os.path.join(path, "precision.txt")

    if os.path.exists(block_file_path):
        with open(block_file_path, "r+", encoding="utf-8") as f:
            map_dict = json.loads(f.read())

        if os.path.exists(prec_file_path):
            with open(prec_file_path, "r+", encoding="utf-8") as f:
                s = f.read()
                pairs, precisions = read_precision(s, map_dict)
            return precisions
        else:
            print(prec_file_path, "doesn't exist")
    else:
        print(block_file_path, "doesn't exist")
    return None


def cal_prec(dataset, blocks):
    path = "../../data_block/%s/%d" % (dataset, blocks)

    all_precision = []
    for i in range(1, blocks+1):
        block_path = os.path.join(path, str(i))
        precisions = read_dir(block_path)
        all_precision += precisions

    with open("../../data_block/%s/%d/all_precision.json" % (dataset, blocks), "w+", encoding="utf-8") as f:
        f.write(json.dumps(all_precision))
