#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Scanner.h"



enum tsymbol tnum[NO_KEYWORD] = {
    tconst,    telse,     tif,     tint,     treturn,   tvoid,     twhile,
	tchar, tdouble, tstring, tfor, tswitch, tcase, tdefault, tcontinue, tbreak
};

char* tokenName[] = {
    "!",        "!=",      "%",       "%=",     "%ident",   "%number",
    /* 0          1           2         3          4          5        */
    "&&",       "(",       ")",       "*",      "*=",       "+",
    /* 6          7           8         9         10         11        */
    "++",       "+=",      ",",       "-",      "--",	    "-=",
    /* 12         13         14        15         16         17        */
    "/",        "/=",      ";",       "<",      "<=",       "=",
    /* 18         19         20        21         22         23        */
    "==",       ">",       ">=",      "[",      "]",        "eof",
    /* 24         25         26        27         28         29        */
    //   ...........    word symbols ................................. //
    /* 30         31         32        33         34         35        */
    "const",    "else",     "if",      "int",     "return",  "void",
    /* 36         37         38        39                              */
    "while",    "{",        "||",       "}",
	/* 40         41         42        43         44         45        */
	"char", "double", "string", "for", "switch", "case",
	/* 46         47         48        49         50         51        */
	"default", "continue", "break", "&", "%char",
	/* 52         53                                                   */
	"%string", "%real_num"
};

char* keyword[NO_KEYWORD] = {
    "const",  "else",    "if",    "int",    "return",  "void",    "while"
	"char", "double", "string", "for", "switch", "case", "default", "continue", "break"
};

struct tokenType scanner(FILE *sourceFile)
{
	struct tokenType token;
    int i, index = 0;
    char ch, id[ID_LENGTH];

    token.number = tnull;
	
