//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_EQUALEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_EQUALEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class EqualExpressionNodeGenerator : BaseGenerator {
public:
    explicit EqualExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_EQUALEXPRESSIONNODEGENERATOR_HPP
