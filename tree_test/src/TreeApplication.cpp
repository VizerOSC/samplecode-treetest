#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "TreeApplication.h"

namespace myapp {
	using std::ifstream;
	using std::ofstream;
	using std::runtime_error;
	using std::exception;
	using std::cout;
	using std::cerr;
	using std::endl;

	///
	/// Main entry point of application, provided with Operation object, from which is known 
	/// what task application need to perform, and which files to use.
	///

	void TreeApplication::operate(Operation op) {
		ITreeNode* root = nullptr;

		try {
			if (op.type & OP_READ) {
				root = read(op.inputFile);
			}

			if (root) {
				cout << "Tree structure readed from file " << op.inputFile << ':' << endl << endl;
				print(cout, root);
				cout << endl << endl;
			}

			if (root && op.type & OP_WRITE) {
				write(op.outputFile, root);
				cout << "Serialized back to file " << op.outputFile << endl;
			}

			//whether anything is selected
			if (op.type & OP_READ ^ OP_READ && op.type & OP_WRITE ^ OP_WRITE) {
				cerr << "Usage: test -i <input_file.txt> [-o <output_file.txt>]";
			}
		}
		catch (const runtime_error& e) {
			cerr << e.what();
		}
		catch (const exception& e) {
			cerr << e.what();
		}
		catch (...) {
			cerr << "Unknown exception occured";
		}

		return;
	}

	ITreeNode* TreeApplication::read(string& filename) {
		ifstream in(filename);

		if (!in) {
			throw runtime_error("Could not open input file " + filename);
		}


		return unserializer.unserialize(in);
	}

	void TreeApplication::write(string& filename, ITreeNode* rootNode) {
		ofstream out(filename);

		if (!out) {
			throw runtime_error("Could not open output file " + filename);
		}

		serializer.output(out, rootNode);
	}

	void TreeApplication::print(ostream& os, ITreeNode* rootNode) {
		printer.output(os, rootNode);
	}
		
}