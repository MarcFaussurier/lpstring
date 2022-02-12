#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define STR(X) (t_pstr) {.p=sizeof(X) - 1, .s=X, }

typedef struct s_pstr
{
	size_t			p;
	char			*s;
}	t_pstr;

typedef struct s_lpstr	
{
	size_t			p;
	char 			*os;
	char			*s;
	struct s_lpstr	*next;
	struct s_lpstr	*prev;
	struct s_lpstr	*last;
}					t_lpstr;

t_lpstr	*lpstr_new(t_pstr pstr)
{
	t_lpstr	*o;

	o = (t_lpstr*) malloc(sizeof(t_lpstr));
	if (!o)
		return (0);
	*o = (t_lpstr) {pstr.p, (char*) malloc((pstr.p * sizeof(char) + 1)), pstr.s, 0, 0, o};
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

t_lpstr	*lpstr_join(t_lpstr **dest, t_lpstr *n)
{
	t_lpstr	*last;

	if (!n)
		return (*dest);
	if (!*dest)
	{
		*dest = lpstr_new((t_pstr){.p=n->p, .s=n->s});
		return (*dest);
	}
	last = *dest;
	while (1)
	{
		if (last->next)
			last = last->next;
		else
		{
			last->next = lpstr_new((t_pstr){.p = n->p, .s=n->s});
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
	pstr.s = (char*) malloc( (pstr.p + 1) * sizeof(char));
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

#include <chrono>
#include <iostream>

using namespace std;

uint64_t nanos()
{
    uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
    return ns; 
}

int main()
{
	cout << "---\nasprintf:\n";

	char *as;
	size_t	i;
	auto ns = nanos();	
	auto total = 0;
	char	*swp;

	asprintf(&as, "%s%s%s", "Hello", " World!", " DHJWHDAWHDIWHDIWAJDHIWAJD");
	swp = as;
	asprintf(&as, "%.4s", as + 4);
	printf("-- %s\n", as);
	i = (int) (strchr(as, ('W')) - as);
	printf("indexof(W): %i\n", i);
	free(as);
	free(swp);
	total += nanos() - ns;
	cout << "TOTAL:\t" << total << endl;
	total = 0;
	cout << "---\nlpstr:\n";
	 ns = nanos();	
	total = 0;
	t_lpstr *l1 = lpstr_new(STR("Hello"));
	t_lpstr *l2 = lpstr_new(STR(" World!"));
	t_lpstr *l3 = lpstr_new(STR(" DHJWHDAWHDIWHDIWAJDHIWAJD"));
	lpstr_join(&l1, l2);
	lpstr_join(&l1, l3);

	total += nanos() - ns;
	cout << nanos() - ns << "\tInit + append !\n";
	ns = nanos();	

	lpstr_trim(&l1,  4,4);


	total += nanos() - ns;
	cout << nanos() - ns << "\tSubstr !\n";
	ns = nanos();	

	t_pstr pstr = lpstr_pstr(l1);
	printf("p: %zu str: [%s]\n", pstr.p, pstr.s);
	printf("indexof: W: %zu\n", lpstr_chr(l1, &is));
	free(pstr.s);
	lpstr_free(&l1);
	lpstr_free(&l2);
	lpstr_free(&l3);

	total += nanos() - ns;
	cout << nanos() - ns << "\tto C + find + freeing!\n";
	cout << "TOTAL:\t" << total << endl;
	total = 0;
	cout << "--------------\nstd::string:\n" << endl;
	ns = nanos();	
	
	auto s1 = std::string("Hello");
	auto s2 = std::string(" World!");
	auto s3 = std::string(" DHJWHDAWHDIWHDIWAJDHIWAJD");
	s1 = s1.append(s2);
	s1 = s1.append(s3);

	total += nanos() - ns;
	cout << nanos() - ns << "\tInit + append !\n";
	ns = nanos();	

	s1 = s1.substr(4, 4);

	total += nanos() - ns;
	cout << nanos() - ns << "\tSubstr !\n";
	ns = nanos();	

	auto cs = s1.c_str();
	printf("p: %zu str: [%s]\n", strlen(cs), cs);
	printf("indexof: W: %zu\n", s1.find("W"));

	total += nanos() - ns;
	cout << nanos() - ns << "\tto C + find + freeing!\n";
	cout << "TOTAL:\t" << total << endl;
}
