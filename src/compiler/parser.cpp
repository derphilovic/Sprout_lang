#include "parser.h"
#include <iostream>

namespace sprout::parser {
    ASTNode* parseExpression(TokenSource & token_source, memManager::Memory& mem);

    ASTNode* parsePrimary(TokenSource& s, memManager::Memory& mem) {
        lexer::Token t = peek(s);

        if (t.type == lexer::NUMBER) {
            advance(s);
            auto ptr = memManager::allocateMemory(mem,sizeof(IntLiteralNode));
            return new(ptr) IntLiteralNode{NODE_INT_LITERAL, std::stoll(t.content)};
        }
        if (t.type == lexer::IDENT) {
            advance(s);

            if (lexer::COLON == peek(s).type) {
                advance(s);
                std::vector<ASTNode*> args;

                while (lexer::SEMICOLON != peek(s).type) {
                    args.push_back(parseExpression(s, mem));
                    if (peek(s).type == lexer::COMMA) {
                        advance(s);
                    } else if (peek(s).type != lexer::SEMICOLON) {
                        throw std::runtime_error("Expected ',' or ';' in argument list, got: " + peek(s).content);
                    }
                }
                advance(s);
                auto ptr = memManager::allocateMemory(mem,sizeof(FuncCallNode));
                return new(ptr) FuncCallNode{NODE_FUNC_CALL, t.content, args};
            }

            auto ptr = memManager::allocateMemory(mem,sizeof(IdentNode));
            return new(ptr) IdentNode{NODE_IDENT, t.content};
        }
        if (t.type == lexer::L_PAREN) {
            advance(s);                          // consume (
            ASTNode* inner = parseExpression(s, mem); // full expression inside
            expect(s, lexer::R_PAREN);           // consume )
            return inner;                        // no new node, just unwrap
        }

        throw std::runtime_error("Unexpected token: " + t.content);
    }

    ASTNode* parseUnary(TokenSource& s, memManager::Memory& mem) {
        if (peek(s).type == lexer::MINUS || peek(s).type == lexer::NOT) {
            lexer::Token op = advance(s);
            ASTNode* operand = parseUnary(s, mem); // recursive, handles !!x or --x
            auto ptr = memManager::allocateMemory(mem,sizeof(UnaryOpNode));
            return new(ptr) UnaryOpNode{NODE_UNARY_OP, op.type, operand};
        }
        return parsePrimary(s, mem);
    }

    ASTNode* parseMultiplicative(TokenSource& s, memManager::Memory& mem) {
        ASTNode* left = parseUnary(s, mem);
        while (peek(s).type == lexer::STAR || peek(s).type == lexer::SLASH) {
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseUnary(s, mem);
            auto ptr = memManager::allocateMemory(mem,sizeof(BinaryOpNode));
            left = new(ptr) BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseAdditive(TokenSource& s, memManager::Memory& mem) {
        ASTNode* left = parseMultiplicative(s, mem);
        while (peek(s).type == lexer::PLUS || peek(s).type == lexer::MINUS) {
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseMultiplicative(s, mem);
            auto ptr = memManager::allocateMemory(mem,sizeof(BinaryOpNode));
            left = new(ptr) BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseComparison(TokenSource& s, memManager::Memory& mem) {
        ASTNode* left = parseAdditive(s, mem);
        while (peek(s).type == lexer::EQEQ || peek(s).type == lexer::NOT_EQ || peek(s).type == lexer::GREATER ||
            peek(s).type == lexer::LOWER) { //Implement <= and >= once interpreter supports
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseAdditive(s, mem);
            auto ptr = memManager::allocateMemory(mem,sizeof(BinaryOpNode));
            left = new(ptr) BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseLogical(TokenSource& s, memManager::Memory& mem) {
        ASTNode* left = parseComparison(s, mem);
        while (peek(s).type == lexer::DOUBLE_AND || peek(s).type == lexer::DOUBLE_VBAR) {
            lexer::TokenType op = advance(s).type;
            ASTNode* right = parseComparison(s, mem);
            auto ptr = memManager::allocateMemory(mem,sizeof(BinaryOpNode));
            left = new(ptr) BinaryOpNode{NODE_BINARY_OP, op, left, right};
        }
        return left;
    }

    ASTNode* parseExpression(TokenSource & s, memManager::Memory& mem) {
        return parseLogical(s, mem);
    }

    ASTNode* parseIdentifier(TokenSource& s, memManager::Memory& mem) {
        if (peek(s).type != lexer::IDENT) throw std::runtime_error("Expected identifier, got: " + peek(s).content);
        auto ptr = memManager::allocateMemory(mem,sizeof(IdentNode));
        return new(ptr) IdentNode{NODE_IDENT,advance(s).content};
    }

    ASTNode* parseVarDecl(TokenSource& s, memManager::Memory& mem) {
        expect(s, lexer::VAR);
        std::string name = peek(s).content;
        expect(s, lexer::IDENT);
        if (peek(s).type == lexer::ARR_RIGHT) {
            advance(s);
            advance(s);
        }
        expect(s, lexer::EQ);
        ASTNode* expr = parseExpression(s, mem);

        auto ptr = memManager::allocateMemory(mem,sizeof(VarDeclNode));
        return new(ptr) VarDeclNode{NODE_VAR_DECL, name, expr};
    }

    ASTNode* parseProgram(std::vector<lexer::Token>& tokens, memManager::Memory& mem) {
        TokenSource s = {tokens, 0};
        std::vector<ASTNode*> program;
        while (s.pos < s.token.size()) {
            switch (s.token[s.pos].type) {
                case lexer::VAR: program.push_back(parseVarDecl(s, mem)); break;
                default: advance(s); break;
            }
        }
        auto ptr = memManager::allocateMemory(mem,sizeof(ProgramNode));
        return new(ptr) ProgramNode{NODE_PROGRAM, program};
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
