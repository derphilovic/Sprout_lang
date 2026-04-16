#include "lexer.h"
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

namespace sprout::lexer {

    Token nextToken(Source& s) {
        skipWhiteSpace(s);

        char c = peek(s);
        if (c == '\0') return makeToken(END_OF_FILE, "");

        if (isalpha(c)) {
            std::string word;
            while (isalnum(peek(s))) word += advance(s); //Getting the full word

            if (word == "int") return makeToken(INT, word);
            if (word == "double") return makeToken(FLOAT, word);
            if (word == "boolean") return makeToken(BOOL, word);
            if (word == "char6") return makeToken(CHAR6, word);
            if (word == "var") return makeToken(VAR, word);
            if (word == "array") return makeToken(ARRAY, word);
            if (word == "print") return makeToken(PRINT, word);
            if (word == "if") return makeToken(IF, word);
            if (word == "else") return makeToken(ELSE, word);
            if (word == "while") return makeToken(WHILE, word);
            if (word == "func") return makeToken(FUNC, word);
            if (word == "return") return makeToken(RET, word);
            return makeToken(IDENT, word);;
        }

        if (isdigit(c)) {
            std::string num;
            while (isdigit(peek(s))) num += advance(s);
            if (peek(s) == '.' && (s.pos + 1) < s.src.size() && isdigit(s.src[s.pos + 1])) {
                num += advance(s);
                while (isdigit(peek(s))) num += advance(s);
            }
            return makeToken(NUMBER, num);
        }

        if (c == '-') {
            advance(s);
            if (peek(s) == '>') {advance(s); return makeToken(ARR_RIGHT, "->");}
            return makeToken(MINUS, "-");
        }

        if (c == '=') {
            advance(s);
            if (peek(s) == '=') {advance(s); return makeToken(EQEQ, "==");}
            return makeToken(EQ, "=");
        }

        if (c == ':') {
            advance(s);
            if (peek(s) == ':') {advance(s); return makeToken(DOUBLECOLON, "::");}
            return makeToken(COLON, ":");
        }

        if (c == '!') {
            advance(s);
            if (peek(s) == '=') {advance(s); return makeToken(NOT_EQ, "!=");}
            return makeToken(UNKNOWN, std::string(1, c));
        }

        if (c == '<') {
            advance(s);
            if (peek(s) == '=') {advance(s); return makeToken(LOWER_EQ, "<=");}
            return makeToken(LOWER, "<");
        }

        if (c == '>') {
            advance(s);
            if (peek(s) == '=') {advance(s); return makeToken(GREATER_EQ, ">=");}
            return makeToken(GREATER, ">");
        }

        if (c == '#') { //Handles all # compiler actions like Import
            advance(s);
            std::string word;
            TokenType type;
            std::string value;

            while (isalpha(peek(s))) word += advance(s);
            if (word == "import") type = IMPORT;

            skipWhiteSpace(s);
            while (isalpha(peek(s))) value += advance(s);

            return makeToken(type, value);
        }

        switch (advance(s)) {
            case '+': return makeToken(PLUS, "+");
            case '*': return makeToken(STAR, "*");
            case '/': return makeToken(SLASH, "/");
            case ',': return makeToken(COMMA, ",");
            case '(': return makeToken(L_PAREN, "(");
            case ')': return makeToken(R_PAREN, ")");
            case ';': return makeToken(SEMICOLON, ";");
            case '[': return makeToken(R_SQR_BRKT, "[");
            case ']': return makeToken(L_SQR_BRKT, "]");
            case '{': return makeToken(R_CURL_BRKT, "{");
            case '}': return makeToken(L_CURL_BRKT, "}");
        }

        return makeToken(UNKNOWN, std::string(1, c));
    }

    std::vector<Token> tokenize(Source& s) {
        std::vector<Token> tokens;
        while (true) {
            Token t = nextToken(s);
            if (t.type == END_OF_FILE) break;
            tokens.push_back(t);
        }
        return tokens;
    }
}

void run() {
        std::string code = "#import time\n array date = [time::date, time::time]\nvar a -> int = 21\n var b = a * 5\n print : b\0";
        sprout::lexer::Source s = {code};

        std::vector<sprout::lexer::Token> tokens = tokenize(s);
        for (auto t : tokens) {
            std::cout << "Token Type: " << t.type << " Token Value: " << t.content << "\n";
        }
}