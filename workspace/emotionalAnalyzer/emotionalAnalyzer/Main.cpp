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
#include <map>

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
struct Evidenz;
void plausability(vector<Evidenz> data); //calculates the plausability from a range of evidenzes



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
struct Evidenz
{
	Evidenz(){} //to add dynamically elements to a vector 
	set<string> emotions;
	double value = 0.0;
};

void dempster(vector<string> data){
	for (size_t i = STARTOFDATA; i < data.size(); i = i + 4)
	{
		cout << "Takt: " << data[i] << endl;
		//Init der evidenzen
		vector<Evidenz*> m_1;
		Evidenz m_1a;
		Evidenz m_1b;
		vector<Evidenz*> m_2;
		Evidenz m_2a;
		Evidenz m_2b;
		vector<Evidenz*> m_3;
		Evidenz m_3a;
		Evidenz m_3b;
		m_1a.emotions = getEmotionOfSprechgeschwindigkeit(toDouble(data[i + 1]));
		m_1a.value = KONF_SPRECHGESCHWINDIGKEIT;
		m_1b.emotions = omega;
		m_1b.value = (1 - m_1a.value);
		m_1.push_back(&m_1a); 
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
		m_3b.value = (1 - m_3a.value);
		m_3.push_back(&m_3a); 
		m_3.push_back(&m_3b); //OMEGA - index 1

		//m_1 union m_2
		vector<Evidenz> m_12; //Call by copy required - variables are set by lower scope (for loop)

		for (size_t i = 0; i < m_1.size(); i++)
		{
			for (size_t j = 0; j < m_2.size(); j++)
			{
				Evidenz temp;
				temp.value = m_1[i]->value * m_2[j]->value;
				set_intersection(m_1[i]->emotions.begin(), m_1[i]->emotions.end(), m_2[j]->emotions.begin(), m_2[j]->emotions.end(), inserter(temp.emotions, temp.emotions.begin()));
				m_12.push_back(temp);
			}
		}

		//Korrektur? - Hier ist es nur möglich, dass nur eine Evidenz eine Korrektur benötigt
		for (size_t i = 0; i < m_12.size(); i++)
		{
			if (m_12[i].emotions.empty()){
				cout << "Hinweis: Korrektur in der Union von m_1 und m_2 durchgefuehrt." << endl;
				double k = 1 / (1 - m_12[i].value);
				m_12.erase(m_12.begin() + i); //delete element
				for (size_t a = 0; a < m_12.size(); a++)
				{
					m_12[a].value *= k;
				}
				break; //leave for loop since there can only be one 'Korrektur'
			}
		}

		//m12 union m3 
		vector<Evidenz> m_123; //call by copy required here - because the variables are created in a loop dynamically - do not save only pointers in this vector
		//int counter = 0;

		for (size_t i = 0; i < m_12.size(); i++)
		{
			for (size_t j = 0; j < m_3.size(); j++)
			{
				Evidenz temp; //empty element
				temp.value = (m_12[i].value) * (m_3[j]->value);
				set_intersection(m_12[i].emotions.begin(), m_12[i].emotions.end(), m_3[j]->emotions.begin(), m_3[j]->emotions.end(), inserter(temp.emotions, temp.emotions.begin()));
				m_123.push_back(temp);
			}
		}

		//Korrektur? Hier können mehrere entstehen!
		bool korrektur = false;
		double temp = 0.0;
		vector<int> index_to_delete;
		for (size_t k = 0; k < m_123.size(); k++)
		{
			if (m_123[k].emotions.empty()){
				cout << "Hinweis: Korrektur in der Union von m_12 und m_3 durchgeführt! " << endl;
				korrektur = true;
				temp += m_123[k].value; 
				index_to_delete.push_back(k); //remember which element needs to be removed
			}
		}
		if (korrektur){
			double k = 1 / (1 - temp);
			//delete elements
			for (size_t b = 0; b < index_to_delete.size(); b++)
			{
				m_123.erase(m_123.begin() + index_to_delete[b]);
			}
			for (size_t a = 0; a < m_123.size(); a++)
			{
				m_123[a].value *= k;
			}
		}


		//Print plausability
		plausability(m_123);
		std::cout << endl;
	}
};

void plausability(vector<Evidenz> data){
	std::cout << "Plausabilitaet" << endl;
	map<string, double> pi; 
	//init
	for (const auto& emotion : omega) {
		pi[emotion] = 0.0;
	}
	pi["max"] = 0.0;

	for (size_t i = 0; i < data.size(); i++)
	{
		//for each emotion
		for (const auto& emotion : omega) {
			if (data[i].emotions.find(emotion) != data[i].emotions.end()){
				//found element
				pi[emotion] += data[i].value;
			}
		}
	}
	//print
	string e;
	bool init = false;
	for (const auto& emotion : omega) {
		if (init == false){
			init = true;
			pi["max"] = pi[emotion];
			e = emotion;
		}
		if (pi["max"] < pi[emotion]){
			pi["max"] = pi[emotion];
			e = emotion;
		}
		std::cout << "PI for Emotion: " << emotion << " : " << pi[emotion] << " " << endl;
	}
	std::cout << "Max PI found in Emotion: " << e << " : " << pi["max"] << endl;
}

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
		std::cout << elem << endl;
	}
	//DELETE TILL HERE

	char dontstopPrgm = getchar();
	return 0;
}

