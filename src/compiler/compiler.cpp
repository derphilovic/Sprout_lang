#include "compiler.h"
#include "lexer.h"
#include "parser.h"

int main() {
    std::vector<sprout::lexer::Token> tokens = run();
    sprout::parser::ASTNode* tree = sprout::parser::parseProgram(tokens);
    sprout::parser::printAST(tree, 1);
    return 0;
}