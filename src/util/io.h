#ifndef _MY_IO_H
#define _MY_IO_H


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <set>

using namespace std;

class FileIO
{
	public:
		static vector<string> readFileLines(string filename)
		{
			vector<string> lines;
			ifstream fin(filename.c_str());
			string line;
			int count=0;
			while(true){
				if(count++ == 100){
					//break;
				}
				getline(fin, line);
				if(!fin.good()){
					break;
				}
				lines.push_back(line);
			}

			return lines;
		}

		static vector<double*> readFileVecs(string filename, int N)
		{
			vector<double*> lines;
			ifstream fin(filename.c_str());
			string line;
			double label;
			while(true){
				fin >> label;
				if(!fin.good()){
					break;
				}
				double* vec = new double[N];
				for(int i=0;i < N;i++){
					fin>>vec[i];
				}
				lines.push_back(vec);
			}
			return lines;
		}

		static set<string> readMatch(string filename)
		{
			set<string> matches;
			string r1, r2;
			ifstream fin(filename.c_str());
			while(true){
				fin >> r1;
				if(!fin.good()){
					break;
				}
				fin >> r2;
				matches.insert(r1+"_"+r2);
			}

			return matches;
		}
		static set<string> readMatch(string filename, int max_id)
		{
			set<string> matches;
			string r1, r2;
			ifstream fin(filename.c_str());
			while(true){
				fin >> r1;
				if(!fin.good()){
					break;
				}
				fin >> r2;
				int id1=atoi(r1.c_str());
				int id2=atoi(r2.c_str());
				if(id1!=id2 && id1<max_id && id2<max_id){
					matches.insert(r1+"_"+r2);
				}
			}

			return matches;
		}
};

#endif
