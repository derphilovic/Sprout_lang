#include "compiler.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    std::string CodeStr;
    for (int i = 1; i < argc; i++) {
        if (!CodeStr.empty()) CodeStr += ' ';
        CodeStr += argv[i];
    }

    std::vector<sprout::lexer::Token> tokens = tokenize(CodeStr);
    sprout::memManager::Memory mem {};
    sprout::parser::ASTNode* tree = sprout::parser::parseProgram(tokens, mem);
    sprout::parser::printAST(tree, 1);
    sprout::memManager::freeMemory(mem);
    return 0;
}