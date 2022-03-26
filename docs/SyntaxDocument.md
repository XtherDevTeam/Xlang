# The Syntax document of Xlang 2

Parsers in Xlang 2 are kind of `LL(1)` Parsers.

We are still developing on AST generating.

### Primary

Status: `Implmented`

Syntax: `Primary ::= TokenInteger | TokenDecimal | TokenString`

### Identifier

Status: `Implmented`

Syntax: `Identifier ::= TokenIdentifier`

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

Status: `Unimplmented`

Syntax:
```
MultiplicationExpression ::= MultiplicationExpression ( TokenPlus | TokenMinus ) AdditionExpression
                           | MultiplicationExpression
```

### BinaryMoveExpression

Status: `Unimplmented`

Syntax:
```
BinaryMoveExpression ::= AdditionExpression ( TokenBinaryLeftMove | TokenBinaryRightMove ) BinaryMoveExpression
                       | AdditionExpression
```

### ComparingExpression

Status: `Implmented`

Syntax:
```
ComparingExpression ::= BinaryMoveExpression ( TokenBinaryLeftMove | TokenBinaryRightMove ) ComparingExpression
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