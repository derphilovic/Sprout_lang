#pragma once
#include <cstdint>
#include <string>

namespace lexer {
    struct Source {
        std::string src;
        uint64_t pos = 0;
    };

    enum TokenType {
        //Keywords
        VAR, ARRAY, INT, FLOAT, BOOL, CHAR6,
        PRINT,
        IMPORT,
        IF, ELSE, WHILE,
        FUNC, RET,

        //Identifiers/Literals
        NUMBER, IDENT,

        //Symbols
        PLUS, MINUS, STAR, SLASH,
        EQ, EQEQ, GREATER, LOWER, GREATER_EQ, LOWER_EQ, NOT_EQ,
        COLON, COMMA,
        DOUBLECOLON, // Is tok for ::
        SEMICOLON,
        L_PAREN, R_PAREN, //Are token for ( and )
        L_SQR_BRKT, R_SQR_BRKT, //Are token for [ and ]
        L_CURL_BRKT, R_CURL_BRKT, //Aree token for { and }
        ARR_RIGHT, // Is token for ->

        UNKNOWN,

        END_OF_FILE
    };

    struct Token {
        TokenType type;
        std::string content;
    };

    inline char peek(Source& s) {
        if (s.pos >= s.src.size()) return '\0';
        return s.src[s.pos];
    }

    inline char advance(Source& s) {
        char c = peek(s);
        s.pos++;
        return c;
    }

    inline void skipWhiteSpace(Source& s) {
        while (true) {
            while (isspace(peek(s))) advance(s);
            // handle comments
            if (peek(s) == '/' && (s.pos + 1) < s.src.size() && s.src[s.pos + 1] == '/') {
                while (peek(s) != '\n' && peek(s) != '\0') advance(s);
                continue;
            }
            break;
        }
    }

    inline Token makeToken(TokenType type, const std::string& text) {
        return Token{type, text};
    }


}
void run();