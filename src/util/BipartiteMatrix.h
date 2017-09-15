#ifndef _BIPARTITE_H
#define _BIPARTITE_H

#include <stdlib.h>    
#include <time.h> 
#include <iostream>
#include "../Eigen/Dense"
using namespace std;

class Bipartite
{
	public:
		Bipartite(int p_num, int w_num)
		{
			word_num = w_num;
			pair_num = p_num;

			t_score = new double[w_num];
			p_score = new double[p_num];

			edges.resize(w_num+p_num);

		}

		void addEdge(int pid, int wid)
		{
			int new_wid = wid+pair_num;
			edges[pid].push_back(wid);
			edges[new_wid].push_back(pid);
		}

		void init()
		{
			double sum=0.0;
			for(int i=0;i < word_num;i++){
				t_score[i] = (rand()/(double)(RAND_MAX));

				if(edges[i+pair_num].size()==0){
					t_score[i]=0;
				}
				sum += t_score[i];
			}
			for(int i=0;i < word_num;i++){
				t_score[i] /= sum;
			}
		}


		void iterate()
		{
			for(int iter=0;iter < 50;iter++){
				// update p_score
				for(int pid=0;pid < pair_num;pid++){
					p_score[pid]=0;
					for(size_t i=0;i < edges[pid].size();i++){
						int wid = edges[pid][i];
						p_score[pid] += t_score[wid];
					}
				}

				// update t_score
				double total_diff = 0;
				for(int wid=0;wid < word_num;wid++){
					int new_wid = wid + pair_num;
					if(edges[new_wid].size() > 0){
						double score = 0;
						for(size_t i=0;i < edges[new_wid].size();i++){
							int pid = edges[new_wid][i];
							score += p_score[pid]*1.0;
						}

						score /= edges[new_wid].size();
						score = 1/(1+1/score);
					


						total_diff += (score > t_score[wid] ? score-t_score[wid] : t_score[wid]-score);
						t_score[wid] = score;
					}
				}


				//cout<<"total difference : "<<total_diff<<endl;
				if(total_diff == 0){
					break;
				}
			}
		}

		void output(map<int,string>& id_word, vector<set<int> >&inv_lists)
		{
			double max_score=-1;
			for(int i=0;i < word_num;i++){
				if(t_score[i] > max_score){
					max_score=t_score[i];
				}
			}
			cout<<"max score "<<max_score<<endl;
			int N=1000;
			double seg = max_score/N+0.001;
			vector<vector<int> > buckets(N);
			for(int i=0;i < word_num;i++){
				int idx = (int)(t_score[i]/seg);
				//cout<<i<<"\t"<<t_score[i]<<"\t"<<idx<<"\t"<<seg<<endl;
				buckets[idx].push_back(i);
			}
			for(int i=N-1;i >=0;i--){
				for(size_t j=0;j < buckets[i].size();j++){
					int wid = buckets[i][j];
					if(t_score[wid]>0){
						cout<<wid<<"\t"<<id_word[wid]<<"\t"<<t_score[wid];
						for(set<int>::iterator iter=inv_lists[wid].begin();iter != inv_lists[wid].end();iter++){
							cout<<"\t"<<*iter;
						}
						cout<<endl;
					}
				}
			}
		}

		int activePairNum()
		{
			int count=0;
			for(int i=0;i < pair_num;i++){
				if(p_score[i] > 0){
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
		void updatePScore(Eigen::MatrixXd& p_conf, int N)
		{
			for(int iter=0;iter < 50;iter++){
				//cout<<"iteration "<<iter<<endl;
				//cout<<t_score[1]<<"\t"<<t_score[2]<<endl;
				// update t_score
				for(int wid=0;wid < word_num;wid++){
					int new_wid = wid + pair_num;
					if(edges[new_wid].size() > 0){
						double score = 0;
						for(size_t i=0;i < edges[new_wid].size();i++){
							int pid = edges[new_wid][i];
							int id1 = pid/N;
							int id2 = pid%N;
							//score += p_score[pid]*(p_conf(id1,id2)+p_conf(id2,id1))/2;
							//double conf = (p_conf(id1,id2)+p_conf(id2,id1))/2;
							double conf = min(p_conf(id1,id2),p_conf(id2,id1));
							//score += conf*p_score[pid];
							score += conf;
						}

						score /= edges[new_wid].size();
						score = 1/(1+1/score);

						//cout<<"+++"<<wid<<"\t"<<t_score[wid]<<"\t"<<score<<"\t"<<edges[new_wid].size()<<endl;
						t_score[wid] = score;

					}
				}

				// update p_score
				for(int pid=0;pid < pair_num;pid++){
					p_score[pid]=0;
					for(size_t i=0;i < edges[pid].size();i++){
						int wid = edges[pid][i];
						//cout<<pid<<"\t"<<wid<<endl;
						p_score[pid] += t_score[wid];
					}
				}
			}


		}

		double* p_score;
		double* t_score;
		int word_num;
		int pair_num;
		const static double scale=1;

		vector<vector<int> > edges;


	private:

};

#endif
