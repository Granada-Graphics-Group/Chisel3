parser grammar OParser;

options 
{
    tokenVocab = OLexer;
}

program         : defs;

defs            : def (';' def)* ';'?
//def ';'?
//                | def ';'
//                | def (';' def)+
                ;

def             : STRING '=' expr
                | NAME ('#' NAME)? ('[' INT ']')? '=' expr
                ;

expr            : '(' expr ')'                                  # parExp
                | function                                      # functionExp 
                | constant                                      # constantExp
                | unitaryOp expr                                # unitaryExp
                | expr '^' expr                                 # exponentialExp
                | expr op=('*' | '/' | '%') expr                # binaryOpExp
                | expr op=('+' | '-') expr                      # binaryOpExp
                | expr op=('<<' | '>>' | '>>>') expr            # shiftExp
                | expr op=('>' | '>=' | '<' | '<=') expr        # binLogOpExp
                | expr op=('==' | '!=') expr                    # binLogOpExp
                | expr '&' expr                                 # binaryOpExp
                | expr '|' expr                                 # binaryOpExp
                | expr op=('&&' | '&&&') expr                   # logAndExp
                | expr op=('||' | '|||') expr                   # logOrExp
                | expr '?' expr ':' expr                        # condExp
//                | NAME '=' expr
                ;

function        : 'abs' '(' expr ')'                            # abs
                | 'acos' '(' expr ')'                           # acos
                | 'asin' '(' expr ')'                           # asin
                | 'atan' '(' expr ')'                           # atan
                | 'atan' '(' expr ',' expr ')'                  # atan2
                | 'ceil' '(' expr ')'                           # ceil
                | 'cos' '(' expr ')'                            # cos
                | 'double' '(' expr ')'                         # castToDouble
                | 'eval' '(' expr_list expr ')'                 # eval
                | 'exp' '(' expr ')'                            # exponential
                | 'exp' '(' expr ',' expr ')'                   # exponential2
                | 'float' '(' expr ')'                          # castToFloat
                | 'floor' '(' expr ')'                          # floor
                | 'graph' '(' expr (',' expr)+ ')'              # graph 
                | 'graph2' '(' expr (',' expr)+ ')'             # graph2
                | 'if' '(' expr ')'                             # if
                | 'if' '(' expr ',' expr ')'                    # if2
                | 'if' '(' expr ',' expr ',' expr ')'           # if3
                | 'if' '(' expr ',' expr ',' expr ',' expr ')'  # if4
                | 'int' '(' expr ')'                            # castToInt
                | 'isnull' '(' expr ')'                         # isNull
                | 'log' '(' expr ')'                            # log
                | 'log' '(' expr ',' expr ')'                   # log2
                | 'max' '(' expr (',' expr)+ ')'                # max
                | 'median' '(' expr (',' expr)+ ')'             # median
                | 'min' '(' expr (',' expr)+ ')'                # min
                | 'mode' '(' expr (',' expr)+ ')'               # mathMode
                | 'nmax' '(' expr (',' expr)+ ')'               # nmax
                | 'nmedian' '(' expr (',' expr)+ ')'            # nmedian
                | 'nmin' '(' expr (',' expr)+ ')'               # nmin
                | 'nmode' '(' expr (',' expr)+ ')'              # nmode
                | 'not' '(' expr ')'                            # not
                | 'pow' '(' expr ',' expr ')'                   # exponential2
                | 'rand' '(' expr ',' expr ')'                  # rand
                | 'round' '(' expr ')'                          # round
                | 'round' '(' expr ',' expr ')'                 # round2
                | 'round' '(' expr ',' expr ',' expr ')'        # round3
                | 'sin' '(' expr ')'                            # sin
                | 'sqrt' '(' expr ')'                           # sqrt
                | 'tan' '(' expr ')'                            # tan
                | 'xor' '(' expr ',' expr ')'                    # xor
                | 'null' '(' ')'                                # null
                | 'sql' '(' NAME ',' NAME ')'                   # sql
                ;

expr_list       : expr (',' expr)*;

constant        : INT                                           # intConst
                | FLOAT                                         # floatConst
                | DOUBLE                                        # doubleConst
                | NAME                                          # identifier    
                | STRING                                        # stringConst
                | NAME '[' expr ',' expr ']'                    # neighborMod
                ;
                
unitaryOp       : '-'                                           # negOp
                | '~'                                           # compOp
                | '!'                                           # notOp
                ;
                
                
                
