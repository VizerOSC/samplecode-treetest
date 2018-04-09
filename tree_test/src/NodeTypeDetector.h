#pragma once
#include <string>
#include <cstring>

namespace myapp {
    using std::string;

    class NodeTypeDetector {
        string buffer;

        struct {
            unsigned char num_signdetected : 1;
            unsigned char num_pointdetected : 1;
            unsigned char definitely_not_num : 1;

            unsigned int chr_lastslash : 1;
            unsigned int chr_openquote : 1;
            unsigned int chr_closequote : 1;
            unsigned int definitely_not_char : 1;

            unsigned int finished : 1;
            unsigned int well_formed : 1;
            
        } detectData;

    public:
        enum NodeType { UNDEFINED = 0, INTEGER = 1, REAL = 2, STRING = 3 };
        NodeType detectedType = UNDEFINED;

        bool pushSymbol(char c);

        void reset() {
            detectedType = UNDEFINED;
            buffer.clear();
            std::memset(&detectData, 0, sizeof(detectData));
        }

        string getBufferCopy() {
            return buffer;
        }

        bool tokenCompleted();
        bool tokenValid();

    private:
        void detectNumeric(char c);
        void detectCharacteric(char c);
    };
}