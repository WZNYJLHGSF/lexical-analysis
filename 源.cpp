#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define SIZE 1024
#define keyword_num 44
//C语言关键字
std::string keywords[44] = { "auto","break","case","char","const","continue",
                          "default","do","double","else","enum","extern",
                          "float","for","goto","if","int","long","register",
                          "return","short","signed","sizeof","static",
                          "struct","switch","typedef","union","unsigned",
                          "void","volatile","while","inline","restrict",
                          "_Bool","_Complex","_Imaginary",//C99新增的关键字
                          "_Alignas","_Alignof","_Atomic","_Static_assert",
                          "_Noreturn","_Thread_local","_Generic" };//C11新增的关键字

int state;//当前状态指示
char C;//存放当前读入的字符
int iskey = -1;
/*表示识别出的单词是否为用户自定义标识符还是关键字，
  如果是用户自定义标识符则为-1，若是关键字则是关键字的记号*/
std::string token;//字符数组，存放当前正在识别的单词字符串
char* lexemebegin;//字符指针，指向输入缓冲区中当前单词的开始位置
int forward = -1;//向前指针
char buffer[SIZE];//输入缓冲区
int lines = 1;//文件的行数
int chars = 0;//文件的总字符数
struct symbol {
    std::string attribute;//属性
    std::string name;//名字
    std::string token;//记号
    std::string category;//类别
    int count = 0;//次数
};//符号表
struct count {
    int count_keyword = 0;//关键字
    int count_id = 0;//标识符
    int count_num = 0;//常数
    int count_string = 0;//字符串
    int count_assignment_operator = 0;//赋值运算符
    int count_bit_operator = 0;//位运算符
    int count_arithmetic_operator = 0;//算数运算符
    int count_logical_operator = 0;//逻辑运算符
    int count_char = 0;//字符
    int count_relational_operator = 0;//关系运算符
    int count_other_symbol = 0;//其他符号
};//统计表

count c;
std::vector<symbol> table;
/*每调用一次，根据向前指针forward的指示，从输入缓冲区中读一个字符，
  并把它放入变量C中，然后移动forward使之指向下一个字符*/
void get_char() {
    forward++;
    C = buffer[forward];
    if (C == '\n')
        lines++;//统计行数
    if (C != '\n' && C != '\t' && C != ' ')
        chars++;//统计字符数
}
/*每次调用时，检查C中的字符是否为空格，若是空格，就反复调用过程get_char，
  直到C中进入一个非空字符为止*/
void get_nbc() {
    if (C == ' ') {
        get_char();
    }
}
//每次调用时，把C中的字符连接在token中的字符串后面
void cat() {
    token.push_back(C);
}
//判断C中的字符是否为字母，若是字符则返回true，否则返回false
bool isLetter() {
    if ((C <= 'z' && C >= 'a') || (C <= 'Z' && C >= 'A') || C == '_')
        return true;
    else
        return false;
}
//判断C语言中的字符是否为数字，若是数字则返回true，否则返回false
bool isDigit() {
    if (C <= '9' && C >= '0')
        return true;
    else
        return false;
}
//向前指针forw后退一个字符
void retract() {
    forward--;
}
/*根据token中的单词查关键字表，若token中的单词是关键字，
  则返回1，否则返回-1*/
int reserve() {
    for (int i = 0; i < 44; i++) {
        if (token == keywords[i])
            return 1;//是关键字就返回1
    }
    return -1;//不是关键字就返回-1

}
//将token中的字符串转换成整数
int SToI() {
    return atoi(token.c_str());
}
//将token中的字符串转换成浮点数
float SToF() {
    return atof(token.c_str());
}
//将识别出来的用记号插入符号表
int table_insert(std::string t, std::string a, std::string c) {
    int i = 0;
    for (i = 0; i < table.size(); i++) {
        if (token == table[i].name) {
            table[i].count++;
            return 0;
        }
    }
    symbol s;
    s.name = token;
    s.token = t;
    s.attribute = a;
    s.count = 1;
    s.category = c;
    table.push_back(s);
    return 0;

}
std::string find(std::string name) {
    int i = 0;
    for (; i < table.size(); i++) {
        if (table[i].name == name) {
            return std::to_string(i);
        }
    }
    return std::to_string(i);
}
//对发现的错误进行响应的处理
void error() {
    std::cout << "There is an error in line " << lines << std::endl;
    state = 0;

}

