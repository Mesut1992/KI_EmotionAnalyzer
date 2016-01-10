/*
	This file contains all settings
*/

#ifndef _SETTINGS_H
#define _SETTINGS_H

// I/O File Settings
#define FILE1 "E_004.csv" // Path of 1st file
#define FILE2 "E_004b.csv" // Path of 2nd file
//#define FILE3 "E_004c.csv" // Path of 3rd file - Not applieable for this project

#define OUTPUT1 "resulta.csv" // Path of Output File
#define OUTPUT2 "resultb.csv" //Path of second output file

#define DATA_START 4 //the data starts at element of 4, or index 4 (to skip header)

// Definitions of Emotions
#define FEAR "A" // Angst
#define SURPRISE "U" // Ueberraschung
#define ANGER "W" // Wut
#define JOY "F" // Freude 
#define DISGUST "E" // Ekel
#define SORROW "T" // Traurigkeit

// Definitions of Confidences
#define CONF_SPEED 0.6
#define CONF_PITCH 0.8
#define CONF_INTENSITY 0.7

// Definitions 
// ...for Speed 
#define SLOW 2.5 //untere Grenze
#define FAST 5.6 //obere Grenze

// ...for Pitch and Intensity
#define LOW1 "sehr niedrig"
#define LOW2 "niedrig"
#define MEDIUM "normal"
#define HIGH1 "hoch"
#define HIGH2 "sehr hoch"

// Definitions for Plausibility
#define PL_MAX "max"

// Definitions for Output File
#define CSV_SEP ';'
#define CSV_ENDL '\n'
#define HEAD_CLK "Takt"
#define HEAD_MAX "Max"
#define HEAD_MAXVAL "PL of Max"

namespace std {

	set<string> OMEGA;

	void global_init() {
		OMEGA.insert(FEAR);
		OMEGA.insert(SURPRISE);
		OMEGA.insert(ANGER);
		OMEGA.insert(JOY);
		OMEGA.insert(DISGUST);
		OMEGA.insert(SORROW);
	};

}

#endif // _SETTINGS_H