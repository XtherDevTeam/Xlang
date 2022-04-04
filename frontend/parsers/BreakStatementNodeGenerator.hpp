//
// Created by Jerry Chou on 2022/4/4.
//

#ifndef XLANG2_BREAKSTATEMENTNODEGENERATOR_HPP
#define XLANG2_BREAKSTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class BreakStatementNodeGenerator : BaseGenerator {
public:
    explicit BreakStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_BREAKSTATEMENTNODEGENERATOR_HPP
