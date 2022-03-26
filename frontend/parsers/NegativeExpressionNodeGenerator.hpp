//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_NEGATIVEEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_NEGATIVEEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "PrimaryNodeGenerator.hpp"
#include "MemberExpressionNodeGenerator.hpp"

class NegativeExpressionNodeGenerator : BaseGenerator {
public:
    explicit NegativeExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_NEGATIVEEXPRESSIONNODEGENERATOR_HPP
