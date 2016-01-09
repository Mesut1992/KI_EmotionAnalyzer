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

//#include "dempster.h" //contains the core functionality - some features are called from this file!

//contains all settings like konfidenz, file paths ...
#include "settings.h" 

//predefined classes, functions
void printFile(string filename); //prints file
vector<string> readFileInVector(string filename); //returns all entries as a vector
void printAverageSprechgeschwindigkeit(vector<string> data); //calculates the average of the Sprechgeschwindigkeit from the given vector (specific structur!) and prints average, min and max value
double toDouble(string s); //converts string with comma to double
set<string> getEmotionOfSprechgeschwindigkeit(double value); //Get Emotions
//uses dempster to find the emotion with the max. plausibility
void dempster(vector<string> data);

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
set<string> getEmotionOfSprechgeschwindigkeit(double value){
	set<string> temp;
	if(value <= SLOW){
		temp.insert("T");
		temp.insert("E");
		temp.insert("F");
	}
	else if(value >= FAST){
		temp.insert("A");
		temp.insert("U");
		temp.insert("W");
		temp.insert("F");
	}
	else{ //Sprechgeschwindigkeit is normal - so it is between slow and fast 2.5 < x < 6.6
		temp.insert("W");
		temp.insert("F");
	}
	return temp;
};
set<string> getEmotionOfTonlage(string value){
	set<string> temp;
	//string.compare returns 0 if equal
	if ((value.compare(LOW1) == 0) || (value.compare(LOW2) == 0)){
		temp.insert("E");
		temp.insert("T");
	}
	else if (value.compare(MEDIUM) == 0){
		temp.insert("F");
	}
	else if ((value.compare(HIGH1) == 0) || (value.compare(HIGH2) == 0)){
		temp.insert("A");
		temp.insert("U");
		temp.insert("W");
	}
	else{
		cout << "Line 140: Could not read the data. Tried to read the following: " << value << " . Are you sure that you set all macros right?" << endl; 
	}
	return temp;
};
set<string> getEmotionOfSchallstaerke(string value){
	set<string> temp;
	//string.compare returns 0 if equal
	if ((value.compare(LOW1) == 0) || (value.compare(LOW2) == 0)){
		temp.insert("E");
		temp.insert("T");
	}
	else if (value.compare(MEDIUM) == 0){
		temp.insert("F");
		temp.insert("E");
	}
	else if ((value.compare(HIGH1) == 0) || (value.compare(HIGH2) == 0)){
		temp.insert("A");
		temp.insert("U");
		temp.insert("W");
		temp.insert("F");
	}
	else{
		cout << "Line 140: Could not read the data. Tried to read the following: " << value << " . Are you sure that you set all macros right?" << endl;
	}
	return temp;
};

//TODO
//Everything from here should be extracted to another file "dempster.h"
struct root_evidenz
{
	set<string> emotions;
	double value = 0.0;
};


