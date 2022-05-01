# The Syntax document of Xlang 2

Parsers in Xlang 2 are kind of `LL(1)` Parsers.

We are still developing on AST generating.

### Primary

Status: `Implmented`

Syntax:

```
Primary ::= TokenInteger | TokenDecimal | TokenString
          | TokenLeftParentheses Expression TokenRightParentheses
```

### Identifier

Status: `Implmented`

Syntax: `Identifier ::= TokenIdentifier`

### TypeCastingExpression

Status: `Implmented`

Syntax: `TypeCastingExpression ::= TokenLeftParentheses Expression TokenRightParentheses TokenTypeCastingSign TypeSpecifier`

### Arguments

Status: `Implmented`

Syntax: `Arguments ::= TokenLeftParentheses [ Expression { ", " Expression } ] TokenRightParentheses`

### IndexExpression

Status: `Implmented`

Syntax:

```
IndexExpression ::= IndexExpression TokenLeftBracket Expression TokenRightBracket
                  | Identifier TokenLeftbracket Expression TokenRightBracket
                  | Identifier
```

### FunctionCallingExpression

Status: `Implmented`

Syntax:

```
FunctionCallingExpression ::= IndexExpression Arguments
```

### MemberExpression

Status: `Implmented`

Syntax:

```
MemberExpression ::= MemberExpression TokenDot FunctionCallingExpression
                   | FunctionCallingExpression [ TokenDot FunctionCallingExpression ]
```

### NegativeExpression

Status: `Implmented`

Syntax:

```
NegativeExpression ::= TokenMinus ( MemberExpression | Primary )
                     | ( MemberExpression | Primary )
```

### IncrementExpression

Status: `Implmented`

Syntax:

```
IncrementExpression ::= TokenIncrementSign NegativeExpression
                      | NegativeExpression
```

### DecrementExpression

Status: `Implmented`

Syntax:

```
DecrementExpression ::= TokenDecrementSign NegativeExpression
                      | NegativeExpression
```

### MultiplicationExpression

Status: `Implmented`

Syntax:

```
MultiplicationExpression ::= ( IncrementExpression | DecrementExpression ) ( TokenAsterisk | TokenSlash | TokenReminderSign ) MultiplicationExpression
                           | ( IncrementExpression | DecrementExpression )
```

### AdditionExpression

Status: `Implmented`

Syntax:

```
MultiplicationExpression ::= MultiplicationExpression ( TokenPlus | TokenMinus ) AdditionExpression
                           | MultiplicationExpression
```

### BinaryMoveExpression

Status: `Implmented`

Syntax:

```
BinaryMoveExpression ::= AdditionExpression ( TokenBinaryLeftMove | TokenBinaryRightMove ) BinaryMoveExpression
                       | AdditionExpression
```

### ComparingExpression

Status: `Implmented`

Syntax:

```
ComparingExpression ::= BinaryMoveExpression ( TokenLessThanOrEqual | TokenMoreThanOrEqual | TokenLessThan | TokenMoreThan ) ComparingExpression
                      | BinaryMoveExpression
```

### EqualExpression

Status: `Implmented`

Syntax:

```
ComparingExpression ::= ComparingExpression ( TokenEqual | TokenNotEqual ) ComparingExpression
                      | ComparingExpression
```

### BinaryExpression

Status: `Implmented`

Syntax:

```
BinaryExpression ::= EqualExpression ( TokeBinaryXOR | TokenBinaryOr | TokenBinaryAnd ) BinaryExpression
                   | EqualExpression
```

### LogicExpression

Status: `Implmented`

Syntax:

```
LogicExpression ::= BinaryExpression ( TokenLogicAnd | TokenLogicOr ) LogicExpression
                  | BinaryExpression
```

### AssignmentExpression

Status: `Implmented`

Syntax:

```
AssignmentExpression ::= MemberExpression ( TokenAssignSign | TokenAdditionAssignment | TokenSubtractionAssignment | TokenMultiplationAssignment | TokenDivisionAssignment | RemainderAssignment ) LogicExpression
```

### Expression

Status: `Implmented`

Syntax:

```
Expression ::= LogicExpression
             | AssignmentExpression
```

### TypeSpecifier

Status: `Implmented`

Syntax:

```
TypeSpecifier ::= Identifier { TokenLeftBracket TokenRightBracket }
```

### AccessDescriptor

Status: `Implmented`

Syntax:

```
AccessDescriptor ::= "public" | "private"
```

### VariableDescriptor

Status: `Implmented`

Syntax:

```
AccessDescriptor ::= "const" | "final"
```

### MethodDescriptor

Status: `Implmented`

Syntax:

```
MethodDescriptor ::= "override" | "virtual" | "final"
```

### VariableDefinition

Status: `Implmented`

Syntax:

```
VariableDeclaration ::= [AccessDescriptor] [VariableDescriptor] TypeSpecifier Identifier TokenEqual Expression
```

### VariableDeclaration

Status: `Implmented`

Syntax:

```
VariableDeclaration ::= [AccessDescriptor] [VariableDescriptor] TypeSpecifier Identifier
```

### CodeBlock

Status: `Unimplmented`

Syntax:

```
CodeBlock ::= TokenLeftBraces [ Statement { TokenSemiColon Statement } ] TokenRightBraces
```

### FunctionDefinition

Status: `Unimplmented`

Syntax:

```
FunctionDefinition ::= [AccessDescriptor] [FunctionDescriptor] TypeSpecifier Identifier Arguments CodeBlock
```

### ReturnStatement

Status: `Implmented`

Syntax:

```
ReturnStatement ::= "return" Expression
```

### ContinueStatement

Status: `Implmented`

Syntax:

```
ContinueStatement ::= "continue"
```

### BreakStatement

Status: `Implmented`

Syntax:

```
BreakStatement ::= "break"
```

### IfStatement

Status: `Implmented`

Syntax:

```
IfStatement ::= "if" TokenLeftParentheses Expression TokenRightParentheses CodeBlock
              | "if" TokenLeftParentheses Expression TokenRightParentheses CodeBlock { "else" Statement }
```

### ForStatement

Status: `Implmented`

Syntax:

```
ForStatement ::= "for" TokenLeftParentheses Statement TokenSemicolon Expression TokenSemicolon Statement TokenRightParentheses CodeBlock
```

### WhileStatement

Status: `Unimplmented`

Syntax:

```
WhilStatement ::= "while" TokenLeftParentheses Expression TokenRightParentheses CodeBlock
```

### Statement

Status: `Unimplmented`

Syntax:

```
Statement ::= VariableDefinition
            | VariableDeclaration
            | FunctionDefinition
            | ReturnStatement
            | ContinueStatement
            | BreakStatement
            | IfStatement
            | IfElseStatement
            | ForStatement
            | WhileStatement
            | CodeBlock
            | Expression
```