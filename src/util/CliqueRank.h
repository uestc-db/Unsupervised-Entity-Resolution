#ifndef _CLIQUERANK_H
#define _CLIQUERANK_H

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
using namespace std;

class CliqueRank
{
    public:
        CliqueRank(double* init_score, int _N, int _Alpha, int _Step)
        {
            N = _N;
            Alpha = _Alpha;
            Step = _Step;

            edges = new int*[N];
            is_edge = new bool[N*N];
            for(int i = 0; i < N; i++){
                edges[i] = new int[N+1];
                edges[i][N] = 0;
            }

            norm = new double[N];
            p_score = init_score;
            p_conf = new double[N*N];
	    for(int i = 0; i < N*N; i++){
		p_conf[i] = 0;
	    }
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
            for(int i = 0; i < len; i++){
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


        void init()
        {
            for(int id1 = 0; id1 < N; id1++){
                double nvalue = 0;
                int len = edges[id1][N];
                for(int i = 0; i < len; i++){
                    int id2 = edges[id1][i];
                    nvalue += pow(p_score[id1*N+id2], Alpha);
                    //nvalue += p_score[id1*N+id2];
                }
                norm[id1] = nvalue;
            }

            for(int i = 0; i < N*N; i++)
		p_conf[i] = 0;

            for(int id1 = 0; id1 < N; id1++){
                int len = edges[id1][N];
                for(int i = 0; i < len; i++){
                    int id2 = edges[id1][i];
		    if(p_score[id1*N+id2] > 0)
                        p_conf[id1*N+id2] = pow(p_score[id1*N+id2], Alpha)/norm[id1];
			p_conf[id2*N+id1] = pow(p_score[id2*N+id1], Alpha)/norm[id2];
                    //p_conf[id1*N+id2] = p_score[id1*N+id2]/norm[id1];
                }
            }
        }

        void iterate()
        {
	    double* temp_conf = new double[N*N+1];
            init();
	    memcpy(temp_conf, p_conf, sizeof(double)*(N*N+1));
            double* buf_conf = new double[N*N+1];
	    ofstream ofile;
	    ofile.open("../../data/test.txt");
	    
            for(int s = 0; s < Step; s++){
	        //cout << "Iteration: " << s << endl;
		//for(int m = 0; m < 10; m++)
	 	//   cout << temp_conf[m] << " ";
		//cout << endl;
		memcpy(buf_conf, temp_conf, sizeof(double)*(N*N+1));
                for(int id1 = 0; id1 < N; id1++){
   		    for(int id2 = id1+1; id2 < N; id2++){
			if(id1 != id2){
			    temp_conf[id1*N+id2] = 0;
			    temp_conf[id2*N+id1] = 0;
			    int len = edges[id1][N];
		   	    for(int i = 0; i < len; i++){
				int nid = edges[id1][i];
				//temp_conf[id1*N+id2] += (pow(buf_score[id1*N+nid], Alpha)/norm[id1])*p_conf[nid*N+id2];
				temp_conf[id1*N+id2] += (pow(p_score[id1*N+nid], Alpha)/norm[id1])*buf_conf[nid*N+id2];
				//temp_conf[id1*N+id2] += (p_score[id1*N+nid]/norm[id1])*buf_conf[nid*N+id2];
		  	    }
			    p_conf[id1*N+id2] += temp_conf[id1*N+id2];
			    len = edges[id2][N];
			    for(int i = 0; i < len; i++){
				int nid = edges[id2][i];
				temp_conf[id2*N+id1] += (pow(p_score[id2*N+nid], Alpha)/norm[id2])*buf_conf[nid*N+id1];
			    }
			    p_conf[id2*N+id1] += temp_conf[id2*N+id1];
			}
		    }
                }
            }
        }

        int** edges;
        bool* is_edge;

        int N;
        int Alpha;
        int Step;

        double* norm;
        double* p_score;
        double* p_conf;
};

#endif
