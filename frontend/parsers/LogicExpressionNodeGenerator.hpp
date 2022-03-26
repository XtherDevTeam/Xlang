//
// Created by Jerry Chou on 2022/3/26.
//

#ifndef XLANG2_LOGICEXPRESSIONNODEGENERATOR_HPP
#define XLANG2_LOGICEXPRESSIONNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class LogicExpressionNodeGenerator : BaseGenerator {
public:
    explicit LogicExpressionNodeGenerator(Lexer& L);

    AST Parse() override;
};


#endif //XLANG2_LOGICEXPRESSIONNODEGENERATOR_HPP
