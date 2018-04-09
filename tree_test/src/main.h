#pragma once
#include <string>

namespace myapp {
    using std::string;
    enum OperationType { OP_READ = 1, OP_WRITE = 2 };

    struct Operation {
        string inputFile;
        string outputFile;
        char type;
    };

    Operation getOperation(int argc, char ** argv);
}
