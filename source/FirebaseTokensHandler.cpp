#include "FirebaseTokensHandler.h"

void FirebaseTokensHandler::setupTokenList()
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

void FirebaseTokensHandler::updateTokenList() {
    ofstream out(TOKEN_PATH, ios::trunc);
    for (string s : tokenList)
        out << s << endl;
    out.close();
}

list<string>& FirebaseTokensHandler::getTokenList() {
    return tokenList;
}
