#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_SIZE 100
#define MAX_TOKEN_LENGTH 30

// function prototype
int checkVar(char token[MAX_TOKEN_LENGTH]);
int caseCLASS(FILE *tokenList, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord);
int caseCONST(FILE *tokenList, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord);
int caseVAR(FILE *tokenList, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH],  int *foundKeyWord);
int caseMath(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord);
int checkOperatorPrecedence(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char operator[MAX_TOKEN_LENGTH]);
int genQuad(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char operator[MAX_TOKEN_LENGTH], char temp[10]);
int caseIF(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord);
int caseTHEN(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord, int *top, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *hasNestedIF);
int caseWHILE(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord);
int caseDO(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord, int *top, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *hasNestedIF);
int caseGET(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], int *foundKeyWord);
int casePUT(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], int *foundKeyWord);
int casePROCEDURE(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH]);
int caseCALL(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH]);


// control the { } of the program
int leftBracket = 0, rightBracket = 0;

// number of temp 
int tempNum = 0;
// IF label
int ifLabelNum = 0;
// WHILE label
int whileLabelNum = 0;
// WHILE exit label
int whileExitLabelNum = 0;

int main()
{
    FILE *tokenList, *quad;
    char token[MAX_TOKEN_LENGTH], classification[MAX_TOKEN_LENGTH];

    // access token list
    tokenList = fopen("token_list.txt", "r");

    // open the quad list
    quad = fopen("quad.txt", "w");

    // skip the second line
    fscanf(tokenList, "%s%s", &token, &classification);

    int foundKeyWord = 1, foundClass = 1, foundProcedure = 0;
    
    while(fscanf(tokenList, "%s%s", &token, &classification) != EOF)
    {
        // CLASS
        if(strcmp(token, "CLASS") == 0)
        {
            caseCLASS(tokenList, token, classification, &foundKeyWord);
            foundClass = 0;
        }
        else if(foundClass)
        {
            // dont see CLASS then prompt error
            printf("Error: Missing CLASS \n");
        }

        // {
        if(strcmp(token, "{") == 0)
        {
            leftBracket = 1;
        }

        // CONST
        if(strcmp(token, "CONST") == 0)
        {
            caseCONST(tokenList, token, classification, &foundKeyWord);
        }

        // VAR
        if(strcmp(token, "VAR") == 0)
        {
            caseVAR(tokenList, token, classification, &foundKeyWord);
        }

        // PROCEDURE
        if(strcmp(token, "PROCEDURE") == 0)
        {
            casePROCEDURE(tokenList, quad, token);
            foundKeyWord  = 0;
            foundProcedure = 1;
        }

        // CALL
        if(strcmp(token, "CALL") == 0)
        {
            caseCALL(tokenList, quad, token);
            foundKeyWord = 0;
        }

        // IF
        if(strcmp(token, "IF") == 0)
        {
            caseIF(tokenList, quad, token, classification, &foundKeyWord);
        }

        // WHILE
        if(strcmp(token, "WHILE") == 0)
        {
            caseWHILE(tokenList, quad, token, classification, &foundKeyWord);
        }

        // ODD
        if(strcmp(token, "ODD") == 0)
        {

        }

        // GET
        if(strcmp(token, "GET") == 0)
        {
            caseGET(tokenList, quad, token, &foundKeyWord);
        }

        // PUT
        if(strcmp(token, "PUT") == 0)
        {
            casePUT(tokenList, quad, token, &foundKeyWord);
        }

        // }
        if(strcmp(token, "}") == 0)
        {
            if(!foundProcedure)
                rightBracket = 1;
            else
            {
                fprintf(quad, "ret\n");
                foundProcedure = 0;
            }
            foundKeyWord = 0;        
        }

        // if we don't found any keyword in a sentence then it must be a math case
        if(foundKeyWord)
        {
            caseMath(tokenList, quad, token, classification, &foundKeyWord);
        }
        // reset found key word everytime we done with a keyword
        foundKeyWord = 1;
    }
    fclose(tokenList);
    fclose(quad);
    return 0;
}

// Simple use of push and pop, I implement these 2 function by 
// reading https://www.tutorialride.com/c-stack-programs/push-pop-items-from-string-stack-c-program.htm
int push(char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char token[MAX_TOKEN_LENGTH])
{
    if(*top == MAX_STACK_SIZE)
    {
        printf("The stack is full");
        exit(1);
    }
    else
    {
        *top += 1;
        strcpy(stack[*top], token);
        printf("Push: %s\n", token);
        return 0;
    }  
}

int pop(char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char token[MAX_TOKEN_LENGTH])
{
    if(*top == 0)
    {
        printf("The stack is empty");
        exit(1);
    }
    else
    {
        strcpy(token, stack[*top]);
        printf("Pop: %s\n", token);
        *top -= 1;
        return 0;
    }  
}

int seek(char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char token[MAX_TOKEN_LENGTH])
{
    if(*top == 0)
    {
        printf("The stack is empty");
        exit(1);
    }
    else
    {
        strcpy(token, stack[*top]);
        printf("Seek: %s\n", token);
        *top -= 1;
        return 0;
    }  
}

/* Precedence table
    0: CONST            13: >
    1: VAR              14: <
    2; PROCEDURE        15: >=
    3: CALL             16: <=
    4: IF               17: +
    5: THEN             18: -
    6: WHILE            19: *
    7: DO               20: /
    8: ODD              21: =
    9: (                22: Delimiter
    10: )               23: GET
    11: !=              24: PUT
    12: ==              25: ident

    (error) = 0         (<) = 1           (=) = 2           (>) = 3
*/
int precedenceTable[26][26] = 
{
    // CONST
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    // VAR
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // PROCEDURE
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // CALL
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // IF
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // THEN
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    // WHILE
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // DO
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    // ODD
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    // (
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // )
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 0, 0, 1},
    // !=
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // ==
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // >
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // <
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // >=
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // <=
    {0, 0, 0, 0, 0, 3, 0, 3, 0, 1, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
    // +
    {0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 0, 3, 0, 0, 1},
    // -
    {0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 0, 3, 0, 0, 1},
    // *
    {0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 0, 1},
    // /
    {0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 0, 0, 1},
    // =
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 3, 0, 0, 1},
    // Delimiter
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
    // GET
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1},
    // PUT
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1},
    // ident
    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2}
};

// throw error when the name of the variable is not start with a character
int checkVar(char token[MAX_TOKEN_LENGTH])
{
    if(isdigit(token[0]))
    {
        printf("Error: Variable's name cannot start with a number\n");
        exit(1);
    }
    else if(token[0] == '~')
    {
        printf("Error: Variable's name cannot start with a ~\n");
        exit(1);
    }
    else
        return 0;
}

