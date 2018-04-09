#include <stdexcept>
#include "Parser.h"

namespace myapp {
    using std::runtime_error;

    ///
    /// Parser is responsible for parsing a input stream and signal its state to its client code,
    /// so Parser user can do things on found structure.
    ///
    /// Client code must call parse() sequentially and observe current parser state. There is a few of it:
    ///  RESET		- initial state where all things begins
    ///  FOUND_DATA - first state poped up, if parser finds something its can parse. 
    ///				  Appears only once before whole tree structure.
    ///  NO_DATA    - poped up, when parsing is either ended (expectedly or not), 
    ///				  or when there is nothing to parse from the beginning
    ///  FOUND_NODE - a node with data found, and you can observe its data by getCurrentNodeData and getCurrentNodeType
    ///  NESTING_STARTED - a nested level of nodes relatively to latest found node is started
    ///  NESTING_ENDED   - previously found nested level ended, so we returned one level up
    ///
    /// Parser also has internal state, namely last type of token its handled. It detects what needs to be
    /// done checking its internal state and determining what next state is possible from current state
    /// (e.g. we know we can except comma or colon followed after node data or curly brace after comma)
    ///

    /* static constants */

    const string Parser::TOK_TREE_START("tree(");
    const char Parser::TOK_TREE_END = ')';
    const char Parser::TOK_NESTING_START = '{';
    const char Parser::TOK_NESTING_END = '}';
    const char Parser::TOK_DELIMITER = ',';
    const char Parser::TOK_CHILDREN_EXISTS = ':';

    Parser* Parser::parse() {
        if (!*src) {
            if (state == NO_DATA || state == RESET) {
                return this;
            }
            else if(src->eof()) {
                throw runtime_error("Unexpected end of file");
            }
            else {
                throw runtime_error("Unable read input file");
            }
            return this;
        }

        ++op_id;
        int expected = getExpectedTokens(currentToken);

        if (expected & TOKEN_TREE_START) {
            if (tryFindTree()) {
                state = FOUND_DATA;
                currentToken = TOKEN_TREE_START;
            }
            else {
                state = NO_DATA;
                currentToken = TOKEN_UNDEFINED;
            }

            return this;
        }

        if (expected & TOKEN_DATA_CONTENT) {
            if (tryFindNodeData()) {
                op_id_stack.push(op_id);
                state = FOUND_NODE;
                currentToken = TOKEN_DATA_CONTENT;
                return this;
            }
        }

        if (expected & TOKEN_CHILDREN_EXISTS) {
            if (tryFindChildrenExists()) {
                currentToken = TOKEN_CHILDREN_EXISTS;
                return parse();
            }
        }

        if (expected & TOKEN_NESTING_START) {
            if (tryFindNestingStart()) {
                state = NESTING_STARTED;
                currentToken = TOKEN_NESTING_START;
                return this;
            }
        }

        if (expected & TOKEN_NESTING_END) {
            if (tryFindNestingEnd()) {
                op_id_stack.pop();
                state = NESTING_ENDED;
                currentToken = TOKEN_NESTING_END;
                return this;
            }
        }

        if (expected & TOKEN_DELIMITER) {
            if (tryFindDelimiter()) {
                op_id_stack.pop();
                currentToken = TOKEN_DELIMITER;
                return parse();
            }
        }

        if (expected & TOKEN_TREE_END) {
            if (tryFindTreeEnd()) {
                op_id_stack.pop();
                state = NO_DATA;
                currentToken = TOKEN_TREE_END;
                return this;
            }
        }

        throw runtime_error("Input source is mailformed or currupt: syntax error");

        return this;
    }

    int Parser::getExpectedTokens(Token token) {
        switch (token)
        {
        case TOKEN_UNDEFINED:
            return TOKEN_TREE_START;
        case TOKEN_DATA_CONTENT:
            return TOKEN_CHILDREN_EXISTS
                | ((op_id_stack.size() <= 1) ? TOKEN_TREE_END : TOKEN_NESTING_END)
                | TOKEN_DELIMITER;
        case TOKEN_CHILDREN_EXISTS:
            return TOKEN_NESTING_START;
        case TOKEN_NESTING_START:
            return TOKEN_DATA_CONTENT | TOKEN_NESTING_END;
        case TOKEN_NESTING_END:
            return TOKEN_DELIMITER | ((op_id_stack.size() <= 1) ? TOKEN_TREE_END : TOKEN_NESTING_END);
        case TOKEN_DELIMITER:
            return TOKEN_DATA_CONTENT;
        case TOKEN_TREE_START:
            return TOKEN_DATA_CONTENT;
        case TOKEN_TREE_END:
            return TOKEN_UNDEFINED;
        }

        return TOKEN_UNDEFINED;
    }

