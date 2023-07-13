
/* This tells flex to read only one input file */
%option noyywrap

ES (\\(['"\?\\abfnrtv]|[0-7]{3}))
WS [ \t\v\n\f\r]

%{

#include <stdio.h>
#include "y.tab.h"

/* prints grammar violation message */
extern void yyerror( const char * );

static void comment( void );

%}

%%

"/*" { comment( ); }

%%
