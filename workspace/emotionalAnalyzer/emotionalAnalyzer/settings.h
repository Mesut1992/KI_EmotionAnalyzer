/*
This file contains all settings
*/

#define FILE1 "E_004.csv" //Path of 1st file
#define FILE2 "E_004b.csv" //Path of 1st file
#define FILE3 "E_004c.csv" //Path of 1st file
#define STARTOFDATA 4 //the data starts at element of 4, or index 4.

//Konfidenzfestlegung
#define KONF_SPRECHGESCHWINDIGKEIT 0.6
#define KONF_TONLAGE 0.8
#define KONF_SCHALLSTAERKE 0.7

//Ausprägungen der Evidenzen können hier gesetzt werden
//Sprechgeschwindigkeit 
#define SLOW 2.5 //untere Grenze
#define FAST 5.6 //obere Grenze
//Tonlage und Schallstärke
#define LOW1 "sehr niedrig"
#define LOW2 "niedrig"
#define MEDIUM "normal"
#define HIGH1 "hoch"
#define HIGH2 "sehr hoch"

set<string> omega;

void global_init(){
	omega.insert("A");
	omega.insert("U");
	omega.insert("W");
	omega.insert("F");
	omega.insert("E");
	omega.insert("T");
};