import json


def read_txt(sentences):
    sentences = sentences.split("\n")
    pairs = []
    for pair in sentences:
        if pair:
            rec = pair.split("\t")
            pairs.append([int(rec[0]), int(rec[1])])
    return pairs


def cal_sco(dataset, blocks):
    with open("../../data_block/%s/%d/all_precision.json" % (dataset, blocks), "r+", encoding="utf-8") as f:
        all_precision = json.loads(f.read())

    with open("../../data/%s/match.txt" % dataset, "r+", encoding="utf-8") as f:
        match = f.read()
        pairs = read_txt(match)

    count = 0
    for pair in all_precision:
        if pair in pairs:
            count += 1

    total_pair = len(all_precision)
    matches = len(pairs)

    precision = 1.0 * count / total_pair
    recall = 1.0 * count / matches
    f1 = 2 * precision * recall / (precision + recall)

    print("==========result============")
    print("total_pair: ", total_pair, "matches: ", matches, "count: ", count)
    print("precision: ", round(precision*100, 2))
    print("recall: ", round(recall*100, 2))
    print("f1_score: ", round(f1*100, 2))

    return round(f1*100, 2)
