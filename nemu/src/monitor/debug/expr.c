#include "nemu.h"
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <memory/memory.h>
enum {
  TK_NOTYPE = 256, TK_EQ, TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE, TK_EQUAL,TK_NUMBERS,TK_LEFT,TK_RIGHT,TK_REG,
  TK_MEMORY,TK_HEX,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\+",TK_PLUS},         // plus
  {"==", TK_EQUAL},        // equal
  {"\\-",TK_MINUS},		//minus
  {"\\*",TK_MULTIPLY},		//multiply
  {"\\/",TK_DIVIDE},		//divide
  {"[0-9]{1,8}",TK_NUMBERS},	//numbers
  {"\\0\\x[0-9a-fA-F]{1,8}",TK_HEX},	//hex
  {"($eax)|($ebx)|($ecx)|($edx)|($ebp)|($esi)|($edi)|($esp)|($eip)",TK_REG},
  {"\\(",TK_LEFT},
  {"\\)",TK_RIGHT},
  {"\\*\\0\\x[0-9a-fA-f]{1,8}",TK_MEMORY},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i, j;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) 
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) 
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
	tokens[nr_token].type = rules[i].token_type;
	for (j = 0; j < substr_len; j++)
	{
		tokens[nr_token].str[j] = e[position - substr_len + j];
	}
	nr_token++;
/*        switch (rules[i].token_type) {
          default: TODO();
        }
*/
	break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
uint32_t HEX_to_DEC(char *arg)
{
	int i; 
	uint32_t result;
	i = 2;
	result = 0;
	while (arg[i] != '\n')
	{
		if (arg[i] <= '9' && arg[i] >= '0')
		{
			result += arg[i] - '0';
		}
		else if(arg[i] <= 'f' && arg[i] >= 'a')
		{
			result += arg[i] - 'a' + 10;
		}
		else if (arg[i] <= 'F' && arg[i] >= 'A')
		{
			result += arg[i] - 'A' + 10;
		}
	}
	return result;
}
int data[32];
int operator[32];
int a, b;
void plus()
{
	data[a - 2] += data[a - 1];
	a--;
}
void minus()
{
	data[a - 2] -= data[a - 1];
	a--;
}
void multiply()
{
	data[a - 2] *= data[a - 1];
	a--;
}
void divide()
{
	data[a - 2] /= data[a - 1];
	a--;
}
bool solve(int type)
{
	if (type == TK_MULTIPLY || type == TK_DIVIDE)
	{
		operator[b] = type;
		b++;
	}
	else if (type == TK_PLUS || type == TK_MINUS)
	{
		if (operator[b - 1] == TK_PLUS)
		{
			plus();
			operator[b - 1] = type;
		}
		else if (operator[b - 1] == TK_MINUS)
		{
			minus();
			operator[b - 1] = type;
		}
		else if (operator[b - 1] == TK_DIVIDE)
		{
			divide();
			operator[b - 1] = type;
		}
		else if (operator[b - 1] == TK_MULTIPLY)
		{
			multiply();
			operator[b - 1] = type;
		}
		else return false;
	}
	return true;
}
uint32_t expr(char *e, bool *success) 
{
 int sum = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
 else
 {
	a = b = 0;
	int i;
	operator[0] = TK_NOTYPE;
	for (i = 0; i < nr_token; i++)
	{
		if (tokens[i].type ==TK_NOTYPE)continue;
		if (tokens[i].type == TK_NUMBERS)
		{
			data[a] = atoi(tokens[i].str);
			a++;
		}
		else if (tokens[i].type == TK_HEX)
		{
			data[a] = HEX_to_DEC(tokens[i].str);
			a++;
		}
		else if (tokens[i].type == TK_LEFT)
		{
			operator[b] = TK_LEFT;
		}
		else if (tokens[i].type == TK_RIGHT)
		{
			if (b == 0 || operator[b - 1] != TK_LEFT)
			{
				*success = false;
				return 0;
			}
			else
			{
				b--;	//pop
				if (b == 0)
				{
					operator[b] = TK_NOTYPE;
				}
			}
		}
		else if (tokens[i].type == TK_REG)
		{
			if (strcmp(tokens[i].str,"$eax")== 0)
			{
				data[a] = cpu.eax;
				a++;
			}
			if (strcmp(tokens[i].str,"$ebx")== 0)
			{
				data[a] = cpu.ebx;
				a++;
			}
			if (strcmp(tokens[i].str,"$ecx")== 0)
			{
				data[a] = cpu.eax;
				a++;
			}
			if (strcmp(tokens[i].str,"$edx")== 0)
			{
				data[a] = cpu.edx;
				a++;
			}
			if (strcmp(tokens[i].str,"$ebp")== 0)
			{
				data[a] = cpu.eax;
				a++;
			}
			if (strcmp(tokens[i].str,"$esi")== 0)
			{
				data[a] = cpu.esi;
				a++;
			}
			if (strcmp(tokens[i].str,"$edi")== 0)
			{
				data[a] = cpu.edi;
				a++;
			}
			if (strcmp(tokens[i].str,"$esp")== 0)
			{
				data[a] = cpu.esp;
				a++;
			}
			if (strcmp(tokens[i].str,"$eip")== 0)
			{
				data[a] = cpu.eip;
				a++;
			}

		}
		if (tokens[i].type == TK_HEX)
		{
			data[a] = HEX_to_DEC(tokens[i].str);
			a++;
		}
		if (tokens[i].type == TK_MEMORY)
		{
			data[a] = paddr_read(HEX_to_DEC(tokens[i].str), 4);
			a++;
		}
		else
		{
			if (a == 0 && b!= 0)
			{
				*success = false;
				return 0;
			}
			if(solve(tokens[i].type) == false)
			{
				*success = false;
				return 0;
			}
		}
	}
 }
 if (a == 0 && operator[0] == TK_NOTYPE)
 {
	 *success = true;
	 return data[0];
 }
 else 
 {
	 *success =false;
 }
  /* TODO: Insert codes to evaluate the expression. */
  return sum;
}
