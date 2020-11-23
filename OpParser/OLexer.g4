lexer grammar OLexer;


BKNL            :   '\\' WS? '\n' -> skip;

SHARP           : '#';

OPENPAR         : '(';
CLOSEPAR        : ')';
OPENBRACK       : '[';
CLOSEBRACK      : ']';
ASSIGN          : '=';
COLON           : ':';
SEMICOLON       : ';';
COMMA           : ',';

NEGSUB          : '-';
COMPLEMENT      : '~';
EXPONENT        : '^';
MODULUS         : '%';
DIV             : '/';
MULT            : '*';
ADD             : '+';
LSHIFT          : '<<';
RSHIFT          : '>>';
RSHIFTU         : '>>>';
GREATER         : '>';
GREATEREQUAL    : '>=';
LESS            : '<';
LESSEQUEAL      : '<=';
EQUAL           : '==';
NEQUAL          : '!=';
BITAND          : '&';
BITOR           : '|';
LAND            : '&&';
LANDONE         : '&&&';
LOR             : '||';
LORONE          : '|||';
LNOT            : '!';
COND            : '?';

ABS             : 'abs';
ACOS            : 'acos';
ASIN            : 'asin';
ATAN            : 'atan';
CEIL            : 'ceil';
COS             : 'cos';
DOUBLECAST      : 'double';
EVAL            : 'eval';
EXPFUNC         : 'exp';
FLOATCAST       : 'float';
FLOOR           : 'floor';
GRAPH           : 'graph';
GRAPH2          : 'graph2';
IF              : 'if';
INTCAST         : 'int';
ISNULL          : 'isnull';
LOG             : 'log';
MAX             : 'max';
MEDIAN          : 'median';
MIN             : 'min';
MODE            : 'mode';
NMAX            : 'nmax';
NMEDIAN         : 'nmedian';
NMIN            : 'nmin';
NMODE           : 'nmode';
NOT             : 'not';
POW             : 'pow';
RAND            : 'rand';
ROUND           : 'round';
SIN             : 'sin';
SQRT            : 'sqrt';
TAN             : 'tan';
XOR             : 'xor';
NULLV           : 'null';
SQL             : 'sql';

FLOAT           :   INT '.' INT? EXP? FL?
                |   '.' INT EXP? FL?
                ;
fragment FL     :   [fF];
fragment EXP    :   [eE][-+]? DIGIT+;
DOUBLE          :   INT '.' INT? EXP?
                |   '.' INT EXP?
                ;
INT             :   DIGIT+
                |   HEX
                ;
fragment DIGIT  :   [0-9];
fragment HEX    :   [0][xX][0-9a-fA-F]+;
NAME            :   LETTER (LETTER | DIGIT)*;
fragment LETTER :   [a-zA-Z\u0080-\u00FF_];
STRING          :   '"' .*? '"'
                |   '\'' .*? '\''
                ;
                
WS              :   [ \t\r]+ -> skip;


