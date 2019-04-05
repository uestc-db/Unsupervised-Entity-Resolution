import os


def read_source(filename):
    with open(filename, "r+", encoding="utf-8") as f:
        a = f.read()
    a_list = a.split("\n")
    if not a_list[-1]:
        a_list = a_list[:-1]
    return a_list


def read_txt(sentences):
    sentences = sentences.split("\n")
    pairs = []
    for pair in sentences:
        if pair:
            rec = pair.split("\t")
            pairs.append([int(rec[0]), int(rec[1])])
    return pairs


def read_dir(path, pairs):
    in_match = []
    for root, dirs, files in os.walk(path, topdown=False):
        if files:
            s1_list = []
            s2_list = []
            map_path = os.path.join(root, "origin_matcher.txt")
            with open(map_path, "r+", encoding="utf-8") as f:
                find_matchers = read_txt(f.read())
                for i in find_matchers:
                    if i not in in_match:
                        in_match.append(i)
    return in_match


def upper_bou(dataset, blocks):
    data_path = "../../data/%s" % dataset
    path = "../../data_block/%s/%d" % (dataset, blocks)

    with open(data_path + "/match.txt", "r+", encoding="utf-8") as f:
        match = f.read()
        pairs = read_txt(match)

    in_match = read_dir(path, pairs)

    result = len(in_match) / len(pairs) * 100
    print("upper_bound: ", len(in_match), "/", len(pairs))
    print(len(in_match) / len(pairs))
    return result
