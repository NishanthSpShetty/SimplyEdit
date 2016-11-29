%{
	#include "CSyntax.h"
%}

%%
("void"|"int"|"float"|"char"|"short"|"const"|"double"|"long")			return DATA_TYPE;
("if"|"for"|"while"|"do")							return LOOP;
([0-9]+(\.[0-9]+)?)								return NUMBER;
("struct"|"enum"|"#include"|"#define"|"class"|"new"|"union"|"++"|"--") 		return LANG_CONSTRUCT;
("return")									return RETURN;
.*										return 0;

%%

int yywrap(){
	return 1;
	}
