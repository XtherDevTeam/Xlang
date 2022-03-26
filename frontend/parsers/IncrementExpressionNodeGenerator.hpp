//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_INCREMENTEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_INCREMENTEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class IncrementExpressionNodeGenerator : BaseGenerator {
public:
    explicit IncrementExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_INCREMENTEXPRESSIONNODEGENERATOR_HPP
