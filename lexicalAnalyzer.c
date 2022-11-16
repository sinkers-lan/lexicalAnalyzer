#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define KEYSIZE 33
#define BUFFERSIZE 1000
#define IDCAPACITY 1000

#define KEY 0
#define ID 1
#define REL 4
#define ASS 5
#define BIT 6
#define LOG 7
#define ARI 8
#define SPE 9
#define NOT 10
#define SYM 11
#define SEL 12
#define STR 13
#define CHR 14
#define U 15
#define UL 16
#define ULL 17
#define I 18
#define L 19
#define LL 20
#define F 21
#define D 22
#define LD 23

int token_num[24]; //存放各种记号单词的数量

union attributes {
    int inum;
    long lnum;
    long long llnum;
    float fnum;
    double dnum;
    long double ldnum;
    unsigned int unum;
    unsigned long ulnum;
    unsigned long long ullnum;
}; //存放二元组中的属性

struct tokens {
    int token;
    union attributes attribute; 
};
struct tokens binary; //存放当前单词二元组

char* signTable; //存放用户自定义字符的符号表
char* strTable; //存放字符串
int signPos; //符号表当前末尾位置
int str_signPos;  //字符串常量表当前末尾位置
char keyTable[KEYSIZE][10] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
                         "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register",
                         "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
                         "union", "unsigned", "void", "volatile"}; //存放关键字字符
int state; //当前状态指示
char C; //存放当前读入的字符
int isKey; //值为-1表示识别出的单词是用户自定义的标识符，否则，表示识别出的单词是关键字，其值为关键字的记号
char token[1000]; //存放当前正在识别的单词字符串
int pos; //token中的字符串尾部；
int forward; //向前指针
int line; //当前行数
int total; //不带换行和空格的字符总数
int total_ns; //带换行和空格的字符总数
char buffer[BUFFERSIZE * 2]; //输入缓冲区
FILE* fp; //文件指针
int re_flag; //向前指针回退时的标记，避免重复加载缓冲区

void get_char(); //根据向前指针forward的指示从输入缓冲区中读一个字符放入变量C中，然后移动forward，使之指向下一个字符
void get_nbc(void); //检查C中的字符是否为空格，若是，则反复调用get_char，直到C中进入一个非空字符为止
void cat(void); //把C中字符链接在token中的字符串后面
void retract(void); //向前指针forward后退一个字符，同时将token的最后一个字符删去
void combine(int tokens, long long attribute_i, long double attribute_f); //把单词的记号和属性组合成一个二元组
void error(int log); //对发现的错误进行相应的处理
int letter(void); //判断C中的字符是否为字母，若是则返回1，否则返回0
int digit(void); //判断C中的字符是否为数字，若是则返回1，否则返回0
int reserve(void); //根据token中的单词查关键字表，若token中的单词是关键字，则返回该关键字的记号，否则，返回值“-1”
void table_insert(void); //将识别出来的用户自定义标识符，即token中的单词，插入符号表，返回该单词在符号表中的位置指针
void str_table_insert(void); //将识别出来的字符串，即token中的单次，插入字符串常量表，返回该字符串在表中的位置指针
void token_print(void); //打印识别出来的记号
void buffer_fill(int start); //填充一半的缓冲区
void outcome_print(void); //打印统计结果


int letter(void) {
    return ((C>='a'&&C<='z')||(C>='A'&&C<='Z'))?1:0;
}

int digit(void) {
    return (C>='0'&&C<='9')?1:0;
}

void get_char() {
    C = buffer[forward];
    if (C == '\n') {
        line++;
    }
    forward++;
    if (forward == BUFFERSIZE && re_flag == 0) {
            buffer_fill(BUFFERSIZE);
    }
    else if (forward == BUFFERSIZE * 2 && re_flag == 0) {
            buffer_fill(0);
    }
    re_flag = 0;
    forward = (forward + BUFFERSIZE * 2) % (BUFFERSIZE * 2);
}

void get_nbc(void) {
    while (C == ' ' || C == '\n' || C == '\t') {
        get_char();
    }
}

void cat(void) {
    token[pos++] = C;
    token[pos] = '\0';
}

void retract(void) {
    re_flag = 1;
    forward = (forward + BUFFERSIZE * 2 - 1) % (BUFFERSIZE * 2);
    if (C == '\n') {
        line--;
    }
}

