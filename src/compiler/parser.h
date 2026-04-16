#pragma once
#include <stdexcept>
#include <vector>
#include <string>
#include "lexer.h"

namespace sprout::parser {
    struct TokenSource {
        std::vector<lexer::Token> token;
        uint64_t pos = 0;
    };

    inline lexer::Token peek(TokenSource s) {
        return s.token[s.pos];
    }

    inline lexer::Token advance(TokenSource s) {
        lexer::Token tok = s.token[s.pos];
        s.pos++;
        return tok;
    }

    inline lexer::Token expect(TokenSource s, lexer::TokenType expected) {
        lexer::Token tok = s.token[s.pos];
        if (tok.type != expected) throw std::runtime_error("Other token expected!");

        return tok;
    }

    enum NodeType {
        ProgramNode,
        VarDeclNode,
        BinaryOpNode,
        IdentNode,
        IntLiteralNode,
        DebugOutNode,
    };

    enum BinaryOpType {
        ADDITION,
        MULTIPLICATION,

    };

    struct ASTNode {
        NodeType type;
    };

    struct ProgramNode : ASTNode {
        std::vector<ASTNode*> program;
    };

    struct binaryOpNode : ASTNode {
        BinaryOpType Operand;
        ASTNode* left;
        ASTNode* right;
    };

    struct IdentNode : ASTNode {
        std::string identifier;
    };

    struct IntLiteralNode {
        int64_t number;
    };

    struct VarDeclNode : ASTNode {
        ASTNode* identifier;
        ASTNode* expression;
    };

    struct debugNode : ASTNode {
        ASTNode* identifier;
    };

}
