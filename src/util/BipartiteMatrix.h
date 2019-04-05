#ifndef _BIPARTITE_H
#define _BIPARTITE_H

#include <stdlib.h>    
#include <time.h> 
#include <iostream>
#include "../Eigen/Dense"
using namespace std;

class Bipartite {
public:
	Bipartite(int p_num, int w_num) {
		word_num = w_num;
		pair_num = p_num;

		t_score = new double[w_num];
		p_score = new double[p_num];

		edges.resize(w_num + p_num);

	}

	void addEdge(int pid, int wid) {
		int new_wid = wid + pair_num;
		edges[pid].push_back(wid);
		edges[new_wid].push_back(pid);
	}

	void init(int N, map<int, double> pid_score) {
		// init p_score
		for (int pid = 0; pid < pair_num; pid++) {
			p_score[pid] = pid_score[pid];
		}

		for (int i = 0; i < N; i++) {
			float max_score = 0;
			for (int j = 0; j < N; j++) {
				int pid = i * N + j;
				if (p_score[pid] > max_score)
					max_score = p_score[pid];
			}
			for (int j = 0; j < N; j++) {
				int pid = i * N + j;
				p_score[pid] = p_score[pid] / max_score;
			}
		}

		// init t_score
		for (int wid = 0; wid < word_num; wid++) {
			int new_wid = wid + pair_num;
			if (edges[new_wid].size() > 0) {
				double score = 0;
				for (size_t i = 0; i < edges[new_wid].size(); i++) {
					int pid = edges[new_wid][i];
					score += p_score[pid] * 1.0;
				}

				score /= edges[new_wid].size();
				score = 1 / (1 + 1 / score);

				t_score[wid] = score;
			}
		}
	}

	void iterate() {
		for (int iter = 0; iter < 50; iter++) {
			// update p_score
			for (int pid = 0; pid < pair_num; pid++) {
				p_score[pid] = 0;
				for (size_t i = 0; i < edges[pid].size(); i++) {
					int wid = edges[pid][i];
					p_score[pid] += t_score[wid];
				}
			}

			// update t_score
			double total_diff = 0;
			for (int wid = 0; wid < word_num; wid++) {
				int new_wid = wid + pair_num;
				if (edges[new_wid].size() > 0) {
					double score = 0;
					for (size_t i = 0; i < edges[new_wid].size(); i++) {
						int pid = edges[new_wid][i];
						score += p_score[pid] * 1.0;
					}
					score /= edges[new_wid].size();
					score = 1 / (1 + 1 / score);

					total_diff += (score > t_score[wid] ? score - t_score[wid] : t_score[wid] - score);
					t_score[wid] = score;
				}
			}
			if (total_diff == 0) {
				break;
			}
		}
	}

	int activePairNum() {
		int count = 0;
		for (int i = 0; i < pair_num; i++) {
			if (p_score[i] > 0) {
				count++;
			}
		}
		return count;
	}

	/*
     * when p_score is updated, we update the term score accordingly and
     * then update the p_score back. There is only one iteration involved.
     *
     */
	void updatePScore(Eigen::MatrixXd &p_conf, int N, map<int, double> wid_idf) {
		for (int iter = 0; iter < 50; iter++) {
			// update t_score
			for (int wid = 0; wid < word_num; wid++) {
				int new_wid = wid + pair_num;
				if (edges[new_wid].size() > 0) {
					double score = 0;
					for (size_t i = 0; i < edges[new_wid].size(); i++) {
						int pid = edges[new_wid][i];
						int id1 = pid / N;
						int id2 = pid % N;
						double conf = min(p_conf(id1, id2), p_conf(id2, id1));
						score += conf;
					}
					// strong term
					if (score < 1 && edges[new_wid].size() == 1 && wid_idf[wid] == 0) {
						score = 1;
					}

					score /= edges[new_wid].size();
					score = 1 / (1 + 1 / score);

					t_score[wid] = score;

				}
			}

			// update p_score
			for (int pid = 0; pid < pair_num; pid++) {
				p_score[pid] = 0;
				for (size_t i = 0; i < edges[pid].size(); i++) {
					int wid = edges[pid][i];
					p_score[pid] += t_score[wid];
				}
			}
		}
	}

	double *p_score;
	double *t_score;
	int word_num;
	int pair_num;
	const static double scale = 1;

	vector <vector<int> > edges;

private:

};

#endif
