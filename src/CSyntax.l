%{
	#include "CSyntax.h"
%}

%%
("int"|"float"|"char"|"short"|"double"|"long")	return DATA_TYPE;
("for"|"while"|"do")				return LOOP;
[0-9]+						return NUMBER;
("struct"|"enum"|"class"|"new"|"union")		return LANG_CONSTRUCT;
.*	return 0;
%%

int yywrap(){
	return 1;
	}
