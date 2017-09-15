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
#include "../util/Bipartite.h"
#include "../util/RandomWalk.h"


using namespace std;

int getWordId(string word, map<string,int>&word_id)
{
	if(word_id.find(word) != word_id.end()){
		return word_id[word];
	}

	for(map<string,int>::iterator iter=word_id.begin();iter != word_id.end();iter++)
	{
		if(Similarity::isADSimilar(word, iter->first)){
			//cout<<word<<"\t"<<iter->first<<endl;
			return iter->second;

		}
	}
	return word_id.size();
}

double precison, recall, f1, score_threshold;

void outputFinal(int N1, int N2, int N, Bipartite* bigraph, RandomWalk* walker, set<string>& matches, double eta)
{
	int Num=1000;
	double max_weight=-1;
	vector<vector<int> > buckets(Num);
	int tmp_count=0;
	for(int i=0;i < N1;i++){
		int id1=i;
		for(int j=0;j < N2;j++){
			int id2=j+N1;
			double weight = walker->p_conf[id1*N+id2];
			if(weight > max_weight){
				max_weight=weight;
			}
		}
	}

	double seg = max_weight/Num+0.001;
	for(int i=0;i < N1;i++){
		int id1=i;
		for(int j=0;j < N2;j++){
			int id2=j+N1;
			double weight = walker->p_conf[id1*N+id2];
			int idx=(int)(weight/seg);
			buckets[idx].push_back(id1*N+id2);
		}
	}

	for(int i=Num-1;i>=0;i--){
		for(size_t j=0;j < buckets[i].size();j++){
			int key = buckets[i][j];
			int id1 = key/N;
			int id2 = key%N;
			stringstream ss;
			ss<<id1<<"_"<<(id2-N1);
			cout<<id1<<"\t"<<id2<<"("<<id2-N1<<")\t"<<walker->p_conf[id1*N+id2]<<"\t"<<bigraph->p_score[id1*N+id2]<<"\t"<<score_threshold<<"\t";
			if(walker->p_conf[id1*N+id2]>=eta && bigraph->p_score[id1*N+id2]>= score_threshold){
				cout<<"\t11111\t";
			}
			if(matches.find(ss.str()) != matches.end()){
				cout<<"true"<<endl;
			}else{
				cout<<"false"<<endl;
			}
		}
	}
}

void calPrecision(int N1, int N2, int N, Bipartite* bigraph, RandomWalk* walker, set<string>& matches, double eta)
{
	// get the average similarity score 
	score_threshold = 0;
	int tmp_count=0;
	for(int i=0;i < N1;i++){
		int id1=i;
		for(int j=0;j < N2;j++){
			int id2=j+N1;
			double weight = walker->p_conf[id1*N+id2];
			if(weight >= 0.6){
				score_threshold += 0.6*bigraph->p_score[id1*N+id2];	
				tmp_count++;
			}
				
		}
	}
	score_threshold /= tmp_count;


	int count=0,total_pair=0;
	for(int i=0;i < N1;i++){
		int id1=i;
		for(int j=0;j < N2;j++){
			int id2=j+N1;
			stringstream ss;
			ss<<id1<<"_"<<(id2-N1);
			//if(walker->p_conf[id1*N+id2]>=eta && bigraph->p_score[id1*N+id2]>= score_threshold){
			if(walker->p_conf[id1*N+id2]>=eta ){
				total_pair++;
				if(matches.find(ss.str()) != matches.end()){
					count++;
				}
			}

		}
	}
	precison = 1.0*count/total_pair;
	recall = 1.0*count/matches.size();
	f1 = 2*precison*recall/(precison+recall);
}


/*
 * Construct the sorted features (words) for each dataset.
 *
 */
