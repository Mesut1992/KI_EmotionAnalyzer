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
void calculate_evidences(vector<string> data);
struct Evidence;
void plausibility(vector<Evidence> data); //calculates the plausability from a range of evidenzes



void printFile(string filename) {
	ifstream file(filename);
	string value;

	while(file.good()) {
		getline(file, value, ';'); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
		//cout << string(value, 1, value.length() - 2); // display value removing the first and the last character from it
		cout << string(value) << "-";
	}
	
	cout << endl;
}

vector<string> readFileInVector(string filename) {
	vector<string> read;
	ifstream file(filename);
	string value;
	int counter = 0; // Every 4th element has a '\n' afterwards. 
					 // This needs to be removed from the element in the vector.
	
	while(file.good()) {
		counter++;
		
		if((counter % 4) == 0) {
			getline(file, value, '\n'); // read a string until next comma: 
										// http://www.cplusplus.com/reference/string/getline/
		}
		else {
			getline(file, value, ';'); // read a string until next comma: 
									   // http://www.cplusplus.com/reference/string/getline/
		}

		read.push_back(value);
	}

	if(read.empty()) {
		cerr << "Vector empty. Probably unreadable file";
	}

	read.pop_back(); //empty element at the end gave me headache

	return read;
}

void printAverageSprechgeschwindigkeit(vector<string> data) {
	int elem = 0;
	int amountOfElements = 0;
	double average = 0.0, min = 0.0, max = 0.0, temp = 0.0;
	bool init = false;

	for(int i = 4; i < data.size(); i += 4) { //empty 
		elem = i + 1; // Get specific element of Sprechgeschwindigkeit 
		temp = toDouble(data[elem]); // specific element of Sprechgeschwindigkeit
		average += temp;
		amountOfElements++;

		//min max calculation
		if(init == false) { //this just once
			init = true;
			min = temp;
			max = min;
		}

		if(min > temp) {
			min = temp;
		}
		else if(max < temp) {
			max = temp;
		}
	}

	average = average / amountOfElements;

	cout << "The average of Sprechgeschwindikgeit is: " << average << endl;
	cout << "The max value is: " << max << endl;
	cout << "The min value is: " << min << endl;
}

double toDouble(string s) {
	replace(s.begin(), s.end(), ',', '.');
	return atof(s.c_str());
}

set<string> getEmotionOfSprechgeschwindigkeit(double value) {
	set<string> temp;

	if(value <= SLOW) {
		temp.insert("T");
		temp.insert("E");
		temp.insert("F");
	}
	else if(value >= FAST) {
		temp.insert("A");
		temp.insert("U");
		temp.insert("W");
		temp.insert("F");
	}
	else { //Sprechgeschwindigkeit is normal - so it is between slow and fast (2.5 < x < 6.6)
		temp.insert("W");
		temp.insert("F");
	}

	return temp;
}

set<string> getEmotionOfTonlage(string value) {
	set<string> temp;

	//string.compare returns 0 if equal
	if((value.compare(LOW1) == 0) || (value.compare(LOW2) == 0)) {
		temp.insert("E");
		temp.insert("T");
	}
	else if(value.compare(MEDIUM) == 0) {
		temp.insert("F");
	}
	else if((value.compare(HIGH1) == 0) || (value.compare(HIGH2) == 0)) {
		temp.insert("A");
		temp.insert("U");
		temp.insert("W");
	}
	else {
		cout << "Line 140: Could not read the data. Tried to read the following: " 
			<< value << " . Are you sure that you set all macros right?" << endl;
	}

	return temp;
}

set<string> getEmotionOfSchallstaerke(string value) {
	set<string> temp;

	//string.compare returns 0 if equal
	if((value.compare(LOW1) == 0) || (value.compare(LOW2) == 0)) {
		temp.insert("E");
		temp.insert("T");
	}
	else if(value.compare(MEDIUM) == 0) {
		temp.insert("F");
		temp.insert("E");
	}
	else if((value.compare(HIGH1) == 0) || (value.compare(HIGH2) == 0)) {
		temp.insert("A");
		temp.insert("U");
		temp.insert("W");
		temp.insert("F");
	}
	else {
		cout << "Line 140: Could not read the data. Tried to read the following: " 
			<< value << " . Are you sure that you set all macros right?" << endl;
	}

	return temp;
}

