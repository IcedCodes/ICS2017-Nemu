#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
	char *arg = strtok(NULL, " ");
	int n;
	if (arg == NULL)n = 1;
	else n = atoi(arg);
	if (n == 0)n = 1;
	else if (n < 0 && n != -1)
	{
		printf("You can only input numbers which are bigger than -1\n");
		return 0;
	}
	cpu_exec(n);
	return 0;
}
static int cmd_info(char* args)
{
	char *arg = strtok(NULL, " ");
	if (arg == NULL)
	{
		printf("Sorry, you must input an argument!\n");
	}
	else if(strcmp(arg, "r") == 0)
	{
		printf("eax: 0x%08x\t\t%d\n", cpu.eax, cpu.eax);
		printf("ebx: 0x%08x\t\t%d\n", cpu.ebx, cpu.ebx);
		printf("ecx: 0x%08x\t\t%d\n", cpu.ecx, cpu.ecx);
		printf("edx: 0x%08x\t\t%d\n", cpu.edx, cpu.edx);
		printf("ebp: 0x%08x\t\t%d\n", cpu.ebp, cpu.ebp);
		printf("esi: 0x%08x\t\t%d\n", cpu.esi, cpu.esi);
		printf("edi: 0x%08x\t\t%d\n", cpu.edi, cpu.edi);
		printf("esp: 0x%08x\t\t%d\n", cpu.esp, cpu.esp);
		printf("eip: 0x%08x\t\t%d\n", cpu.eip, cpu.eip);
	}
	else if(strcmp(arg, "w") == 0)
	{
		show();
	}
	return 0;
}

static int cmd_x(char *args)
{
	int n, i;
	uint32_t address;
	char *arg = strtok(NULL, " ");
	n = atoi(arg);
	arg = strtok(NULL, " ");
	//change the inputed address to uint32_t
	if (arg[0] == '0' && arg[1] == 'x')
	{
		i = 2;
		address = 0;
		while (arg[i] != '\0')
		{
			if (arg[i] >= '0' && arg[i] <= '9')
			{
				address = address * 16 + arg[i] - '0';
			}
			else if (arg[i] >= 'a' && arg[i] <= 'f')
			{
				address = address * 16 + arg[i] - 'a' + 10;
			}
			else 
			{
				printf("Wrong Address Format!\n");
				return 0;
			}
			i++;
		}
	}
	else
	{
		printf("Wrong Address Format!\n");
		return 0;
	}
	printf("  Address\tHexadecimal\tDecimal\n");
	for (i = 0; i < n; i++)
	{
		printf("0x%08x\t%x       \t%d\n", address + i * 4,paddr_read(address + i * 4, 4), paddr_read(address + i * 4, 4));
	}
	return 0;
}

static int cmd_p(char *args)
{
	bool success;
	uint32_t result;
	result = expr(args, &success);
	if (success)
	{
		printf("The result is: %d\n", result);
	}
	else printf("Wrong expression!\n");
	return 0;
}

static int cmd_w(char *args)
{
	bool success;
	if(args[0] == '$' && args[1] == 'e' && args[2] == 'i' && args[3] == 'p')
	{
		char *arg = strtok(NULL, " ");
		arg = strtok(NULL, " ");
		printf("arg: %s\n",arg);
		printf("Check == \n");
		if(arg != NULL && arg[0] == '=' && arg[1] == '=')
		{
			arg = strtok(NULL, " ");
			printf("arg after == : %s\n",arg);
			printf("OK\n");
			int i = 2;
			int result = 0;
			if(arg!= NULL && arg[0] == '0' && arg[1] == 'x')
			{
				while (arg[i] != '\0')
				{
					if (arg[i] <= '9' && arg[i] >= '0')
					{
						result = result *16 + arg[i] - '0';
					}
					else if(arg[i] <= 'f' && arg[i] >= 'a')
					{
						result = result * 16 + arg[i] - 'a' + 10;
					}
					else if (arg[i] <= 'F' && arg[i] >= 'A')
					{
						result = result * 16 + arg[i] - 'A' + 10;
					}
					i++;
				}
				if(new_bp(arg, result) != NULL)
				{
					printf("Successfully create a new breakpoint!\n");
				}
			}
			return 0;
		}
	}
	expr(args, &success);
	if (success)
	{
		WP *p = new_wp(args);
		printf("Successfully create a new watchpoint!\n");
		printf("NO: %d\t Value: %d\t Expression: %s\n",p->NO,p->value,p->expression);
	}
	else printf("Wrong expression!\n");
	return 0;
}

static int cmd_d(char *args)
{
	int num = atoi(args);
	free_wp(num);
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si", "step by step n times", cmd_si},
  {"info","printf the infomation of registers or watchpoints\n\t-r show the infomation of the 9 regsisters\n\t -w show the infomation of the watchpoints\n",cmd_info},
  {"x","Scan Memory\n\t - n adress :scan n bytes from adress",cmd_x},
  {"p", "Print the result of an expression.\n", cmd_p},
  {"w", "Build a new watchpoint\n", cmd_w},
  {"d", "Delete a watchpoint by NO.\n", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
