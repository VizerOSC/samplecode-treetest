#include <string>
#include <cstring>
#include "main.h"
#include "TreeApplication.h"

namespace myapp {
    using std::string;
    using std::strcmp;



    Operation getOperation(int argc, char ** argv) {

        Operation op = {};
        if (argc != 2 && argc != 4) return op;

        int i = 0;

        do {

            if (strcmp(*argv, "-i") == 0) {
                op.type |= OP_READ;
                op.inputFile = *(argv + 1);
            }
            else if (strcmp(*argv, "-o") == 0) {
                op.type |= OP_WRITE;
                op.outputFile = *(argv + 1);
            }

            argv += 2;
        } while (*argv != nullptr);

        return op;
    }
}

int main(int argc, char ** argv) {
    myapp::TreeApplication app;

    app.operate(myapp::getOperation(argc - 1, ++argv));

    return 0;
}

