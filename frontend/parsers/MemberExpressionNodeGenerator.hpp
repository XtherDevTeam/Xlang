//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_MEMBEREXPRESSIONNODEGENERATOR_HPP
#define XLANG2_MEMBEREXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

#include "FunctionCallingExpressionNodeGenerator.hpp"

class MemberExpressionNodeGenerator : BaseGenerator {
public:
    explicit MemberExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_MEMBEREXPRESSIONNODEGENERATOR_HPP
