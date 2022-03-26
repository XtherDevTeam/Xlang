//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_ASSIGNMENTEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_ASSIGNMENTEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"
#include "MemberExpressionNodeGenerator.hpp"
#include "LogicExpressionNodeGenerator.hpp"

class AssignmentExpressionNodeGenerator : BaseGenerator {
public:
    explicit AssignmentExpressionNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_ASSIGNMENTEXPRESSIONNODEGENERATOR_HPP
