#include "compiler.h"
#include "lexer.h"
#include "parser.h"

int main() {
    std::vector<sprout::lexer::Token> tokens = run();
    sprout::memManager::Memory mem {};
    sprout::parser::ASTNode* tree = sprout::parser::parseProgram(tokens, mem);
    sprout::parser::printAST(tree, 1);
    sprout::memManager::freeMemory(mem);
    return 0;
}