//TODO
//Everything from here should be extracted to another file "dempster.h"
struct Evidence {
	Evidence() {} // to dynamically add elements to a vector 
	set<string> emotions;
	double value = 0.0;
};

vector<Evidence> dempster(set<string> speedEmotions, double speedConfidence, 
			  set<string> pitchEmotions, double pitchConfidence, 
			  set<string> intensityEmotions, double intensityConfidence ) {//,
			  //vector<Evidence*>* result) {

	// Initialization of the three evidences
	vector<Evidence*> m_1;
	Evidence m_1a;
	Evidence m_1o; // Omega
	vector<Evidence*> m_2;
	Evidence m_2a;
	Evidence m_2o; // Omega
	vector<Evidence*> m_3;
	Evidence m_3a;
	Evidence m_3o; // Omega

	// m_1: Speed
	m_1a.emotions = speedEmotions;
	m_1a.value = speedConfidence;
	m_1o.emotions = omega;
	m_1o.value = (1 - m_1a.value);
	m_1.push_back(&m_1a);
	m_1.push_back(&m_1o); //OMEGA - index 1

	// m_2: Pitch
	m_2a.emotions = pitchEmotions;
	m_2a.value = pitchConfidence;
	m_2o.emotions = omega;
	m_2o.value = (1 - m_2a.value);
	m_2.push_back(&m_2a);
	m_2.push_back(&m_2o); //OMEGA - index 1

	// m_3: Intensity
	m_3a.emotions = intensityEmotions;
	m_3a.value = intensityConfidence;
	m_3o.emotions = omega;
	m_3o.value = (1 - m_3a.value);
	m_3.push_back(&m_3a);
	m_3.push_back(&m_3o); //OMEGA - index 1

	// m_1 U m_2 = m_12
	vector<Evidence> m_12; // Call by copy required - variables are set by lower scope (for loop)

	for(size_t i = 0; i < m_1.size(); i++) {
		for(size_t j = 0; j < m_2.size(); j++) {
			Evidence temp;
			temp.value = m_1[i]->value * m_2[j]->value;

			set_intersection(m_1[i]->emotions.begin(), m_1[i]->emotions.end(),
							 m_2[j]->emotions.begin(), m_2[j]->emotions.end(),
							 inserter(temp.emotions, temp.emotions.begin()));

			m_12.push_back(temp);
		}
	}

	// Check for empty set and correct if necessary
	// (only possible once: in intersection m_1 U m_2 without Omega)
	for(size_t i = 0; i < m_12.size(); i++) {
		if(m_12[i].emotions.empty()) {
			cout << "Note: Found empty set in m_12" << endl;

			double k = 1 / (1 - m_12[i].value); // calculate correction factor k

			m_12.erase(m_12.begin() + i); // delete empty element i

			for(size_t a = 0; a < m_12.size(); a++) { // correct remaining elements with k
				m_12[a].value *= k;
			}

			cout << "Note: Corrected m_12 with value k=" << k << endl;

			break; // leave for loop since there can only be one correction
		}
	}

	// m_12 U m_3 = m_123
	vector<Evidence> m_123; //call by copy required here - because the variables are created in a loop dynamically - do not save only pointers in this vector

	for(size_t i = 0; i < m_12.size(); i++) {
		for(size_t j = 0; j < m_3.size(); j++) {
			Evidence temp; //empty element
			temp.value = (m_12[i].value) * (m_3[j]->value);

			set_intersection(m_12[i].emotions.begin(), m_12[i].emotions.end(),
							 m_3[j]->emotions.begin(), m_3[j]->emotions.end(),
							 inserter(temp.emotions, temp.emotions.begin()));

			m_123.push_back(temp);
		}
	}

	// Check for empty set and correct if necessary
	// (multiple empty sets possible)
	bool correction = false;
	double sum_empty = 0.0;
	vector<size_t> index_to_delete;

	for(size_t k = 0; k < m_123.size(); k++) {
		if(m_123[k].emotions.empty()) {
			cout << "Note: Found empty set in m_123" << endl;

			correction = true;
			sum_empty += m_123[k].value;

			index_to_delete.push_back(k); //remember which element needs to be removed
		}
	}

	// correct if empty set found
	if(correction) {
		double k = 1 / (1 - sum_empty);

		// delete empty elements
		for(size_t b = 0; b < index_to_delete.size(); b++) {
			m_123.erase(m_123.begin() + index_to_delete[b]);
		}

		// correct remaining elements with k
		for(size_t a = 0; a < m_123.size(); a++) {
			m_123[a].value *= k;
		}

		cout << "Note: Corrected m_123 with k=" << k << endl;
	}

	return m_123;
}

