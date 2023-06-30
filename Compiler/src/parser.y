%{
#include "../include/pcheaders.h"
#include "../include/AST.h"

void yyerror (char const *s);
int yylex();
int get_line_number();

AST *ast;

std::vector<std::pair<std::string, int>> includeStack;

std::unordered_set<std::string> parserTypeIdentifiers;
std::vector<std::unordered_set<std::string>> parserTypeIdentifierStack;

//Constants
std::vector<std::unordered_map<std::string, AST*>> constantsStack = {{}};

void CSClear(){
  constantsStack[0].clear();
}

void CSAdd(const std::string& c, AST *exp){
  if(constantsStack[constantsStack.size() - 1].find(c) == constantsStack[constantsStack.size() - 1].end())
    constantsStack[constantsStack.size() - 1].emplace(c, exp);
}

AST* CSFind(const std::string& key){
  int i = (int) constantsStack.size() - 1;
  while(i >= 0){
    auto it = constantsStack[i].find(key);
    if(it != constantsStack[i].end())
      return it->second;
    i--;
  }
  return NULL;
}

void CSSubstitute(AST **ast){
  if((*ast)->getChildren().size() == 0){
    AST *sub = CSFind((*ast)->getValue());
    if(sub != NULL){
      delete (*ast);
      *ast = sub->Clone();
    }
  }
}

void CSPush(){
  constantsStack.push_back({});
}

void CSPop(){
  constantsStack.pop_back();
}

%}

%define parse.error verbose

//%language "c++"

%token TYPEDEF
%token IDENTIFIER
%token TYPE_IDENTIFIER
%token STRING_LITERAL
%token INTEGER
%token HEADER
%token HEADER_UNION
%token BOOL
%token INT
%token ERROR
%token STRING
%token BIT
%token VARBIT
%token CONST
%token CONTROL
%token APPLY
%token IN
%token OUT
%token INOUT
%token IF
%token ELSE
%token MATCH_KIND
%token KEY
%token ACTION
%token ACTIONS
%token STATE
%token ENTRIES
%token TYPE
%token DONTCARE
%token VOID
%token PRAGMA
%token TRUE
%token FALSE
%token THIS
%token PREFIX
%token RETURN
%token EXIT
%token TABLE
%token DEFAULT
%token MASK
%token RANGE
%token COMMA
%token SWITCH
%token SELECT
%token STRUCT
%token VALUESET
%token PARSER
%token PACKAGE
%token ENUM
%token TRANSITION
%token TUPLE
%token EXTERN
%token ABSTRACT

%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TOKEN_ERROR
%start p4program

%left ','
%nonassoc '?'
%nonassoc ':'
%left TK_OC_OR
%left TK_OC_AND
%left TK_OC_EQ TK_OC_NE
%left '<' '>' TK_OC_LE TK_OC_GE
%left '|'
%left '^'
%left '&'
/*
%left '++' '+' '-' '|+|' '|-|'
*/
%left '+' '-'
%left '*' '/' MODULUS
%right PREFIX
%nonassoc ']' '(' '['
%left '.'

%code requires {
  #include "../include/AST.h"
}

%union {
  LexVal* lexval;
  AST* ast;
}

%type<ast> p4program declaration optAnnotations annotation annotations annotationBody controlDeclaration controlBody controlLocalDeclarations controlLocalDeclaration constantDeclaration externDeclaration tableDeclaration
actionDeclaration parserDeclaration typeDeclaration instantiation errorDeclaration matchKindDeclaration functionDeclaration blockStatement tableProperty tablePropertyList
statOrDeclList optConstructorParameters statementOrDeclaration statement conditionalStatement exitStatement returnStatement switchStatement expression initializer optInitializer variableDeclaration assignmentOrMethodCallStatement
lvalue controlTypeDeclaration baseType typedefDeclaration typeRef headerTypeDeclaration headerUnionDeclaration derivedTypeDeclaration enumDeclaration structTypeDeclaration
structField structFieldList parserTypeDeclaration packageTypeDeclaration nonEmptyParameterList parameter parameterList direction argument argumentList nonEmptyArgList
entry entriesList actionList actionRef keyElementList keyElement identifierList parserLocalElements parserLocalElement parserBlockStatement parserState parserStates parserStatement parserStatements transitionStatement
selectExpression selectCase selectCaseList keysetExpression reducedSimpleKeysetExpression tupleKeysetExpression simpleExpressionList simpleKeysetExpression directApplication stateExpression valueSetDeclaration
tupleType emptyStatement typeArg typeArgumentList realTypeArg realTypeArgumentList expressionList typeOrVoid namedType specializedType functionPrototype methodPrototype methodPrototypes typeParameterList typeParameters optTypeParameters specifiedIdentifier specifiedIdentifierList
switchCase switchCases switchLabel nonBraceExpression

%type<lexval> nonTypeName prefixedNonTypeName name annotationToken member dotPrefix typeName prefixedType nonTableKwName
IDENTIFIER APPLY KEY ACTIONS STATE TYPE_IDENTIFIER THIS ENTRIES TYPE INTEGER STRING_LITERAL FALSE TRUE TK_OC_AND TK_OC_OR TK_OC_LE TK_OC_GE TK_OC_EQ TK_OC_NE IF ELSE EXIT RETURN SWITCH
CONTROL BIT BOOL ERROR STRING INT TYPEDEF HEADER STRUCT IN OUT INOUT CONST ACTION TABLE DONTCARE EXTERN PARSER PACKAGE VALUESET TRANSITION TUPLE VOID SELECT DEFAULT
MASK RANGE ABSTRACT MATCH_KIND ENUM HEADER_UNION PRAGMA VARBIT
MODULUS '?' '~' '^' '!' '&' '|' '_' '+' '-' '=' '*' '/' '(' ')' '{' '}' ';' '.' '>' '<' ',' '[' ']' ':' '@'

//%type<lexval> '%'

