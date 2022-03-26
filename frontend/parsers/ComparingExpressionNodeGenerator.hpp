//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_COMPARINGEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_COMPARINGEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class ComparingExpressionNodeGenerator : BaseGenerator {
    explicit ComparingExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_COMPARINGEXPRESSIONNODEGENERATOR_HPP
