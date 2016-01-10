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

// Predefined Functions (all functions in this file):

double toDouble(string s);
void printFile(string filename);
vector<string> readFileInVector(string filename);
void writeResultsToFile(string filename, 
						vector<vector<Evidence>> evidences, 
						vector<map<string, double>> plausibilities);
void printAverageSpeed(vector<string> data);
set<string> getEmotionOfSpeed(double);
set<string> getEmotionOfPitch(string);
set<string> getEmotionOfIntensity(string);
void calculatePlausibilities(string input, string output);

// Implementation

double toDouble(string s) {
	replace(s.begin(), s.end(), ',', '.');
	return atof(s.c_str());
}

void printFile(string filename) {
	ifstream file(filename);
	string value;

	while(file.good()) {
		getline(file, value, ';'); // read a string until next comma:
								// http://www.cplusplus.com/reference/string/getline/
		//cout << string(value, 1, value.length() - 2); 
		// display value removing the first and the last character from it
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
			getline(file, value, CSV_ENDL); // read a string until next comma
		}
		else {
			getline(file, value, CSV_SEP); // read a string until next comma
		}

		read.push_back(value);
	}

	if(read.empty()) {
		cerr << " Error: Vector empty. Probably unreadable file";
	}

	read.pop_back(); //empty element at the end gave me headache

	file.close();

	return read;
}

void writeResultsToFile(string input, string filename, 
						vector<vector<Evidence>> evidences, 
						vector<map<string, double>> plausibilities) {
	if(evidences.size() != plausibilities.size()) {
		cout << " Error: Something went wrong while calculating "
			<< "evidences and plausibilities!" << endl;

		return;
	}
	
	ofstream file(filename);

	file << HEAD_CLK << CSV_SEP
		<< FEAR << CSV_SEP
		<< SURPRISE << CSV_SEP
		<< ANGER << CSV_SEP
		<< JOY << CSV_SEP
		<< DISGUST << CSV_SEP
		<< SORROW << CSV_SEP
		<< HEAD_MAX << CSV_SEP
		<< HEAD_MAXVAL << CSV_ENDL;

	double crt;
	double max;
	string e;

	for(size_t i = 0; i < evidences.size() && file.good(); i++) {
		crt = 0.0;
		max = 0.0;

		file << (i + 1) << CSV_SEP;

		for(const auto& emotion : OMEGA) {
			file << (crt = plausibilities[i][emotion]) << CSV_SEP;

			if(crt > max) {
				e = emotion;
				max = crt;
			}
		}

		file << e << CSV_SEP << max << CSV_ENDL;
	}

	file.close();
	std::cout << "Saved results of data " << input << " in file: " << filename << "." << endl;
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
	else { //Speed is normal - so it is between slow and fast (2.5 < x < 6.6)
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

void calculatePlausibilities(string input, string output) {
	vector<string> data = readFileInVector(input);

	vector<vector<Evidence>> evidences;
	vector<map<string, double>> plausibilities;

	for(size_t i = DATA_START; i < data.size(); i = i + 4) {
		cout << "Takt " << data[i] << endl;

		set<string> speedEmotions = getEmotionOfSpeed(toDouble(data[i + 1]));
		double speedConfidence = CONF_SPEED;

		set<string> pitchEmotions = getEmotionOfPitch(data[i + 2]);
		double pitchConfidence = CONF_PITCH;

		set<string> intensityEmotions = getEmotionOfIntensity(data[i + 3]);
		double intensityConfidence = CONF_INTENSITY;

		evidences.push_back(
			dempster(speedEmotions, speedConfidence,
						pitchEmotions, pitchConfidence,
						intensityEmotions, intensityConfidence));

		plausibilities.push_back(plausibility(evidences.back()));

		std::cout << endl;
	}

	writeResultsToFile(input, output, evidences, plausibilities);

	cout << " Note: Done." << endl;
}

int main() {
	global_init();

	char pause;
	std::cout << "Press 'enter' to start with the file " << FILE1 << "." << endl;
	pause = getchar();
	calculatePlausibilities(FILE1, OUTPUT1);
		
	std::cout << "Press 'enter' to continue with the file " << FILE2 << "." << endl;
	pause = getchar();
	calculatePlausibilities(FILE2, OUTPUT2);

	std::cout << "To exit this programm press 'enter'." << endl;
	pause = getchar();

	return 0;
}