    do {
		while (isspace(ch = fgetc(sourceFile)));	// state 1: skip blanks
        if (superLetter(ch)) { // identifier or keyword 알파벳 혹은 "_"인지 확인
            i = 0;
            do {
                if (i < ID_LENGTH) id[i++] = ch;
                ch = fgetc(sourceFile);
            } while (superLetterOrDigit(ch));
            if (i >= ID_LENGTH) lexicalError(1);
            id[i] = '\0';
            ungetc(ch, sourceFile);  //  retract
            // find the identifier in the keyword table
			/*for (index = 0; index < NO_KEYWORD; index++)
				if (!strcmp(id, keyword[index])) break;*/
			//위에가 원래 방법, 아래가 오류로인해 수정한 방법
			if (!strcmp(id, "const")) index = 0;
			else if(!strcmp(id,"else"))index = 1;
			else if(!strcmp(id , "if"))index = 2;
			else if (!strcmp(id, "int"))index = 3;
			else if (!strcmp(id, "return"))index = 4;
			else if (!strcmp(id, "void"))index = 5;
			else if (!strcmp(id, "while"))index = 6;
			else if (!strcmp(id, "char"))index = 7;
			else if (!strcmp(id, "double"))index = 8;
			else if (!strcmp(id, "string"))index = 9;
			else if (!strcmp(id, "for"))index = 10;
			else if (!strcmp(id, "switch"))index = 11;
			else if (!strcmp(id, "case"))index = 12;
			else if (!strcmp(id, "default"))index = 13;
			else if (!strcmp(id, "continue"))index = 14;
			else if (!strcmp(id, "break"))index = 15;
			else index = 16;

            if (index < NO_KEYWORD)    // found, keyword exit
                token.number = tnum[index];
            else {                     // not found, identifier exit
                token.number = tident;
                strcpy_s(token.value.id, ID_LENGTH, id);
            }
        }  // end of identifier or keyword
        else if (isdigit(ch)) {  // number

			token.value.num = getNumber(sourceFile, ch);
			ch = fgetc(sourceFile);

			if (ch != '.') {
				token.number = tnumber;
				ungetc(ch, sourceFile);
			}//정수로만 되어있을 때 tnumber로 지정
			else {
				token.number = tsreal_num;
				ch = fgetc(sourceFile); 
				if (isdigit(ch)) {
					token.value.real = token.value.num + GetReal_Num(getNumber(sourceFile, ch));
					//정확한 실수의 값 value에 표시.
					ch = fgetc(sourceFile);
					if (ch == 'e') {
						ch = fgetc(sourceFile); 
						if (isdigit(ch)) {
							index = getNumber(sourceFile, ch);
							for (i = 0; i < index; i++)
								token.value.real *= 10;
						}
						else if (ch == '+') {
							ch = fgetc(sourceFile);
							index = getNumber(sourceFile, ch);
							for (i = 0; i < index; i++)
								token.value.real *= 10;
						}
						else lexicalError(8);						
					}
					else {
						ungetc(ch, sourceFile);
					}
				}
				else lexicalError(7);
			}

        }
		else {

			switch (ch) {  // special character
			case '/':
				ch = fgetc(sourceFile);
				if (ch == '*') {
					ch = fgetc(sourceFile);
					if (ch == '*') {			// text comment(주석)
						printf("Documented Comments -----> ", ch);
						do {
							while (ch != '*') { 
								ch = fgetc(sourceFile);
								printf("%c", ch);
							};
							ch = fgetc(sourceFile);
						} while (ch != '/');
					}
					else {
						do {
							while (ch != '*') { 
								ch = fgetc(sourceFile); 
							}
							ch = fgetc(sourceFile);
						} while (ch != '/');
					}
				}
				else if (ch == '/') {		// line comment
					while (fgetc(sourceFile) != '\n');
				}
				else if (ch == '=')  token.number = tdivAssign;
				else {
					token.number = tdiv;
					ungetc(ch, sourceFile); // retract
				}
				break;
			case '!':
				ch = fgetc(sourceFile);
				if (ch == '=')  token.number = tnotequ;
				else {
					token.number = tnot;
					ungetc(ch, sourceFile); // retract
				}
				break;
			case '%':
				ch = fgetc(sourceFile);
				if (ch == '=') {
					token.number = tremAssign;
				}
				else {
					token.number = tremainder;
					ungetc(ch, sourceFile);
				}
				break;
			case '&':
				ch = fgetc(sourceFile);
				if (ch == '&')  token.number = tand;
				else {
					//lexicalError(2);
					token.number = tampersand;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '*':
				ch = fgetc(sourceFile);
				if (ch == '=')  token.number = tmulAssign;
				else {
					token.number = tmul;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '+':
				ch = fgetc(sourceFile);
				if (ch == '+')  token.number = tinc;
				else if (ch == '=') token.number = taddAssign;
				else {
					token.number = tplus;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '-':
				ch = fgetc(sourceFile);
				if (ch == '-')  token.number = tdec;
				else if (ch == '=') token.number = tsubAssign;
				else {
					token.number = tminus;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '<':
				ch = fgetc(sourceFile);
				if (ch == '=') token.number = tlesse;
				else {
					token.number = tless;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '=':
				ch = fgetc(sourceFile);
				if (ch == '=')  token.number = tequal;
				else {
					token.number = tassign;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '>':
				ch = fgetc(sourceFile);
				if (ch == '=') token.number = tgreate;
				else {
					token.number = tgreat;
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '|':
				ch = fgetc(sourceFile);
				if (ch == '|')  token.number = tor;
				else {
					lexicalError(3);
					ungetc(ch, sourceFile);  // retract
				}
				break;
			case '(': token.number = tlparen;         break;
			case ')': token.number = trparen;         break;
			case ',': token.number = tcomma;          break;
			case ';': token.number = tsemicolon;      break;
			case '[': token.number = tlbracket;       break;
			case ']': token.number = trbracket;       break;
			case '{': token.number = tlbrace;         break;
			case '}': token.number = trbrace;         break;
			case '\'':
				ch = fgetc(sourceFile); 
				if (ch == '\\') {
					ch = fgetc(sourceFile);
					token.value.id[0] = '\\';
					token.value.id[1] = ch;
					token.value.id[2] = '\0';
					ch = fgetc(sourceFile);
					if (ch == '\'') {
						token.number = tschar;
					}
					else {
						lexicalError(5);
					}
				}//문자 리터럴
				else {
					token.value.id[0] = ch;
					token.value.id[1] = '\0';
					ch = fgetc(sourceFile); 
					if (ch == '\'') {
						token.number = tschar;
					}
					else {
						lexicalError(5);
					}
				}//문자 리터럴
				break;
			case '\"':
				i = 0;
				ch = fgetc(sourceFile);
				while (ch != '\"') {
					if (i < ID_LENGTH) { id[i++] = ch; }
					ch = fgetc(sourceFile);
				}
				if (i >= ID_LENGTH) { lexicalError(1); }
				id[i] = '\0';
				if (ch == '\"') {
					token.number = tstring;
					strcpy(token.value.id, id);
				}//문자열
				else lexicalError(6);
				break;
			case EOF: 
				token.number = teof;            
				break;
			default: {
				printf("Current character : %c", ch);
				lexicalError(4);
				break;
			}
			}
        } // switch end
    } while (token.number == tnull);
    return token;
} // end of scanner

void lexicalError(int n)
{
    printf(" *** Lexical Error : ");
    switch (n) {
    case 1: printf("an identifier length must be less than 12.\n");
        break;
    case 2: printf("next character must be &\n");
        break;
    case 3: printf("next character must be |\n");
        break;
    case 4: printf("invalid character\n");
        break;
	case 5: printf("next character must be \'\n");
		break;
	case 6: printf("next character must be \"\n");
		break;
	case 7: printf("next character must be integer\n");
		break;
	case 8: printf("next character must be integer or '+', '-'\n");
		break;
    }
}

int superLetter(char ch)
{
    if (isalpha(ch) || ch == '_') return 1;
    else return 0;
}

int superLetterOrDigit(char ch)
{
    if (isalnum(ch) || ch == '_') return 1;
    else return 0;
}

double GetReal_Num(int num) {
	int k = 1;
	while (num%k != 0) {
		k *= 10;
	}
	return (double)num / (double)k;
}//실수를 만들기 위한 함수

int getNumber(FILE *sourceFile, char firstCharacter)
{
    int num = 0;
    int value;
    char ch;

    if (firstCharacter == '0') {
        ch = fgetc(sourceFile);
        if ((ch == 'X') || (ch == 'x')) {		// hexa decimal
			while ((value = hexValue(ch = fgetc(sourceFile))) != -1) {
				num = 16 * num + value;
			}
        }
        else if ((ch >= '0') && (ch <= '7'))	// octal
            do {
                num = 8 * num + (int)(ch - '0');
                ch = fgetc(sourceFile);
            } while ((ch >= '0') && (ch <= '7'));
        else num = 0;						// zero
    }
    else {									// decimal
        ch = firstCharacter;
        do {
            num = 10 * num + (int)(ch - '0');
            ch = fgetc(sourceFile);
        } while (isdigit(ch));
    }
    ungetc(ch, sourceFile);  /*  retract  */
    return num;
}

int hexValue(char ch)
{
    switch (ch) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return (ch - '0');
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        return (ch - 'A' + 10);
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        return (ch - 'a' + 10);
    default: return -1;
    }
}

void writeToken(struct tokenType token, FILE *outputFile)
{
	if (tconst <= token.number && token.number <= twhile) {
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
	}
	else if (tchar <= token.number && token.number <= tcase) {
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
	}
	else if (token.number == tident) {
		fprintf(outputFile, "Token %10s ( %3d, %12s )\n", tokenName[token.number], token.number, token.value.id);
	}
	else if (token.number == tnumber) {
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.num);
	}
	else if (token.number == tschar) {	
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.id);
	}
	else if (token.number == tsstring) {	
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, token.value.id);
	}
	else if (token.number == tsreal_num) {	
		fprintf(outputFile, "Token %10s ( %3d, %12.3lf )\n", tokenName[token.number], token.number, token.value.real);
	}
	else if (tnot <= token.number && token.number < teof) {	
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
	}
	else if (tlbrace <= token.number && token.number <= trbrace) {
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);
	}
	else if (token.number == teof) {
		fprintf(outputFile, "Token %10s ( %3d, %12d )\n", tokenName[token.number], token.number, 0);

	}
}