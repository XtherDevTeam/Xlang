# The Grammar document of Xlang 2

Parsers in Xlang 2 are kind of `LL(1)` Parsers.

We are still developing on AST generating.

### Primary

Status: `Implmented`

Grammar: `Primary ::= TokenInteger | TokenDecimal | TokenString`


### ExtendPrimary

Status: `Unimplmented`

Grammar: `ExtendPrimary ::= Primary | Identifier`

### Identifier

Status: `Implmented`

Grammar: `Identifier ::= TokenIdentifier`

### Arguments

Status: `Unimplmented`

Grammar: `Arguments ::= TokenLeftParentheses [ Expression { ", " Expression } ] TokenRightParentheses`

### IndexExpression

Status: `Unimplmented`

Grammar: 
```
IndexExpression ::= IndexExpression LeftBracket Expression RightBracket
                  | Identifier Leftbracket Expression RightBracket
                  | Identifier
```

### FunctionCallingExpression

Status: `Unimplmented`

Grammar:
```
FunctionCallingExpression ::= FunctionCallingExpression LeftBracket Expression RightBracket
                            | IndexExpression Leftbracket Expression RightBracket
                            | IndexExpression
```

### MemberExpression

Status: `Unimplmented`

Grammar: 
```
MemberExpression ::= FunctionCallingExpression TokenDot MemberExpression
                   | FunctionCallingExpression
```

### NegativeExpression

Status: `Unimplmented`

Grammar:
```
NegativeExpression ::= TokenMinus MemberExpression
                     | MemberExpression
```

### IncrementExpression

Status: `Unimplmented`

Grammar:
```
IncrementExpression ::= TokenIncrementSign NegativeExpression
                      | NegativeExpression
```

### DecrementExpression

Status: `Unimplmented`

Grammar:
```
DecrementExpression ::= TokenDecrementSign NegativeExpression
                      | NegativeExpression
```

### MultiplicationExpression

Status: `Unimplmented`

Grammar:
```
MultiplicationExpression ::= ( IncrementExpression | DecrementExpression ) ( TokenAsterisk | TokenSlash | TokenReminderSign ) MultiplicationExpression
                           | ( IncrementExpression | DecrementExpression )
```

### AdditionExpression

Status: `Unimplmented`

Grammar:
```
MultiplicationExpression ::= MultiplicationExpression ( TokenPlus | TokenMinus ) AdditionExpression
                           | MultiplicationExpression
```

### BinaryMoveExpression

Status: `Unimplmented`

Grammar:
```
BinaryMoveExpression ::= MultiplicationExpression ( TokenBinaryLeftMove | TokenBinaryRightMove ) BinaryMoveExpression
                       | MultiplicationExpression
```

### ComparingExpression

Status: `Unimplmented`

Grammar:
```
ComparingExpression ::= BinaryMoveExpression ( TokenBinaryLeftMove | TokenBinaryRightMove ) ComparingExpression
                      | BinaryMoveExpression
```

### EqualExpression

Status: `Unimplmented`

Grammar:
```
ComparingExpression ::= ComparingExpression ( TokenEqual | TokenNotEqual ) ComparingExpression
                      | ComparingExpression
```

### BinaryExpression

Status: `Unimplmented`

Grammar:
```
BinaryExpression ::= EqualExpression ( TokeBinaryXOR | TokenBinaryOr | TokenBinaryAnd ) BinaryExpression
                   | EqualExpression
```

### LogicExpression

Status: `Unimplmented`

Grammar:
```
LogicExpression ::= BinaryExpression ( TokenLogicAnd | TokenLogicOr ) LogicExpression
                  | BinaryExpression
```

### AssignmentExpression

Status: `Unimplmented`

Grammar:
```
AssignmentExpression ::= MemberExpression ( TokenAssignSign | TokenAdditionAssignment | TokenSubtractionAssignment | TokenMultiplationAssignment | TokenDivisionAssignment | RemainderAssignment ) LogicExpression
```

### Expression

Status: `Unimplmented`

Grammar:
```
Expression ::= LogicExpression
             | AssignmentExpression
```