// CLASS
// simple check for the rule of class name
int caseCLASS(FILE *tokenList, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    char tempToken[MAX_TOKEN_LENGTH], tempClassification[MAX_TOKEN_LENGTH], assigmentToken[MAX_TOKEN_LENGTH];
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        if(strcmp(tempClassification, "<ProgramName>") == 0)
        {
            // check if the var name format
            checkVar(tempToken);
        }
        else if(strcmp(tempClassification, "<LeftBracket>") == 0)
        {
            // we have left bracket
            leftBracket = 0;
            // notify that the next line does not have a key word
            *foundKeyWord = 0;
            break;
        }
        // user forgot the Delimiter at the end
        else if(strcmp(tempClassification, "<VAR>") == 0 || strcmp(tempClassification, "<IF>") == 0 || strcmp(tempClassification, "<PROCEDURE>") == 0 ||
                strcmp(tempClassification, "<WHILE>") == 0 || strcmp(tempClassification, "<ODD>") == 0 || strcmp(tempClassification, "<CALL>") == 0 ||
                strcmp(tempClassification, "<GET>") == 0 || strcmp(tempClassification, "<PUT>") == 0 || strcmp(tempClassification, "<CONST>") == 0)
        {
            if(leftBracket)
                printf("Error at CLASS: Expected { at the end of line\n");
            exit(1);
        }
    }
    return 0;
}

// CONST
// since we use CONST to declare and intialize variable, we don't have to generate the 
// quads for this case, we just need to check for error;
int caseCONST(FILE *tokenList, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    char tempToken[MAX_TOKEN_LENGTH], tempClassification[MAX_TOKEN_LENGTH], assigmentToken[MAX_TOKEN_LENGTH];
    int assignFlag = 0, seperatorFlag = 1;
    // procude an error if we found an inappropriate token 
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            // check if the user forget to put a comma
            if(seperatorFlag > 1)
            {
                printf("Error at CONST: Missing ,\n");
                exit(1);
            }
            // check if the var name format
            checkVar(tempToken);
            // keep track of the current variable
            strcpy(assigmentToken, tempToken);
            seperatorFlag += 1;
        }
        // set the assign flag to keep track if the user forgot to put in the variable
        else if(strcmp(tempClassification, "<$assign>") == 0)
            assignFlag = 1;
        else if(strcmp(tempClassification, "<Comma>") == 0)
        {
            // report error if the assign flag is not off
            if(assignFlag)
            {
                printf("Error at CONST: Missing value for variable %s\n", assigmentToken);
                return 1;
            }
            seperatorFlag -= 1;
        }
        // confirm assigment and set the assign flag off
        else if(strcmp(tempClassification, "<NumLit>") == 0)
        {
            if(assignFlag)
                assignFlag = 0;
            else
            {
                printf("Error at CONST: Missing = \n");
                exit(1);
            }
        }
        else if(strcmp(tempClassification, "<Delimiter>") == 0)
        {
            // report error if the assign flag is not off
            if(assignFlag)
            {
                printf("Error at CONST: Missing value for variable %s\n", assigmentToken);
                exit(1);
            }
            seperatorFlag = 0;
            // notify that the next line does not have a key word
            *foundKeyWord = 0;
            break;
        }
        // user forgot the Delimiter at the end
        else if(strcmp(tempClassification, "<CONST>") == 0 || strcmp(tempClassification, "<IF>") == 0 || strcmp(tempClassification, "<PROCEDURE>") == 0 ||
                strcmp(tempClassification, "<WHILE>") == 0 || strcmp(tempClassification, "<ODD>") == 0 || strcmp(tempClassification, "<CALL>") == 0 ||
                strcmp(tempClassification, "<GET>") == 0 || strcmp(tempClassification, "<PUT>") || strcmp(tempClassification, "<VAR>") == 0)
        {
            printf("Error at CONST: Missing ;\n");
            exit(1);
        }
    }
    return 0;
}

// VAR
// we use VAR to declare variable only
int caseVAR(FILE *tokenList, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH],  int *foundKeyWord)
{
    char tempToken[MAX_TOKEN_LENGTH], tempClassification[MAX_TOKEN_LENGTH], assigmentToken[MAX_TOKEN_LENGTH];
    int seperatorFlag = 0;
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            // check if the var name format
            checkVar(tempToken);
            // check if the user forget to put a comma
            if(seperatorFlag)
            {
                printf("Error at VAR: Missing ,\n");
                exit(1);
            }
            seperatorFlag = 1;
        }
        else if(strcmp(tempClassification, "<Comma>") == 0)
        {
            // set the seperator flag off
            seperatorFlag = 0;
        }
        else if(strcmp(tempClassification, "<$assign>") == 0)
        {
            printf("Error at VAR: Don't allow variable initailize at VAR\n");
            exit(1);
        }
        else if(strcmp(tempClassification, "<Delimiter>") == 0)
        {
            // notify that the next line does not have a key word
            *foundKeyWord = 0;
            break;
        }
        // user forgot the Delimiter at the end
        else if(strcmp(tempClassification, "<CONST>") == 0 || strcmp(tempClassification, "<IF>") == 0 || strcmp(tempClassification, "<PROCEDURE>") == 0 ||
                strcmp(tempClassification, "<WHILE>") == 0 || strcmp(tempClassification, "<ODD>") == 0 || strcmp(tempClassification, "<CALL>") == 0 ||
                strcmp(tempClassification, "<GET>") == 0 || strcmp(tempClassification, "<PUT>") == 0)
        {
            printf("Error at VAR: Missing ;\n");
            exit(1);
        }
    }
    return 0;
}

// GET
int caseGET(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null", tempClassification[MAX_TOKEN_LENGTH];
    *foundKeyWord = 0;
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            fprintf(quad, "GET %s\n", tempToken);
        }
        else if(strcmp(tempToken, ";") == 0)
        {
            return 1;
        }
        else
        {
            printf("Error at GET: only accept VAR\n");
            exit(1);
        }
    }
    return 0;
}

// PUT
int casePUT(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null", tempClassification[MAX_TOKEN_LENGTH];

    *foundKeyWord = 0;
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            fprintf(quad, "PUT %s\n", tempToken);
        }
        else if(strcmp(tempToken, ";") == 0)
        {
            return 1;
        }
        else
        {
            printf("Error at PUT: only accept VAR\n");
            exit(1);
        }
    }
    return 0;
}

