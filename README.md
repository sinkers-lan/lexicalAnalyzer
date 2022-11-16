# lexicalAnalyzer

## C语言词法分析实验报告

### 实验题目

词法分析程序的设计与实现

### 实验要求

1. 选定源语言，比如：C、Pascal、Python、Java 等，任何一种语言均可；
2. 可以识别出用源语言编写的源程序中的每个单词符号，并以记号的形式输出每个单词符号。
3. 可以识别并跳过源程序中的注释。
4. 可以统计源程序中的语句行数、各类单词的个数、以及字符总数，并输出统计结果。
5. 检查源程序中存在的词法错误，并报告错误所在的位置。
6. 对源程序中出现的错误进行适当的恢复，使词法分析可以继续进行，对源程序进行一次扫描，即可检查并报告源程序中存在的所有词法错误。

采用 C/C++作为实现语言，手工编写词法分析程序。

### 程序设计说明

#### 语言说明

C语言定义有以下记号及单词：

1. 标识符：C语言中的标识符只能由字母、数字和下划线三种字符组成，且第一个字符必须是字母或下划线；

2. 保留字：标识符集合的子集。C语言共有33个关键字（不包含C11新增的关键字）：

   ```c
   auto, break, case, char, const, continue, default, do, double, else, enum, extern, float, for, goto, if, inline, int, long, register, restrict, return, short, signed, sizeof, static, struct, switch, typedef, union, unsigned, void, volatile
   ```

3. 整数常量：

   整数常量可以是十进制、八进制或十六进制的常量。前缀指定基数：0x 或 0X 表示十六进制，0 表示八进制，不带前缀则默认表示十进制。

   整数常量也可以带一个后缀，后缀是 U 和 L 的组合，U 表示无符号整数（unsigned），L 表示长整数（long）。后缀可以是大写，也可以是小写，U 和 L 的顺序任意。

4. 浮点常量：

   浮点常量由整数部分、小数点、小数部分和指数部分组成。您可以使用小数形式或者指数形式来表示浮点常量。

   当使用小数形式表示时，必须包含整数部分、小数部分，或同时包含两者。当使用指数形式表示时， 必须包含小数点、指数，或同时包含两者。带符号的指数是用 e 或 E 引入的。

   浮点常量也可以带一个后缀，后缀是L或者F，L代表double，F代表float。后缀可以是大写，也可以是小写。

5. 运算符：

   1. 关系运算符6种：`<`, `<=`, `>`, `>=`, `==`, `!=`；
   2. 算术运算符7种：`+`, `-`, `*`, `/`, `%`, `++`, `--`；
   3. 逻辑运算符3种：`&&`, `||`, `!`；
   4. 位操作运算符6种：`&`, `|`, `~`, `^`, `<<`, `>>`；
   5. 赋值运算符11种：`=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `>>=`, `<<=`；
   6. 条件运算符1种：`?:`；
   7. 逗号运算符1种：`,`；
   8. 指针运算符2种：`*`, `&`；
   9. 特殊运算符4种：`()`, `[]`, `->`, `.`；

6. 标点符号：`{`, `}`, `'`, `"`, `;`, `,`, `#`；

7. 注释标记：

   1. 以`/*`开始，以`*/`结束；
   2. 以`//`开始，以换行符结束；

8. 单词符号间的分隔符：空格。

#### 记号的正规文法

1. 标识符的文法

   *id* → letter *rid* | _ *rid*

   *rid* → ε | letter *rid* | _ *rid* | digit *rid*

   letter → A | B | ... | Z | a | b | ... | z

   digit → 0 | 1 | 2 | ... | 9

2. 整数的文法

   *digits* → digit *remainder*
   *remainder* → ε | digit *remainder*

3. 无符号数的文法

   *num* → *digit num*1 

   *num*1 → *digit num*1 ∣ . *num*2 ∣ E *num*4 ∣ *ε*

   *num2* → digit *num3* 

   *num3* → digit *num3* | E *num4* | ε

   *num*4 → + *digits* ∣ − *digits* ∣ digit *num*5

   *digits* → digit *num*5 

   *num*5 → digit *num*5 ∣ ε