void combine(int tokens, long long attribute_i, long double attribute_f) {
    binary.token = tokens;
    if (binary.token == F) {
        binary.attribute.fnum = (float)attribute_f;
    } else if (binary.token == D) {
        binary.attribute.dnum = (double)attribute_f;
    } else if (binary.token == LD) {
        binary.attribute.ldnum = attribute_f;
    } else if (binary.token == I) {
        binary.attribute.inum = (int)attribute_i;
    } else if (binary.token == L) {
        binary.attribute.lnum = (long)attribute_i;
    } else if (binary.token == LL) {
        binary.attribute.llnum = attribute_i;
    } else if (binary.token == U) {
        binary.attribute.unum = (unsigned int)attribute_i;
    } else if (binary.token == UL) {
        binary.attribute.ulnum = (unsigned long)attribute_i;
    } else if (binary.token == ULL) {
        binary.attribute.ullnum = (unsigned long long)attribute_i;
    } else {
        binary.attribute.inum = (int)attribute_i;
    }
    
    token_print();
    pos = 0;
}

void token_print() {
    token_num[binary.token]++;
    printf("\t<%s, ", token);
    switch (binary.token)
    {
    case KEY:
        printf("KEY, %ld>\n", binary.attribute.inum);
        break;
    case ID:
        printf("ID, %ld>\n", binary.attribute.inum);
        break;
    case REL:
        printf("REL, %ld>\n", binary.attribute.inum);
        break;
    case ASS:
        printf("ASS, %ld>\n", binary.attribute.inum);
        break;
    case BIT:
        printf("BIT, %ld>\n", binary.attribute.inum);
        break;
    case LOG:
        printf("LOG, %ld>\n", binary.attribute.inum);
        break;
    case ARI:
        printf("ARI, %ld>\n", binary.attribute.inum);
        break;
    case SPE:
        printf("SPE, %ld>\n", binary.attribute.inum);
        break;
    case NOT:
        printf("NOT, %ld>\n", binary.attribute.inum);
        break;
    case SYM:
        printf("SYM, %ld>\n", binary.attribute.inum);
        break;
    case SEL:
        printf("SEL, %ld>\n", binary.attribute.inum);
        break;
    case STR:
        printf("STR, %s>\n", &strTable[binary.attribute.inum]);
        break;
    case CHR:
        printf("CHR, %s>\n", &strTable[binary.attribute.inum]);
        break; 
    case U:
        printf("U, %u>\n", binary.attribute.unum); break;
    case UL:
        printf("UL, %lu>\n", binary.attribute.ulnum); break;
    case ULL:
        printf("ULL, %llu>\n", binary.attribute.ullnum); break;
    case I:
        printf("I, %d>\n", binary.attribute.inum); break;
    case L:
        printf("L, %ld>\n", binary.attribute.lnum); break;
    case LL:
        printf("LL, %lld>\n", binary.attribute.llnum); break;
    case F:
        printf("F, %f>\n", binary.attribute.fnum); break;
    case D:
        printf("D, %lf>\n", binary.attribute.dnum); break;
    case LD:
        printf("LD, %llf>\n", binary.attribute.ldnum); break;
    default:
        break;
    }
}

int reserve(void) {
    token[pos++] = '\0';
    for (int i = 0; i < KEYSIZE; i++) {
        if (strcmp(keyTable[i], token) == 0) {
            return i;
        }
    }
    return -1;
}

//将当前token添加到符号表中，同时增加signPos的值。为了区分不同的ID，每两个ID之间添加一个\0。
void table_insert(void) {
    int i;
    //自动扩容
    char* copy;
    if (signPos == 0) {
        signTable = malloc(IDCAPACITY);
    }
    if (signPos + strlen(token) + 1 >= IDCAPACITY){
        copy = malloc(sizeof(signTable) + IDCAPACITY);
        for (i = 0; i<sizeof(signTable); i++) {
            copy[i] = signTable[i];
        }
        free(signTable);
        signTable = copy;
    }
    //若先执行了combine，此时虽然token的pos已经指向0，但是/0仍然在原处，可以顺利读取token
    for (i = 0; i < strlen(token); i++) {
        signTable[signPos++] = token[i];
    }
    signTable[signPos++] = '\0';

}

