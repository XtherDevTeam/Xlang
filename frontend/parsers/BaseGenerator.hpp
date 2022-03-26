//
// Created by Jerry Chou on 2022/3/25.
//

#ifndef XLANG2_BASEGENERATOR_HPP
#define XLANG2_BASEGENERATOR_HPP

#include "../../share/config.hpp"
#include "../AST.hpp"

#include "ParserException.hpp"

/**
 * @biref a base class of generators
 */
class BaseGenerator {
    /**
     * For rolling-back to initial state.
     */
    XInteger InitialLine, InitialColumn, InitialPosition;
    Lexer::Token InitialLastToken;
protected:
    Lexer& L;
public:
    explicit BaseGenerator(Lexer &L);

    /**
     * @biref Rolling-back to initial state.
     */
    void Rollback();

    /**
     * @biref Parse and return a AST node.
     * @return a AST node
     */
    virtual AST Parse();

    void MakeException(const XString &Reason) const;
};


#endif //XLANG2_BASEGENERATOR_HPP