4. 关系运算符的正规文法

   *relop* → < | < *equal* | = *equal* | > | > *equal* | ! *equal*

   *greater* → >

   *equal* → =

5. 算数运算符的正规文法

   *ariop* → + ∣ − ∣ ∗ ∣ / ∣ % ∣ + *plus* ∣ − *minus*

   plus → + 

   *minus* → −

6. 逻辑运算符的正规文法

   *logop* → & *and* ∣ ∣ *or* ∣ !

   *and* → & 

   *or* → ∣

7. 位操作符的正规文法

   *bitop* → & ∣ ∣ ∣ ∼ ∣ ^ ∣ < *left* ∣ > *right*

   *left* → < 

   *right* → >

8. 赋值运算符的正规文法

   *assop* → = | +*equal* ∣ −*equal* ∣ ∗*equal* ∣ /*equal* ∣ %*equal* ∣ &*equal* ∣ ∣*equal* ∣ ^*equal* ∣ > *right* ∣ < *left*

   *right* → > *equal*

   *left* → < *equal*

   *equal* → =

9. 条件运算符的文法：

   *conop* → ?

10. 逗号运算符的文法：

    *comop* → ,

11. 指针运算符的文法：

    *ponop* → ∗ ∣ &

12. 特殊运算符的文法：

    *speop* → ( ∣ ) ∣ [ ∣ ] ∣ . ∣ − *right*

    *right* → >

13. 标点符号的文法：

    *symbol* → { ∣ } ∣ : ∣ ′ ∣ " ∣ ; ∣ , ∣ #

14. 注释头符号的文法：

    *note* → / *star* ∣ / *slash*

    *star* → ∗ 

    *slash* → /

#### 状态转换图

![10.9.13.00](https://sinkers-pic.oss-cn-beijing.aliyuncs.com/img/10.9.13.00.svg)

#### 输出形式

| 正则表达式             | 记号 | 属性                 |
| ---------------------- | ---- | -------------------- |
| auto                   | KEY  | 0                    |
| break                  | KEY  | 1                    |
| case                   | KEY  | 2                    |
| char                   | KEY  | 3                    |
| const                  | KEY  | 4                    |
| continue               | KEY  | 5                    |
| default                | KEY  | 6                    |
| do                     | KEY  | 7                    |
| double                 | KEY  | 8                    |
| else                   | KEY  | 9                    |
| enum                   | KEY  | 10                   |
| extern                 | KEY  | 11                   |
| float                  | KEY  | 12                   |
| for                    | KEY  | 13                   |
| goto                   | KEY  | 14                   |
| if                     | KEY  | 15                   |
| inline                 | KEY  | 16                   |
| int                    | KEY  | 17                   |
| long                   | KEY  | 18                   |
| register               | KEY  | 19                   |
| restrict               | KEY  | 20                   |
| return                 | KEY  | 21                   |
| short                  | KEY  | 22                   |
| signed                 | KEY  | 23                   |
| sizeof                 | KEY  | 24                   |
| static                 | KEY  | 25                   |
| struct                 | KEY  | 26                   |
| switch                 | KEY  | 27                   |
| typedef                | KEY  | 28                   |
| union                  | KEY  | 29                   |
| unsigned               | KEY  | 30                   |
| void                   | KEY  | 31                   |
| volatile               | KEY  | 32                   |
| 整形int                | I    | val                  |
| 整形long               | L    | val                  |
| 整形long long          | LL   | val                  |
| 浮点型float            | F    | val                  |
| 浮点型double           | D    | val                  |
| 浮点型long double      | LD   | val                  |
| 整形unsigned int       | U    | val                  |
| 整形unsigned long      | UL   | val                  |
| 整形unsigned long long | ULL  | val                  |
| 用户自定义标识符id     | ID   | 单词在符号表的下标   |
| <                      | REL  | 0                    |
| <=                     | REL  | 1                    |
| >                      | REL  | 2                    |
| >=                     | REL  | 3                    |
| ==                     | REL  | 4                    |
| !=                     | REL  | 5                    |
| =                      | ASS  | 0                    |
| +=                     | ASS  | 1                    |
| -=                     | ASS  | 2                    |
| *=                     | ASS  | 3                    |
| /=                     | ASS  | 4                    |
| %=                     | ASS  | 5                    |
| &=                     | ASS  | 6                    |
| =                      | ASS  | 7                    |
| ^=                     | ASS  | 8                    |
| >>=                    | ASS  | 9                    |
| <<=                    | ASS  | 10                   |
| ~                      | ASS  | 11                   |
| &                      | BIT  | 0                    |
| \|                     | BIT  | 1                    |
| ~                      | BIT  | 2                    |
| ^                      | BIT  | 3                    |
| <<                     | BIT  | 4                    |
| >>                     | BIT  | 5                    |
| &&                     | LOG  | 0                    |
| \|\|                   | LOG  | 1                    |
| !                      | LOG  | 2                    |
| +                      | ARI  | 0                    |
| -                      | ARI  | 1                    |
| *                      | ARI  | 2                    |
| /                      | ARI  | 3                    |
| %                      | ARI  | 4                    |
| ++                     | ARI  | 5                    |
| --                     | ARI  | 6                    |
| (                      | SPE  | 0                    |
| )                      | SPE  | 1                    |
| [                      | SPE  | 2                    |
| ]                      | SPE  | 3                    |
| ->                     | SPE  | 4                    |
| .                      | SPE  | 5                    |
| /*                     | NOT  | 0                    |
| //                     | NOT  | 1                    |
| {                      | SYM  | 0                    |
| }                      | SYM  | 1                    |
| :                      | SYM  | 2                    |
| \'                     | SYM  | 3                    |
| \"                     | SYM  | 4                    |
| ;                      | SYM  | 5                    |
| ,                      | SYM  | 6                    |
| #                      | SYM  | 7                    |
| 字符串常量string       | STR  | 单词在字符串表的下标 |
| 字符常量char           | CHAR | 单词在字符串表的下标 |

#### 设计全局变量和过程

```c
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
char keyTable[KEYSIZE][10] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile"}; //存放关键字字符
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
```

#### 编制词法分析程序

此处简要说明一下程序中的主要内容，完整程序见附件*源代码。*

该函数在从缓冲区获取下一个字符的同时，前移向前指针，统计程序的总行数，并在向前指针到达缓冲区边界时更新缓冲区。

```c
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
```

该函数向后回退一个向前指针，并将回退标记置1，防止重复更新缓冲区；同时若超前扫描时导致行数计数器增加，在这里一起回退。

```c
void retract(void) {
    re_flag = 1;    
    forward = (forward + BUFFERSIZE * 2 - 1) % (BUFFERSIZE * 2);
    if (C == '\n') {
        line--;
    }
}
```

该函数将每个单词的记号和属性组合成一个二元组，并清空token，为读新的单词作准备。

```c
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
```

该函数是错误处理函数，针对词法分析时可能出现的各种错误给出相应的报告及错误所在的行数，由于行数的增加是在get_char()函数中，若导致错误的字符恰好是换行符，line中储存的行数就会不正确，这里用局部变量eline(error line)解决这个问题。

```c
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
            printf("\t[Error] invalid suffix \"x\" on integer constant. (line %d)\n", eline); break;
        case 5:
            printf("\t[Error] invalid suffix \"b\" on integer constant. (line %d)\n", eline); break;
    }
    pos = 0;
}
```

该函数更新缓冲区，并在文件结束时即时停止读入，同时在读入字符的同时统计程序总字符数。为方便之后的判断，将文件结束符设置为'\0'。

```c
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
```

词法分析器的主体函数，程序逻辑比较简单，就是用switch-case实现上述状态转换图，每个状态就是一个case，状态的转换就是case的跳转。由于该函数过长，请在完整程序中查看。

