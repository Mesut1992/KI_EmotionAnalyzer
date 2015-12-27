/*	
	Authors: Mesut Kuscu, Helge Bruegner
*/
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define FILE1 "E_004.csv" //Path of 1st file
#define FILE2 "E_004b.csv" //Path of 1st file
#define FILE3 "E_004c.csv" //Path of 1st file

using namespace std;

//predefined classes, functions
void printFile(string filename); //prints file
vector<string> readFileInVector(string filename) //returns all entries as a vector

//Implementation
void printFile(string filename){
	ifstream file(filename);
	string value;
	while (file.good())
	{
		getline(file, value, ';'); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
		//cout << string(value, 1, value.length() - 2); // display value removing the first and the last character from it
		cout << string(value) << "-";
	}
};
vector<string> readFileInVector(string filename){
	vector<string> read1;
	ifstream file(filename);
	string value;
	int counter = 0; //every 4th element has a '\n' afterwards. This needs to be get removed from the element in the vector.
	while (file.good())
	{
		counter++;
		if ((counter % 4) == 0){
			getline(file, value, '\n'); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
		}
		else{
			getline(file, value, ';'); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
		}
		read1.push_back(value);
	}
	if (read1.empty()){
		cout << "Vector empty. Probably could not read file.";
	}
	return read1;
}

int main(){
	vector<string> file1 = readFileInVector(FILE1);
	vector<string> file2 = readFileInVector(FILE2);
	vector<string> file3 = readFileInVector(FILE3);

	char dontstop = getchar();
	return 0;
}