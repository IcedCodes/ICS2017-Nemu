#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].busy = false;
    wp_pool[i].breakpoint = false;
  }
  wp_pool[NR_WP - 1].next = NULL;
  head = NULL;
  free_ = wp_pool;
}
WP* new_bp(char *args, int value)
{
	WP *p = free_;
	if (free_->busy == false)
	{
		free_ = p->next;
		p->next = NULL;
		strcpy(p->expression, args);
		p->busy = true;
		p->breakpoint = true;
		p->stop = value;
		WP *q = head;
		if (q == NULL)
		{
			head = p;
			return p;
		}
		else
		{
			while(q->next != NULL)q = q->next;
			q->next = p;
			return p;
		}
	}
	WP *q;
	q = p->next;
	while(q!= NULL)
	{
		if(q->busy == false)
		{
			p ->next = q->next;
			q->next = NULL;
			strcpy(q->expression,args);
			q->stop = value;
			q->busy = true;
			p = head;
			if(p == NULL)
			{
				head = q;
				return q;
			}
			else
			{
				while(p->next != NULL)p = p->next;
				p->next = q;
				return q;
			}
		}
		else
		{
			q = q->next;
		}
	}
	printf("No aviliable watchpoint!\n");
	return NULL;
}
WP* new_wp(char *args)
{
	bool success;
	WP *p = free_;
	if (free_->busy == false)
	{
		free_ = p->next;
		p -> next = NULL;
		strcpy(p->expression, args);
		p->busy = true;
		WP *q = head;
		if (q == NULL)
		{
			head = p;
		}
		else
		{
			while(q ->next != NULL)
			{
				q = q->next;
			}
			q->next = p;
		}
		p->value = expr(args, &success);
		return p;
	}
	WP *q;
	q = p->next;
	while (q!= NULL)
	{
		if (q->busy == false)
		{
			p->next = q->next;	//relink
			q->next = NULL;
			strcpy(q->expression, args);
			q->busy = true;
			p = head;
			if (p == NULL)
			{
				head = q;
			}
			else
			{
				while(p->next != NULL)p = p->next;
				p->next = q;
			}
			q->value = expr(args, &success);
			return q;
		}
		else
		{
			p = q;
			q = q->next;
		}
	}
	printf("No aviliable watchpoint!\n");
	return NULL;

}
void free_wp(int num)
{
	WP *p, *q, *r;
	p = head;
	if (p->NO == num)
	{
		q = p;
		head = p->next;
	}
	else
	{
		q = head->next;
		while (q != NULL)
		{
			if (q->NO == num)
			{
				p ->next = q->next;
				break;
			}
			else
			{
				p = q;
				q= q->next;
			}
		}
	}
	if (q == NULL)
	{
		printf("There is no such watchpoint!\n");
		return;
	}
	p = free_;
	if (p->NO > q->NO)
	{
		free_ = q;
		q->next = p;
		q->busy = false;
		q->breakpoint = false;
		printf("Successfully delete watchpoint NO.%d\n",num);
		return;
	}
	else
	{
		r = p->next;
		while (r != NULL)
		{
			if (q->NO < r->NO)
			{
				p->next = q;
				q->next = r;
				q->busy = false;
				q->breakpoint = false;
				printf("Successfully delete watchpoint NO.%d\n",num);
				return;
			}
			else
			{
				p = r;
				r = r->next;
			}
		}
		p->next = q;
		q->next = NULL;
	}
}
void show()
{
	WP* p;
	p = head;
	if (p == NULL)
	{
		printf("There is no watchpoint!\n");
		return;
	}
	printf("NO    Breakpoint\t Value\t\t\tExpression\n");
	while(p != NULL)
	{
		printf("%d \t",p->NO);
		if(p->breakpoint == false)printf("No\t\t%d     \t\t%s\n",p->value,p->expression);
		else printf("Yes\t\t%d     \t\t$eip == %s\n",p->stop,p->expression);
		p = p->next;
	}
}

WP* checkchange()
{
	WP* p;
	p = head;
	bool success = false;
	while (p != NULL)
	{
		if (p->breakpoint)
		{
			p->stop = expr(p->expression, &success);
			if(cpu.eip == p->stop)return p;
		}
		else if(p->value != expr(p->expression, &success))return p;
		p = p->next;
	}
	return NULL;
}
