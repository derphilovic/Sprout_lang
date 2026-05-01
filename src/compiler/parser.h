#pragma once
#include <stdexcept>
#include <vector>
#include <string>
#include "lexer.h"
#include "memManager.h"

namespace sprout::parser {
    struct TokenSource {
        std::vector<lexer::Token> token;
        uint64_t pos = 0;
    };

    inline lexer::Token peek(TokenSource& s) {
        if (s.pos >= s.token.size()) return {lexer::END_OF_FILE, ""};
        return s.token[s.pos];
    }

    inline lexer::Token advance(TokenSource& s) {
        if (s.pos >= s.token.size()) return {lexer::END_OF_FILE, ""};
        lexer::Token tok = s.token[s.pos];
        s.pos++;
        return tok;
    }

    inline lexer::Token expect(TokenSource& s, lexer::TokenType expected) {
        if (s.pos >= s.token.size()) throw std::runtime_error("Unexpected end of input");
        lexer::Token tok = s.token[s.pos];
        if (tok.type != expected) throw std::runtime_error("Other token expected!");
        s.pos++;
        return tok;
    }

    enum NodeType {
        NODE_PROGRAM,
        NODE_VAR_DECL,
        NODE_VAR_ASSIGN,
        NODE_FUNC_CALL,
        NODE_BINARY_OP,
        NODE_UNARY_OP,
        NODE_IDENT,
        NODE_INT_LITERAL,
        NODE_DEBUG_OUT,
    };

    struct ASTNode {
        NodeType type;
    };

    struct ProgramNode : ASTNode {
        std::vector<ASTNode*> program;
    };

    struct BinaryOpNode : ASTNode {
        lexer::TokenType op;
        ASTNode* left;
        ASTNode* right;
    };

    struct UnaryOpNode : ASTNode {
        lexer::TokenType op;
        ASTNode* operand;
    };

    struct IdentNode : ASTNode {
        std::string identifier;
    };

    struct IntLiteralNode : ASTNode {
        int64_t number;
    };

    struct VarDeclNode : ASTNode {
        std::string identifier;
        ASTNode* expression;
    };

    struct VarAssignNode : ASTNode {
        std::string identifier;
        ASTNode* expression;
    };

    struct FuncCallNode : ASTNode {
        std::string identifier;
        std::vector<ASTNode*> args;
    };

    struct DebugNode : ASTNode {
        ASTNode* identifier;
    };

    ASTNode* parseProgram(std::vector<lexer::Token>& tokens, memManager::Memory& mem);

    //======================
    //      DEBUG FUNC
    //======================
    inline std::string tokenTypeName(lexer::TokenType t) {
        switch (t) {
            case lexer::PLUS:     return "+";
            case lexer::MINUS:    return "-";
            case lexer::STAR:     return "*";
            case lexer::SLASH:    return "/";
            case lexer::EQEQ:     return "==";
            case lexer::NOT_EQ:   return "!=";
            case lexer::LOWER:    return "<";
            case lexer::GREATER:  return ">";
            default:              return "?";
        }
    }
    void printAST(ASTNode* node, int indent = 1);

}
