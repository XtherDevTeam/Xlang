//
// Created by Jerry Chou on 2022/4/3.
//

#ifndef XLANG2_ACCESSDESCRIPTORNODEGENERATOR_HPP
#define XLANG2_ACCESSDESCRIPTORNODEGENERATOR_HPP

#include "BaseGenerator.hpp"

class AccessDescriptorNodeGenerator : BaseGenerator {
public:
    explicit AccessDescriptorNodeGenerator(Lexer &L);

    AST Parse() override;
};


#endif //XLANG2_ACCESSDESCRIPTORNODEGENERATOR_HPP