%%

p4program
: /* empty */ {$$ = NULL;}
| p4program declaration {if($1 == NULL) {$$ = new AST(NULL, NT_PROGRAM); $$->addChild($2);} else {$$ = $1; $$->addChild($2);} ast = $$;}
| p4program ';' {delete $2; $$ = $1;}
/* empty declaration */
;

declaration
: constantDeclaration {$$ = $1;}
| externDeclaration {$$ = $1;}
| actionDeclaration {$$ = $1;}
| parserDeclaration {$$ = $1;}
| typeDeclaration {$$ = $1;}
| controlDeclaration {$$ = $1;}
| instantiation {$$ = $1;}
| errorDeclaration {$$ = $1;}
| matchKindDeclaration {$$ = $1;}
| functionDeclaration {$$ = $1;}
;

externDeclaration
: optAnnotations EXTERN nonTypeName optTypeParameters {std::unordered_set<std::string> newTypeIds; parserTypeIdentifiers.emplace(*$3); if($4 != NULL) {
    for(AST* child : $4->getChildren()){
      parserTypeIdentifiers.emplace(child->getValue());
    } 
  } parserTypeIdentifierStack.push_back(newTypeIds);
} '{' methodPrototypes '}' {parserTypeIdentifierStack.pop_back(); delete $2; delete $6; delete $8; $$ = new AST($3, NT_EXTERN_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($7);}
| optAnnotations EXTERN functionPrototype ';' {delete $2; delete $4; $$ = new AST(NULL, NT_EXTERN_DECLARATION); $$->addChild($1); $$->addChild($3);}
;

methodPrototypes
: /* empty */ {$$ = NULL;}
| methodPrototypes methodPrototype {if($1 == NULL) {$$ = new AST(NULL, NT_METHOD_PROTOTYPE_LIST);} else {$$ = $1;} $$->addChild($2);}
;

methodPrototype
: optAnnotations functionPrototype ';' {delete $3; $$ = new AST(NULL, NT_METHOD_PROTOTYPE); $$->addChild($1); $$->addChild($2);}
| optAnnotations TYPE_IDENTIFIER '(' parameterList ')' ';' {delete $3; delete $5; delete $6; $$ = new AST(NULL, NT_METHOD_PROTOTYPE); AST *c = new AST($2, NT_CONSTRUCTOR_METHOD_PROTOTYPE); c->addChild($4); $$->addChild($1); $$->addChild(c);} //constructor
| optAnnotations ABSTRACT functionPrototype ';' {delete $2; delete $4; $$ = new AST(NULL, NT_METHOD_PROTOTYPE); $$->addChild($1); $$->addChild($3); $3->setNodeType(NT_ABSTRACT_FUNCTION_PROTOTYPE);}
;

typedefDeclaration
: TYPEDEF typeRef name ';' {delete $1; delete $4; $$ = new AST($3, NT_TYPEDEF_DECLARATION); $$->addChild($2);}
| TYPEDEF derivedTypeDeclaration name ';' {delete $1; delete $4; $$ = new AST($3, NT_TYPEDEF_DECLARATION); $$->addChild($2);}
/*
: optAnnotations TYPEDEF typeRef name ';'
| optAnnotations TYPEDEF derivedTypeDeclaration name ';'
*/
;

typeDeclaration
: derivedTypeDeclaration {$$ = $1; parserTypeIdentifiers.emplace($1->getValue());}
| typedefDeclaration {$$ = $1; parserTypeIdentifiers.emplace($1->getValue());}
| parserTypeDeclaration ';' {delete $2; $$ = $1;}
| controlTypeDeclaration ';' {delete $2; $$ = $1;}
| packageTypeDeclaration ';' {delete $2; $$ = $1;}
;

parserTypeDeclaration
: optAnnotations PARSER name optTypeParameters {std::unordered_set<std::string> newTypeIds; if($4 != NULL) {
  for(AST* child : $4->getChildren()){
    newTypeIds.emplace(child->getValue());
  }
} parserTypeIdentifierStack.push_back(newTypeIds);} '(' parameterList ')' {delete $2; delete $6; delete $8; $$ = new AST($3, NT_PARSER_TYPE_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($7); parserTypeIdentifiers.emplace($$->getValue());}
;

parserDeclaration
: parserTypeDeclaration optConstructorParameters '{' {CSPush();} parserLocalElements parserStates '}' {CSPop(); delete $3; delete $7; $$ = new AST(NULL, NT_PARSER_DECLARATION); $$->addChild($1); $$->addChild($5); $$->addChild($6);}
;

packageTypeDeclaration
: optAnnotations PACKAGE name optTypeParameters '(' parameterList ')' {delete $2; delete $5; delete $7; $$ = new AST($3, NT_PACKAGE_TYPE_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($6); parserTypeIdentifiers.emplace($$->getValue());}
;

derivedTypeDeclaration
: headerTypeDeclaration {$$ = $1;}
| headerUnionDeclaration {$$ = $1;}
| structTypeDeclaration {$$ = $1;}
| enumDeclaration {$$ = $1;}
;

enumDeclaration
: optAnnotations ENUM name '{' identifierList '}' {delete $2; delete $4; delete $6; $$ = new AST($3, NT_ENUM_DECLARATION); $$->addChild($1); $$->addChild(NULL); $$->addChild($5);}
| optAnnotations ENUM typeRef name '{' specifiedIdentifierList '}' {delete $2; delete $5; delete $7; $$ = new AST($4, NT_ENUM_DECLARATION); $$->addChild($1); $$->addChild($3); $$->addChild($6);}
;

specifiedIdentifierList
: specifiedIdentifier {$$ = new AST(NULL, NT_SPECIFIED_IDENTIFIER_LIST); $$->addChild($1);}
| specifiedIdentifierList ',' specifiedIdentifier {delete $2; $$ = $1; $$->addChild($3);}
;

specifiedIdentifier
: name '=' initializer {delete $2; $$ = new AST($1, NT_SPECIFIED_IDENTIFIER); $$->addChild($3);}
;

parserLocalElements
: /* empty */ {$$ = NULL;}
| parserLocalElements parserLocalElement {if($1 == NULL) {$$ = new AST(NULL, NT_PARSER_LOCAL_ELEMENT_LIST);} else {$$ = $1;} $$->addChild($2);}
;

parserStates
: parserState {$$ = new AST(NULL, NT_PARSER_STATE_LIST); $$->addChild($1);}
| parserStates parserState {$$ = $1; $$->addChild($2);}
;

parserLocalElement
: constantDeclaration {$$ = $1;}
| variableDeclaration {$$ = $1;}
| valueSetDeclaration {$$ = $1;}
| instantiation {$$ = $1;}
;

parserState
: STATE name '{' {CSPush();} parserStatements transitionStatement '}' {CSPop(); delete $1; delete $3; delete $7; $$ = new AST($2, NT_PARSER_STATE); $$->addChild($5); $$->addChild($6);}
/* : optAnnotations STATE name '{' parserStatements transitionStatement '}' */
;

parserStatements
: /* empty */ {$$ = NULL;}
| parserStatements parserStatement  {if($1 == NULL) {$$ = new AST(NULL, NT_PARSER_STATEMENT_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
;

parserStatement
: assignmentOrMethodCallStatement {$$ = $1;}
| directApplication {$$ = $1;}
| parserBlockStatement {$$ = $1;}
| constantDeclaration {$$ = $1;}
| variableDeclaration {$$ = $1;}
| emptyStatement {$$ = $1;}
| conditionalStatement {$$ = $1;}
;

parserBlockStatement
: '{' {CSPush();} parserStatements '}' {CSPop(); delete $1; delete $4; $$ = $3;}
/* : optAnnotations '{' parserStatements '}' */
;

directApplication
: typeName '.' APPLY '(' argumentList ')' ';' {delete $2; delete $3; delete $4; delete $6; delete $7; $$ = new AST($1, NT_DIRECT_APPLICATION); $$->addChild($5);}
;

transitionStatement
: /* empty */ {$$ = new AST("reject", NT_NAME_STATE_EXPRESSION);} //Explicit reject trasition
| TRANSITION stateExpression {delete $1; $$ = $2;}
;

stateExpression
: name ';' {delete $2; $$ = new AST($1, NT_NAME_STATE_EXPRESSION);}
| selectExpression {$$ = $1;}
;

valueSetDeclaration
: VALUESET '<' baseType '>' '(' expression ')' name ';' {delete $1; delete $2; delete $4; delete $5; delete $7; delete $9; $$ = new AST($8, NT_VALUE_SET_DECLARATION); $$->addChild($3); $$->addChild($6);}
| VALUESET '<' tupleType '>' '(' expression ')' name ';' {delete $1; delete $2; delete $4; delete $5; delete $7; delete $9; $$ = new AST($8, NT_VALUE_SET_DECLARATION); $$->addChild($3); $$->addChild($6);}
| VALUESET '<' typeName '>' '(' expression ')' name ';' {delete $1; delete $2; delete $4; delete $5; delete $7; delete $9; $$ = new AST($8, NT_VALUE_SET_DECLARATION); $$->addChild(new AST($3, NT_TYPE_NAME)); $$->addChild($6);}
/*
: optAnnotations VALUESET '<' baseType '>' '(' expression ')' name ';'
| optAnnotations VALUESET '<' tupleType '>' '(' expression ')' name ';'
| optAnnotations VALUESET '<' typeName '>' '(' expression ')' name ';'
*/
;

tupleType
: TUPLE '<' typeArgumentList '>' {delete $2; delete $4; $$ = new AST($1, NT_TUPLE_TYPE); $$->addChild($3);}
;

errorDeclaration
: ERROR '{' identifierList '}' {delete $2; delete $4; $$ = new AST($1, NT_ERROR_DECLARATION); $$->addChild($3);}
;

matchKindDeclaration
: MATCH_KIND '{' identifierList '}' {delete $2; delete $4; $$ = new AST($1, NT_MATCH_KIND_DECLARATION); $$->addChild($3);}
;

identifierList
: name {$$ = new AST(NULL, NT_IDENTIFIER_LIST); $$->addChild(new AST($1, NT_NONE));}
| identifierList ',' name {delete $2; $$ = $1; $$->addChild(new AST($3, NT_NONE));}
;

headerTypeDeclaration
: optAnnotations HEADER name optTypeParameters '{' structFieldList '}' {delete $2; delete $5; delete $7; $$ = new AST($3, NT_HEADER_TYPE_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($6);}
;

headerUnionDeclaration
: optAnnotations HEADER_UNION name optTypeParameters '{' structFieldList '}' {delete $2; delete $5; delete $7; $$ = new AST($3, NT_HEADER_UNION_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($6);}
;

structTypeDeclaration
: optAnnotations STRUCT name optTypeParameters '{' structFieldList '}' {delete $2; delete $5; delete $7; $$ = new AST($3, NT_STRUCT_TYPE_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($6);}
;

structFieldList
: /* empty */ {$$ = NULL;}
| structFieldList structField {if($1 == NULL) {$$ = new AST(NULL, NT_STRUCT_FIELD_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
;

structField
: optAnnotations typeRef name ';' {delete $4; $$ = new AST($3, NT_STRUCT_FIELD); $$->addChild($1); $$->addChild($2);}
;

constantDeclaration
: optAnnotations CONST typeRef name '=' initializer ';' {delete $2; delete $5; delete $7; $$ = new AST($4, NT_CONSTANT_DECLARATION); $$->addChild($1); $$->addChild($3); $$->addChild($6); CSAdd($$->getValue(), $6);}
;

controlDeclaration
: controlTypeDeclaration optConstructorParameters
/* no type parameters allowed in controlTypeDeclaration */
'{' {CSPush();} controlLocalDeclarations APPLY controlBody '}' {CSPop(); delete $3; delete $6; delete $8; $$ = new AST(NULL, NT_CONTROL_DECLARATION); $$->addChild($1); $$->addChild($2); $$->addChild($5); $$->addChild($7);}
;

controlTypeDeclaration
: optAnnotations CONTROL name optTypeParameters '(' parameterList ')' {delete $2; delete $5; delete $7; $$ = new AST($3, NT_CONTROL_TYPE_DECLARATION); $$->addChild($1); $$->addChild($4); $$->addChild($6); parserTypeIdentifiers.emplace($$->getValue());}
;

optConstructorParameters
: /* empty */ {$$ = NULL;}
| '(' parameterList ')' {delete $1; delete $3; $$ = $2;}
;

initializer
: expression {$$ = $1;}
;

expression
: INTEGER {$$ = new AST($1, NT_EXPRESSION);}
| TRUE {$$ = new AST($1, NT_EXPRESSION);}
| FALSE {$$ = new AST($1, NT_EXPRESSION);}
| THIS {$$ = new AST($1, NT_EXPRESSION);}
| STRING_LITERAL {$$ = new AST($1, NT_EXPRESSION);}
| nonTypeName {$$ = new AST($1, NT_EXPRESSION); CSSubstitute(&($$));}
| dotPrefix nonTypeName
| expression '[' expression ']'
| expression '[' expression ':' expression ']'
| '{' expressionList '}' {delete $1; delete $3; $$ = $2;}
| '{' kvList '}'
| '(' expression ')' {delete $1; delete $3; $$ = $2;}
| '!' expression %prec PREFIX
| '~' expression %prec PREFIX
| '-' expression %prec PREFIX
| '+' expression %prec PREFIX
| typeName '.' member {$$ = new AST($2, NT_EXPRESSION); $$->addChild(new AST($1, NT_NONE)); $$->addChild(new AST($3, NT_NONE));}
| ERROR '.' member
| expression '.' member {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild(new AST($3, NT_NONE));}
| expression '*' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '/' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression MODULUS expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '+' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '-' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
/*
| expression '|+|' expression
| expression '|-|' expression
*/
| expression TK_OC_LE expression
| expression TK_OC_GE expression
| expression '<' '<' expression
| expression '>' '>' expression {delete $2; delete $3; $$ = new AST("SR", NT_EXPRESSION); $$->addChild($1); $$->addChild($4);}
| expression '<' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '>' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression TK_OC_NE expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression TK_OC_EQ expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '&' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '^' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '|' expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
/*
| expression '++' expression
*/
| expression TK_OC_AND expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression TK_OC_OR expression {$$ = new AST($2, NT_EXPRESSION); $$->addChild($1); $$->addChild($3);}
| expression '?' expression ':' expression
| expression '<' realTypeArgumentList '>' '(' argumentList ')' {delete $2; delete $4; delete $5; delete $7; $$ = $1; $$->addChild($3); $$->addChild($6);}
| expression '(' argumentList ')' {delete $2; delete $4; $$ = $1; $$->addChild($3);}
| namedType '(' argumentList ')' {delete $2; delete $4; $$ = $1; $$->setNodeType(NT_EXPRESSION); $$->addChild($3);}
| '(' typeRef ')' expression
;

member
: name {$$ = $1;}
;

parameterList
: /* empty */ {$$ = NULL;}
| nonEmptyParameterList {$$ = $1;}
;

nonEmptyParameterList
: parameter {$$ = new AST(NULL, NT_PARAMETER_LIST); $$->addChild($1);}
| nonEmptyParameterList ',' parameter {delete $2; $$ = $1; $$->addChild($3);}
;

parameter
: optAnnotations direction typeRef name {$$ = new AST($4, NT_PARAMETER); $$->addChild($1); $$->addChild($2); $$->addChild($3);}
| optAnnotations direction typeRef name '=' expression {delete $5; $$ = new AST($4, NT_PARAMETER); $$->addChild($1); $$->addChild($2); $$->addChild($3); $$->addChild($6);}
;

direction
: IN {delete $1; $$ = new AST("in", NT_DIRECTION);}
| OUT {delete $1; $$ = new AST("out", NT_DIRECTION);}
| INOUT {delete $1; $$ = new AST("inout", NT_DIRECTION);}
| /* empty */ {$$ = NULL;}
;

controlLocalDeclarations
: /* empty */ {$$ = NULL;}
| controlLocalDeclarations controlLocalDeclaration {if($1 == NULL) {$$ = new AST(NULL, NT_CONTROL_LOCAL_DEC_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
;

controlLocalDeclaration
: constantDeclaration {$$ = $1;}
| variableDeclaration {$$ = $1;}
| actionDeclaration {$$ = $1;}
| tableDeclaration {$$ = $1;}
| instantiation {$$ = $1;}
;

actionDeclaration
: optAnnotations ACTION name '(' parameterList ')' blockStatement {delete $2; delete $4; delete $6; $$ = new AST($3, NT_ACTION_DECLARATION); $$->addChild($1); $$->addChild($5); $$->addChild($7);}
;

tableDeclaration
: optAnnotations TABLE name '{' tablePropertyList '}' {delete $2; delete $4; delete $6; $$ = new AST($3, NT_TABLE_DECLARATION); $$->addChild($1); $$->addChild($5);}
;

tablePropertyList
: tableProperty {$$ = new AST(NULL, NT_TABLE_PROPERTY_LIST); $$->addChild($1);}
| tablePropertyList tableProperty {$$ = $1; $$->addChild($2);}
;

tableProperty
: KEY '=' '{' keyElementList '}' {delete $2; delete $3; delete $5; $$ = new AST($1, NT_TABLE_PROPERTY); $$->addChild($4);}
| ACTIONS '=' '{' actionList '}' {delete $2; delete $3; delete $5; $$ = new AST($1, NT_TABLE_PROPERTY); $$->addChild($4);}
| CONST ENTRIES '=' '{' entriesList '}' {delete $1; delete $2; delete $3; delete $4; delete $6; $$ = new AST("const entries", NT_TABLE_PROPERTY); $$->addChild($5);}
| CONST nonTableKwName '=' initializer ';' {delete $1; delete $3; delete $5; *($2) = "const " + *($2); $$ = new AST($2, NT_TABLE_PROPERTY); $$->addChild($4);}
| nonTableKwName '=' initializer ';' {delete $2; delete $4; $$ = new AST($1, NT_TABLE_PROPERTY); $$->addChild($3);}
/*
| optAnnotations CONST ENTRIES '=' '{' entriesList '}'
| optAnnotations CONST nonTableKwName '=' initializer ';'
| optAnnotations nonTableKwName '=' initializer ';'
*/
;

actionList
: /* empty */ {$$ = NULL;}
| actionList actionRef ';' {delete $3; if($1 == NULL) {$$ = new AST(NULL, NT_ACTION_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
/* | actionList optAnnotations actionRef ';' */
;

actionRef
: prefixedNonTypeName {$$ = new AST($1, NT_ACTION_REF); $$->addChild(NULL);}
| prefixedNonTypeName '(' argumentList ')' {delete $2; delete $4; $$ = new AST($1, NT_ACTION_REF); $$->addChild($3);}
;

entriesList
: entry {$$ = new AST(NULL, NT_ENTRIES_LIST); $$->addChild($1);}
| entriesList entry {$$ = $1; $$->addChild($2);}
;

entry
: keysetExpression ':' actionRef optAnnotations ';' {delete $2; delete $5; $$ = new AST(NULL, NT_ENTRY); $$->addChild($1); $$->addChild($3); $$->addChild($4);}
;

nonTableKwName
: IDENTIFIER {$$ = $1;}
| TYPE_IDENTIFIER {$$ = $1;}
| APPLY {$$ = $1;}
| STATE {$$ = $1;}
| TYPE {$$ = $1;}
;

keyElementList
: /* empty */ {$$ = NULL;}
| keyElementList keyElement {if($1 == NULL) {$$ = new AST(NULL, NT_KEY_ELEMENT_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
;

keyElement
: expression ':' name ';' {delete $2; delete $4; $$ = new AST($3, NT_KEY_ELEMENT); $$->addChild($1);}
/* : expression ':' name optAnnotations ';' */
;

controlBody
: blockStatement {$$ = $1;}
;

instantiation
: typeRef '(' argumentList ')' name ';' {delete $2; delete $4; delete $6; $$ = new AST($5, NT_INSTANTIATION); $$->addChild(NULL); $$->addChild($1); $$->addChild($3);}
| annotations typeRef '(' argumentList ')' name ';' {delete $3; delete $5; delete $7; $$ = new AST($6, NT_INSTANTIATION); $$->addChild($1); $$->addChild($2); $$->addChild($4);}
| annotations typeRef '(' argumentList ')' name '=' objInitializer ';'
| typeRef '(' argumentList ')' name '=' objInitializer ';'
;

objInitializer
: '{' objDeclarations '}'
;

objDeclarations
: /* empty */
| objDeclarations objDeclaration
;

objDeclaration
: functionDeclaration
| instantiation
;

typeRef
: baseType {$$ = $1;}
| typeName {$$ = new AST($1, NT_TYPE_NAME);}
| specializedType {$$ = $1;}
| headerStackType
| tupleType {$$ = $1;}
;

headerStackType
: typeName '[' expression ']'
| specializedType '[' expression ']'
;

dotPrefix
: '.' {$$ = $1;}
;

namedType
: typeName {$$ = new AST($1, NT_TYPE_NAME);}
| specializedType {$$ = $1;}
;

prefixedType
: TYPE_IDENTIFIER {$$ = $1;}
| dotPrefix TYPE_IDENTIFIER
;

typeName 
: prefixedType {$$ = $1;}
;

specializedType
: prefixedType '<' typeArgumentList '>' {delete $2; delete $4; $$ = new AST($1, NT_TYPE_NAME); $$->addChild($3);}
;

typeArgumentList
: /* empty */ {$$ = NULL;}
| typeArg {$$ = new AST(NULL, NT_TYPE_ARGUMENT_LIST); $$->addChild($1);}
| typeArgumentList ',' typeArg {delete $2; if($1 == NULL) {$$ = new AST(NULL, NT_TYPE_ARGUMENT_LIST);} else {$$ = $1;} $$->addChild($3);}
;

typeArg
: DONTCARE {$$ = new AST($1, NT_TYPEARG_DONTCARE);}
| typeRef {$$ = $1;}
| nonTypeName {$$ = new AST($1, NT_TYPEARG_NONTYPENAME);}
| VOID {$$ = new AST($1, NT_TYPEARG_VOID);}
;

baseType
: BOOL {$$ = new AST($1, NT_BASE_TYPE);}
| ERROR {$$ = new AST($1, NT_BASE_TYPE);}
| STRING {$$ = new AST($1, NT_BASE_TYPE);}
| INT {$$ = new AST($1, NT_BASE_TYPE);}
| BIT {$$ = new AST($1, NT_BASE_TYPE);}
| BIT '<' INTEGER '>' {delete $2; delete $4; AST *c = new AST($3, NT_LITERAL); $$ = new AST($1, NT_BASE_TYPE); $$->addChild(c);}
| INT '<' INTEGER '>' {delete $2; delete $4; AST *c = new AST($3, NT_LITERAL); $$ = new AST($1, NT_BASE_TYPE); $$->addChild(c);}
| VARBIT '<' INTEGER '>'
| BIT '<' '(' expression ')' '>'
| INT '<' '(' expression ')' '>'
| VARBIT '<' '(' expression ')' '>'
;

nonTypeName 
: IDENTIFIER {$$ = $1;}
| APPLY {$$ = $1;}
| KEY {$$ = $1;}
| ACTIONS {$$ = $1;}
| STATE {$$ = $1;}
| ENTRIES {$$ = $1;}
| TYPE {$$ = $1;}
;

name 
: nonTypeName {$$ = $1;}
| TYPE_IDENTIFIER {$$ = $1;}
;

optTypeParameters
: /* empty */ {$$ = NULL;}
| typeParameters {$$ = $1;}
;

typeParameters
: '<' typeParameterList '>' {delete $1; delete $3; $$ = $2;}
;

typeParameterList
: name {$$ = new AST(NULL, NT_TYPE_PARAMETER_LIST); $$->addChild(new AST($1, NT_TYPE_PARAMETER));}
| typeParameterList ',' name {delete $2; $$ = $1; $$->addChild(new AST($3, NT_TYPE_PARAMETER));}
;

realTypeArg
: DONTCARE {$$ = new AST($1, NT_TYPEARG_DONTCARE);}
| typeRef {$$ = $1;}
| VOID {$$ = new AST($1, NT_TYPEARG_VOID);}
;

functionDeclaration
: functionPrototype blockStatement {$$ = new AST(NULL, NT_FUNCTION_DECLARATION); $$->addChild($1); $$->addChild($2);}
;

functionPrototype
: typeOrVoid name optTypeParameters {std::unordered_set<std::string> newTypeIds; if($3 != NULL) {
  for(AST* child : $3->getChildren()){
    newTypeIds.emplace(child->getValue());
  }
} parserTypeIdentifierStack.push_back(newTypeIds); } '(' parameterList ')' {parserTypeIdentifierStack.pop_back(); delete $5; delete $7; $$ = new AST($2, NT_FUNCTION_PROTOTYPE); $$->addChild($1); $$->addChild($3); $$->addChild($6);}
;

typeOrVoid
: typeRef {$$ = $1;}
| VOID {$$ = new AST($1, NT_TYPE_OR_VOID);}
| IDENTIFIER {$$ = new AST($1, NT_TYPE_OR_VOID);}
// may be a type variable
;

argumentList 
: /* empty */ {$$ = NULL;}
| nonEmptyArgList {$$ = $1;}
;

nonEmptyArgList
: argument {$$ = new AST(NULL, NT_ARGUMENT_LIST); $$->addChild($1);}
| nonEmptyArgList ',' argument {delete $2; $$ = $1; $$->addChild($3);}
;

argument
: expression {$$ = new AST(NULL, NT_ARGUMENT); $$->addChild($1);}
| name '=' expression {delete $2; $$ = new AST($1, NT_ARGUMENT); $$->addChild($3);}
| DONTCARE {delete $1; $$ = new AST("_", NT_ARGUMENT); $$->addChild(NULL);}
;

kvList
: kvPair
| kvList ',' kvPair
;

kvPair
: name '=' expression
;

expressionList
: /* empty */ {$$ = NULL;}
| expression {$$ = new AST(NULL, NT_EXPRESSION_LIST); $$->addChild($1);}
| expressionList ',' expression {delete $2; if($1 == NULL) {$$ = new AST(NULL, NT_EXPRESSION_LIST);} else {$$ = $1;} $$->addChild($3);}
;

statement
: assignmentOrMethodCallStatement {$$ = $1;}
| conditionalStatement {$$ = $1;}
| emptyStatement {$$ = $1;}
| blockStatement {$$ = $1;}
| exitStatement {$$ = $1;}
| returnStatement {$$ = $1;}
| switchStatement {$$ = $1;}
;

conditionalStatement
: IF '(' expression ')' statement {delete $1; delete $2; delete $4; $$ = new AST("IF", NT_CONDITIONAL_STATEMENT); $$->addChild($3); $$->addChild($5); $$->addChild(NULL);}
| IF '(' expression ')' statement ELSE statement {delete $1; delete $2; delete $4; delete $6; $$ = new AST("IF", NT_CONDITIONAL_STATEMENT); $$->addChild($3); $$->addChild($5); $$->addChild($7);}
;

switchStatement
: SWITCH '(' expression ')' '{' switchCases '}' {delete $1; delete $2; delete $4; delete $5; delete $7; $$ = new AST(NULL, NT_SWITCH_STATEMENT); $$->addChild($3); $$->addChild($6);}
;

switchCases
: /* empty */ {$$ = NULL;}
| switchCases switchCase {if($1 == NULL) {$$ = new AST(NULL, NT_SWITCH_CASE_LIST);} else {$$ = $1;} $$->addChild($2);}
;

switchCase
: switchLabel ':' blockStatement {delete $2; $$ = new AST(NULL, NT_SWITCH_CASE); $$->addChild($1); $$->addChild($3);}
| switchLabel ':' {delete $2; $$ = new AST(NULL, NT_SWITCH_CASE); $$->addChild($1); $$->addChild(NULL);}
;

switchLabel
: DEFAULT {$$ = new AST($1, NT_SIMPLE_KEYSET_DEFAULT);}
| nonBraceExpression {$$ = $1;}
;

nonBraceExpression
: INTEGER {$$ = new AST($1, NT_EXPRESSION);}
| STRING_LITERAL {$$ = new AST($1, NT_EXPRESSION);}
| TRUE {$$ = new AST($1, NT_EXPRESSION);}
| FALSE {$$ = new AST($1, NT_EXPRESSION);}
| THIS {$$ = new AST($1, NT_EXPRESSION);}
| nonTypeName
| dotPrefix nonTypeName
| nonBraceExpression '[' expression ']'
| nonBraceExpression '[' expression ':' expression ']'
| '(' expression ')'
| '!' expression %prec PREFIX
| '~' expression %prec PREFIX
| '-' expression %prec PREFIX
| '+' expression %prec PREFIX
| typeName '.' member
| ERROR '.' member
| nonBraceExpression '.' member
| nonBraceExpression '*' expression
| nonBraceExpression '/' expression
| nonBraceExpression MODULUS expression
| nonBraceExpression '+' expression
| nonBraceExpression '-' expression
/*
| nonBraceExpression '|+|' expression
| nonBraceExpression '|-|' expression
*/
| nonBraceExpression TK_OC_LE expression
| nonBraceExpression TK_OC_GE expression
| nonBraceExpression '<' expression
| nonBraceExpression '>' expression
| nonBraceExpression TK_OC_NE expression
| nonBraceExpression TK_OC_EQ expression
| nonBraceExpression '<' '<' expression
| nonBraceExpression '>' '>' expression
| nonBraceExpression '&' expression
| nonBraceExpression '^' expression
| nonBraceExpression '|' expression
/*
| nonBraceExpression '++' expression
*/
| nonBraceExpression TK_OC_AND expression
| nonBraceExpression TK_OC_OR expression
| nonBraceExpression '?' expression ':' expression
| nonBraceExpression '<' realTypeArgumentList '>' '(' argumentList ')'
| nonBraceExpression '(' argumentList ')'
| namedType '(' argumentList ')'
| '(' typeRef ')' expression
;

realTypeArgumentList
: realTypeArg {$$ = new AST(NULL, NT_TYPE_ARGUMENT_LIST); $$->addChild($1);}
| realTypeArgumentList ',' typeArg {delete $2; if($1 == NULL) {$$ = new AST(NULL, NT_TYPE_ARGUMENT_LIST);} else {$$ = $1;} $$->addChild($3);}
;

assignmentOrMethodCallStatement
: lvalue '(' argumentList ')' ';' {delete $2; delete $4; delete $5; $$ = new AST(NULL, NT_METHOD_CALL_STATEMENT); $$->addChild($1); $$->addChild($3);}
| lvalue '<' typeArgumentList '>' '(' argumentList ')' ';'
| lvalue '=' expression ';' {delete $4; $$ = new AST($2, NT_ASSIGN_STATEMENT); $$->addChild($1); $$->addChild($3);}
;

emptyStatement
: ';' {delete $1; $$ = new AST(NULL, NT_EMPTY_STATEMENT);}
;

blockStatement
: '{' {CSPush();} statOrDeclList '}' {CSPop(); delete $1; delete $4; $$ = $3;}
/* : optAnnotations '{' statOrDeclList '}' */
;

statOrDeclList
: /* empty */ {$$ = NULL;}
| statOrDeclList statementOrDeclaration {if($1 == NULL) {$$ = new AST(NULL, NT_STAT_OR_DEC_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
;

statementOrDeclaration
: variableDeclaration {$$ = $1;}
| constantDeclaration {$$ = $1;}
| statement {$$ = $1;}
| instantiation
;

lvalue
: prefixedNonTypeName {$$ = new AST($1, NT_NONE);}
| THIS {$$ = new AST($1, NT_NONE);}
| lvalue '.' member  {$$ = new AST($2, NT_NONE); $$->addChild($1); $$->addChild(new AST($3, NT_NONE));}
| lvalue '[' expression ']' {delete $2; delete $4; $$ = new AST("[]", NT_NONE); $$->addChild($1); $$->addChild($3);}
| lvalue '[' expression ':' expression ']'
;

prefixedNonTypeName
: nonTypeName {$$ = $1;}
| dotPrefix nonTypeName
;

returnStatement
: RETURN ';' {delete $2; $$ = new AST($1, NT_RETURN_STATEMENT);}
| RETURN expression ';' {delete $3; $$ = new AST($1, NT_RETURN_STATEMENT); $$->addChild($2);}
;

exitStatement
: EXIT ';' {delete $2; $$ = new AST($1, NT_EXIT_STATEMENT);}
;

variableDeclaration
: typeRef name optInitializer ';' {delete $4; $$ = new AST($2, NT_VARIABLE_DECLARATION); $$->addChild(NULL); $$->addChild($1); $$->addChild($3);}
| annotations typeRef name optInitializer ';' {delete $5; $$ = new AST($3, NT_VARIABLE_DECLARATION); $$->addChild($1); $$->addChild($2); $$->addChild($4);}
;

optInitializer
: /* empty */ {$$ = NULL;}
| '=' initializer {delete $1; $$ = $2;}
;

selectExpression
: SELECT '(' expressionList ')' '{' selectCaseList '}' {delete $2; delete $4; delete $5; delete $7; $$ = new AST($1, NT_SELECT_EXPRESSION); $$->addChild($3); $$->addChild($6);}
;

selectCaseList
: /* empty */ {$$ = NULL;}
| selectCaseList selectCase {if($1 == NULL) {$$ = new AST(NULL, NT_SELECT_CASE_LIST); $$->addChild($2);} else {$$ = $1; $$->addChild($2);}}
;

selectCase
: keysetExpression ':' name ';' {delete $2; delete $4; $$ = new AST($3, NT_SELECT_CASE); $$->addChild($1);}
;

keysetExpression
: tupleKeysetExpression {$$ = $1;}
| simpleKeysetExpression {$$ = $1;}
;

tupleKeysetExpression
: '(' simpleKeysetExpression ',' simpleExpressionList ')' {delete $1; delete $3; delete $5; $$ = new AST(NULL, NT_TUPLE_KEYSET_EXPRESSION); $4->addChildStart($2); $$->addChild($4);}
| '(' reducedSimpleKeysetExpression ')' {delete $1; delete $3; $$ = new AST(NULL, NT_TUPLE_KEYSET_EXPRESSION); $$->addChild($2);}
;

simpleExpressionList
: simpleKeysetExpression {$$ = new AST(NULL, NT_SIMPLE_EXPRESSION_LIST); $$->addChild($1);}
| simpleExpressionList ',' simpleKeysetExpression {delete $2; $$ = $1; $$->addChild($3);}
;

reducedSimpleKeysetExpression
: DEFAULT {$$ = new AST($1, NT_SIMPLE_KEYSET_DEFAULT);}
| DONTCARE {$$ = new AST($1, NT_SIMPLE_KEYSET_DONTCARE);}
/*
: expression '&&&' expression
| expression '..' expression
*/
;

simpleKeysetExpression
: expression {$$ = $1;}
| DEFAULT {$$ = new AST($1, NT_SIMPLE_KEYSET_DEFAULT);}
| DONTCARE {$$ = new AST($1, NT_SIMPLE_KEYSET_DONTCARE);}
| expression MASK expression {$$ = new AST($2, NT_MASK); $$->addChild($1); $$->addChild($3);}
| expression RANGE expression {$$ = new AST($2, NT_RANGE); $$->addChild($1); $$->addChild($3);}
;

optAnnotations
: /* empty */ {$$ = NULL;}
| annotations {$$ = $1;}
;

annotations
: annotation {$$ = new AST(NULL, NT_ANNOTATION_LIST); $$->addChild($1);}
| annotations annotation {$$ = $1; $$->addChild($2);}
;

annotation
: '@' name {delete $1; $$ = new AST($2, NT_ANNOTATION); $$->addChild(NULL);}
| '@' name '(' annotationBody ')' {delete $1; delete $3; delete $5; $$ = new AST($2, NT_ANNOTATION); $$->addChild($4);}
/*| '@' name '[' structuredAnnotationBody ']'*/ 
;

annotationBody
: /* empty */ {$$ = NULL;}
| annotationBody '(' annotationBody ')' {delete $2; delete $4; $$ = new AST(NULL, NT_ANNOTATION_BODY); $$->addChild($1); $$->addChild($3);}
| annotationBody annotationToken {if($1 == NULL) {$$ = new AST(NULL, NT_ANNOTATION_TOKEN_LIST); AST *c = new AST($2, NT_ANNOTATION_TOKEN); $$->addChild(c);} else { $$ = $1; AST *c = new AST($2, NT_ANNOTATION_TOKEN); $$->addChild(c);}}
;

structuredAnnotationBody
: expressionList
| kvList
;

annotationToken
: ABSTRACT {$$ = $1;}
| ACTION {$$ = $1;}
| ACTIONS {$$ = $1;}
| APPLY {$$ = $1;}
| BOOL {$$ = $1;}
| BIT {$$ = $1;}
| CONST {$$ = $1;}
| CONTROL {$$ = $1;}
| DEFAULT {$$ = $1;}
| ELSE {$$ = $1;}
| ENTRIES {$$ = $1;}
| ENUM {$$ = $1;}
| ERROR {$$ = $1;}
| EXIT {$$ = $1;}
| EXTERN {$$ = $1;}
| FALSE {$$ = $1;}
| HEADER {$$ = $1;}
| HEADER_UNION {$$ = $1;}
| IF {$$ = $1;}
| IN {$$ = $1;}
| INOUT {$$ = $1;}
| INT {$$ = $1;}
| KEY {$$ = $1;}
| MATCH_KIND {$$ = $1;}
| TYPE {$$ = $1;}
| OUT {$$ = $1;}
| PARSER {$$ = $1;}
| PACKAGE {$$ = $1;}
| PRAGMA {$$ = $1;}
| RETURN {$$ = $1;}
| SELECT {$$ = $1;}
| STATE {$$ = $1;}
| STRING {$$ = $1;}
| STRUCT {$$ = $1;}
| SWITCH {$$ = $1;}
| TABLE {$$ = $1;}
| TRANSITION {$$ = $1;}
| TRUE {$$ = $1;}
| TUPLE {$$ = $1;}
| TYPEDEF {$$ = $1;}
| VARBIT {$$ = $1;}
| VALUESET {$$ = $1;}
| VOID {$$ = $1;}
| '_' {$$ = $1;}
| IDENTIFIER {$$ = $1;}
| TYPE_IDENTIFIER {$$ = $1;}
| STRING_LITERAL {$$ = $1;}
| INTEGER {$$ = $1;}
/* | "&&&" */
/* | ".." */
| TK_OC_AND {$$ = $1;}
| TK_OC_OR {$$ = $1;}
| TK_OC_EQ {$$ = $1;}
| TK_OC_NE {$$ = $1;}
| TK_OC_GE {$$ = $1;}
| TK_OC_LE {$$ = $1;}
/* | '++' */
| '+' {$$ = $1;}
/* | '|+|' */
| '-' {$$ = $1;}
/* | '|-|' */
| '*' {$$ = $1;}
| '/' {$$ = $1;}
| MODULUS {$$ = $1;}
| '|' {$$ = $1;}
| '&' {$$ = $1;}
| '^' {$$ = $1;}
| '~' {$$ = $1;}
| '[' {$$ = $1;}
| ']' {$$ = $1;}
| '{' {$$ = $1;}
| '}' {$$ = $1;}
| '<' {$$ = $1;}
| '>' {$$ = $1;}
| '!' {$$ = $1;}
| ':' {$$ = $1;}
| ',' {$$ = $1;}
| '?' {$$ = $1;}
| '.' {$$ = $1;}
| '=' {$$ = $1;}
| ';' {$$ = $1;}
| '@' {$$ = $1;}
/*| UNKNOWN_TOKEN*/
;

%%

void yyerror (char const *s) {
  printf("Error at line: %d:\n", get_line_number());
  printf("---> %s\n", s);

  if(includeStack.size() > 0){
    printf("\n");
    for(int x = (int) includeStack.size() - 1; x >= 0; x--){
      printf("...in file \"%s\" included at line %d\n", includeStack[x].first.c_str(), includeStack[x].second);
    }
  }
}