void dempster(vector<string> data){
	/*1st element*/
	size_t takt = 0;
	for (size_t i = STARTOFDATA; i < data.size(); i = i + 4)
	{
		cout << "Takt: " << data[i] << endl;
		//Init der evidenzen
		//Sprechgeschwindigkeit
		vector<root_evidenz*> m_1;
		root_evidenz m_1a;
		root_evidenz m_1b;
		vector<root_evidenz*> m_2;
		root_evidenz m_2a;
		root_evidenz m_2b;
		vector<root_evidenz*> m_3;
		root_evidenz m_3a;
		root_evidenz m_3b;
		m_1a.emotions = getEmotionOfSprechgeschwindigkeit(toDouble(data[i + 1]));
		m_1a.value = KONF_SPRECHGESCHWINDIGKEIT;
		m_1b.emotions = omega;
		m_1b.value = (1 - m_1a.value);
		m_1.push_back(&m_1a); //Emotions
		m_1.push_back(&m_1b); //OMEGA - index 1

		m_2a.emotions = getEmotionOfTonlage(data[i + 2]);
		m_2a.value = KONF_TONLAGE;
		m_2b.emotions = omega;
		m_2b.value = (1 - m_2a.value);
		m_2.push_back(&m_2a); 
		m_2.push_back(&m_2b); //OMEGA - index 1
		
		m_3a.emotions = getEmotionOfSchallstaerke(data[i + 3]);
		m_3a.value = KONF_SCHALLSTAERKE;
		m_3b.emotions = omega;
		m_3b.value = (1 - m_3b.value);
		m_3.push_back(&m_3a); 
		m_3.push_back(&m_3b); //OMEGA - index 1

		//m_1 union m_2
		vector<root_evidenz> m_12;
		
		//omega u omega
		root_evidenz m_12a;
		//m_12a.emotions = omega;
		m_12a.value = m_1[1]->value * m_2[1]->value;
		set_intersection(m_1[1]->emotions.begin(), m_1[1]->emotions.end(), m_2[1]->emotions.begin(), m_2[1]->emotions.end(), inserter(m_12a.emotions, m_12a.emotions.begin()));
		
		//omega u m_2-emotions
		root_evidenz m_12b;
		m_12b.value = m_1[1]->value * m_2[0]->value;
		//Omega with something is always something, so this could be reduced. Didn't optimized it for clearity purposes
		set_intersection(m_1[1]->emotions.begin(), m_1[1]->emotions.end(), m_2[0]->emotions.begin(), m_2[0]->emotions.end(), inserter(m_12b.emotions, m_12b.emotions.begin()));

		//m_1-emotions u omega
		root_evidenz m_12c;
		m_12c.value = m_1[0]->value * m_2[1]->value;
		set_intersection(m_1[0]->emotions.begin(), m_1[0]->emotions.end(), m_2[1]->emotions.begin(), m_2[1]->emotions.end(), inserter(m_12c.emotions, m_12c.emotions.begin()));

		//m_1-emotions u m_2-emotions
		//only case in which a 'Korrektur' need to be considered
		root_evidenz m_12d;
		m_12d.value = m_1[0]->value * m_2[0]->value;
		set_intersection(m_1[0]->emotions.begin(), m_1[0]->emotions.end(), m_2[0]->emotions.begin(), m_2[0]->emotions.end(), inserter(m_12d.emotions, m_12d.emotions.begin()));
		
		//Korrektur
		if (m_12d.emotions.empty()){
			//cout << "Oh dear, there is no intersection!" << endl;
			double k = 1 / (1 - m_12d.value);
			m_12a.value *= k;
			m_12b.value *= k;
			m_12c.value *= k;

			//m_12d is an empty set and not relevant anymore
			m_12.push_back(m_12a);

		}
		
		cout << "dont stop " << endl;
	}
};

int main(){
	global_init();
	//Read file input
	vector<string> file1 = readFileInVector(FILE1);
	vector<string> file2 = readFileInVector(FILE2);
	//vector<string> file3 = readFileInVector(FILE3); File3 is not relevant for this project.
	//this functions are printing the example data
	//use this for debug purposes
	
	printFile(FILE1);
	/*printFile(FILE2);
	printAverageSprechgeschwindigkeit(file1);
	printAverageSprechgeschwindigkeit(file2);
	*/

	//Init program
	dempster(file1);
	
	//TEST - DEBUG DELETE LATER
	set<string> test=  getEmotionOfSprechgeschwindigkeit(toDouble(file1[5]));
	set<string> test2 = (getEmotionOfTonlage(file1[6]));
	set<string> test3 = (getEmotionOfSchallstaerke(file1[7]));

	//Goes through set and finds each element
	for (const auto& elem : test) {
		cout << elem << endl;
	}
	//DELETE TILL HERE

	char dontstopPrgm = getchar();
	return 0;
}

