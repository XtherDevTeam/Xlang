//
// Created by Jerry Chou on 2022/4/2.
//

#ifndef XLANG2_BYTECODEGENERATOR_HPP
#define XLANG2_BYTECODEGENERATOR_HPP

#include "../share/config.hpp"

#include "BytecodeGenerateException.hpp"

#include "../frontend/AST.hpp"
#include "GlobalEnvironment.hpp"
#include "../share/HashLib.hpp"

class BytecodeGenerator {
public:
    GlobalEnvironment Environment;
    XIndexType EnvIndex;

    BytecodeGenerator();

    BytecodeCommandArray Generate(AST &Target);

    TypenameDerive GetTypeOfAST(AST &Target);

    /**
     * @biref Parse a member expression (Identifier, MemberExpression, etc.)
     * @warning When you're end with a assignment, it won't pop the last result from the emulating stack, you should pop it MANUALLY.
     * @param Target The AST to parse.
     * @param EndWithAssignment Method will generate a `store` command if this param is true.
     * @param ParseTo Default as -1.
     * @return Return a command array contains the parse result.
     */
    BytecodeCommandArray ParseMemberExpression(AST &Target, bool EndWithAssignment, XClassIndexType &ParseTo);

    BytecodeCommandArray CovertExpressionResultToStatementResult(AST &Target);
};


#endif //XLANG2_BYTECODEGENERATOR_HPP
