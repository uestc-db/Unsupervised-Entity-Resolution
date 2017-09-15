#ifndef _UTIL_H
#define _UTIL_H

#include <sstream>
#include <vector>
#include <string>

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

bool isdigit(char c)
{
	return '0' <= c && '9' >= c;
}

bool isAlpha(char c)
{
	return ('a'<=c && 'z'>= c) || ('A'<=c && 'Z'>=c);
}

string format(string word)
{
	int len = word.length();
	int i=0, j=len-1;
	while(i<len && !isdigit(word[i]) && !isAlpha(word[i])){
		i++;
	}
	while(j>=0 && !isdigit(word[j]) && !isAlpha(word[j])){
		j--;
	}
	if(i > j){
		return "";
	}
	return word.substr(i,j-i+1);	
}

#endif
