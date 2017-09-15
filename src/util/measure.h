#ifndef _MEASURE_H
#define _MEASURE_H

#include <iostream>
#include <set>
#include <vector>
#include <math.h>
#include "util.h"
#include <string>
using namespace std;

class Similarity
{
	public:
		static double jaccard(string str1, string str2)
		{
			vector<string> segs1 = split(str1, ' ');
			vector<string> segs2 = split(str2, ' ');
			set<string> set1(segs1.begin(), segs1.end());
			set<string> set2(segs2.begin(), segs2.end());
			int intersect=0;
			set<string>::iterator iter;
			for(iter=set1.begin();iter != set1.end();iter++){
				if(set2.find(*iter) != set2.end()){
					intersect++;
				}
			}
			int total = set1.size() + set2.size() - intersect;
			return 1.0*intersect/total;
		}
		static float stof(string str)
		{
			istringstream buffer(str);
			float temp;
			buffer >> temp;
			return temp;
		}
		
		static double cosine_similarity(string str1, string str2)
                {
			double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
			vector<string> segs1 = split(str1, ' ');
			vector<string> segs2 = split(str2, ' ');
			map<string, float> map1, map2;
			for(int i = 0; i < segs1.size() - 1; i+=2){
				//cout << i << " " <<  segs1[i] << " " << segs1[i+1] << endl;
				string word = segs1[i];
				float weight = stof(segs1[i+1]);
				//float weight = 1;
				map1[word] = weight;
			}
			for(int i = 0; i < segs2.size() - 1; i+=2){
				string word = segs2[i];
				float weight = stof(segs2[i+1]);
				//float weight = 1;
				map2[word] = weight;
			}
			map<string, float>::iterator it;
			for(it = map1.begin(); it != map1.end(); ++it){
				string word1 = it->first;
				float weight = it->second;
				//float weight = 1;
				map<string, float>::iterator it1 = map2.find(word1);
				if(it1 != map2.end()){
					dot += weight * it1->second;
				}
				denom_a += weight*weight;
			}
			for(it = map2.begin(); it != map2.end(); ++it){
				denom_b += it->second*it->second;
			}
			
			return dot/(sqrt(denom_a)*sqrt(denom_b));
		}

		static bool isADSimilar(string word1, string word2)
		{
			int l1 = word1.length();
			int l2 = word2.length();
			int i=0,j=0;
			bool match=true;
			//cout<<word1<<"\t"<<word2<<endl;
			while(i <= l1 && j <= l2){
				if(i<l1 && !isdigit(word1[i]) && !isAlpha(word1[i])){
					i++;
					continue;
				}
				if(j<l2 && !isdigit(word2[j]) && !isAlpha(word2[j])){
					j++;
					continue;
				}
				//cout<<i<<"\t"<<l1<<"\t"<<j<<"\t"<<l2<<endl;
				if(j == l2 || i == l1){
					match=false;
					break;
				}
				if(word1[i] == word2[j]){
					i++;
					j++;
					continue;
				}else{
					match=false;
					break;
				}
			}
			return match;
		}
		static bool isSimilar(string word1, string word2)
		{
			// check if the contained strings are similar
			int l1 = word1.length();
			int l2 = word2.length();
			int i=0,j=0;
			bool match=true;
			//cout<<word1<<"\t"<<word2<<endl;
			while(i <= l1 && j <= l2){
				if(i<l1 && !isdigit(word1[i]) && !isAlpha(word1[i])){
					i++;
					continue;
				}
				if(j<l2 && !isdigit(word2[j]) && !isAlpha(word2[j])){
					j++;
					continue;
				}
				//cout<<i<<"\t"<<l1<<"\t"<<j<<"\t"<<l2<<endl;
				if(j == l2 || i == l1){
					match=false;
					break;
				}
				if(word1[i] == word2[j]){
					i++;
					j++;
					continue;
				}else{
					match=false;
					break;
				}
			}
			if(match){
				return true;
			}
			return EditDistance(word1,word2) < 0.2;
		}
		static double EditDistance(string word1, string word2)
		{
			double Z = max(word1.size(), word2.size());
			int i,j,m;
			int l1 = word1.length();
			int l2 = word2.length();
			vector< vector<int> > t(l1 + 1, vector<int>(l2 + 1));

			for (i = 0; i <= l1; i++)
				t[i][0] = i;
			for (i = 1; i <= l2; i++)
				t[0][i] = i;

			for (i = 1; i <= l1; i++)
			{
				for (j = 1; j <= l2; j++)
				{
					m = min(t[i-1][j], t[i][j-1]) + 1;
					t[i][j] = min(m, t[i-1][j-1] + (word1[i-1] == word2[j-1] ? 0 : 1));
				}
			}
			return t[l1][l2]/Z;
		}

		static double NW(string word1, string word2)
		{
			double Z = max(word1.size(), word2.size());
			int l1 = word1.length();
			int l2 = word2.length();
			vector< vector<int> > T(l1 + 1, vector<int>(l2 + 1));

			for (int i = 0; i <= l1; i++)
				T[i][0] = i;

			for (int i = 0; i <= l2; i++)
				T[0][i] = i;

			for (int i = 1; i <= l1; i++) {
				for (int j = 1; j <= l2; j++) {
					if (word1[i-1] == word2[j-1])
						T[i][j] = T[i - 1][j - 1];
					else
						T[i][j] = min(T[i - 1][j], T[i][j - 1]) + 1;
				}
			}

			int count=0;
			for (int i = l1, j = l2; i > 0 || j > 0; ) {
				if (i > 0 && T[i][j] == T[i - 1][j] + 1) {
					//aa.append(a.charAt(--i));
					//bb.append("-");
					i--;
				} else if (j > 0 && T[i][j] == T[i][j - 1] + 1) {
					//bb.append(b.charAt(--j));
					//aa.append("-");
					j--;
				} else if (i > 0 && j > 0 && T[i][j] == T[i - 1][j - 1]) {
					//aa.append(a.charAt(--i));
					//bb.append(b.charAt(--j));
					count++;
					i--;
					j--;
				}
			}

			return count/Z;
		}

		static double ED(double* vec1, double* vec2, int N)
		{
			double dist=0;
			for(int i=0;i < N;i++){
				dist += (vec1[i]-vec2[i])*(vec1[i]-vec2[i]);
			}
			return sqrt(dist)/sqrt(N);

		}
};


#endif
