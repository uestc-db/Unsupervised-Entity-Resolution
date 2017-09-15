#ifndef _RANDOM_WALK_H
#define _RANDOM_WALK_H

#include <stdlib.h>    
#include <time.h> 
#include <iostream>
#include <memory.h>
using namespace std;

class RandomWalk
{
	public:
		RandomWalk(double* init_score, int _N, int _Alpha, int _M, int _Step)
		{
			N=_N;
			Alpha=_Alpha;
			M=_M;
			Step=_Step;

			edges = new int*[N];
			is_edge = new bool[N*N]; // important data structures
			for(int i=0;i < N;i++){
				edges[i] = new int[N+1];
				edges[i][N]=0;
			}

			p_score = init_score;
			p_conf = new double[N*N];
			for(int i=0;i < N*N;i++){
				p_conf[i] = 0;
			}
			sum_prob = new double[N];
		}

		int edgeNum(int id)
		{
			return edges[id][N];
		}

		void addEdge(int id1, int id2)
		{
			edges[id1][edges[id1][N]] = id2;
			edges[id1][N]++;

			edges[id2][edges[id2][N]] = id1;
			edges[id2][N]++;

			is_edge[id1*N+id2] = true;
			is_edge[id2*N+id1] = true;
		}

		
		void deleteEdge(int id1, int id2)
		{
			int len = edges[id1][N];
			bool flag = false;
			for(int i=0;i < len;i++){
				if(flag){
					edges[id1][i-1] = edges[id1][i];
				}
				if(edges[id1][i] == id2){
					flag = true;
				}
			}
			edges[id1][N]--;


			is_edge[id1*N+id2] = false;
			is_edge[id2*N+id1] = false;
		}
 
                
	        void load(string file)
        	{
            		ifstream ifile;
            		ifile.open(file.c_str());

            		for(int i = 0; i < N; i++){
                		for(int j = 0; j < N; j++){
                    		ifile >> p_conf[i*N+j];
                		}
            		}

            		ifile.close();
        	}


		/*
		 * pick the next neighbor from a node using power normalization
		 *
		 */
		int nextNode(int id1, int origin)
		{
			int len = edges[id1][N];
			double r1 = (rand()/(double)(RAND_MAX));

			for(int i=0;i < len;i++){
				int id2 = edges[id1][i];
				double score = p_score[id1*N+id2];
				if(id2 == origin){
					score += r1*score;
				}
				if(i == 0){
					sum_prob[i] = pow(score, Alpha);
				}else{
					sum_prob[i] = sum_prob[i-1] + pow(score, Alpha);
				}
				//cout<<origin<<"\t"<<id1<<"\t"<<id2<<"\t"<<score<<"\t"<<sum_prob[i]<<endl;
			}

			double r = (rand()/(double)(RAND_MAX));
			for(int i=0;i < len;i++){
				if(r <= sum_prob[i]/sum_prob[len-1]){
					return edges[id1][i];
				}
			}

			return -1;
		}

		int flipCoin(int origin, int cur)
		{
			// Each coin will go at most Step steps
			for(int s=0;s < Step;s++){
				// construct the normalized probability array to determine which way to go
				int next = nextNode(cur, origin);
				if(next == -1){
					return 0;
				}
				if(next == origin){
					return 1;
				}
				if(!is_edge[origin*N+next]){
					return 0;
				}
				cur=next;
			}
			return 0;
		}


		int iterate()
		{
			// the edges may be updated in the iteration. We need to a buf to keep the set of original edges.
			int* buf = new int[N+1];
			int delete_edge=0;
			for(int id1=0;id1 < N;id1++){
				//cout<<id1<<endl;
				int len = edges[id1][N];
				memcpy(buf, edges[id1], sizeof(int)*(N+1));
				for(int i=0;i < len;i++){
					int id2 = buf[i];

					// flip a coin from id1 to id2 for M/2 times
					int back1 = 0, back2=0;
					for(int j=0;j < M/2;j++){
						back1 += flipCoin(id1, id2);
					}
					if(back1 == 0){
						delete_edge++;
						deleteEdge(id1, id2);
						p_conf[id1*N+id2] = 0;
						p_conf[id2*N+id1] = 0;
						//cout<<id1<<"\t"<<id2<<"\t"<<p_score[id1*N+id2]<<endl;
						continue;
					}

					// flip a coin from id2 to id1 for M/2 times
					for(int j=0;j < M/2;j++){
						back2 += flipCoin(id2, id1);
					}
					if(back2 == 0){
						delete_edge++;
						deleteEdge(id1, id2);
						p_conf[id1*N+id2] = 0;
						p_conf[id2*N+id1] = 0;
						//cout<<id1<<"\t"<<id2<<"\t"<<p_score[id1*N+id2]<<endl;
						continue;
					}
					double new_score = 1.0*(back1+back2)/M;

					p_conf[id1*N+id2] = new_score;
					p_conf[id2*N+id1] = new_score;
				}
			}
			return delete_edge;
		}



		int** edges;
		bool* is_edge;
		int N;


		double Alpha; // used in power normalization
		int M;        // times of sampling expansion
		int Step;     // maximum number of steps in an expansion


		double* sum_prob; // a temporal buffer used in picking the next node
		double* p_conf;  // confidence for the weight
		double* p_score;  // the weight for each pair of edge

	private:

};

#endif