// generate quad
int genQuad(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char operator[MAX_TOKEN_LENGTH], char temp[10])
{
    // keep poping if we don't find any +, -, *, or /
    char tempToken[MAX_TOKEN_LENGTH], tempToken2[MAX_TOKEN_LENGTH], tempOperator[MAX_TOKEN_LENGTH], lastParen[MAX_TOKEN_LENGTH] = "null";

    // since we are doing binary operation, we need to put them in the correct order
    // stack | ,, | A | = | B
    // if we just pop it, it would be B = A 
    // so we need to keep track of some tokens 
    int tokenOrder = 1;

    // pop we only pop 3 times from the stack
    int popNum = 3;

    // keep track if we have a parenthesis or not
    int hasParen = 0;

    while(*top > 0 && popNum > 0)
    {
        pop(stack, top, tempToken);

        // check the token on the stack
        if (strcmp(tempToken, "=") == 0 || strcmp(tempToken, "-") == 0 || strcmp(tempToken, "+") == 0 || strcmp(tempToken, "/") == 0 || strcmp(tempToken, "*") == 0)
            strcpy(tempOperator, tempToken);
        // the next token on the stack is the second token of the binary operation
        else if(tokenOrder)
        {
            strcpy(tempToken2, tempToken);
            tokenOrder = 0;
        }

        // if found a ( then do nothing
        if(strcmp(tempToken, "(") == 0 )
        {
            strcpy(lastParen, tempToken); 
        }
        else
        {
            popNum -= 1;
        }
    }

    // print the operator first
    fprintf(quad, "%s ", tempOperator);
    // then the first token
    fprintf(quad, "%s ", tempToken);
    // then the second token
    fprintf(quad, "%s ", tempToken2);
    // check the temporary if it is not null then we have to push it to the stack
    if(strcmp(temp, "null") != 0)
    {
        push(stack, top, temp);
        fprintf(quad, "%s", temp);
    }
    
    // change temp back to null
    strcpy(temp, "null");

    fprintf(quad, "\n");
}

// check the type of the operator and send it to generate the appropriate quad
int checkOperatorPrecedence(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char operator[MAX_TOKEN_LENGTH])
{
    // create a new Temporary and push it to the top temp stack
    char temp[10] = "~T";
    char num[3];

    // convert topTempStack to string
    sprintf(num, "%d", tempNum);
    tempNum += 1;
    strcat(temp, num);

    // put the temp to the symbol table
    FILE *symbolTable;
    symbolTable = fopen("symbol_table.txt", "a");
    fprintf(symbolTable, "%s 0 0 0 0\n", temp);
    fclose(symbolTable);

    // pop the current stuff in the
    genQuad(quad, stack, top, operator, temp);
    return 0;
}

