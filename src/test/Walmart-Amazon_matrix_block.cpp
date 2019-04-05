#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "../util/io.h"
#include "../util/util.h"
#include "../util/measure.h"
#include "../util/BipartiteMatrix.h"
#include "../util/RandomWalk.h"
#include "../util/CliqueRankMatrix.h"

using namespace std;

int getWordId(string word, map<string,int>&word_id) {
    if (word_id.find(word) != word_id.end()) {
        return word_id[word];
    }

    for (map<string, int>::iterator iter = word_id.begin(); iter != word_id.end(); iter++) {
        if (Similarity::isADSimilar(word, iter->first)) {
            return iter->second;
        }
    }
    return word_id.size();
}

void str2int(int &int_temp,const string &string_temp) {
    stringstream stream(string_temp);
    stream >> int_temp;
}

int getWordIdNoAdd(string word, map<string,int>&word_id) {
    if (word_id.find(word) != word_id.end()) {
        return word_id[word];
    } else {
        return -1;
    }
}

double stringToDouble(string num) {
    bool minus = false;
    string real = num;
    if (num.at(0) == '-') {
        minus = true;
        real = num.substr(1, num.size() - 1);
    }

    char c;
    int i = 0;
    double result = 0.0, dec = 10.0;
    bool isDec = false;
    unsigned long size = real.size();
    while (i < size) {
        c = real.at(i);
        if (c == '.') {
            isDec = true;
            i++;
            continue;
        }
        if (!isDec) {
            result = result * 10 + c - '0';
        } else {
            result = result + (c - '0') / dec;
            dec *= 10;
        }
        i++;
    }
    if (minus) {
        result = -result;
    }
    return result;
}


double precison, recall, f1, score_threshold;

void calPrecision(int N1, int N2, int N, Bipartite* bigraph, CliqueRankMatrix* walker, double eta, set<string>& matches) {
    // get the average similarity score
    score_threshold = 0;
    int tmp_count = 0;
    for (int i = 0; i < N1; i++) {
        int id1 = i;
        for (int j = 0; j < N2; j++) {
            int id2 = j + N1;
            double weight = walker->p_conf(id1, id2);
            if (weight >= eta * 0.6) {
                score_threshold += 0.6 * bigraph->p_score[id1 * N + id2];
                tmp_count++;
            }

        }
    }
    score_threshold /= tmp_count;

    int count = 0, total_pair = 0;
    for (int i = 0; i < N1; i++) {
        int id1 = i;
        for (int j = 0; j < N2; j++) {
            int id2 = j + N1;
            stringstream ss;
            ss << id1 << "_" << (id2 - N1);
            if (walker->p_conf(id1, id2) >= eta && bigraph->p_score[id1 * N + id2] >= score_threshold) {
                total_pair++;
                if (matches.find(ss.str()) != matches.end()) {
                    count++;
                }
            }
        }
    }
    precison = 1.0 * count / total_pair;
    recall = 1.0 * count / matches.size();
    f1 = 2 * precison * recall / (precison + recall);
    cout << count << "\t" << total_pair << "\t" << matches.size() << endl;
}

void final(int N1, int N2, int N, Bipartite* bigraph, CliqueRankMatrix* walker,
           double eta, string outfilename, set<string>& matches) {
    ofstream fout(outfilename.c_str());
    // get the average similarity score
    score_threshold = 0;
    int tmp_count = 0;
    for (int i = 0; i < N1; i++) {
        int id1 = i;
        for (int j = 0; j < N2; j++) {
            int id2 = j + N1;
            double weight = walker->p_conf(id1, id2);
            if (weight >= eta * 0.6) {
                score_threshold += 0.6 * bigraph->p_score[id1 * N + id2];
                tmp_count++;
            }
        }
    }
    score_threshold /= tmp_count;

    int count = 0, total_pair = 0;
    for (int i = 0; i < N1; i++) {
        int id1 = i;
        for (int j = 0; j < N2; j++) {
            int id2 = j + N1;
            stringstream ss;
            ss << id1 << "_" << (id2 - N1);
            if (walker->p_conf(id1, id2) >= eta && bigraph->p_score[id1 * N + id2] >= score_threshold) {
                fout << id1 << "\t" << (id2 - N1) << endl;
                total_pair++;

                if (matches.find(ss.str()) != matches.end()) {
                    count++;
                }
            }

        }
    }
    precison = 1.0 * count / total_pair;
    recall = 1.0 * count / matches.size();
    f1 = 2 * precison * recall / (precison + recall);
    cout << count << "\t" << total_pair << "\t" << matches.size() << endl;
}


/*
 * Construct the sorted features (words) for each dataset.
 *
 */
