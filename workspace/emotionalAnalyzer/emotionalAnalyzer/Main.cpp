/*	
	Authors: Mesut Kuscu, Helge Bruegner
*/
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <set>
using namespace std;

#define FILE1 "E_004.csv" //Path of 1st file
#define FILE2 "E_004b.csv" //Path of 1st file
#define FILE3 "E_004c.csv" //Path of 1st file

//predefined classes, functions
void printFile(string filename); //prints file
vector<string> readFileInVector(string filename); //returns all entries as a vector
void printAverageSprechgeschwindigkeit(vector<string> data); //calculates the average of the Sprechgeschwindigkeit from the given vector (specific structur!) and prints average, min and max value
double toDouble(string s); //converts string with comma to double

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
	cout << endl;
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
	read1.pop_back(); //empty element at the end gave me headache
	return read1;
};
void printAverageSprechgeschwindigkeit(vector<string> data){
	int elem = 0;
	int amountOfElements = 0;
	double average=0.0, min=0.0, max=0.0, temp=0.0;
	bool init = false;
	for (int i = 4; i < data.size(); i+=4){ //empty 
		elem = i + 1; //Get specific element of Sprechgeschwindigkeit 
		temp = toDouble(data[elem]); //specific element of Sprechgeschwindigkeit
		average += temp;
		amountOfElements++;

		//min max calculation
		if (init == false){ //this just once
			init = true;
			min = temp;
			max = min;
		}
		if (min > temp){
			min = temp;
		}
		else if (max < temp){
			max = temp;
		}
	}
	average = average / amountOfElements;
	cout << "The average of Sprechgeschwindikgeit is: " << average << endl;
	cout << "The max value is: " << max << endl;
	cout << "The min value is: " << min << endl;
};
double toDouble(string s){
	replace(s.begin(), s.end(), ',', '.');
	return atof(s.c_str());
};


int main(){
	//Read file input
	vector<string> file1 = readFileInVector(FILE1);
	vector<string> file2 = readFileInVector(FILE2);
	//vector<string> file3 = readFileInVector(FILE3); File3 is not relevant for this project.
	//this functions are printing the example data
	//use this for debug purposes
	/*
	printFile(FILE1);
	printFile(FILE2);
	printAverageSprechgeschwindigkeit(file1);
	printAverageSprechgeschwindigkeit(file2);
	*/

	//Init program
	set<string> emotions;
	emotions.insert("A");
	emotions.insert("U");
	emotions.insert("W");
	emotions.insert("F");
	emotions.insert("E");
	emotions.insert("T");


	char dontstopPrgm = getchar();
	return 0;
}