void calculate_evidences(vector<string> data) {
	vector<vector<Evidence>> results;

	for(size_t i = STARTOFDATA; i < data.size(); i = i + 4) {
		cout << "Takt " << data[i] << endl;

		set<string> speedEmotions = getEmotionOfSprechgeschwindigkeit(toDouble(data[i + 1]));
		double speedConfidence = KONF_SPRECHGESCHWINDIGKEIT;

		set<string> pitchEmotions = getEmotionOfTonlage(data[i + 2]);
		double pitchConfidence = KONF_TONLAGE;

		set<string> intensityEmotions = getEmotionOfSchallstaerke(data[i + 3]);
		double intensityConfidence = KONF_SCHALLSTAERKE;

		results.push_back(
			dempster(speedEmotions, speedConfidence,
					 pitchEmotions, pitchConfidence,
					 intensityEmotions, intensityConfidence));
		
		/*// Initialization of the three evidences
		vector<Evidence*> m_1;
		Evidence m_1a; 
		Evidence m_1b; // Omega
		vector<Evidence*> m_2;
		Evidence m_2a;
		Evidence m_2b; // Omega
		vector<Evidence*> m_3;
		Evidence m_3a;
		Evidence m_3b; // Omega

		// m_1: Speed
		m_1a.emotions = getEmotionOfSprechgeschwindigkeit(toDouble(data[i + 1]));
		m_1a.value = KONF_SPRECHGESCHWINDIGKEIT;
		m_1b.emotions = omega;
		m_1b.value = (1 - m_1a.value);
		m_1.push_back(&m_1a);
		m_1.push_back(&m_1b); //OMEGA - index 1

		// m_2: Pitch
		m_2a.emotions = getEmotionOfTonlage(data[i + 2]);
		m_2a.value = KONF_TONLAGE;
		m_2b.emotions = omega;
		m_2b.value = (1 - m_2a.value);
		m_2.push_back(&m_2a);
		m_2.push_back(&m_2b); //OMEGA - index 1

		// m_3: Intensity
		m_3a.emotions = getEmotionOfSchallstaerke(data[i + 3]);
		m_3a.value = KONF_SCHALLSTAERKE;
		m_3b.emotions = omega;
		m_3b.value = (1 - m_3a.value);
		m_3.push_back(&m_3a);
		m_3.push_back(&m_3b); //OMEGA - index 1

		// m_1 U m_2 = m_12
		vector<Evidence> m_12; // Call by copy required - variables are set by lower scope (for loop)

		for(size_t i = 0; i < m_1.size(); i++) {
			for(size_t j = 0; j < m_2.size(); j++) {
				Evidence temp;
				temp.value = m_1[i]->value * m_2[j]->value;

				set_intersection(m_1[i]->emotions.begin(), m_1[i]->emotions.end(), 
								 m_2[j]->emotions.begin(), m_2[j]->emotions.end(), 
								 inserter(temp.emotions, temp.emotions.begin()));

				m_12.push_back(temp);
			}
		}

		// Check for empty set and correct if necessary
		// (only possible once: in intersection m_1 U m_2 without Omega)
		for(size_t i = 0; i < m_12.size(); i++) {
			if(m_12[i].emotions.empty()) {
				cout << "Note: Found empty set in m_12" << endl;

				double k = 1 / (1 - m_12[i].value); // calculate correction factor k

				m_12.erase(m_12.begin() + i); // delete empty element i

				for(size_t a = 0; a < m_12.size(); a++) { // correct remaining elements with k
					m_12[a].value *= k;
				}

				cout << "Note: Corrected m_12 with value k=" << k << endl;

				break; // leave for loop since there can only be one correction
			}
		}

		// m_12 U m_3 = m_123
		vector<Evidence> m_123; //call by copy required here - because the variables are created in a loop dynamically - do not save only pointers in this vector

		for(size_t i = 0; i < m_12.size(); i++) {
			for(size_t j = 0; j < m_3.size(); j++) {
				Evidence temp; //empty element
				temp.value = (m_12[i].value) * (m_3[j]->value);

				set_intersection(m_12[i].emotions.begin(), m_12[i].emotions.end(), 
								 m_3[j]->emotions.begin(), m_3[j]->emotions.end(), 
								 inserter(temp.emotions, temp.emotions.begin()));

				m_123.push_back(temp);
			}
		}

		// Check for empty set and correct if necessary
		// (multiple empty sets possible)
		bool correction = false;
		double sum_empty = 0.0;
		vector<size_t> index_to_delete;

		for(size_t k = 0; k < m_123.size(); k++) {
			if(m_123[k].emotions.empty()) {
				cout << "Note: Found empty set in m_123" << endl;

				correction = true;
				sum_empty += m_123[k].value;

				index_to_delete.push_back(k); //remember which element needs to be removed
			}
		}

		if(correction) {
			double k = 1 / (1 - sum_empty);
			
			// delete empty elements
			for(size_t b = 0; b < index_to_delete.size(); b++) {
				m_123.erase(m_123.begin() + index_to_delete[b]);
			}

			// correct remaining elements with k
			for(size_t a = 0; a < m_123.size(); a++) {
				m_123[a].value *= k;
			}

			cout << "Note: Corrected m_123 with k=" << k << endl;
		}*/

		//Print plausability

		//plausibility(m_123);
		std::cout << endl;
	}
}

