//
// Created by Jerry Chou on 2022/4/4.
//

#ifndef XLANG2_CONTINUESTATEMENTNODEGENERATOR_HPP
#define XLANG2_CONTINUESTATEMENTNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class ContinueStatementNodeGenerator : BaseGenerator {
public:
    explicit ContinueStatementNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_CONTINUESTATEMENTNODEGENERATOR_HPP
