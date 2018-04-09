#include <cctype>
#include "NodeTypeDetector.h"

namespace myapp {
	///
	/// NodeTypeDetector is used to accumulate node data into buffer and detect its type.
	/// reset method is called to set initial state, and then characters provided one by one to pushSymbol method
	/// pushSymbol will return true when node data end detected, and then you can retreive detected type and accumulated string
	/// it also detects if accumulated data is well-formed, e.g. number does not contain alphas within it
	///
	bool NodeTypeDetector::pushSymbol(char c) {
		if (tokenCompleted()) return true;

		detectNumeric(c);
		detectCharacteric(c);

		bool completed = tokenCompleted();

		if (!completed) {
			buffer += c;
		}

		return completed;
	}
	
	///
	/// Check if accumulation end is reached. 
	/// Either we have completely valid accumulated value, or something wrong happened and there is no point to accumulate anything.
	///
	bool NodeTypeDetector::tokenCompleted() {
		if (detectData.finished) return true;
		if (detectData.definitely_not_char && detectData.definitely_not_num) return true;
		return false;
	}

	///
	/// Check if accumulated value is good
	/// There may be the case when value is good but we can accumulate more (new digits keep coming to a number)
	///
	bool NodeTypeDetector::tokenValid() {
		return detectData.well_formed;
	}

	void NodeTypeDetector::detectNumeric(char c) {
		if (detectedType != UNDEFINED && detectedType != INTEGER && detectedType != REAL) return;
		if (detectData.definitely_not_num) return;

		bool issign = (c == '+' || c == '-');
		bool isdigit = std::isdigit(c);

		//attempt to detect number
		if (detectedType == UNDEFINED) {
			(issign || isdigit) ? (detectedType = INTEGER) : (detectData.definitely_not_num = 1);
		}

		//keep reading and detect ill-formed
		if (detectedType == INTEGER || detectedType == REAL) {

			//number only allowed in the beginning
			if (issign) {
				detectData.num_signdetected || buffer.size() > 0 ? (detectData.finished = 1) : (detectData.num_signdetected = 1);
			}
			//no two points, or point in begin or end of number allowed
			else if (c == '.') {
				if (detectData.well_formed && !detectData.num_pointdetected) {
					detectedType = REAL;
					detectData.num_pointdetected = 1;
					detectData.well_formed = 0;
				}
				else {
					detectData.finished = 1;
				}
			}
			//consider any number that has at least one digit is well-formed
			else if (isdigit) {
				detectData.well_formed = 1;
			}
			//not numeric character is met
			else {
				detectData.finished = 1;
			}
		}
	}

	void NodeTypeDetector::detectCharacteric(char c) {
		if (detectedType != UNDEFINED && detectedType != STRING) return;
		if (detectData.definitely_not_char) return;

		//attempt to detect string (starts with a ")
		if (detectedType == UNDEFINED) {
			if (c == '"' && buffer.size() == 0) {
				detectedType = STRING;
				detectData.chr_openquote = 1;
			}
			else {
				detectData.definitely_not_char = 1;
			}
		}

		if (detectedType == STRING) {
			if (detectData.chr_closequote) {
				detectData.finished = 1;
				return;
			}

			//closing quote is detected, so we will finish accumulation on next call
			if (c == '"' && buffer.size() > 0) {
				detectData.chr_closequote = 1;
				detectData.well_formed = 1;
			}
		}
	}
}