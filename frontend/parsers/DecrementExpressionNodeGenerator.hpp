//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_DECREMENTEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_DECREMENTEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class DecrementExpressionNodeGenerator : BaseGenerator {
public:
    explicit DecrementExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_DECREMENTEXPRESSIONNODEGENERATOR_HPP
