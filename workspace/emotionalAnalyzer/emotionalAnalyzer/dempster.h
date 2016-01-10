//TODO Extract all macros to header file and import it, should be handled like a settings file
//#include "macros.h"

#ifndef _DEMPSTER_H
#define _DEMPSTER_H

#include <vector>
#include <string>
#include <set>
#include "settings.h"

namespace std {

	struct Evidence {
		Evidence() {} // to dynamically add elements to a vector 
		set<string> emotions;
		double value = 0.0;
	};

	vector<Evidence> dempster(set<string> speedEmotions, double speedConfidence,
							  set<string> pitchEmotions, double pitchConfidence,
							  set<string> intensityEmotions, double intensityConfidence) {

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
		m_1o.emotions = OMEGA;
		m_1o.value = (1 - m_1a.value);
		m_1.push_back(&m_1a);
		m_1.push_back(&m_1o); //OMEGA - index 1

		// m_2: Pitch
		m_2a.emotions = pitchEmotions;
		m_2a.value = pitchConfidence;
		m_2o.emotions = OMEGA;
		m_2o.value = (1 - m_2a.value);
		m_2.push_back(&m_2a);
		m_2.push_back(&m_2o); //OMEGA - index 1

		// m_3: Intensity
		m_3a.emotions = intensityEmotions;
		m_3a.value = intensityConfidence;
		m_3o.emotions = OMEGA;
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
				cout << " Note: Found empty set in m_12" << endl;

				double k = 1 / (1 - m_12[i].value); // calculate correction factor k

				m_12.erase(m_12.begin() + i); // delete empty element i

				for(size_t a = 0; a < m_12.size(); a++) { // correct remaining elements with k
					m_12[a].value *= k;
				}

				cout << " Note: Corrected m_12 with value k=" << k << endl;

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
				cout << " Note: Found empty set in m_123" << endl;

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

			cout << " Note: Corrected m_123 with k=" << k << endl;
		}

		return m_123;
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

			std::cout << "PI for Emotion: " << emotion << " : "
				<< plausibility[emotion] << " " << endl;
		}

		std::cout << "Max PI found in Emotion: " << e << " : "
			<< plausibility["max"] << endl;

		return plausibility;
	}
}

#endif // _DEMPSTER_H


