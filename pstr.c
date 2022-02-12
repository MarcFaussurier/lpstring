#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define STR(X) (t_pstr) {.s=X, .p=sizeof(X) - 1}

typedef struct s_pstr
{
	size_t			p;
	char			*s;
}	t_pstr;

typedef struct s_lpstr	
{
	size_t			p;
	char *const		os;
	char			*s;
	struct s_lpstr	*next;
	struct s_lpstr	*prev;
	struct s_lpstr	*last;
}					t_lpstr;

t_lpstr	*lpstr_new(t_pstr pstr)
{
	t_lpstr	*o;

	o = malloc(sizeof(t_lpstr));
	if (!o)
		return (0);
	*o = (t_lpstr) {pstr.p, malloc((pstr.p * sizeof(char) + 1)), 0, 0, 0, o};
	if (!o->os)
	{
		free(o);
		return (0);
	}
	o->s = o->os;
	o->os[pstr.p] = 0;
	while (pstr.p)
	{
		pstr.p -= 1;
		o->os[pstr.p] = pstr.s[pstr.p];
	}
	return (o);
}

size_t	lpstr_len(t_lpstr *l)
{
	size_t	i;

	i = 0;
	while (l)
	{
		i += l->p;
		l = l->next;
	}
	return (i);
}

t_lpstr	*lpstr_join(t_lpstr **dest, t_lpstr *new)
{
	t_lpstr	*last;

	if (!new)
		return (*dest);
	if (!*dest)
	{
		*dest = lpstr_new((t_pstr){.s=new->s, .p=new->p});
		return (*dest);
	}
	last = *dest;
	while (1)
	{
		if (last->next)
			last = last->next;
		else
		{
			last->next = lpstr_new((t_pstr){.s=new->s, .p=new->p});
			last->next->prev = last;
			if (!last->next)
				return (0);
			break ;
		}
	}
	(*dest)->last = last;
	return (*dest);
}

t_lpstr	*lpstr_trim(t_lpstr **dest, size_t i, size_t l)
{
	t_lpstr	*swp;
	t_lpstr	*it;
	t_lpstr *begin;
	t_lpstr *last;
	size_t	y;
	size_t	o;
	size_t	z;

	begin = 0;
	y = 0;
	o = 0;
	it = *dest;
	z = 0;
	while (it)
	{

		if (y + it->p >= i && (y < l))
		{
			if (!begin)
			{
				begin = it;
				it->prev = 0;
				it->s += i - z;
				it->p -= i - y;

			}
			o += it->p;
			last = it;
			y += it->p;
			it = it->next;
			continue ;
		}
		if (!begin)
			z += it->p;
		swp = it->next;
		free(it->os);
		free(it);
		it = swp;
	}
	if (last)
	{
		last->p = (l) - (o - last->p) ;
		last->next = 0;
		begin->last = last; 
	}
	*dest = begin;
	return (begin);
}

t_pstr	lpstr_pstr(t_lpstr *l)
{
	t_pstr	pstr;
	size_t	i;
	size_t	y;

	pstr.p = lpstr_len(l);
	pstr.s = malloc( (pstr.p + 1) * sizeof(char));
	if (!pstr.s)
		return (pstr);
	i = 0;
	while (l)
	{
		y = 0;
		while (i < pstr.p && y < l->p)
			pstr.s[i++] = l->s[y++];
		l = l->next;
	}
	pstr.s[i] = 0;
	return (pstr);
}

void	lpstr_iter(t_lpstr *l, void (*f)(char c, void *d), void *d)
{

}

size_t	lpstr_chr(t_lpstr *l, int(*is)(char))
{
	size_t	i;
	size_t	y;

	if (!l)
		return (-1);
	i = 0;
	while (l)
	{
		y = 0;
		while (y < l->p)
		{
			if (is(l->s[y]))
				return (i + y);
			y += 1;
		}
		i += y;
		l = l->next;
	}
	return (-1);
}

size_t	lpstr_rchr(t_lpstr *l, int(*is)(char))
{
	size_t	i;
	size_t	y;
	size_t	len;

	if (!l)
		return (-1);
	len = lpstr_len(l);
	l = l->last;
	i = len;
	while (l)
	{
		y = l->p;
		if (y)
		{
			y -= 1;
			while (y >= 0 && y < -1)
			{
				y -= 1;
				printf("-- %c -> %zu %zu %zu %zu\n", l->s[y], y, len, l->p, i);
				if (is(l->s[y]))
					return (i - l->p + y);
			}
		}
		i -= l->p;
		l = l->prev;
	}
	return (-1);
}

void	lpstr_free(t_lpstr **l)
{
	t_lpstr	*it;
	t_lpstr	*swp;

	it = *l;
	while (it)
	{
		swp = it->next;
		free(it->os);
		free(it);
		it = swp;
	}
	*l = 0;
}

int	is(char c)
{
	return (c == 'W');
}

int main()
{


	t_lpstr *l1 = lpstr_new(STR("Hello"));
	t_lpstr *l2 = lpstr_new(STR(" World!"));


	lpstr_join(&l1, l2);


	printf("size: %zu\nlen: %zu\n", l1->p, lpstr_len(l1));

	lpstr_trim(&l1,  4,4);

	t_pstr pstr = lpstr_pstr(l1);
	printf("p: %zu str: [%s]\n", pstr.p, pstr.s);
	
	printf("indexof: W: %zu\n", lpstr_chr(l1, &is));

	free(pstr.s);
	lpstr_free(&l1);
	lpstr_free(&l2);
}
