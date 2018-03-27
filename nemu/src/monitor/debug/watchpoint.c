#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].busy = false;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp()
{
	WP *p = free_;
	if (free_->busy == false)
	{
		free_ = p->next;
		p -> next = NULL;
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
void free_wp(WP *wp)
{
	WP *p, *q, *r;
	p = head;
	if (p == wp)
	{
		q = p;
		head = p->next;
	}
	else
	{
		q = head->next;
		while (q != NULL)
		{
			if (q == wp)
			{
				p ->next = q->next;
			}
			else
			{
				p = q;
				q= q->next;
			}
		}
	}
	p = free_;
	if (p->NO > q->NO)
	{
		free_ = q;
		q->next = p;
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
	printf("NO \t Value\t Expression\n");
	while(p != NULL)
	{
		printf("%d \t%d\t %s\n",p->NO,p->value,p->expression);
		p = p->next;
	}
}

WP* checkchange()
{
	WP* p;
	p = head;
	bool *success = false;
	while (p != NULL)
	{
		if (p->value != expr(p->expression,success))return p;
		p = p->next;
	}
	return NULL;
}
