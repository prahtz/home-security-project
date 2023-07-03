#include <string>
#include <list>
#include <fstream>

#include "Definitions.h"

using namespace std;

class FirebaseTokensHandler
{
private:
    list<string> tokenList;

public:
    void setupTokenList()
    {
        ifstream readingFile(TOKEN_PATH);
        string line;
        if (readingFile.is_open())
        {
            while (getline(readingFile, line))
                tokenList.push_back(line);
            readingFile.close();
        }
        else
        {
            ofstream createdFile(TOKEN_PATH);
            createdFile.close();
        }
    };

    void updateTokenList() {
        ofstream out(TOKEN_PATH, ios::trunc);
        for (string s : tokenList)
            out << s << endl;
        out.close();
    }

    list<string>& getTokenList() {
        return tokenList;
    }
};