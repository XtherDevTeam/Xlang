//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_INDEXEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_INDEXEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "ExpressionNodeGenerator.hpp"

class IndexExpressionNodeGenerator : BaseGenerator {
public:
    explicit IndexExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_INDEXEXPRESSIONNODEGENERATOR_HPP