// Mathematic case
int caseMath(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    // initialize the first token to be a delimiter
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null", lastOperator[10] = "null";
    char tempClassification[MAX_TOKEN_LENGTH], assigmentToken[MAX_TOKEN_LENGTH], stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], operator[10] = "null";
    int top = 0, hasOperator = 0, parenOrder = 0, hasOperatorBeforeParen = 0;
 
    // If the last token is < or = than the current token then push it to the stack
    // else pop the stack until >

    // check if there is something in token
    // if not move on the next line else process it
    // first time there will always the delimiter in the stack
    if(strcmp(token, "") != 0)
    {
        // we push the ident to the stack
        if(strcmp(classification, "<$var>") == 0)
        {
            // check if the var name format
            checkVar(token);
            // check the precedent table
            if(precedenceTable[22][25] == 1)
                push(stack, &top, token);
        }
        else
        {
            printf("Error: Missing variable\n");
        }
        
    }
    // scan the rest of the line
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {     
        // we push the ident to the stack
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            // if has operator then push the operator to the stack
            if(hasOperator)
            {
                if(strcmp(operator, "null") != 0)
                    push(stack, &top, operator);
                // push the num lit
                push(stack, &top, tempToken);
                hasOperator = 0;
            }
            else
            {
                //error 
                printf("Error: Missing operator\n");
                exit(1);
            }                
        }
        else if(strcmp(tempClassification, "<NumLit>") == 0)
        {
            // if has operator then push the operator to the stack
            if(hasOperator)
            {
                push(stack, &top, operator);
                // push the num lit
                push(stack, &top, tempToken);
                hasOperator = 0;
            }
            else
            {
                //error 
                printf("Error: Missing operator\n");
                exit(1);
            }
        }
        // if we found an operator then look it up at the precedence table
        else if(strcmp(tempClassification, "<$assign>") == 0 || strcmp(tempClassification, "<MULT>") == 0 || strcmp(tempClassification, "<ADD>") == 0 || strcmp(tempToken, "(") == 0 || strcmp(tempToken, ")") == 0)
        {
            hasOperator = 1;
            // if the current operator is null then we know this is the first operator in the line
            if(strcmp(operator, "null") == 0)
            {
                strcpy(operator, tempToken);
            }
            // current operator is = go to precedentTable[21][]
            else if(strcmp(operator, "=") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[21][17] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[21][18] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[21][19] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[21][20] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    // found a (
                    if(precedenceTable[21][9] == 1)
                    {
                        push(stack, &top, operator);
                        strcpy(operator, tempToken);
                        parenOrder += 1;
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is + go to precedentTable[17][]
            else if(strcmp(operator, "+") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[17][17] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[17][18] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[17][19] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[17][20] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[17][9] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[17][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is - go to precedentTable[18][]
            else if(strcmp(operator, "-") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[18][17] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[18][18] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[18][19] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[18][20] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[18][9] == 1)
                    {
                        // push the - to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[18][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }                
            }
            // current operator is * go to precedentTable[19][]
            else if(strcmp(operator, "*") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[19][17] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[19][18] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);                            
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[19][19] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[19][20] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[19][9] == 1)
                    {
                        // push the * to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }     
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[19][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                } 
            }
            // current operator is / go to precedentTable[20][]
            else if(strcmp(operator, "/") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[20][17] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[20][18] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[20][19] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[20][20] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[20][9] == 1)
                    {
                        // push the * to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }     
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[20][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is ( go to precedentTable[9][]
            else if(strcmp(operator, "(") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[9][17] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[9][18] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[9][19] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[9][20] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    // found a (
                    if(precedenceTable[9][9] == 1)
                    {
                        // push the - to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is ) go to precedentTable[10][]
            else if(strcmp(operator, ")") == 0)
            {
                parenOrder -= 1;
                // go look for the operator before the parenthesis that we have missed
                if(!parenOrder && hasOperatorBeforeParen)
                {
                    char restoreOperatorBeforeParen[MAX_TOKEN_LENGTH];
                    int tempTop = top;
                    // keep looking for the operator before the ()
                    while(1)
                    {
                        seek(stack, &tempTop, restoreOperatorBeforeParen);
                        if(strcmp(restoreOperatorBeforeParen, "*") == 0 || strcmp(restoreOperatorBeforeParen, "/") == 0)
                        {
                            strcpy(lastOperator, restoreOperatorBeforeParen);
                            checkOperatorPrecedence(quad, stack, &top, lastOperator);
                            break;
                        }
                        else if(strcmp(restoreOperatorBeforeParen, "+") == 0 || strcmp(restoreOperatorBeforeParen, "-") == 0)
                        {
                            strcpy(operator, restoreOperatorBeforeParen);
                            break;
                        }
                    }
                    hasOperatorBeforeParen = 0;
                }

                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[10][17] == 3)
                    {
                        
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[10][18] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[10][19] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[10][20] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[10][10] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
        }
        // found the delimiter then generate the appropriate quad
        else if(strcmp(tempClassification, "<Delimiter>") == 0)
        {
            hasOperator = 1;
            // delimiter is at 22
            if(strcmp(operator, "=") == 0)
            {
                // the assignment case usually does not need a loop to pop everything out of the stack
                if(precedenceTable[21][22] == 3)
                {
                    genQuad(quad, stack, &top, operator, temp);
                }
            }
            else if(strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0 || strcmp(operator, "*") == 0 || strcmp(operator, "/") == 0 || strcmp(operator, ")") == 0)
            {
                // +
                if(strcmp(operator, "+") == 0)
                {
                    // generate the last quad 
                    if(precedenceTable[17][22] == 3)
                        // generate the quad of the operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);

                    // pop everything out
                    genQuad(quad, stack, &top, operator, temp);
                }
                // -
                else if(strcmp(operator, "-") == 0)
                {
                    // generate the last quad 
                    if(precedenceTable[18][22] == 3)
                        // generate the quad of the operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);

                    // pop everything out
                    genQuad(quad, stack, &top, operator, temp);
                }
                // *
                else if(strcmp(operator, "*") == 0)
                {
                    // generate the last quad 
                    if(precedenceTable[19][22] == 3)
                        // generate the quad of the operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                    
                    // make sure we don't miss any operator
                    if(strcmp(lastOperator, "null") != 0)
                        checkOperatorPrecedence(quad, stack, &top, lastOperator);

                    // pop everything out
                    genQuad(quad, stack, &top, operator, temp);
                }
                else if(strcmp(operator, "/") == 0)
                {
                    // generate the last quad 
                    if(precedenceTable[20][22] == 3)
                        // generate the quad of the operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);

                    // make sure we don't miss any operator
                    if(strcmp(lastOperator, "null") != 0)
                        checkOperatorPrecedence(quad, stack, &top, lastOperator);

                    // pop everything out
                    genQuad(quad, stack, &top, operator, temp);
                }
                else if(strcmp(operator, ")") == 0)
                {


                    // pop everything out
                    genQuad(quad, stack, &top, operator, temp);
                }
                if(strcmp(operator, "null") == 0)
                    break;
            }
            break;
        }
        // user forgot the Delimiter at the end
        else if(strcmp(tempClassification, "<CONST>") == 0 || strcmp(tempClassification, "<IF>") == 0 || strcmp(tempClassification, "<PROCEDURE>") == 0 ||
                strcmp(tempClassification, "<WHILE>") == 0 || strcmp(tempClassification, "<ODD>") == 0 || strcmp(tempClassification, "<CALL>") == 0 ||
                strcmp(tempClassification, "<GET>") == 0 || strcmp(tempClassification, "<PUT>") || strcmp(tempClassification, "<VAR>") == 0)
        {
            printf("Error: Missing ;\n");
            exit(1);
        }
    }
    return 0;
}

// gen quad for IF
int genQuad_IF(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char operator[MAX_TOKEN_LENGTH], char temp[10])
{
    // keep poping if we don't find any +, -, *, or /
    char tempToken[MAX_TOKEN_LENGTH], tempToken2[MAX_TOKEN_LENGTH], tempOperator[MAX_TOKEN_LENGTH];

    // since we are doing binary operation, we need to put them in the correct order
    // stack | ,, | A | = | B
    // if we just pop it, it would be B = A 
    // so we need to keep track of some tokens 
    int tokenOrder = 1;

    // pop we only pop 3 times from the stack
    int popNum = 3;

    while(*top > 0 && popNum > 0)
    {
        pop(stack, top, tempToken);

        // check the token on the stack
        if (strcmp(tempToken, "<") == 0 || strcmp(tempToken, "<=") == 0 || strcmp(tempToken, ">") == 0 || strcmp(tempToken, ">=") == 0 
        || strcmp(tempToken, "!=") == 0 || strcmp(tempToken, "==") == 0)
            strcpy(tempOperator, tempToken);
        // the next token on the stack is the second token of the binary operation
        else if(tokenOrder)
        {
            strcpy(tempToken2, tempToken);
            tokenOrder = 0;
        }
        // if found a ( then do nothing
        if(strcmp(tempToken, "(") == 0 )
            continue;
        else
            popNum -= 1;
    }

    // print the operator first
    fprintf(quad, "%s ", tempOperator);
    // then the first token
    fprintf(quad, "%s ", tempToken);
    // then the second token
    fprintf(quad, "%s\n", tempToken2);

    // print the Jump condition
    if(strcmp(tempOperator, ">") == 0)
        fprintf(quad, "JLE ");
    else if(strcmp(tempOperator, ">=") == 0)
        fprintf(quad, "JL ");
    else if(strcmp(tempOperator, "<") == 0)
        fprintf(quad, "JGE ");
    else if(strcmp(tempOperator, "<=") == 0)
        fprintf(quad, "JG ");
    else if(strcmp(tempOperator, "==") == 0)
        fprintf(quad, "JNE ");
    else if(strcmp(tempOperator, "!=") == 0)
        fprintf(quad, "JE ");

    // generate the IF label
    char num[3], ifLabel[10] = "~IF";
    // convert topTempStack to string
    sprintf(num, "%d", ifLabelNum);
    ifLabelNum += 1;
    strcat(ifLabel, num);
    // push it to the stack
    push(stack, top, ifLabel);

    // put the temp to the symbol table
    FILE *symbolTable;
    symbolTable = fopen("symbol_table.txt", "a");
    fprintf(symbolTable, "%s 0 0 0 0\n", ifLabel);
    fclose(symbolTable);

    // write out the Jump quad
    fprintf(quad, "%s\n",ifLabel);

    // change operator to if the is no paren
    strcpy(operator, "null");
        
    // change temp back to null
    strcpy(temp, "null");
}

// gen quad for THEN
int genQuad_THEN(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top)
{
    // keep poping if we don't find any +, -, *, or /
    char tempToken[MAX_TOKEN_LENGTH], tempToken2[MAX_TOKEN_LENGTH];

    // pop we only pop 3 times from the stack
    // IF ifLabel THEN
    int popNum = 3;

    while(*top > 0 && popNum > 0)
    {
        pop(stack, top, tempToken);

        // we don't need the THEN and IF token
        if(strcmp(tempToken, "IF") != 0 && strcmp(tempToken, "THEN") != 0)
        {
            strcpy(tempToken2, tempToken);
        }
        popNum -= 1;
    }

    // print out only if the tempToken2 is the IF label
    if(strcmp(tempToken2, "") != 0)
        fprintf(quad, "%s NOP\n", tempToken2);

    return 0;
}

// IF case
int caseIF(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    // initialize the first token to be a delimiter
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null", lastOperator[10] = "null", conditionOperator[10];
    char tempClassification[MAX_TOKEN_LENGTH], assigmentToken[MAX_TOKEN_LENGTH], stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], operator[10] = "null";
    int top = 0, topTempStack = 0, hasOperator = 1, hasCondition = 0, hasNestedIF = 0, hasOperatorBeforeParen = 0, parenOrder = 0;
    *foundKeyWord = 0;

    // If the last token is < or = than the current token then push it to the stack
    // else pop the stack until >

    // check if there is something in token
    // if not move on the next line else process it
    // first time there will always the delimiter in the stack
    if(strcmp(token, "") != 0)
    {
        // we push the ident to the stack
        if(strcmp(classification, "<IF>") == 0)
        {
            // check if the var name format
            checkVar(token);
            // check the precedent table
            // if the last token is < then
            if(precedenceTable[22][4] == 1)
            {
                push(stack, &top, token);
                fprintf(quad, "%s\n", token);
            }
        }
        else
        {
            printf("Error at IF: Missing keyword IF\n");
        }
    }

    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {     
        // we push the ident to the stack
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            // if has operator then push the operator to the stack
            if(hasOperator)
            {
                if(strcmp(operator, "null") != 0)
                    push(stack, &top, operator);
                // push the num lit
                push(stack, &top, tempToken);
                hasOperator = 0;
            }
            else
            {
                //error 
                printf("Error at IF:: Missing operator\n");
                exit(1);
            }                
        }
        else if(strcmp(tempClassification, "<NumLit>") == 0)
        {
            // if has operator then push the operator to the stack
            if(hasOperator)
            {
                push(stack, &top, operator);
                // push the num lit
                push(stack, &top, tempToken);
                hasOperator = 0;
            }
            else
            {
                //error 
                printf("Error at IF:: Missing operator\n");
                exit(1);
            }
        }
        // if we found an operator then look it up at the precedence table
        else if(strcmp(tempClassification, "<$assign>") == 0 || strcmp(tempClassification, "<MULT>") == 0 || strcmp(tempClassification, "<ADD>") == 0 || strcmp(tempToken, "(") == 0 || strcmp(tempToken, ")") == 0
        || strcmp(tempToken, "<") == 0 || strcmp(tempToken, "<=") == 0 || strcmp(tempToken, ">") == 0 || strcmp(tempToken, ">=") == 0 || strcmp(tempToken, "!=") == 0 || strcmp(tempToken, "==") == 0)
        {
            hasOperator = 1;
            // if the current operator is null then we know this is the first operator in the line
            if(strcmp(operator, "null") == 0)
            {
                strcpy(operator, tempToken);
                if(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 
                || strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0)
                {
                    hasCondition = 1;
                }
            }
            // current operator is = go to precedentTable[21][]
            else if(strcmp(operator, "=") == 0)
            {
                printf("Error at IF: not allow = in condition for IF\n");
                exit(1);
            }
            // current operator is + go to precedentTable[17][]
            else if(strcmp(operator, "+") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[17][17] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[17][18] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[17][19] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[17][20] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[17][9] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[17][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is - go to precedentTable[18][]
            else if(strcmp(operator, "-") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[18][17] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[18][18] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[18][19] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[18][20] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[18][9] == 1)
                    {
                        // push the - to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[18][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }                
            }
            // current operator is * go to precedentTable[19][]
            else if(strcmp(operator, "*") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[19][17] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[19][18] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);                            
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[19][19] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[19][20] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[19][9] == 1)
                    {
                        // push the * to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }     
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[19][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                } 
            }
            // current operator is / go to precedentTable[20][]
            else if(strcmp(operator, "/") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[20][17] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[20][18] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[20][19] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[20][20] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[20][9] == 1)
                    {
                        // push the * to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }     
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[20][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is ( go to precedentTable[9][]
            else if(strcmp(operator, "(") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[9][17] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[9][18] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[9][19] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[9][20] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    // found a (
                    if(precedenceTable[9][9] == 1)
                    {
                        // push the - to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is ) go to precedentTable[10][]
            else if(strcmp(operator, ")") == 0)
            {
                parenOrder -= 1;
                // go look for the operator before the parenthesis that we have missed
                if(!parenOrder && hasOperatorBeforeParen)
                {
                    char restoreOperatorBeforeParen[MAX_TOKEN_LENGTH];
                    int tempTop = top;
                    // keep looking for the operator before the ()
                    while(1)
                    {
                        seek(stack, &tempTop, restoreOperatorBeforeParen);
                        if(strcmp(restoreOperatorBeforeParen, "*") == 0 || strcmp(restoreOperatorBeforeParen, "/") == 0)
                        {
                            strcpy(lastOperator, restoreOperatorBeforeParen);
                            checkOperatorPrecedence(quad, stack, &top, lastOperator);
                            break;
                        }
                        else if(strcmp(restoreOperatorBeforeParen, "+") == 0 || strcmp(restoreOperatorBeforeParen, "-") == 0)
                        {
                            strcpy(operator, restoreOperatorBeforeParen);
                            break;
                        }
                    }
                    hasOperatorBeforeParen = 0;
                }

                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[10][17] == 3)
                    {
                        
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[10][18] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[10][19] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[10][20] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[10][10] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // condition operator
            else if(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 
            || strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0)
            {
                push(stack, &top, operator);
                strcpy(conditionOperator, operator);
                strcpy(operator, tempToken);
                hasCondition = 1;
            }
        }
        // found the delimiter then generate the appropriate quad
        else if(strcmp(tempClassification, "<THEN>") == 0)
        {
            if(hasCondition)
            {
                genQuad_IF(quad, stack, &top, conditionOperator, temp);

                // push the THEN with the IF label
                push(stack, &top, tempToken);

                // write to the quad file the IF label
                fprintf(quad, "%s\n", tempToken);

                // go to THEN case
                caseTHEN(tokenList, quad, tempToken, classification, foundKeyWord, &top, stack, &hasNestedIF);

                if(hasNestedIF)
                    caseTHEN(tokenList, quad, tempToken, classification, foundKeyWord, &top, stack, &hasNestedIF);
                break;
            }
            else
            {
                printf("Error at IF: Missing condition\n");
                exit(1);
            }
        }
        // user forgot the Delimiter at the end
        else if(strcmp(tempClassification, "<CONST>") == 0 || strcmp(tempClassification, "<IF>") == 0 || strcmp(tempClassification, "<PROCEDURE>") == 0 ||
                strcmp(tempClassification, "<WHILE>") == 0 || strcmp(tempClassification, "<ODD>") == 0 || strcmp(tempClassification, "<CALL>") == 0 ||
                strcmp(tempClassification, "<GET>") == 0 || strcmp(tempClassification, "<PUT>") || strcmp(tempClassification, "<VAR>") == 0)
        {
            printf("Error at IF: Missing ;\n");
            exit(1);
        }
    }
    return 0;
}

// THEN case
int caseTHEN(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord, int *top, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *hasNestedIF)
{
    // initialize the first token to be a delimiter
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null";
    char tempClassification[MAX_TOKEN_LENGTH];
    int hasLeftBracket = 0, hasRightBracket = 0, noError;

    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        // look for left bracket
        if(strcmp(tempClassification, "<LeftBracket>") == 0)
        {
            hasLeftBracket = 1;
        }
        // look for right bracket
        else if(strcmp(tempClassification, "<RightBracket>") == 0)
        {
            hasRightBracket = 1;
            if(hasRightBracket)
                // pop THEN then generate the 
                genQuad_THEN(quad, stack, top);
            else if(hasLeftBracket && !hasRightBracket)
                printf("Error at IF: Missing Right Bracket\n");
            else if(!hasLeftBracket && hasRightBracket)
                printf("Error at IF: Missing Left Bracket\n");  
            break;  
        }
        // process nested IF
        else if(strcmp(tempClassification, "<IF>") == 0)
        {
            *hasNestedIF = 1;
            caseIF(tokenList, quad, tempToken, tempClassification, foundKeyWord);
            break;
        }
        // process the Math statement
        else if(strcmp(tempClassification, "<$var>") == 0)
        {
            caseMath(tokenList, quad, tempToken, tempClassification, foundKeyWord);
        }
        // process the PUT statement
        else if(strcmp(tempClassification, "<PUT>") == 0)
        {
            noError = casePUT(tokenList, quad, tempToken, foundKeyWord);
        }
        // process the GET statement
        else if(strcmp(tempClassification, "<GET>") == 0)
        {
            noError = caseGET(tokenList, quad, tempToken, foundKeyWord);
        }
        else if(strcmp(tempClassification, "<CALL>") == 0)
        {
            caseCALL(tokenList, quad, tempToken);
        }
        else if(strcmp(tempToken, ";") == 0)
        {
            // pop the last IF label from the stack
            genQuad_THEN(quad, stack, top);
            break;
        }
        else
        {
            printf("Error at IF: check your IF body\n");
            exit(1);
        }
    }  
}

// gen quad for WHILE
int genQuad_WHILE(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top, char operator[MAX_TOKEN_LENGTH], char temp[10])
{
    // keep poping if we don't find any +, -, *, or /
    char tempToken[MAX_TOKEN_LENGTH], tempToken2[MAX_TOKEN_LENGTH], tempOperator[MAX_TOKEN_LENGTH];

    // since we are doing binary operation, we need to put them in the correct order
    // stack | ,, | A | = | B
    // if we just pop it, it would be B = A 
    // so we need to keep track of some tokens 
    int tokenOrder = 1;

    // pop we only pop 3 times from the stack
    int popNum = 3;

    while(*top > 0 && popNum > 0)
    {
        pop(stack, top, tempToken);

        // check the token on the stack
        if (strcmp(tempToken, "<") == 0 || strcmp(tempToken, "<=") == 0 || strcmp(tempToken, ">") == 0 || strcmp(tempToken, ">=") == 0 
        || strcmp(tempToken, "!=") == 0 || strcmp(tempToken, "==") == 0)
            strcpy(tempOperator, tempToken);
        // the next token on the stack is the second token of the binary operation
        else if(tokenOrder)
        {
            strcpy(tempToken2, tempToken);
            tokenOrder = 0;
        }
        // if found a ( then do nothing
        if(strcmp(tempToken, "(") == 0 )
            continue;
        else
            popNum -= 1;
    }

    // print the operator first
    fprintf(quad, "%s ", tempOperator);
    // then the first token
    fprintf(quad, "%s ", tempToken);
    // then the second token
    fprintf(quad, "%s\n", tempToken2);

    // print the Jump condition
    if(strcmp(tempOperator, ">") == 0)
        fprintf(quad, "JLE ");
    else if(strcmp(tempOperator, ">=") == 0)
        fprintf(quad, "JL ");
    else if(strcmp(tempOperator, "<") == 0)
        fprintf(quad, "JGE ");
    else if(strcmp(tempOperator, "<=") == 0)
        fprintf(quad, "JG ");
    else if(strcmp(tempOperator, "==") == 0)
        fprintf(quad, "JNE ");
    else if(strcmp(tempOperator, "!=") == 0)
        fprintf(quad, "JE ");

    // generate the WHILE label
    char num[3], whileLabel[10] = "~W";
    // convert topTempStack to string
    sprintf(num, "%d", whileLabelNum);
    whileLabelNum += 1;
    strcat(whileLabel, num);
    // push it to the stack
    push(stack, top, whileLabel);

    // put the temp to the symbol table
    FILE *symbolTable;
    symbolTable = fopen("symbol_table.txt", "a");
    fprintf(symbolTable, "%s 0 0 0 0\n", whileLabel);

    // generate the WHILE exit label
    char num2[3], whileExitLabel[10] = "~L";
    // convert topTempStack to string
    sprintf(num2, "%d", whileExitLabelNum);
    whileExitLabelNum += 1;
    strcat(whileExitLabel, num2);
    // push it to the stack
    push(stack, top, whileExitLabel);

    // put the temp to the symbol table
    fprintf(symbolTable, "%s 0 0 0 0\n", whileExitLabel);
    fclose(symbolTable);

    // write out the Jump quad
    fprintf(quad, "%s\n",whileExitLabel);

    // change operator to if the is no paren
    strcpy(operator, "null");
        
    // change temp back to null
    strcpy(temp, "null");
}

// gen quad for DO
int genQuad_DO(FILE *quad, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *top)
{
    // keep poping if we don't find any +, -, *, or /
    char tempToken[MAX_TOKEN_LENGTH], tempToken2[MAX_TOKEN_LENGTH], tempToken3[MAX_TOKEN_LENGTH];

    // pop we only pop 3 times from the stack
    // IF ifLabel THEN
    int popNum = 4;

    while(*top > 0 && popNum > 0)
    {
        pop(stack, top, tempToken);

        // we don't need the WHILE and DO token
        if(popNum == 3)
        {
            strcpy(tempToken3, tempToken);
        }
        if(popNum == 2)
        {
            strcpy(tempToken2, tempToken);
        }

        popNum -= 1;
    }

    // print out only if the tempToken2 is the IF label
    if(strcmp(tempToken2, "") != 0)
        fprintf(quad, "jmp %s\n", tempToken2);
    if(strcmp(tempToken3, "") != 0)
        fprintf(quad, "%s NOP\n", tempToken3);

    return 0;
}

// WHILE case
int caseWHILE(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord)
{
    // initialize the first token to be a delimiter
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null", lastOperator[10] = "null", conditionOperator[10];
    char tempClassification[MAX_TOKEN_LENGTH], assigmentToken[MAX_TOKEN_LENGTH], stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], operator[10] = "null";
    int top = 0, topTempStack = 0, hasOperator = 1, hasCondition = 0, hasNestedWhile = 0, hasOperatorBeforeParen = 0, parenOrder = 0;;
    *foundKeyWord = 0;

    // If the last token is < or = than the current token then push it to the stack
    // else pop the stack until >

    // check if there is something in token
    // if not move on the next line else process it
    // first time there will always the delimiter in the stack
    if(strcmp(token, "") != 0)
    {
        // we push the ident to the stack
        if(strcmp(classification, "<WHILE>") == 0)
        {
            // check if the var name format
            checkVar(token);
            // check the precedent table
            // if the last token is < then
            if(precedenceTable[22][4] == 1)
            {
                push(stack, &top, token);
                fprintf(quad, "%s\n", token);
            }
        }
        else
        {
            printf("Error at WHILE: Missing keyword WHILE\n");
        }
    }

    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {     
        // we push the ident to the stack
        if(strcmp(tempClassification, "<$var>") == 0)
        {
            // if has operator then push the operator to the stack
            if(hasOperator)
            {
                if(strcmp(operator, "null") != 0)
                    push(stack, &top, operator);
                // push the num lit
                push(stack, &top, tempToken);
                hasOperator = 0;
            }
            else
            {
                //error 
                printf("Error at WHILE: Missing operator\n");
                exit(1);
            }                
        }
        else if(strcmp(tempClassification, "<NumLit>") == 0)
        {
            // if has operator then push the operator to the stack
            if(hasOperator)
            {
                push(stack, &top, operator);
                // push the num lit
                push(stack, &top, tempToken);
                hasOperator = 0;
            }
            else
            {
                //error 
                printf("Error at WHILE: Missing operator\n");
                exit(1);
            }
        }
        // if we found an operator then look it up at the precedence table
        else if(strcmp(tempClassification, "<$assign>") == 0 || strcmp(tempClassification, "<MULT>") == 0 || strcmp(tempClassification, "<ADD>") == 0 || strcmp(tempToken, "(") == 0 || strcmp(tempToken, ")") == 0
        || strcmp(tempToken, "<") == 0 || strcmp(tempToken, "<=") == 0 || strcmp(tempToken, ">") == 0 || strcmp(tempToken, ">=") == 0 || strcmp(tempToken, "!=") == 0 || strcmp(tempToken, "==") == 0)
        {
            hasOperator = 1;
            // if the current operator is null then we know this is the first operator in the line
            if(strcmp(operator, "null") == 0)
            {
                strcpy(operator, tempToken);
                if(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 
                || strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0)
                {
                    hasCondition = 1;
                }
            }
            // current operator is = go to precedentTable[21][]
            else if(strcmp(operator, "=") == 0)
            {
                printf("Error at WHILE: not allow = in condition for IF\n");
                exit(1);
            }
            // current operator is + go to precedentTable[17][]
            else if(strcmp(operator, "+") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[17][17] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[17][18] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[17][19] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[17][20] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[17][9] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[17][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is - go to precedentTable[18][]
            else if(strcmp(operator, "-") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[18][17] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[18][18] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[18][19] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[18][20] == 1)
                    {
                        // push the + to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[18][9] == 1)
                    {
                        // push the - to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[18][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }                
            }
            // current operator is * go to precedentTable[19][]
            else if(strcmp(operator, "*") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[19][17] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[19][18] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);                            
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[19][19] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[19][20] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[19][9] == 1)
                    {
                        // push the * to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }     
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[19][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                } 
            }
            // current operator is / go to precedentTable[20][]
            else if(strcmp(operator, "/") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[20][17] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[20][18] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // update the operator to the current operator 
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[20][19] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[20][20] == 3)
                    {
                        // generate the quad of the + then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    hasOperatorBeforeParen = 1;
                    // found a (
                    if(precedenceTable[20][9] == 1)
                    {
                        // push the * to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }     
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[20][10] == 3)
                    {
                        // generate the quad of the last operator then push the temporary to the stack
                        checkOperatorPrecedence(quad, stack, &top, operator);
                        // check if we have an operator before this
                        if(strcmp(lastOperator, "*") == 0 || strcmp(lastOperator, "/") == 0)
                        {
                            // make sure we don't lose the last operator
                            strcpy(operator, lastOperator);
                            // pop the stack of the last operator
                            checkOperatorPrecedence(quad, stack, &top, operator);
                            strcpy(operator, tempToken);
                        }
                        else
                            // update the operator to the current operator 
                            strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is ( go to precedentTable[9][]
            else if(strcmp(operator, "(") == 0)
            {
                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[9][17] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[9][18] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[9][19] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[9][20] == 1)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "(") == 0)
                {
                    parenOrder += 1;
                    // found a (
                    if(precedenceTable[9][9] == 1)
                    {
                        // push the - to the stack
                        push(stack, &top, operator);
                        // keep track of the last operator                    
                        strcpy(lastOperator, operator);
                        // move to the new operator
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // current operator is ) go to precedentTable[10][]
            else if(strcmp(operator, ")") == 0)
            {
                parenOrder -= 1;
                // go look for the operator before the parenthesis that we have missed
                if(!parenOrder && hasOperatorBeforeParen)
                {
                    char restoreOperatorBeforeParen[MAX_TOKEN_LENGTH];
                    int tempTop = top;
                    // keep looking for the operator before the ()
                    while(1)
                    {
                        seek(stack, &tempTop, restoreOperatorBeforeParen);
                        if(strcmp(restoreOperatorBeforeParen, "*") == 0 || strcmp(restoreOperatorBeforeParen, "/") == 0)
                        {
                            strcpy(lastOperator, restoreOperatorBeforeParen);
                            checkOperatorPrecedence(quad, stack, &top, lastOperator);
                            break;
                        }
                        else if(strcmp(restoreOperatorBeforeParen, "+") == 0 || strcmp(restoreOperatorBeforeParen, "-") == 0)
                        {
                            strcpy(operator, restoreOperatorBeforeParen);
                            break;
                        }
                    }
                    hasOperatorBeforeParen = 0;
                }

                if(strcmp(tempToken, "+") == 0)
                {
                    // found a +
                    if(precedenceTable[10][17] == 3)
                    {
                        
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "-") == 0)
                {
                    // found a -
                    if(precedenceTable[10][18] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "*") == 0)
                {
                    // found a *
                    if(precedenceTable[10][19] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, "/") == 0)
                {
                    // found a /
                    if(precedenceTable[10][20] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else if(strcmp(tempToken, ")") == 0)
                {
                    // found a )
                    if(precedenceTable[10][10] == 3)
                    {
                        strcpy(operator, tempToken);
                    }
                    else
                    {
                        printf("Error in parsing mathematical\n");
                        exit(1);
                    }
                }
                else
                {
                    printf("Error in parsing mathematical\n");
                    exit(1);
                }
            }
            // condition operator
            else if(strcmp(operator, ">") == 0 || strcmp(operator, ">=") == 0 || strcmp(operator, "<") == 0 || strcmp(operator, "<=") == 0 
            || strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0)
            {
                push(stack, &top, operator);
                strcpy(conditionOperator, operator);
                strcpy(operator, tempToken);
                hasCondition = 1;
            }
        }
        // found the delimiter then generate the appropriate quad
        else if(strcmp(tempClassification, "<DO>") == 0)
        {
            if(hasCondition)
            {
                genQuad_WHILE(quad, stack, &top, conditionOperator, temp);

                // push the THEN with the IF label
                push(stack, &top, tempToken);

                // write to the quad file the IF label
                fprintf(quad, "%s\n", tempToken);

                // go to THEN case
                caseDO(tokenList, quad, tempToken, classification, foundKeyWord, &top, stack, &hasNestedWhile);

                if(hasNestedWhile)
                    caseDO(tokenList, quad, tempToken, classification, foundKeyWord, &top, stack, &hasNestedWhile);
                break;
            }
            else
            {
                printf("Error at WHILE: Missing condition\n");
                exit(1);
            }
        }
        // user forgot the Delimiter at the end
        else if(strcmp(tempClassification, "<CONST>") == 0 || strcmp(tempClassification, "<IF>") == 0 || strcmp(tempClassification, "<PROCEDURE>") == 0 ||
                strcmp(tempClassification, "<WHILE>") == 0 || strcmp(tempClassification, "<ODD>") == 0 || strcmp(tempClassification, "<CALL>") == 0 ||
                strcmp(tempClassification, "<GET>") == 0 || strcmp(tempClassification, "<PUT>") || strcmp(tempClassification, "<VAR>") == 0)
        {
            printf("Error at WHILE: Missing ;\n");
            exit(1);
        }
    }
    return 0;
}

// DO case
int caseDO(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH], char classification[MAX_TOKEN_LENGTH], int *foundKeyWord, int *top, char stack[MAX_STACK_SIZE][MAX_TOKEN_LENGTH], int *hasNestedWhile)
{
    // initialize the first token to be a delimiter
    char tempToken[MAX_TOKEN_LENGTH], temp[10] = "null";
    char tempClassification[MAX_TOKEN_LENGTH];
    int hasLeftBracket = 0, hasRightBracket = 0, noError;
    while(fscanf(tokenList, "%s%s", &tempToken, &tempClassification) != EOF)
    {
        // look for left bracket
        if(strcmp(tempClassification, "<LeftBracket>") == 0)
        {
            hasLeftBracket = 1;
        }
        // look for right bracket
        else if(strcmp(tempClassification, "<RightBracket>") == 0)
        {
            hasRightBracket = 1;
            if(hasRightBracket)
                // pop THEN then generate the 
                genQuad_DO(quad, stack, top);
            else if(hasLeftBracket && !hasRightBracket)
                printf("Error at WHILE: Missing Right Bracket\n");
            else if(!hasLeftBracket && hasRightBracket)
                printf("Error at WHILE: Missing Left Bracket\n");  
            break;  
        }
        // process nested IF
        else if(strcmp(tempClassification, "<WHILE>") == 0)
        {
            *hasNestedWhile = 1;
            caseWHILE(tokenList, quad, tempToken, tempClassification, foundKeyWord);
            break;
        }
        // process the Math statement
        else if(strcmp(tempClassification, "<$var>") == 0)
        {
            caseMath(tokenList, quad, tempToken, tempClassification, foundKeyWord);
        }
        // process the PUT statement
        else if(strcmp(tempClassification, "<PUT>") == 0)
        {
            noError = casePUT(tokenList, quad, tempToken, foundKeyWord);
        }
        // process the GET statement
        else if(strcmp(tempClassification, "<GET>") == 0)
        {
            noError = caseGET(tokenList, quad, tempToken, foundKeyWord);
        }
        else if(strcmp(tempClassification, "<CALL>") == 0)
        {
            caseCALL(tokenList, quad, tempToken);
        }
        else if(strcmp(tempToken, ";") == 0)
        {
            // pop the last IF label from the stack
            genQuad_THEN(quad, stack, top);
            break;
        }
        else
        {
            printf("Error at WHILE: check your WHILE body\n");
            exit(1);
        }
    }   
}

// PROCEDURE case
int casePROCEDURE(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH])
{
    char tempToken[MAX_TOKEN_LENGTH], tempClassification[MAX_TOKEN_LENGTH];
    int foundName = 1;
    // print the PROCEDURE keyword to the quad file
    fprintf(quad, "%s\n", token);

    // Procedure name
    fscanf(tokenList, "%s%s", tempToken, tempClassification);
    if (strcmp(tempClassification, "<ProcedureName>") == 0)
    {
        fprintf(quad, "%s\n", tempToken);
        foundName = 0;
    }
    else if (foundName)
    {
        // dont see Procedure name then prompt error
        printf("Error: Missing Procedure Name\n");
        exit(1);
    }

    // {
    fscanf(tokenList, "%s%s", tempToken, tempClassification);
    if (strcmp(token, "{") == 0)
    {
        leftBracket = 0;    
    }
    else if(leftBracket)
    {
        printf("Error: Missing left bracket\n");
        exit(1);
    }
    return 0;
}

// CALL case
int caseCALL(FILE *tokenList, FILE *quad, char token[MAX_TOKEN_LENGTH])
{
    char tempToken[MAX_TOKEN_LENGTH], tempClassification[MAX_TOKEN_LENGTH];
    // print the CALL
    fprintf(quad, "%s\n", token);
    // find where we need to jump
    fscanf(tokenList, "%s%s", tempToken, tempClassification);
    fprintf(quad, "%s\n", tempToken);

    // look for the ;
    fscanf(tokenList, "%s%s", tempToken, tempClassification);
    if(strcmp(tempToken, ";") == 0)
    {
        // null body
    }
    else
    {
        printf("Error at CALL: Missing ;");
        exit(1);
    }
    return 0;
}
