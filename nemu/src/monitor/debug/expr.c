#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_PLUS, TK_MINUS, TK_MULTIPLY, TK_DIVIDE, TK_EQUAL,TK_NUMBERS,

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

uint32_t expr(char *e, bool *success) 
{
 int sum = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
 else
 {
	*success = true;
//	 int result[32];
//	 int symbols[32];
	 int i, b;
	 b = 0;
	 sum = atoi(tokens[0].str);
	 for (i = 1; i < nr_token; i++)
	 {
		 if (tokens[i].type == TK_NUMBERS)
		 {
			if (b != 0)
			{
				sum += b;
			}
			else
			{
				b = 0;
			}
		 }
	 }
 }
  /* TODO: Insert codes to evaluate the expression. */
  return sum;
}