void str_table_insert(void) {
    int i;
    char* copy;
    if (str_signPos == 0) {
        strTable = malloc(IDCAPACITY);
    }
    if (str_signPos + strlen(token) + 1 >= IDCAPACITY){
        copy = malloc(sizeof(strTable) + IDCAPACITY);
        for (i = 0; i<sizeof(strTable); i++) {
            copy[i] = strTable[i];
        }
        free(strTable);
        strTable = copy;
    }

    for (i = 0; i < strlen(token); i++) {
        strTable[str_signPos++] = token[i];
    }
    strTable[str_signPos++] = '\0';
}

void error(int log) {
    int eline;
    if (C == '\n') {
        eline = line - 1;
    }
    else {
        eline = line;
    }
    switch (log) {
        case 1:
            printf("\t[Error] exponent has no digits. (line %d)\n", eline); break;
        case 2:
            printf("\t[Error] exponent has no digits. (line %d)\n", eline); break;
        case 3:
            printf("\t[Error] stray '%c' in program. (line %d)\n", C, eline); break;
        case 4:
            printf("\t[Error] invalid suffix \"b\" on integer constant. (line %d)\n", eline); break;
        case 5:
            printf("\t[Error] invalid suffix \"x\" on integer constant. (line %d)\n", eline); break;
    }
    pos = 0;
}

void buffer_fill(int start) {
    int i = 0;
    char c;
    while (i < BUFFERSIZE && (c = fgetc(fp)) != EOF) {
        buffer[start + i] = c;
        i++;
        total_ns++;
        if (c != ' ' && c != '\n') {
            total++;
        }
    }
    if (c == EOF) {
        buffer[start + i] = '\0';
    }
}