void analyse(std::ifstream& fs) {
    //读取文件内容
    fs.read(buffer, SIZE - 1);

    if (fs.gcount() < SIZE - 1) {
        buffer[fs.gcount()] = EOF;//向输入缓存区加入结束标志
    }
    state = 0;//初始状态
    int j = 0;
    while (C != EOF) {
        switch (state) {
        case 0:
            token.clear();
            get_char();
            get_nbc();
            if (isLetter()) {
                state = 1;
                break;
            }
            if (isDigit()) {
                state = 2;
                break;
            }
            switch (C) {
            case '<':
                state = 8;
                break;
            case '>':
                state = 9;
                break;
            case ':':
                state = 10;
                break;
            case '/':
                state = 11;
                break;
            case '=':
                state = 14;
                break;
            case '+':
                state = 15;
                break;
            case '-':
                state = 16;
                break;
            case '(':
                state = 0;
                cat();
                table_insert("(", "-", "ohter symbol");
                break;
            case ')':
                state = 0;
                cat();
                table_insert(")", "-", "ohter symbol");
                break;
            case ';':
                state = 0;
                cat();
                table_insert(";", "-", "ohter symbol");
                break;
            case '?':
                state = 0;
                cat();
                table_insert("?", "-", "conditional operator");
                break;
            case '%':
                state = 0;
                cat();
                table_insert("%", "-", "arithmetic operator");
                break;
            case '{':
                state = 0;
                cat();
                table_insert("{", "-", "ohter symbol");
                break;
            case '}':
                state = 0;
                cat();
                table_insert("}", "-", "ohter symbol");
                break;
            case '[':
                state = 0;
                cat();
                table_insert("[", "-", "ohter symbol");
                break;
            case ']':
                state = 0;
                cat();
                table_insert("]", "-", "ohter symbol");
                break;
            case '|':
                state = 18;
                break;
            case '&':
                state = 19;
                break;
            case '^':
                state = 0;
                cat();
                table_insert("^", "-", "bit operator");
                break;
            case '*':
                state = 20;
                break;
            case '"':
                state = 21;
                break;
            case '.':
                state = 0;
                break;
            case '~':
                state = 0;
                cat();
                table_insert("~", "-", "bit operator");
                break;
            case ',':
                state = 0;
                cat();
                table_insert(",", "-", "ohter symbol");
                break;
            case '#':
                state = 0;
                cat();
                table_insert("#", "-", "ohter symbol");
                break;
            case '\'':
                state = 22;
                break;
            case ' ':
                break;
            case '\n':
                break;
            case '\t':
                break;
            default:
                state = 13;
                break;
            }
            break;
        case 1://标识符状态
            cat();
            get_char();
            if (isLetter() || isDigit())
                state = 1;
            else {
                retract();
                state = 0;
                if (reserve() == 1) {
                    table_insert(token, "-", "keyword");
                }
                else {
                    table_insert("id", find(token), "id");
                }
            }
            break;
        case 2://常数状态
            cat();
            get_char();
            if (isDigit())
                state = 2;
            else if (C == 'e' || C == 'E')
                state = 5;
            else if (C == '.')
                state = 3;
            else {
                retract();
                state = 0;
                table_insert("num", token, "integer");
            }
            break;
        case 3://小数点状态
            cat();
            get_char();
            if (isDigit())
                state = 4;
            else {
                state = 13;
            }
            break;
        case 4://小数状态
            cat();
            get_char();
            if (isDigit())
                state = 4;
            else if (C == 'e' || C == 'E')
                state = 5;
            else {
                retract();
                state = 0;
                table_insert("num", token, "float");
            }
            break;
        case 5://指数状态
            cat();
            get_char();
            if (isDigit())
                state = 7;
            else if (C == '+' || C == '-')
                state = 6;
            else {
                retract();
                state = 13;
            }
            break;
        case 6:
            cat();
            get_char();
            if (isDigit())
                state = 7;
            else {
                retract();
                state = 13;
            }
            break;
        case 7:
            cat();
            get_char();
            if (isDigit())
                state = 7;
            else {
                retract();
                state = 0;
                table_insert("num", token, "exponent");
            }
            break;
        case 8://'<'状态
            cat();
            get_char();
            switch (C) {
            case '=':
                cat();
                state = 0;
                table_insert("relop", "LE", "relational operator");
                break;
            case '>':
                cat();
                state = 0;
                table_insert("relop", "NE", "relational operator");
                break;
            case '<':
                cat();
                state = 0;
                table_insert(token, "-", "bit operator");
                break;
            default:
                retract();
                state = 0;
                table_insert("relop", "LT", "relational operator");
                break;
            }
            break;
        case 9://'>'状态
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert("relop", "GE", "relational operator");
            }
            else if (C == '>') {
                cat();
                state = 0;
                table_insert(token, "-", "bit operator");
            }
            else {
                retract();
                state = 0;
                table_insert("relop", "GT", "relational operator");
            }
            break;
        case 10://':'状态
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert("assign-op", "-", "assign-op");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "ohter symbol");
            }
            break;
        case 11://'/'状态
            cat();
            get_char();
            if (C == '*')
                state = 12;
            else if (C == '/')
                state = 17;
            else if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }

            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 12://处理/*注释状态标志
            get_char();
            while (C != '*') {
                get_char();
                chars--;
            }
            get_char();
            if (C == '/') {
                token.clear();
                chars = chars - 4;
                state = 0;
            }
            else
                state = 12;
            break;
        case 13:
            error();
            state = 0;
            break;
        case 14://'='状态
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "relational operator");
            }
            else {
                retract();
                state = 0;
                table_insert("relop", "EQ", "assignment operator");
            }
            break;
        case 15://'+'状态
            cat();
            get_char();
            if (C == '+') {
                cat();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            else if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 16://'-'状态
            cat();
            get_char();
            if (C == '-') {
                cat();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            else if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 17://处理//注释状态标志
            get_char();
            while (C != '\n') {
                get_char();
                chars--;
            }
            token.clear();
            chars = chars - 2;
            state = 0;
            break;
        case 18: //处理'|'状态标志
            cat();
            get_char();
            if (C == '|') {
                cat();
                state = 0;
                table_insert(token, "-", "logical operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "bit operator");
            }
            break;
        case 19://处理'&'状态标志
            cat();
            get_char();
            if (C == '&') {
                cat();
                state = 0;
                table_insert(token, "-", "logical operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "bit operator");
            }
            break;
        case 20://处理'*'状态标志
            cat();
            get_char();
            if (C == '=') {
                cat();
                state = 0;
                table_insert(token, "-", "assignment operator");
            }
            else {
                retract();
                state = 0;
                table_insert(token, "-", "arithmetic operator");
            }
            break;
        case 21://处理'"'状态标志
            cat();
            get_char();
            while (C != '"') {
                cat();
                get_char();
            }
            cat();
            state = 0;
            table_insert("literal", token, "string");
            break;

        case 22://处理'''状态标志
            cat();
            get_char();
            j = 0;

            while (C != '\'') {
                cat();
                get_char();
                j++;

                if (C == '\n') {
                    state = 13;
                    break;
                }
            }
            if (j > 1) {
                error();
                state = 0;
                break;
            }
            cat();
            state = 0;
            table_insert("char", token, "char");
            break;
        default:error(); state = 0; break;
        }
    }
    fs.close();
}
void output() {

    std::cout << "name \t\ttimes\t\ttoken\t\tattribute\t\tcategory\n";
    for (int i = 0; i < table.size(); i++) {
        if (table[i].category == "keyword")
            c.count_keyword++;
        if (table[i].category == "id")
            c.count_id++;
        if (table[i].token == "num")
            c.count_num++;
        if (table[i].category == "string")
            c.count_string++;
        if (table[i].category == "relational operator")
            c.count_relational_operator++;
        if (table[i].category == "relational operator")
            c.count_relational_operator++;
        if (table[i].category == "assignment operator")
            c.count_assignment_operator++;
        if (table[i].category == "bit operator")
            c.count_bit_operator++;
        if (table[i].category == "arithmetic operator")
            c.count_arithmetic_operator++;
        if (table[i].category == "logical operator")
            c.count_logical_operator++;
        if (table[i].category == "other symbol")
            c.count_other_symbol++;
        if (table[i].category == "char")
            c.count_char++;
        std::cout << table[i].name << "\t\t" << table[i].count << "\t\t" << table[i].token <<
            "\t\t" << table[i].attribute << "\t\t\t" << table[i].category << std::endl;
    }

    if (c.count_keyword > 0)
        std::cout << "count_keyword:" << c.count_keyword << std::endl;
    if (c.count_id > 0)
        std::cout << "count_id:" << c.count_id << std::endl;
    if (c.count_num > 0)
        std::cout << "count_num:" << c.count_num << std::endl;
    if (c.count_string > 0)
        std::cout << "count_string:" << c.count_string << std::endl;
    if (c.count_char > 0)
        std::cout << "count_char:" << c.count_char << std::endl;
    if (c.count_relational_operator > 0)
        std::cout << "count_relational_operator:" << c.count_relational_operator << std::endl;
    if (c.count_assignment_operator > 0)
        std::cout << "count_assignment_operator:" << c.count_assignment_operator << std::endl;
    if (c.count_bit_operator > 0)
        std::cout << "count_bit_operator:" << c.count_bit_operator << std::endl;
    if (c.count_arithmetic_operator > 0)
        std::cout << "count_arithmetic_operator:" << c.count_arithmetic_operator << std::endl;
    if (c.count_logical_operator > 0)
        std::cout << "count_logical_operator:" << c.count_logical_operator << std::endl;
    std::cout << "line:" << lines << std::endl << "chars:" << chars;


}
int main() {
    std::ifstream fs;
    fs.open("t.txt", std::ios::in);
    if (fs.is_open() == false)
        exit(0);
    analyse(fs);
    output();
    fs.close();
    return 0;
}