int main(int argc, char** argv)
{
	int scale = 2;
	int alpha = 20;
	int S = 20;
	float eta = 0.98;

	// read the data source file
	string dataset = "product";
	vector<string> source1 = FileIO::readFileLines("../../data/"+dataset+"/source_1.txt");
	vector<string> source2 = FileIO::readFileLines("../../data/"+dataset+"/source_2.txt");
	int N1 = source1.size();
	int N2 = source2.size();
	int N = N1+N2;
	int pair_num = N*N;
	srand (time(NULL));
	cout<<"finish loading source fle..."<<endl;


	// scan the source file and hash the word from string to int
	map<string, int> word_id;
	map<int, string> id_word;
	for(size_t i=0;i < source1.size();i++){
		vector<string> segs = split(source1[i], ' ');
		for(size_t j=0;j < segs.size();j++){
			string word = format(segs[j]);
			int id = getWordId(word, word_id);
			word_id[word] = id;
			id_word[id] = word;
		}
	}

	for(size_t i=0;i < source2.size();i++){
		vector<string> segs = split(source2[i], ' ');
		for(size_t j=0;j < segs.size();j++){
			string word = format(segs[j]);
			int id = getWordId(word, word_id);
			word_id[word] = id;
			id_word[id] = word;
		}
	}
	cout<<"finish constructing word id..."<<endl;


	// construct the inverted lists. Each list is sorted by entity id.
	vector<set<int> > combine_inv_lists(word_id.size());
	vector<set<int> > inv_lists1(word_id.size());
	for(size_t i=0;i < source1.size();i++){
		vector<string> segs = split(source1[i], ' ');
		for(size_t j=0;j < segs.size();j++){
			string word = format(segs[j]);
			int id = word_id[word];
			inv_lists1[id].insert(i);
			combine_inv_lists[id].insert(i);
		}
	}
	vector<set<int> > inv_lists2(word_id.size());
	for(size_t i=0;i < source2.size();i++){
		vector<string> segs = split(source2[i], ' ');
		for(size_t j=0;j < segs.size();j++){
			string word = format(segs[j]);
			int id = word_id[word];
			inv_lists2[id].insert(i);
			combine_inv_lists[id].insert(i+N1);
		}
	}
	cout<<"finish constructing inverted lists..."<<endl;


	// refine the inverted lists by removing stop-words. 
	for(size_t wid=0;wid < word_id.size();wid++){
		int len = combine_inv_lists[wid].size(); 
		if(len == 1 || len > 0.1*scale*N){
			inv_lists1[wid].clear();
			inv_lists2[wid].clear();
			combine_inv_lists[wid].clear();
		}
	}
	

	time_t start, end;
	time(&start);

	// construct the bipartite graph between entity-pairs and terms
	int word_num = word_id.size();
	int max_id1=N1;
	int max_id2=N2;
	Bipartite *bigraph = new Bipartite(pair_num, word_num);
	cout<<"finish init bigraph"<<endl;
	for(size_t wid=0;wid < word_id.size();wid++){
		for(set<int>::iterator id1=inv_lists1[wid].begin();id1!=inv_lists1[wid].end();id1++){
			for(set<int>::iterator id2=inv_lists2[wid].begin();id2!=inv_lists2[wid].end();id2++){
				if((*id1) < max_id1 && (*id2) < max_id2){
					int pid = (*id1)*N+N1+(*id2);
					bigraph->addEdge(pid, wid);
				}
				//cout<<(*id1)<<"\t"<<(*id2)<<"\t"<<pid<<"\t"<<wid<<"\t"<<id_word[wid]<<endl;
			}
		}
	}

	bigraph->init();
	

	bigraph->iterate();
	//cout<<"edge num: "<<bigraph->activePairNum()<<endl;


	set<string> matches = FileIO::readMatch("../../data/"+dataset+"/match.txt", max(max_id1,max_id2));
	RandomWalk* walker;
	for(int iter=0;;iter++){
		cout<<"Iteration "<<iter<<endl;
		walker = new RandomWalk(bigraph->p_score, N1+N2, alpha, 60, S);
		for(int i=0;i < N1;i++){
			int id1=i;
			for(int j=0;j < N2;j++){
				int id2=j+N1;
				if(bigraph->p_score[id1*N+id2]>0){
					bigraph->p_score[id2*N+id1]=bigraph->p_score[id1*N+id2];
					if(i < max_id1 && j < max_id2){
						walker->addEdge(id1,id2);
					}
				}
			}
		}
		cout<<"edge num: "<<bigraph->activePairNum()<<endl;

		walker->iterate();
		calPrecision(N1, N2, N, bigraph, walker, matches, eta);
		cout<<"accuracy: "<<"\t"<<precison<<"\t"<<recall<<"\t"<<f1<<endl;
		time(&end);

		double dif = difftime(end, start);
		printf("Time taken : %.2fs\n", dif);

		if(iter==5){
			cout<<"final_result: "<<"\t"<<precison<<"\t"<<recall<<"\t"<<f1<<"\t"<<dif<<endl;
			break;
		}
		bigraph->updatePScore(walker->p_conf);
	}
	return 1;

}	
