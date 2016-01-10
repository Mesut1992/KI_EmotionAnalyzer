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

#include "settings.h"
#include "dempster.h"

using namespace std;

//

double toDouble(string s) {
	replace(s.begin(), s.end(), ',', '.');
	return atof(s.c_str());
}

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
		cerr << " Error: Vector empty. Probably unreadable file";
	}

	read.pop_back(); //empty element at the end gave me headache

	return read;
}

void printAverageSpeed(vector<string> data) {
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

	cout << "The average Speed is: " << average << endl;
	cout << "The max value is: " << max << endl;
	cout << "The min value is: " << min << endl;
}

set<string> getEmotionOfSpeed(double value) {
	set<string> temp;

	if(value <= SLOW) {
		//temp.insert(SORROW); // Traurigkeit
		temp.insert(DISGUST); // Ekel
		temp.insert(JOY); // Freude
	}
	else if(value >= FAST) {
		temp.insert(FEAR); // Angst
		temp.insert(SURPRISE); // Ueberraschung
		temp.insert(ANGER); // Wut
		temp.insert(JOY); // Freude
	}
	else { //Sprechgeschwindigkeit is normal - so it is between slow and fast (2.5 < x < 6.6)
		temp.insert(ANGER); // Wut
		temp.insert(JOY); // Freude
	}

	return temp;
}

set<string> getEmotionOfPitch(string value) {
	set<string> temp;

	//string.compare returns 0 if equal
	if((value.compare(LOW1) == 0) || (value.compare(LOW2) == 0)) {
		temp.insert(DISGUST); // Ekel
		temp.insert(SORROW); // Traurigkeit
	}
	else if(value.compare(MEDIUM) == 0) {
		temp.insert(JOY); // Freude
	}
	else if((value.compare(HIGH1) == 0) || (value.compare(HIGH2) == 0)) {
		temp.insert(FEAR); // Angst
		temp.insert(SURPRISE); // Ueberraschung
		temp.insert(ANGER); // Wut
		temp.insert(JOY); // Freude
	}
	else {
		cerr << " Error: Could not read the data. Tried to read the following: "
			<< value << " . Are you sure that you set all macros right?" << endl;
	}

	return temp;
}

set<string> getEmotionOfIntensity(string value) {
	set<string> temp;

	//string.compare returns 0 if equal
	if((value.compare(LOW1) == 0) || (value.compare(LOW2) == 0)) {
		temp.insert(DISGUST); // Ekel
		temp.insert(SORROW); // Traurigkeit
	}
	else if(value.compare(MEDIUM) == 0) {
		temp.insert(JOY); // Freude
		temp.insert(DISGUST); // Ekel
	}
	else if((value.compare(HIGH1) == 0) || (value.compare(HIGH2) == 0)) {
		//temp.insert(FEAR); // Angst
		temp.insert(SURPRISE); // Ueberraschung
		temp.insert(ANGER); // Wut
		temp.insert(JOY); // Freude
	}
	else {
		cerr << " Error: Could not read the data. Tried to read the following: "
			<< value << " . Are you sure that you set all macros right?" << endl;
	}

	return temp;
}

map<string, double> plausibility(vector<Evidence> data) {
	std::cout << "Plausibility:" << endl;

	map<string, double> plausibility;

	//init
	for(const auto& emotion : OMEGA) {
		plausibility[emotion] = 0.0;
	}

	plausibility[PL_MAX] = 0.0;

	for(size_t i = 0; i < data.size(); i++) {
		//for each emotion
		for(const auto& emotion : OMEGA) {
			if(data[i].emotions.find(emotion) != data[i].emotions.end()) {
				//found element
				plausibility[emotion] += data[i].value;
			}
		}
	}

	//print
	string e;
	bool init = false;
	for(const auto& emotion : OMEGA) {
		if(init == false) {
			init = true;
			plausibility[PL_MAX] = plausibility[emotion];
			e = emotion;
		}

		if(plausibility[PL_MAX] < plausibility[emotion]) {
			plausibility[PL_MAX] = plausibility[emotion];
			e = emotion;
		}

		std::cout << "PI for Emotion: " << emotion << " : " << plausibility[emotion] << " " << endl;
	}

	std::cout << "Max PI found in Emotion: " << e << " : " << plausibility["max"] << endl;

	return plausibility;
}

void calculate_evidences(vector<string> data) {
	vector<vector<Evidence>> results;

	for(size_t i = DATA_START; i < data.size(); i = i + 4) {
		cout << "Takt " << data[i] << endl;

		set<string> speedEmotions = getEmotionOfSpeed(toDouble(data[i + 1]));
		double speedConfidence = CONF_SPEED;

		set<string> pitchEmotions = getEmotionOfPitch(data[i + 2]);
		double pitchConfidence = CONF_PITCH;

		set<string> intensityEmotions = getEmotionOfIntensity(data[i + 3]);
		double intensityConfidence = CONF_INTENSITY;

		results.push_back(
			dempster(speedEmotions, speedConfidence,
						pitchEmotions, pitchConfidence,
						intensityEmotions, intensityConfidence));

		plausibility(results.back());
		std::cout << endl;
	}
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