int main(int argc, char** argv) {
    int scale = 2;
    int alpha = 20;
    int S = 20;
    float eta = 0.98;

    string block;
    string blocks;
    block = argv[1];
    blocks = argv[2];
    cout << "[Info]Handle Block " << block << endl;
    // read the data source file
    string dataset = "Walmart-Amazon";
    string outfilename;
    outfilename = "../../data_block/" + dataset + "/" + blocks + "/" + block + "/precision.txt";
    cout << "outfile: " << outfilename << endl;
    vector<string> source1 = FileIO::readFileLines(
            "../../data_block/" + dataset + "/" + blocks + "/" + block + "/source_1.txt");
    vector<string> source2 = FileIO::readFileLines(
            "../../data_block/" + dataset + "/" + blocks + "/" + block + "/source_2.txt");
    int N1 = source1.size();
    int N2 = source2.size();
    int N = N1 + N2;
    int pair_num = N * N;
    cout << N1 << '\t' << N2 << endl;
    srand(time(NULL));
    cout << "finish loading source fle..." << endl;
    // add
    vector<string> idf_file = FileIO::readFileLines("../../data/" + dataset + "/idf_file.txt");
    vector<string> pid_score_file = FileIO::readFileLines(
            "../../data_block/" + dataset + "/" + blocks + "/" + block + "/pid_score.txt");

    // scan the source file and hash the word from string to int
    map<string, int> word_id;
    map<int, string> id_word;
    for (size_t i = 0; i < source1.size(); i++) {
        vector<string> segs = split(source1[i], ' ');
        for (size_t j = 0; j < segs.size(); j++) {
            string word = format(segs[j]);
            int id = getWordId(word, word_id);
            word_id[word] = id;
            id_word[id] = word;
        }
    }

    for (size_t i = 0; i < source2.size(); i++) {
        vector<string> segs = split(source2[i], ' ');
        for (size_t j = 0; j < segs.size(); j++) {
            string word = format(segs[j]);
            int id = getWordId(word, word_id);
            word_id[word] = id;
            id_word[id] = word;
        }
    }
    cout << "finish constructing word id..." << endl;

    map<int, double> wid_idf;
    // add read idf file <-------------------------->
    for (size_t i = 0; i < idf_file.size(); i++) {
        vector<string> segs = split(idf_file[i], ' ');
        int id = getWordIdNoAdd(segs[0], word_id);
        if (id != -1)
            wid_idf[id] = stringToDouble(segs[1]);
    }
    cout << "finish reading idf file..." << endl;

    map<int, double> pid_score;
    // add read pid_score file <------------------------>
    for (size_t i = 0; i < pid_score_file.size(); i++) {
        vector<string> segs = split(pid_score_file[i], ' ');
        int pid;
        str2int(pid, segs[0]);
        pid_score[pid] = stringToDouble(segs[1]);
    }

    // construct the inverted lists. Each list is sorted by entity id.
    vector<set<int> > combine_inv_lists(word_id.size());
    vector<set<int> > inv_lists1(word_id.size());
    for (size_t i = 0; i < source1.size(); i++) {
        vector<string> segs = split(source1[i], ' ');
        for (size_t j = 0; j < segs.size(); j++) {
            string word = format(segs[j]);
            int id = word_id[word];
            inv_lists1[id].insert(i);
            combine_inv_lists[id].insert(i);
        }
    }
    vector<set<int> > inv_lists2(word_id.size());
    for (size_t i = 0; i < source2.size(); i++) {
        vector<string> segs = split(source2[i], ' ');
        for (size_t j = 0; j < segs.size(); j++) {
            string word = format(segs[j]);
            int id = word_id[word];
            inv_lists2[id].insert(i);
            combine_inv_lists[id].insert(i + N1);
        }
    }
    cout << "finish constructing inverted lists..." << endl;


    // refine the inverted lists by removing stop-words.
    for (size_t wid = 0; wid < word_id.size(); wid++) {
        int len = combine_inv_lists[wid].size();
        if (len == 1 || len > 0.1 * scale * N) {
            inv_lists1[wid].clear();
            inv_lists2[wid].clear();
            combine_inv_lists[wid].clear();
        }
    }
    cout << "finish removing stop-words... " << endl;

    time_t start, end;
    time(&start);

    // construct the bipartite graph between entity-pairs and terms
    int word_num = word_id.size();
    int max_id1 = N1;
    int max_id2 = N2;
    set<string> matches = FileIO::readMatch("../../data_block/" + dataset + "/" + blocks + "/" + block + "/matcher.txt",
                                            max(max_id1, max_id2));
    Bipartite *bigraph = new Bipartite(pair_num, word_num);
    cout << "finish init bigraph" << endl;
    for (size_t wid = 0; wid < word_id.size(); wid++) {
        for (set<int>::iterator id1 = inv_lists1[wid].begin(); id1 != inv_lists1[wid].end(); id1++) {
            for (set<int>::iterator id2 = inv_lists2[wid].begin(); id2 != inv_lists2[wid].end(); id2++) {
                if ((*id1) < max_id1 && (*id2) < max_id2) {
                    int pid = (*id1) * N + N1 + (*id2);
                    bigraph->addEdge(pid, wid);
                }
            }
        }
    }

    bigraph->init(N, pid_score);
    bigraph->iterate();
    cout << "edge num: " << bigraph->activePairNum() << endl;

    CliqueRankMatrix *walker;
    for (int iter = 0;; iter++) {
        cout << "Iteration " << iter << endl;
        walker = new CliqueRankMatrix(bigraph->p_score, N1 + N2, alpha, S, 0.15);
        for (int i = 0; i < N1; i++) {
            int id1 = i;
            for (int j = 0; j < N2; j++) {
                int id2 = j + N1;
                if (bigraph->p_score[id1 * N + id2] > 0) {
                    bigraph->p_score[id2 * N + id1] = bigraph->p_score[id1 * N + id2];
                    walker->addEdge(id1, id2);
                }
            }
        }

        walker->iterate();
        if (iter == 5) {
            final(N1, N2, N, bigraph, walker, eta, outfilename, matches);
            break;
        } else
            calPrecision(N1, N2, N, bigraph, walker, eta, matches);
        time(&end);
        double dif = difftime(end, start);
        cout << "accuracy: " << "\t" << precison << "\t" << recall << "\t" << f1 << endl;
        bigraph->updatePScore(walker->p_conf, N1 + N2, wid_idf);
    }
    return 1;
}