void lexicalAnalyzer(void) {
    state = 0;
    pos = 0;
    do {
        switch(state) {
            case 0: //初始状态
                get_char();
                get_nbc();
                if (letter() || C == '_') {
                    state = 1; //设置标识符状态
                }
                else if (digit() && C != '0') {
                    state = 2; 
                }
                else if (C == '.') {
                    state = 3;
                }
                else if (C == '0') {
                    state = 10;
                }
                else {
                    switch(C) {
                        case '<': state = 58; break;
                        case '>': state = 60; break;
                        case '=': state = 62; break;
                        case '!': state = 62; break;
                        case '+': state = 64; break;
                        case '-': state = 65; break;
                        case '&': state = 66; break;
                        case '|': state = 67; break;
                        case '*': state = 68; break;
                        case '/': state = 69; break;
                        case '%': state = 72; break;
                        case '^': state = 73; break;
                        case '{':
                            cat(); state = 0; combine(SYM, 0, 0); break;
                        case '}':
                            cat(); state = 0; combine(SYM, 1, 0); break;
                        case '(':
                            cat(); state = 0; combine(SPE, 0, 0); break;
                        case ')':
                            cat(); state = 0; combine(SPE, 1, 0); break;
                        case '[':
                            cat(); state = 0; combine(SPE, 2, 0); break;
                        case ']':
                            cat(); state = 0; combine(SPE, 3, 0); break;
                        case ':':
                            cat(); state = 0; combine(SYM, 2, 0); break;
                        case ';':
                            cat(); state = 0; combine(SYM, 5, 0); break;
                        case '?':
                            cat(); state = 0; combine(SEL, 0, 0); break;
                        case ',':
                            cat(); state = 0; combine(SYM, 6, 0); break;
                        case '#':
                            cat(); state = 0; combine(SYM, 7, 0); break;
                        case '~':
                            cat(); state = 0; combine(ASS, 11, 0); break;
                        case '\"':
                            cat(); state = 76; combine(SYM, 4, 0); break;//26
                        case '\'':
                            cat(); state = 77; combine(SYM, 3, 0); break;//27
                        default: state = 74; break;
                    }
                }
                break;

            case 1: //标识符状态
                cat();
                get_char();
                if (letter() || digit() || C == '_') {
                    state = 1;
                }
                else {
                    state = 0;
                    isKey = reserve();  //查关键字表
                    if (isKey != -1) {
                        combine(KEY, isKey, 0);  //是关键字
                    }
                    else {  //识别出用户自定义标识符
                        combine(ID, signPos, 0);
                        table_insert(); 
                    }
                    retract();
                }
                break;

            case 2:
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': state = 2; break;
                    case '.': state = 4; break;
                    case 'e':
                    case 'E': state = 5; break;
                    case 'l':
                    case 'L': state = 30; break;
                    case 'u':
                    case 'U': state = 34; break;
                    default: //识别出常整数
                        retract(); 
                        state = 0; 
                        combine(I, atoi(token), 0); 
                        break;
                }
                break;

            case 3: //小数点状态
                cat();
                get_char();
                if (digit()) {
                    state = 4;
                }
                else {
                    retract(); state = 0; combine(SPE, 5, 0);
                }
                break;

            case 4: //小数状态
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': state = 4; break;
                    case 'e':
                    case 'E': state = 5; break;
                    case 'l':
                    case 'L': state = 8; break;
                    case 'f':
                    case 'F': state = 9; break;
                    default: //识别出小数
                        retract(); 
                        state = 0; 
                        combine(D, 0, strtod(token ,NULL)); 
                        break;
                }
                break;

            case 5: //指数状态
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9': state = 7; break;
                    case '+':
                    case '-':state = 6; break;
                    default:
                        retract(); error(1); state = 0; break;
                }
                break;

            case 6:
                cat();
                get_char();
                if (digit()) {
                    state = 7;
                }
                else {
                    retract(); error(2); state = 0;
                }
                break;

            case 7:
                cat();
                get_char();
                if(digit()) {
                    state = 7;
                }
                else if (C == 'L' || C == 'l') {
                    state = 8;
                }
                else if (C == 'F' || C == 'f') {
                    state = 9;
                }
                else {
                    retract(); state = 0; combine(D, 0, strtod(token ,NULL));
                }
                break;

            case 8:
                cat();
                state = 0;
                combine(LD, 0, strtold(token ,NULL));
                break;

            case 9:
                cat();
                state = 0;
                combine(F, 0, atof(token));
                break;

            case 10:
                cat();
                get_char();
                switch(C) {
                    case '.': state = 4; break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7': state = 20; break;
                    case 'X':
                    case 'x': state = 11; break;
                    case 'B':
                    case 'b': state = 21; break;
                    default: state = 0; combine(I, 0, 0); break;
                }
                break;

            case 11:
                cat();
                get_char();
                if(digit()){
                    state = 12; break;
                }
                switch(C) {
                    case 'A': case 'B': case 'C': case 'D': case 'E': case'F':
                    case 'a': case 'b': case 'c': case 'd': case 'e': case'f':
                    state = 12; break;
                    default: retract(); state = 0; error(5); break;
                }
                break;

            case 12:
                cat();
                get_char();
                if(digit()){
                    state = 12; break;
                }
                switch(C) {
                    case 'A': case 'B': case 'C': case 'D': case 'E': case'F':
                    case 'a': case 'b': case 'c': case 'd': case 'e': case'f':
                    state = 12; break;
                    case 'p':
                    case 'P': state = 15; break;
                    case 'l':
                    case 'L': state = 40; break;
                    case 'u':
                    case 'U': state = 44; break;
                    default: 
                        retract(); 
                        state = 0; 
                        combine(I, strtol(token, NULL, 16), 0); 
                        break;
                }
                break;

            case 15:
                cat();
                get_char();
                if(digit()){
                    state = 17; break;
                }
                switch(C) {
                    case 'A': case 'B': case 'C': case 'D': case 'E': case'F':
                    case 'a': case 'b': case 'c': case 'd': case 'e': case'f':
                    state = 17; break;
                    case '+':
                    case '-': state = 16; break;
                    default:
                        retract(); error(1); state = 0; break;
                }
                break;

            case 16:
                cat();
                get_char();
                if (digit()) {
                    state = 17;
                }
                else {
                    retract(); error(2); state = 0;
                }
                break;

            case 17:
                cat();
                get_char();
                if(digit()){
                    state = 17; break;
                }
                switch(C) {
                    case 'A': case 'B': case 'C': case 'D': case 'E': case'F':
                    case 'a': case 'b': case 'c': case 'd': case 'e': case'f':
                    state = 17; break;
                    case 'L':
                    case 'l': state = 18; break;
                    default: retract(); state = 0; combine(D, 0, strtod(token, NULL));
                }
                break;

            case 18:
                cat();
                state = 0;
                combine(LD, 0, strtold(token, NULL));
                break;

            case 20:
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7': state = 20; break;
                    case 'L':
                    case 'l': state = 50; break;
                    case 'U':
                    case 'u': state = 54; break;
                    default: retract(); state = 0; combine(I, strtol(token, NULL, 8), 0); break;
                }
                break;

            case 21:
                cat();
                get_char();
                if (C == '0' || C == '1') {
                    state = 22;
                }
                else {
                    retract();
                    state = 0;
                    error(4);
                }
                break;

            case 22:
                cat();
                get_char();
                switch (C) {
                    case '0':
                    case '1': state = 22; break;
                    case 'U':
                    case 'u': state = 25; break;
                    case 'L':
                    case 'l': state = 23; break;
                    default:
                    retract();
                    state = 0;
                    combine(I, strtol(token, NULL, 2), 0);
                }
                break;

            case 23:
                cat();
                get_char();
                switch (C) {
                    case 'U':
                    case 'u': cat(); state = 0; combine(UL, strtoul(token, NULL, 2), 0); break;
                    case 'L':
                    case 'l': state = 24; break;
                    default: retract(); state = 0; combine(L, strtol(token, NULL, 2), 0); break;
                }
                break;

            case 24:
                cat();
                get_char();
                switch (C) {
                    case 'U':
                    case 'u': cat(); state = 0; combine(ULL, strtoull(token, NULL, 2), 0); break;
                    default: retract(); state = 0; combine(LL, strtoll(token, NULL, 2), 0); break;
                }
                break;

            case 25:
                cat();
                get_char();
                switch (C) {
                    case 'L':
                    case 'l': state = 26; break;
                    default: retract(); state = 0; combine(U, strtoul(token, NULL, 2), 0); break;
                }
                break;

            case 26:
                cat();
                get_char();
                switch (C) {
                    case 'L':
                    case 'l': cat(); state = 0; combine(ULL, strtoull(token, NULL, 2), 0); break;
                    default: retract(); state = 0; combine(U, strtoul(token, NULL, 2), 0); break;
                }
                break;

            case 30:
                cat();
                get_char();
                switch (C) {
                    case 'U':
                    case 'u': state = 31; break;
                    case 'L':
                    case 'l': state = 32; break;
                    default: retract(); state = 0; combine(L, strtol(token, NULL, 10), 0); break;
                }
                break;

            case 31:
                cat();
                state = 0;
                combine(UL, strtoul(token, NULL, 10), 0);
                break;

            case 32:
                cat();
                get_char();
                switch (C) {
                    case 'U':
                    case 'u': state = 33; break;
                    default: retract(); state = 0; combine(LL, strtoll(token, NULL, 10), 0); break;
                }
                break;

            case 33:
                cat();
                state = 0;
                combine (ULL, strtoull(token, NULL, 10), 0);
                break;

            case 34:
                cat();
                get_char();
                if (C == 'L' || C == 'l') {
                    state = 35;
                }
                else {
                    retract();
                    state = 0;
                    combine(U, strtoul(token, NULL, 10), 0);//
                }
                break;

            case 35:
                cat();
                get_char();
                if (C == 'L' || C == 'l') {
                    state = 36;
                }
                else {
                    retract();
                    state = 0;
                    combine(UL, strtoul(token, NULL, 10), 0);
                }
                break;
                
            case 36:
                cat();
                state = 0;
                combine(ULL, strtoull(token, NULL, 10), 0);
                break;

            case 40:
                cat();
                get_char();
                switch (C) {
                    case 'U':
                    case 'u': state = 41; break;
                    case 'L':
                    case 'l': state = 42; break;
                    default: retract(); state = 0; combine(L, strtol(token, NULL, 16), 0);
                    break;
                }
                break;

            case 41:
                cat();
                state = 0;
                combine(UL, strtoul(token, NULL, 16), 0);
                break;

            case 42:
                cat();
                get_char();
                if(C == 'U' || C == 'u') {
                    state = 43;
                }
                else {
                    retract();
                    state = 0;
                    combine(LL, strtoll(token, NULL, 16), 0);
                }
                break;

            case 43:
                cat();
                state = 0;
                combine(ULL, strtoull(token, NULL, 16), 0);
                break;

            case 44:
                cat();
                get_char();
                if(C == 'L' || C == 'l') {
                    state = 45;                }
                else{
                    retract();
                    state = 0;
                    combine(U, strtoul(token, NULL, 16), 0);
                }
                break;

            case 45:
                cat();
                get_char();
                if(C == 'L' || C == 'l') {
                    state = 46;
                }
                else{
                    retract();
                    state = 0;
                    combine(UL, strtoul(token, NULL, 16), 0);
                }
                break;

            case 46:
                cat();
                state = 0;
                combine(ULL, strtoull(token, NULL, 16), 0);
                break;

            case 50:
                cat();
                get_char();
                if(C == 'U' || C == 'u') {
                    state = 51;
                }
                else if(C == 'L' || C == 'l') {
                    state = 52;
                }
                else {
                    retract();
                    state = 0;
                    combine(L, strtol(token, NULL, 8), 0);
                }
                break;

            case 51:
                cat();
                state = 0;
                combine(UL, strtoul(token, NULL, 8), 0);
                break;

            case 52:
                cat();
                get_char();
                state = 0;
                if(C == 'U' || C == 'u'){
                    state = 53;
                }
                else {
                    retract();
                    state = 0;
                    combine(LL, strtoll(token, NULL, 8), 0);
                }
                break;

            case 53:
                cat();
                state = 0;
                combine(ULL, strtoull(token, NULL, 8), 0);
                break;

            case 54:
                cat();
                get_char();
                if(C == 'L' || C == 'l') {
                    state = 55;
                }
                else {
                    retract();
                    state = 0;
                    combine(U, strtoul(token, NULL, 8), 0);//
                }
                break;

            case 55:
                cat();
                get_char();
                if(C == 'L' || C == 'l') {
                    state = 56;
                }
                else {
                    retract();
                    state = 0;
                    combine(UL, strtoul(token, NULL, 8), 0);
                }
                break;

            case 56:
                cat();
                state = 0;
                combine(ULL, strtoull(token, NULL, 8), 0);
                break;

            case 58: // <
                cat();
                get_char();
                switch (C) {
                    case '=':
                        cat(); state = 0; combine(REL, 1, 0); break; // <=
                    case '<':
                        state = 59; break; // <<?
                    default:
                        retract(); state = 0; combine(REL, 0, 0); break; // <
                }
                break;

            case 59: // <<
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 10, 0); // <<=
                }
                else {
                    retract(); state = 0; combine(BIT, 4, 0); // <<
                }
                break;

            case 60: // >
                cat();
                get_char();
                switch (C) {
                    case '=':
                        cat(); state = 0; combine(REL, 3, 0); break; // >=
                    case '>':
                        state = 61; break; // >>
                    default:
                        retract(); state = 0; combine(REL, 2, 0); break; // >
                }
                break;

            case 61: // >>
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 9, 0); // >>=
                }
                else {
                    retract(); state = 0; combine(BIT, 5, 0); // >>
                }
                break;

            case 62: // =
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(REL, 4, 0); // ==
                }
                else {
                    retract(); state = 0; combine(ASS, 0, 0); // =
                }
                break;

            case 63: // !
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(REL, 5, 0); // !=
                }
                else {
                    retract(); state = 0; combine(LOG, 2, 0); // !
                }
                break;

            case 64: // +
                cat();
                get_char();
                switch (C) {
                    case '+':
                        cat(); state = 0; combine(ARI, 5, 0); break; // ++
                    case '=':
                        cat(); state = 0; combine(ASS, 1, 0); break; // +=
                    default:
                        retract(); state = 0; combine(ARI, 0, 0); break; // +
                }
                break;

            case 65: // -
                cat();
                get_char();
                switch (C) {
                    case '-':
                        cat(); state = 0; combine(ARI, 6, 0); break; // --
                    case '=':
                        cat(); state = 0; combine(ASS, 2, 0); break; // -=
                    case '>':
                        cat(); state = 0; combine(SPE, 4, 0);  break; // ->
                    default:
                        retract(); state = 0; combine(ARI, 1, 0); break; // -
                }
                break;

            case 66: // &
                cat();
                get_char();
                switch (C) {
                    case '&':
                        cat(); state = 0; combine(LOG, 0, 0); break;// &&
                    case '=':
                        cat(); state = 0; combine(ASS, 6, 0); break;// &=
                    default:
                        retract(); state = 0; combine(BIT, 0, 0); break;// &
                }
                break;

            case 67: // |
                cat();
                get_char();
                switch (C) {
                    case '|':
                        cat(); state = 0; combine(LOG, 1, 0); break;// ||
                    case '=':
                        cat(); state = 0; combine(ASS, 7, 0); break;// |=
                    default:
                        retract(); state = 0; combine(BIT, 1, 0); break;// |
                }
                break;

            case 68: // *
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 3, 0); // *=
                }
                else {
                    retract(); state = 0; combine(ARI, 2, 0); // *
                }
                break;

            case 69: // /
                cat();
                get_char();
                switch (C) {
                    case '=':
                        cat(); state = 0; combine(ASS, 4, 0); break;// /=
                    case '*':
                        cat(); state = 70; combine(NOT, 0, 0); break;// /*
                    case '/':
                        cat(); state = 71; combine(NOT, 1, 0); break;// //
                    default:
                        retract(); state = 0; combine(ARI, 3, 0); break;// /
                }
                break;

            case 70: // /*
                do{
                    get_char();
                    if (C == '*') {
                        state = 75;  // /* *
                    }
                } while (C != '*' && C != '\0');
                break;

            case 75: // /* *
                do{
                    get_char();
                    if (C == '/') {
                        state = 0;
                    } else if (C != '*') {
                        state = 70;
                    }
                } while (C == '*');  // /* */
                break;

            case 71: // //
                do {
                    get_char();
                } while (C != '\n' && C != '\0');
                if (C == '\n') {
                    state = 0;
                } 
                break;

            case 72: // %
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 5, 0); // %=
                }
                else {
                    retract(); state = 0; combine(ARI, 4, 0); // %
                }
                break;

            case 73: // ^
                cat();
                get_char();
                if (C == '=') {
                    cat(); state = 0; combine(ASS, 8, 0); // ^=
                }
                else {
                    retract(); state = 0; combine(BIT, 3, 0); // ^
                }
                break;

            case 74: //error
                error(3);
                state = 0;
                break;

            case 76: // "
                do {
                    get_char();
                    if (C != '\"') {
                        cat();
                    }
                } while (C != '\"' && C != '\0');
                int temp_pos = str_signPos;//为了解决combine函数中打印时，内容还没有被添加到表中的问题
                str_table_insert(); 
                combine(STR, temp_pos, 0);
                if (C == '\"') {
                    cat();
                    state = 0;
                    combine(SYM, 4, 0);
                }
                break;

            case 77: // '
                do {
                    get_char();
                    if (C != '\'')  cat();
                } while (C != '\'' && C != '\0');
                temp_pos = str_signPos;
                str_table_insert(); 
                combine(STR, temp_pos, 0);
                if (C == '\'') {
                    cat();
                    state = 0;
                    combine(SYM, 3, 0);
                }
                break;

            default: break;
        }
    } while (C != '\0');
}


