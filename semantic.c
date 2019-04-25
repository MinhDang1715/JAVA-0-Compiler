#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 30

int genCode_GET(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_PUT(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_ADD(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_SUB(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_MUL(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_DIV(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_IF(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_WHILE(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_ASSIGN(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);
int genCode_PROCEDURE(FILE *quad, FILE *temp, char token[MAX_TOKEN_LENGTH]);
int genCode_CALL(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH]);

// just to keep track of IF and WHILE label order
int ifLabelNum = 0;
int whileLabelNum = 0;

int main()
{
    // the temp file is used to keep all the procedure
    FILE *symbolTable, *quad, *assembly, *temp;

    temp = fopen("temp.txt", "w");
    int hasProcedure = 0;

    char token[30] = "null", 
    classification[30] = "null", 
    value[30] = "null", 
    address[30] = "null", 
    segment[30] = "null";

    // skip the first line in the symbol table
    symbolTable = fopen("symbol_table.txt", "r");
    fscanf(symbolTable, "%s %s %s %s %s", token, classification, value, address, segment);

    // get to the second line and get the program's name
    fscanf(symbolTable, "%s %s %s %s %s", token, classification, value, address, segment);
    assembly = fopen(strcat(token, ".asm"), "w");

    // set up the basic according to Dr. Burris IONasmLinux32.asm
    fprintf(assembly, "sys_exit\tequ\t1\n");
    fprintf(assembly, "sys_read\tequ\t3\n");
    fprintf(assembly, "sys_write\tequ\t4\n");
    fprintf(assembly, "stdin\tequ\t0\t; default keyboard\n");
    fprintf(assembly, "stdout\tequ\t1\t; default terminal screen\n");
    fprintf(assembly, "stderr\tequ\t3\n\n");

    // section .data
    fprintf(assembly, "section .data\n");
    fprintf(assembly, "\tuserMsg\t\t\tdb\t\t\t'Enter an integer (less than 32765): '\n");
    fprintf(assembly, "\tlenUserMsg\t\t\tequ\t\t\t$-userMsg\n");
    fprintf(assembly, "\tdisplayMsg\t\t\tdb\t\t\t'Your input: '\n");
    fprintf(assembly, "\tlenDisplayMsg\t\t\tequ\t\t\t$-displayMsg\n");
    fprintf(assembly, "\tnewLine\t\t\tdb\t\t\t0xA\n");
    fprintf(assembly, "\tnewLineLen\t\t\tequ\t\t\t$-newLine\n");
    fprintf(assembly, "\tTen\t\t\tdw\t\t\t10\t\t\t; Used in converting to base ten\n");
    fprintf(assembly, "\tprintTempChar\t\t\tdb\t\t\t'Temp char = '\n");
    fprintf(assembly, "\tlenPrintTempChar\t\t\tequ\t\t\t$-printTempChar\n");
    fprintf(assembly, "\tresult\t\t\tdb\t\t\t'ans = '\n");
    fprintf(assembly, "\tresultValue\t\t\tdb\t\t\t'aaaaa'\n");
    fprintf(assembly, "\t\t\tdb\t\t\t0xA\t\t\t; line feed\n");
    fprintf(assembly, "\tresultEnd\t\t\tequ\t\t\t$-result\n");
    fprintf(assembly, "\tnum\t\t\ttimes 6 db\t\t\t'ABCDEF'\n");
    fprintf(assembly, "\tnumEnd\t\t\tequ\t\t\t'$-num'\n");

    // scan through the symbol table to put the CONST in section .data
    while(fscanf(symbolTable, "%s %s %s %s %s", token, classification, value, address, segment) != EOF)
    {
        // put the CONST in the section .data
        if(strcmp(classification, "<CONST>") == 0)
        {
            fprintf(assembly, "\t%s\t\t\tDW\t\t\t%s\n", token, value);
        }
        else if(strcmp(classification, "<NumLit>") == 0)
        {
            // create a lit# for the NumLit in the symbol table
            char temp[10] = "lit";
            strcat(temp, token);
            fprintf(assembly, "\t%s\t\t\tDW\t\t\t%s\n", temp, value);
        }
    }
    fclose(symbolTable);

    // section .bss
    fprintf(assembly, "section .bss\n");
    fprintf(assembly, "\ttempChar\t\t\tRESB\t\t\t1\t\t\t; tempspace for use by GetNextChar\n");
    fprintf(assembly, "\ttestChar\t\t\tRESB\t\t\t1\n");
    fprintf(assembly, "\treadInt\t\t\tRESB\t\t\t1\t\t\t; temporary storage GetAnInteger\n");
    fprintf(assembly, "\ttempInt\t\t\tRESB\t\t\t1\t\t\t; used in converting to base 10\n");

    // reopen symbol_table
    symbolTable = fopen("symbol_table.txt", "r");
    // scan through the symbol table to put the CONST in section .bss
    while(fscanf(symbolTable, "%s %s %s %s %s", token, classification, value, address, segment) != EOF)
    {
        // put the CONST in the section .data
        if(strcmp(classification, "<VAR>") == 0)
        {
            fprintf(assembly, "\t%s\t\t\tRESW\t\t\t1\n", token);
        }
        // only need to put in the ~T
        else if(token[0] == '~' && token[1] == 'T')
        {
            // we don't need the ~ since it will cause error in the asm 
            fprintf(assembly, "\t%c%c\t\t\tRESW\t\t\t1\n", token[1], token[2]);
        }
    }
    fclose(symbolTable);

    // section .text
    fprintf(assembly, "section .text\n");
    fprintf(assembly, "\tglobal main\n");
    fprintf(assembly, "main:\n");
    quad = fopen("quad.txt", "r");

    int hasIO = 0;
    // scan through the symbol table to put the CONST in section .bss
    // since we cannot know the exact how many character in a line we cannot use fscanf
    while(fscanf(quad, "%s", token) != EOF)
    {
        if(strcmp(token, "GET") == 0)
        {
            genCode_GET(quad, assembly, token);
            hasIO = 1;
        }
        else if(strcmp(token, "PUT") == 0)
        {
            genCode_PUT(quad, assembly, token);
            hasIO = 1;
        }
        else if(strcmp(token, "+") == 0)
        {
            genCode_ADD(quad, assembly, token);
        }
        else if(strcmp(token, "-") == 0)
        {
            genCode_SUB(quad, assembly, token);
        }
        else if(strcmp(token, "*") == 0)
        {
            genCode_MUL(quad, assembly, token);
        }
        else if(strcmp(token, "/") == 0)
        {
            genCode_DIV(quad, assembly, token);
        }
        else if(strcmp(token, "=") == 0)
        {
            genCode_ASSIGN(quad, assembly, token);
        }
        else if(strcmp(token, "IF") == 0)
        {
            genCode_IF(quad, assembly, token);
        }
        else if(strcmp(token, "WHILE") == 0)
        {
            genCode_WHILE(quad, assembly, token);
        }
        else if(strcmp(token, "CALL") == 0)
        {
            genCode_CALL(quad, assembly, token);
        }
        // to handle a procedure, we put the asm to a temp file then copy it at the end of the code when we had finished
        // process other code
        else if(strcmp(token, "PROCEDURE") == 0)
        {
            genCode_PROCEDURE(quad, temp, token);
            hasProcedure = 1;
        }
    }

    // finsh
    fprintf(assembly, "finish:\n");
    fprintf(assembly, "\tmov eax, sys_exit ;terminate, sys_exit = 1\n");
    fprintf(assembly, "\txor ebx, ebx ;successfully, zero in ebx indicates success\n");
    fprintf(assembly, "\tint 80h\n");
    fprintf(assembly, "; ENDP main\n\n");

    // put Dr. Burris IO code
    fprintf(assembly, "; PrintString PROC\n");
    fprintf(assembly, "PrintString:\n");
    fprintf(assembly, "\tpush ax ; Save registers;\n");
    fprintf(assembly, "\tpush dx\n");
    fprintf(assembly, "\t; subpgm:\n");
    fprintf(assembly, "\t; prompt user\n");
    fprintf(assembly, "\tmov eax, 4		;Linux print device register conventions\n");
    fprintf(assembly, "\tmov ebx, 1 ; print default output device\n");
    fprintf(assembly, "\tmov ecx, userMsg ; pointer to string\n");
    fprintf(assembly, "\tmov edx, lenUserMsg\n");
    fprintf(assembly, "\tint 80h ; interrupt 80 hex, call kernel\n");
    fprintf(assembly, "\tpop dx ;Restore registers.\n");
    fprintf(assembly, "\tpop ax\n");
    fprintf(assembly, "\tret\n");
    fprintf(assembly, "\t; PrintString ENDP\n\n");
    fprintf(assembly, "; GetAnInteger    PROC\n");
    fprintf(assembly, "GetAnInteger: ; Get an integer as a string\n");
    fprintf(assembly, "\t; get response\n");
    fprintf(assembly, "\tmov eax, 3 ; read\n");
    fprintf(assembly, "\tmov ebx, 2 ; device\n");
    fprintf(assembly, "\tmov ecx,num ; buffer address\n");
    fprintf(assembly, "\tmov edx,6 ; max characters\n");
    fprintf(assembly, "\tint 0x80\n");
    fprintf(assembly, "\t; print number ;works\n\n");
    fprintf(assembly, "ConvertStringToInteger:\n");
    fprintf(assembly, "\tmov ax, 0 ; hold integer\n");
    fprintf(assembly, "\tmov [readInt], ax ;initialize 16 bit number to zero\n");
    fprintf(assembly, "\tmov ecx, num ; pt - 1st or next digit of number as a string terminated by <lf>.\n");
    fprintf(assembly, "\tmov bx, 0\n");
    fprintf(assembly, "\tmov bl, byte [ecx] ;contains first or next digit\n");
    fprintf(assembly, "Next: sub bl, '0'	; convert character to number\n");
    fprintf(assembly, "\tmov ax, [readInt]\n");
    fprintf(assembly, "\tmov dx, 10\n");
    fprintf(assembly, "\tmul dx ; eax = eax * 10\n");
    fprintf(assembly, "\tadd ax, bx\n");
    fprintf(assembly, "\tmov [readInt], ax\n");
    fprintf(assembly, "\tmov bx, 0\n");
    fprintf(assembly, "\tadd ecx, 1 ;pt = pt + 1\n");
    fprintf(assembly, "\tmov bl, byte[ecx]\n");
    fprintf(assembly, "\tcmp bl, 0xA ;is it a <lf>\n");
    fprintf(assembly, "\tjne Next ; get next digit\n");
    fprintf(assembly, "\tret\n");
    fprintf(assembly, "\t; ENDP GetAnInteger\n\n");
    fprintf(assembly, "; ConvertIntegerToString PROC\n");
    fprintf(assembly, "ConvertIntegerToString:\n");
    fprintf(assembly, "\tmov ebx, resultValue + 4 ;Store the integer as a five digit char string at result for printing\n");
    fprintf(assembly, "ConvertLoop:\n");
    fprintf(assembly, "\tsub dx, dx ; repeatedly divide dx:ax by 10\n");
    fprintf(assembly, "\t; to obtain last digit of number\n");
    fprintf(assembly, "\tmov cx, 10  ; as the remainder in the DX\n");
    fprintf(assembly, "\t; register. Quotient in AX.\n");
    fprintf(assembly, "\tdiv cx\n");
    fprintf(assembly, "\tadd dl, '0' ; Add '0' to dl to\n");
    fprintf(assembly, "\t;convert from binary to character.\n");
    fprintf(assembly, "\tmov [ebx], dl\n");
    fprintf(assembly, "\tdec ebx\n");
    fprintf(assembly, "\tcmp ebx, resultValue\n");
    fprintf(assembly, "\tjge ConvertLoop\n");
    fprintf(assembly, "\tret\n");
    fprintf(assembly, "\t; ConvertIntegerToString  ENDP\n\n");

    // go get all the procedure from the temp if there are any procedure
    if(hasProcedure)
    {
        char buffer;
        temp = fopen("temp.txt", "r");
        // copy everything from the temp to the assembly file
        buffer = fgetc(temp);
        while(buffer != EOF)
        {
            fputc(buffer, assembly);
            buffer = fgetc(temp);
        }
    }

    fclose(quad);
    fclose(assembly);
    fclose(temp);
    return 0;
}

// this function is used to convert the <NumLit> to Lit...
int convertToLit(char token[MAX_TOKEN_LENGTH])
{
    char lit[MAX_TOKEN_LENGTH] = "lit";
    if(isdigit(token[0]))
    {
        strcat(lit, token);
        strcpy(token, lit);
    }
    return 0;
}

int genCode_GET(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);

    fprintf(assembly, "\tcall PrintString\n");
    fprintf(assembly, "\tcall GetAnInteger\n");
    fprintf(assembly, "\tmov ax, [readInt]\n");
    fprintf(assembly, "\tmov [%s], ax\n\n", temp);
    
    return 0;
}

int genCode_PUT(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);

	fprintf(assembly, "\tmov ax, [%s]\n", temp);
	fprintf(assembly, "\tcall ConvertIntegerToString\n");
	fprintf(assembly, "\tmov eax, 4\n");
    fprintf(assembly, "\tmov ebx, 1\n");
	fprintf(assembly, "\tmov ecx, result\n", temp);
    fprintf(assembly, "\tmov edx, resultEnd\n", temp);
	fprintf(assembly, "\tint 80h\n\n");

    return 0;
}

int genCode_ADD(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov ax, [%c%c]\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov ax, [%s]\n", temp);

    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tadd ax, [%c%c]\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tadd ax, [%s]\n", temp);
        
    // scan the next line to get the token;
    // the ~ in ~T0 is not allow in nasm so we need to get rid of it
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov [%c%c], ax\n\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov [%s], ax\n\n", temp);

    return 0;
}

int genCode_SUB(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov ax, [%c%c]\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov ax, [%s]\n", temp);

    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tsub ax, [%c%c]\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tsub ax, [%s]\n", temp);

    // scan the next line to get the token;
    // the ~ in ~T0 is not allow in nasm so we need to get rid of it
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov [%c%c], ax\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov [%s], ax\n\n", temp);
    
    return 0;
}

int genCode_MUL(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov ax, [%c%c]\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov ax, [%s]\n", temp);

    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
    {
        fprintf(assembly, "\tmov dx, [%c%c]\n", temp[1], temp[2]);
        fprintf(assembly, "\tmul dx\n");
    }
    else
    {
        fprintf(assembly, "\tmov dx, [%s]\n", temp);
        fprintf(assembly, "\tmul dx\n", temp);
    }

    // scan the next line to get the token;
    // the ~ in ~T0 is not allow in nasm so we need to get rid of it
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov [%c%c], ax\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov [%s], ax\n\n", temp);
    
    return 0;
}

int genCode_DIV(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    fprintf(assembly, "\tmov dx, 0\n", temp);
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov ax, [%c%c]\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov ax, [%s]\n", temp);

    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
    {
        fprintf(assembly, "\tmov bx, [%c%c]\n", temp[1], temp[2]);
        fprintf(assembly, "\tdiv bx\n");
    }
    else
    {
        fprintf(assembly, "\tmov bx, [%s]\n", temp);
        fprintf(assembly, "\tdiv bx\n", temp);
    }

    // scan the next line to get the token;
    // the ~ in ~T0 is not allow in nasm so we need to get rid of it
    fscanf(quad, "%s", temp);
    // check if it is a NumLit or not
    convertToLit(temp);
    // check if it is a temp
    if(temp[0] == '~')
        fprintf(assembly, "\tmov [%c%c], ax\n", temp[1], temp[2]);
    else
        fprintf(assembly, "\tmov [%s], ax\n\n", temp);
    
    return 0;
}

int genCode_ASSIGN(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    // = A B
    // mov ax, [B]
    // mov [A], ax

    char temp[MAX_TOKEN_LENGTH], temp2[MAX_TOKEN_LENGTH];
    // scan the next line to get the token;
    fscanf(quad, "%s", temp);
    fscanf(quad, "%s", temp2);
    // check if it is a NumLit or not
    convertToLit(temp2);
    if(temp2[0] == '~')
        fprintf(assembly, "\tmov ax, [%c%c]\n", temp2[1], temp2[2]);
    else
        fprintf(assembly, "\tmov ax, [%s]\n", temp2);

    fprintf(assembly, "\tmov [%s], ax\n\n", temp);

    return 0;
}

int genCode_IF(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    char ifLabel[] = "__IF", num[MAX_TOKEN_LENGTH];
    // we will process the IF with the order
    int order = 0;
    // convert ifLabelNum to string
    sprintf(num, "%d", ifLabelNum);
    ifLabelNum += 1;
    strcat(ifLabel, num);
    fprintf(assembly, "%s:\n", ifLabel);

    while(fscanf(quad, "%s", temp) != EOF)
    {
        if(strcmp(temp, "<") != 0 && strcmp(temp, "<=") != 0 && strcmp(temp, ">") != 0 && strcmp(temp, ">=") != 0 && strcmp(temp, "==") != 0 && strcmp(temp, "!=") != 0)
        {
            if(order == 0)
            {
                // check if it is a NumLit or not
                convertToLit(temp);
                // check if it is a temp
                if(temp[0] == '~')
                    fprintf(assembly, "\tmov ax, [%c%c]\n", temp[1], temp[2]);
                else
                    fprintf(assembly, "\tmov ax, [%s]\n", temp);
                order += 1;
            }
            else if(order == 1)
            {
                // check if it is a NumLit or not
                convertToLit(temp);
                // check if it is a temp
                if(temp[0] == '~')
                    fprintf(assembly, "\tcmp ax, [%c%c]\n", temp[1], temp[2]);
                else
                    fprintf(assembly, "\tcmp ax, [%s]\n", temp);
                order += 1;
            }
            else if(strcmp(temp, "JLE") == 0 || strcmp(temp, "JL") == 0 || strcmp(temp, "JGE") == 0 || strcmp(temp, "JG") == 0 || strcmp(temp, "JNE") == 0 || strcmp(temp, "JE") == 0)
            {
                fprintf(assembly, "\t%s ", temp);
                // scan to get the label we need
                fscanf(quad, "%s", temp);
                fprintf(assembly, "%c%c%c\n", temp[1], temp[2], temp[3]);
            }
            else if(strcmp(temp, "+") == 0)
            {
                genCode_ADD(quad, assembly, temp);
            }
            else if(strcmp(temp, "-") == 0)
            {
                genCode_SUB(quad, assembly, temp);
            }
            else if(strcmp(temp, "*") == 0)
            {
                genCode_MUL(quad, assembly, temp);
            }
            else if(strcmp(temp, "/") == 0)
            {
                genCode_DIV(quad, assembly, temp);
            }
            else if(strcmp(temp, "=") == 0)
            {
                genCode_ASSIGN(quad, assembly, temp);
            }
            else if(strcmp(temp, "PUT") == 0)
            {
                genCode_PUT(quad, assembly, temp);
            }
            else if(strcmp(temp, "GET") == 0)
            {
                genCode_GET(quad, assembly, temp);
            }
            // this is the case where we see a ~T0 that means we need to generate the empty label for IF
            else if(temp[0] == '~')
            {
                fprintf(assembly, "%c%c%c: NOP\n\n", temp[1], temp[2], temp[3]);
                break;
            }
            else if(strcmp(temp, "IF") == 0)
            {
                genCode_IF(quad, assembly, temp);
            }
            else if(strcmp(temp, "CALL") == 0)
            {
                fscanf(quad, "%s", temp);
                fprintf(assembly, "\tcall %s\n\n", temp);
            }
        }
    }
    return 0;
}

int genCode_WHILE(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char temp[MAX_TOKEN_LENGTH];
    char whileLabel[] = "__W", num[MAX_TOKEN_LENGTH];
    // we will process the IF with the order
    int order = 0;
    // convert whileLabelNum to string
    sprintf(num, "%d", whileLabelNum);
    whileLabelNum += 1;
    strcat(whileLabel, num);
    fprintf(assembly, "%s:\n", whileLabel);

    while(fscanf(quad, "%s", temp) != EOF)
    {
        if(strcmp(temp, "<") != 0 && strcmp(temp, "<=") != 0 && strcmp(temp, ">") != 0 && strcmp(temp, ">=") != 0 && strcmp(temp, "==") != 0 && strcmp(temp, "!=") != 0)
        {
            if(order == 0)
            {
                // check if it is a NumLit or not
                convertToLit(temp);
                // check if it is a temp
                if(temp[0] == '~')
                    fprintf(assembly, "\tmov ax, [%c%c]\n", temp[1], temp[2]);
                else
                    fprintf(assembly, "\tmov ax, [%s]\n", temp);
                order += 1;
            }
            else if(order == 1)
            {
                // check if it is a NumLit or not
                convertToLit(temp);
                // check if it is a temp
                if(temp[0] == '~')
                    fprintf(assembly, "\tcmp ax, [%c%c]\n", temp[1], temp[2]);
                else
                    fprintf(assembly, "\tcmp ax, [%s]\n", temp);
                order += 1;
            }
            else if(strcmp(temp, "JLE") == 0 || strcmp(temp, "JL") == 0 || strcmp(temp, "JGE") == 0 || strcmp(temp, "JG") == 0 || strcmp(temp, "JNE") == 0 || strcmp(temp, "JE") == 0)
            {
                fprintf(assembly, "\t%s ", temp);
                // scan to get the label we need
                fscanf(quad, "%s", temp);
                fprintf(assembly, "%c%c\n", temp[1], temp[2]);
            }
            else if(strcmp(temp, "+") == 0)
            {
                genCode_ADD(quad, assembly, temp);
            }
            else if(strcmp(temp, "-") == 0)
            {
                genCode_SUB(quad, assembly, temp);
            }
            else if(strcmp(temp, "*") == 0)
            {
                genCode_MUL(quad, assembly, temp);
            }
            else if(strcmp(temp, "/") == 0)
            {
                genCode_DIV(quad, assembly, temp);
            }
            else if(strcmp(temp, "=") == 0)
            {
                genCode_ASSIGN(quad, assembly, temp);
            }
            else if(strcmp(temp, "PUT") == 0)
            {
                genCode_PUT(quad, assembly, temp);
            }
            else if(strcmp(temp, "GET") == 0)
            {
                genCode_GET(quad, assembly, temp);
            }
            // this is the case where we see a ~T0 that means we need to generate the empty label for IF
            else if(temp[0] == '~')
            {
                fprintf(assembly, "%c%c: NOP\n\n", temp[1], temp[2]);
                break;
            }
            else if(strcmp(temp, "WHILE") == 0)
            {
                genCode_WHILE(quad, assembly, temp);
            }
            else if(strcmp(temp, "jmp") == 0)
            {
                // get the Label we need to jump to
                fscanf(quad, "%s", temp);
                fprintf(assembly, "\tjmp __%c%c\n\n", temp[1], temp[2]);
            }
            else if(strcmp(temp, "CALL") == 0)
            {
                fscanf(quad, "%s", temp);
                fprintf(assembly, "\tcall %s\n\n", temp);
            }
        }
    }
    return 0;
}

int genCode_PROCEDURE(FILE *quad, FILE *temp, char token[MAX_TOKEN_LENGTH])
{
    char tempToken[MAX_TOKEN_LENGTH];
    // print the procedure label
    fscanf(quad, "%s", token);
    fprintf(temp, "%s:\n", token);
    while(fscanf(quad, "%s", token) != EOF)
    {
        if(strcmp(token, "GET") == 0)
        {
            genCode_GET(quad, temp, token);
        }
        else if(strcmp(token, "PUT") == 0)
        {
            genCode_PUT(quad, temp, token);
        }
        else if(strcmp(token, "+") == 0)
        {
            genCode_ADD(quad, temp, token);
        }
        else if(strcmp(token, "-") == 0)
        {
            genCode_SUB(quad, temp, token);
        }
        else if(strcmp(token, "*") == 0)
        {
            genCode_MUL(quad, temp, token);
        }
        else if(strcmp(token, "/") == 0)
        {
            genCode_DIV(quad, temp, token);
        }
        else if(strcmp(token, "=") == 0)
        {
            genCode_ASSIGN(quad, temp, token);
        }
        else if(strcmp(token, "IF") == 0)
        {
            genCode_IF(quad, temp, token);
        }
        else if(strcmp(token, "WHILE") == 0)
        {
            genCode_WHILE(quad, temp, token);
        }
        else if(strcmp(token, "CALL") == 0)
        {
            fscanf(quad, "%s", token);
            fprintf(temp, "\tcall %s\n\n", token);
        }
        // stop when we find the ret
        else if(strcmp(token, "ret") == 0)
        {
            // print the ret
            fprintf(temp, "\tret\n\n", token);
            break;
        }
    }
    // close the temp file so we can open to read it 
    fclose(temp);
    return 0;
}

int genCode_CALL(FILE *quad, FILE *assembly, char token[MAX_TOKEN_LENGTH])
{
    char tempToken[MAX_TOKEN_LENGTH];
    // go get the procedure
    fscanf(quad, "%s", tempToken);
    fprintf(assembly, "\tcall %s\n\n", tempToken);
    return 0;
}