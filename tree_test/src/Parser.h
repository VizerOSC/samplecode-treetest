#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <stdexcept>
#include "NodeTypeDetector.h"

namespace myapp {
	using std::istream;
	using std::vector;
	using std::string;
	using std::stack;
	using std::runtime_error;
	
	class Parser {
	public:
		enum NodeType { UNDEFINED = 0, INTEGER = 1, REAL = 2, STRING = 3 };
		enum State { RESET = 0, FOUND_DATA, NO_DATA, NESTING_STARTED, NESTING_ENDED, FOUND_NODE };

	protected:
		/* types */
		enum Token {
			TOKEN_UNDEFINED			= 0,
			TOKEN_DATA_CONTENT		= 1 << 0,	// the data itself (string/numeric)
			TOKEN_CHILDREN_EXISTS	= 1 << 1,   // :
			TOKEN_NESTING_START		= 1 << 2,   // {
			TOKEN_NESTING_END		= 1 << 3,   // }
			TOKEN_DELIMITER			= 1 << 4,   // ,
			TOKEN_TREE_START		= 1 << 5,   // tree(
			TOKEN_TREE_END			= 1 << 6    // )
		};

		/* vars */
		istream* src;
		string currentNodeBuffer;
		NodeTypeDetector typeDetector;

		State state = RESET;
		Token currentToken = TOKEN_UNDEFINED;
		NodeType currentNodeType = UNDEFINED;

		int op_id = 0;
		stack<int> op_id_stack;	

		static const string TOK_TREE_START;
		static const char TOK_TREE_END;
		static const char TOK_NESTING_START;
		static const char TOK_NESTING_END;
		static const char TOK_DELIMITER;
		static const char TOK_CHILDREN_EXISTS;
				

	public:
		void setSource(istream& source);
		NodeType getCurrentNodeType();
		State getCurrentState();
		int getCurrentOpId();
		int getCurrentParentOpId();
		void reset();
		void* getCurrentNodeData();
		Parser* parse();


	protected:
		bool tryFindTree();
		bool tryFindNodeData();
		bool tryFindChildrenExists();
		bool tryFindNestingStart();
		bool tryFindNestingEnd();
		bool tryFindDelimiter();
		bool tryFindTreeEnd();

		int getExpectedTokens(Token token);
	};

	/* inline methods */

	inline void Parser::setSource(istream& source) {
		reset();

		if (source) {
			src = &source;
		}
		else {
			throw runtime_error("Unable to read input file");
		}
	}

	inline Parser::NodeType Parser::getCurrentNodeType() {
		return currentNodeType;
	}

	inline Parser::State Parser::getCurrentState() {
		return state;
	}

	inline int Parser::getCurrentOpId() {
		return op_id;
	}

	inline int Parser::getCurrentParentOpId() {
		return op_id_stack.top();
	}

	inline void Parser::reset() {
		src = nullptr;
		state = RESET;
		currentNodeType = UNDEFINED;
		op_id = 0;
		op_id_stack = stack<int>();
	}
}