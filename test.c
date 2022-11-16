#include <stdio.h>
#include <stdlib.h>
/**
 * @auther: sinkers
 * @date: 2022-10-9
 * 测试用例
 */
void print(void){
	printf("%s\n", "hello world");
}

int main(void){
	//整数与浮点数常量
	int _a, _, a_;
	int a_i_1 = 123;
	int a_i_2 = -123;
	int a_i_3 = 0x123abc;
	a_i_3 = 0x;  // 错误
	int a_i_4 = 0123;
	int a_i_5 = 0b1010;
	a_i_5 = 0B;  // 错误
	long int a_l = 123l;
	long int a_l_2 = 0x123abcl;
	long int a_l_3 = 0123l;
	long int a_l_4 = 0b1010l;
	long long int a_ll = 123ll;
	long long int a_ll_2 = 0x123abcll;
	long long int a_ll_3 = 0123ll;
	long long int a_ll_4 = 0b1010ll;
	float a_f_1 = 1.23f;
	float a_f_2 = 123.f;
	float a_f_3 = .123f;
	float a_f_4 = 1.23e4f;
	float a_f_5 = 123.e4f;
	float a_f_6 = .123e4f;
	float a_f_7 = 123e4f;
	float a_f_8 = 123e-4f;
	float a_f_9 = 12ef;  //错误
	float a_f = 12e+;  //错误
	double a_d_1 =  1.23;
	double a_d_2 = 0xep2;
	double a_d_3 = 0x1p-2;
	double a_d_4 = 0x1p;  //错误
	long double a_ld = 1.23l;
	unsigned int a_u_1 = 123u;
	unsigned int a_u_2 = 0x80000001u;
	unsigned long a_ul_1 = 123ul;
	unsigned long a_ul_2 = 123LU;
	unsigned long a_ul_3 = 0x123abcul;
	unsigned long a_ul_4 = 0123lu;
	unsigned long a_ul_5 = 0b1010lu;
	unsigned long long a_ull_1 = 123ull;
	unsigned long long a_ull_2 = 123LLu;
	unsigned long long a_uul_3 = 0x123abcull;
	unsigned long long a_uul_4 = 0123llu;
	unsigned long long a_uul_5 = 0b1010llu;
	//运算符
	int a, b, c;
	a < b;
	a <= b;
	a > b;
	a >= b;
	a == b;
	a != b;
	c = a + b;
	c = a - b;
	c = a * b;
	c = a / b;
	c = a % b;
	a++;
	a--;
	c = a && b;
	c = a || b;
	c = !a;
	c = a & 0b1;
	c = a | 0b1;
	c = ~ 0b1;
	c = a ^ 1;
	c = a << 1;
	c = a >> 1;
	a += b;
	a -= b;
	a *= b;
	a /= b;
	a %= b;
	a &= b;
	a |= b;
	a ^= b;
	a >>= 1;
	a <<= 1;
	c = 1>2?a:b;

	int i;
	for (i=0; i<5; i++) {
		print();	
	}

	//符号错误
	int j；
}