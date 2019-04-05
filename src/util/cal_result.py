import json
import csv


def readcsv(filename):
    with open(filename, encoding="ISO-8859-1") as f:
        reader = csv.reader(f)
        return list(reader)


def cal_res(dataset, blocks):
    candidate_sets = readcsv("../../data/Textual/%s/candidate.csv" % dataset)[1:]

    with open("../../data_block/%s/%d/all_precision.json" % (dataset, blocks), "r+", encoding="utf-8") as f:
        precisions = json.loads(f.read())

    precision_tuple = set()
    for pre in precisions:
        precision_tuple.add(tuple(pre))

    candidate_tuple = {}
    true_tuple = set()
    for can in candidate_sets:
        candidate_tuple[tuple([int(can[1]), int(can[2])])] = can[3]
        if can[3] == '1':
            true_tuple.add(tuple([int(can[1]), int(can[2])]))

    delete_tuple = []
    for pre in precision_tuple:
        if not candidate_tuple.get(pre):
            delete_tuple.append(pre)

    for delete in delete_tuple:
        precision_tuple.remove(delete)

    count = len(precision_tuple & true_tuple)

    len_matches = len(true_tuple)

    len_precision = len(precision_tuple)

    precision = 1.0 * count / len_precision
    recall = 1.0 * count / len_matches
    f1 = 2 * precision * recall / (precision + recall)

    print("==============candidate set result=================")
    print("precision: ", round(precision*100, 2))
    print("recall: ", round(recall*100, 2))
    print("f1_score: ", round(f1*100, 2))
    return round(f1*100, 2)
