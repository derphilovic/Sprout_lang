#include "lexer.h"
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

namespace lexer {

    Token nextToken(Source& s) {
        skipWhiteSpace(s);

        char c = peek(s);
        if (c == '\0') return makeToken(END_OF_FILE, "");

        if (isalpha(c)) {
            std::string word;
            while (isalnum(peek(s))) word += advance(s); //Getting the full word

            if (word == "int") return makeToken(INT, word);
            if (word == "var") return makeToken(VAR, word);
            if (word == "print") return makeToken(PRINT, word);
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

        switch (advance(s)) {
            case '+': return makeToken(PLUS, "+");
            case '*': return makeToken(STAR, "*");
            case '/': return makeToken(SLASH, "/");
            case '=': return makeToken(EQ, "=");
            case ':': return makeToken(COLON, ":");
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
        std::string code = "var a -> int = 21\n var b = a * 5\n print : b\0";
        lexer::Source s = {code};

        std::vector<lexer::Token> tokens = lexer::tokenize(s);
        for (auto t : tokens) {
            std::cout << "Token Type: " << t.type << " Token Value: " << t.content << "\n";
        }
}