void plausibility(vector<Evidence> data) {
	std::cout << "Plausabilitaet" << endl;
	map<string, double> pi;
	//init
	for(const auto& emotion : omega) {
		pi[emotion] = 0.0;
	}
	pi["max"] = 0.0;

	for(size_t i = 0; i < data.size(); i++) {
		//for each emotion
		for(const auto& emotion : omega) {
			if(data[i].emotions.find(emotion) != data[i].emotions.end()) {
				//found element
				pi[emotion] += data[i].value;
			}
		}
	}
	//print
	string e;
	bool init = false;
	for(const auto& emotion : omega) {
		if(init == false) {
			init = true;
			pi["max"] = pi[emotion];
			e = emotion;
		}
		if(pi["max"] < pi[emotion]) {
			pi["max"] = pi[emotion];
			e = emotion;
		}
		std::cout << "PI for Emotion: " << emotion << " : " << pi[emotion] << " " << endl;
	}
	std::cout << "Max PI found in Emotion: " << e << " : " << pi["max"] << endl;
}

int main() {
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
	calculate_evidences(file1);

	//TEST - DEBUG DELETE LATER
	//set<string> test = getEmotionOfSprechgeschwindigkeit(toDouble(file1[5]));
	//set<string> test2 = (getEmotionOfTonlage(file1[6]));
	//set<string> test3 = (getEmotionOfSchallstaerke(file1[7]));

	//Goes through set and finds each element
	//for(const auto& elem : test) {
	//	std::cout << elem << endl;
	//}
	//DELETE TILL HERE

	char dontstopPrgm = getchar();
	return 0;
}