    bool Parser::tryFindTree() {
        char c;
        bool found = false;

        while (!found && *src >> c) {

            string::size_type i = 0;

            while (i < TOK_TREE_START.size()) {
                if (TOK_TREE_START[i] != c) break;
                if (i == TOK_TREE_START.size() - 1) {
                    found = true;
                    break;
                }

                *src >> c;
                ++i;

                if (!src->good()) break;
            }
        }		

        return found;
    }

    bool Parser::tryFindNodeData() {
        char c;
        typeDetector.reset();

        *src >> c;

        if (typeDetector.pushSymbol(c)) {
            if (!typeDetector.tokenValid()) {
                src->unget();
                return false;
            }
        }

        while (src->get(c) && !typeDetector.pushSymbol(c));

        src->unget();

        if (!typeDetector.tokenCompleted()) {
            throw runtime_error("Input source is mailformed or currupt: unexpected end of data");
        }

        if (typeDetector.tokenCompleted() && !typeDetector.tokenValid()) {
            throw runtime_error("Input source is mailformed or corrupt: cannot read node content");
        }

        currentNodeType = static_cast<NodeType>(typeDetector.detectedType);
        currentNodeBuffer = typeDetector.getBufferCopy();

        return true;
    }

    bool Parser::tryFindChildrenExists() {
        char c;
        *src >> c;

        if (c == TOK_CHILDREN_EXISTS) {
            return true;
        }
        else {
            src->unget();
            return false;
        }
    }

    bool Parser::tryFindNestingStart() {
        char c;
        *src >> c;
        if (c == TOK_NESTING_START) {
            return true;
        }
        else {
            src->unget();
            return false;
        }
    }

    bool Parser::tryFindNestingEnd() {
        char c;
        *src >> c;
        if (c == TOK_NESTING_END) {
            return true;
        }
        else {
            src->unget();
            return false;
        }
    }

    bool Parser::tryFindDelimiter() {
        char c;
        *src >> c;
        if (c == TOK_DELIMITER) {
            return true;
        }
        else {
            src->unget();
            return false;
        }
    }

    bool Parser::tryFindTreeEnd() {
        char c;
        *src >> c;
        if (c == TOK_TREE_END) {
            return true;
        }
        else {
            src->unget();
            return false;
        }
    }

    void* Parser::getCurrentNodeData() {
        if (currentNodeType == INTEGER) {
            long long int *newint = nullptr;

            try {
                newint = new long long int(std::stoll(currentNodeBuffer));
                return static_cast<void*>(newint);
            }
            catch (const std::exception& e) {
                string msg;
                if (typeid(e) == typeid(std::out_of_range)) {
                    msg = "Input source is mailformed or corrupt: too big integer is met";
                } 
                else {
                    msg = "Input source is mailformed or corrupt: error while interpreting data node";
                }

                if(newint != nullptr) delete newint;

                throw std::runtime_error(msg);
            }			
        }
        else if (currentNodeType == REAL) {
            long double *newdouble = nullptr;

            try {
                newdouble = new long double(std::stold(currentNodeBuffer));
                return static_cast<void*>(newdouble);
            }
            catch (const std::exception& e) {
                string msg;
                if (typeid(e) == typeid(std::out_of_range)) {
                    msg = "Input source is mailformed or corrupt: too big integer is met";
                }
                else {
                    msg = "Input source is mailformed or corrupt: error while interpreting data node";
                }

                if (newdouble != nullptr) delete newdouble;

                throw std::runtime_error(msg);
            }

        }
        else if (currentNodeType == STRING) {
            string *newstr = nullptr;

            try {
                newstr = new string(currentNodeBuffer);
                return static_cast<void*>(newstr);
            }
            catch (...) {
                if (newstr != nullptr) delete newstr;

                throw std::runtime_error("Input source is mailformed or corrupt: error while interpreting data node");
            }
        }
        return nullptr;
    }
}