void outcome_print(void) {
    printf("\n\n");
    printf("该程序共有 %d 行\n", line);
    printf("该程序的字符总数为 %d / %d\n", total, total_ns);
    printf("各种记号的的个数为：\n");
    printf("\tKEY:\t%d\n", token_num[0]);
    printf("\tID:\t\t%d\n", token_num[1]);
    printf("\tREL:\t%d\n", token_num[4]);
    printf("\tASS:\t%d\n", token_num[5]);
    printf("\tBIT:\t%d\n", token_num[6]);
    printf("\tLOG:\t%d\n", token_num[7]);
    printf("\tARI:\t%d\n", token_num[8]);
    printf("\tSPE:\t%d\n", token_num[9]);
    printf("\tNOT:\t%d\n", token_num[10]);
    printf("\tSYM:\t%d\n", token_num[11]);
    printf("\tSEL:\t%d\n", token_num[12]);
    printf("\tSTR:\t%d\n", token_num[13]);
    printf("\tCHR:\t%d\n", token_num[14]);
    printf("\tU  :\t%d\n", token_num[15]);
    printf("\tUL :\t%d\n", token_num[16]);
    printf("\tULL:\t%d\n", token_num[17]);
    printf("\tI  :\t%d\n", token_num[18]);
    printf("\tL  :\t%d\n", token_num[19]);
    printf("\tLL :\t%d\n", token_num[20]);
    printf("\tF  :\t%d\n", token_num[21]);
    printf("\tD  :\t%d\n", token_num[22]);
    printf("\tLD :\t%d\n", token_num[23]);
}


int main(void) {

    if ((fp = fopen("test.c", "r")) == NULL) { 
        printf("File doesn't exit.");
        return 0;
    }

    //初始化变量
    forward = 0;
    line = 1;
    total = 0;
    total_ns = 0;
    pos = 0;
    signPos = 0;
    str_signPos = 0;

    buffer_fill(0); //先读入左半边缓冲区
    
    lexicalAnalyzer(); //词法分析

    outcome_print(); //打印结果
    
    fclose(fp);
    return 0;
}
