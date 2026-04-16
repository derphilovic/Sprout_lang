#include "parser.h"

#include <iostream>

namespace sprout::parser {
    ASTNode* parseExpression(TokenSource& s);

    ASTNode* parsePrimary(TokenSource& s) {
        lexer::Token t = peek(s);

        if (t.type == lexer::NUMBER) {
            advance(s);
            return new IntLiteralNode{NODE_INT_LITERAL, std::stoll(t.content)};
        }
        if (t.type == lexer::IDENT) {
            advance(s);
            return new IdentNode{NODE_IDENT, t.content};
        }
        if (t.type == lexer::L_PAREN) {
            advance(s);                          // consume (
            ASTNode* inner = parseExpression(s); // full expression inside
            expect(s, lexer::R_PAREN);           // consume )
            return inner;                        // no new node, just unwrap
        }

        throw std::runtime_error("Unexpected token: " + t.content);
    }

    ASTNode* parseUnary(TokenSource& s) {
        if (peek(s).type == lexer::MINUS || peek(s).type == lexer::NOT) {
            lexer::Token op = advance(s);
            ASTNode* operand = parseUnary(s); // recursive, handles !!x or --x
            return new UnaryOpNode{NODE_UNARY_OP, op.type, operand};
        }
        return parsePrimary(s);
    }

    ASTNode* parseMultiplicative(TokenSource& s) {
        ASTNode* left = parseUnary(s);
        while (peek(s).type == lexer::STAR || peek(s).type == lexer::SLASH) {
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseUnary(s);
            left = new BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseAdditive(TokenSource& s) {
        ASTNode* left = parseMultiplicative(s);
        while (peek(s).type == lexer::PLUS || peek(s).type == lexer::MINUS) {
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseMultiplicative(s);
            left = new BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseComparison(TokenSource& s) {
        ASTNode* left = parseAdditive(s);
        while (peek(s).type == lexer::EQEQ || peek(s).type == lexer::NOT_EQ || peek(s).type == lexer::GREATER ||
            peek(s).type == lexer::LOWER) { //Implement <= and >= once interpreter supports
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseAdditive(s);
            left = new BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseLogical(TokenSource& s) {
        ASTNode* left = parseComparison(s);
        while (peek(s).type == lexer::DOUBLE_AND || peek(s).type == lexer::DOUBLE_VBAR) {
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseComparison(s);
            left = new BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseExpression(TokenSource& s) {
        return parseLogical(s);
    }

    ASTNode* parseIdentifier(TokenSource& s) {
        if (peek(s).type != lexer::IDENT) throw std::runtime_error("Expected identifier, got: " + peek(s).content);
        return new IdentNode{NODE_IDENT,advance(s).content};
    }

    ASTNode* parseVarDecl(TokenSource& s) {
        expect(s, lexer::VAR);
        std::string name = peek(s).content;
        expect(s, lexer::IDENT);
        if (peek(s).type == lexer::ARR_RIGHT) {
            advance(s);
            advance(s);
        }
        expect(s, lexer::EQ);
        ASTNode* expr = parseExpression(s);

        return new VarDeclNode{NODE_VAR_DECL, name, expr};
    }

    ASTNode* parseProgram(std::vector<lexer::Token>& tokens) {
        TokenSource s = {tokens, 0};
        std::vector<ASTNode*> program;
        while (s.pos < s.token.size()) {
            switch (s.token[s.pos].type) {
                case lexer::VAR: program.push_back(parseVarDecl(s)); break;
                default: advance(s); break;
            }
        }
        return new ProgramNode{NODE_PROGRAM, program};
    }

    //======================
    //      DEBUG FUNC
    //======================

    void printAST(ASTNode* node, int indent) {
        std::string pad(indent * 2, ' ');

        switch (node->type) {
            case NODE_PROGRAM: {
                auto* n = static_cast<ProgramNode*>(node);
                std::cout << pad << "Program\n";
                for (auto* child : n->program)
                    printAST(child, indent + 1);
                break;
            }
            case NODE_BINARY_OP: {
                auto* n = static_cast<BinaryOpNode*>(node);
                std::cout << pad << "BinaryOp(" << tokenTypeName(n->op) << ")\n";
                printAST(n->left, indent + 1);
                printAST(n->right, indent + 1);
                break;
            }
            case NODE_UNARY_OP: {
                auto* n = static_cast<UnaryOpNode*>(node);
                std::cout << pad << "UnaryOp(" << tokenTypeName(n->op) << ")\n";
                printAST(n->operand, indent + 1);
                break;
            }
            case NODE_VAR_DECL: {
                auto* n = static_cast<VarDeclNode*>(node);
                std::cout << pad << "VarDecl(" << n->identifier << ")\n";
                printAST(n->expression, indent + 1);
                break;
            }
            case NODE_INT_LITERAL: {
                auto* n = static_cast<IntLiteralNode*>(node);
                std::cout << pad << "Int(" << n->number << ")\n";
                break;
            }
            case NODE_IDENT: {
                auto* n = static_cast<IdentNode*>(node);
                std::cout << pad << "Ident(" << n->identifier << ")\n";
                break;
            }
            default:
                std::cout << pad << "Unknown node\n";
        }
    }

}
