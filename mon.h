/******************************************************************************

    Copyright (C) 2009, 2010, 2011 Sebastian Pancratz

******************************************************************************/

#ifndef MON_H
#define MON_H

#include <stdlib.h>
#include <stdio.h>

#include "flint/flint.h"

typedef unsigned long mon_t;
typedef unsigned long exp_t;

#define MON_BITS_PER_EXP   8
#define MON_BITMASK_BLOCK  ((1UL << MON_BITS_PER_EXP) - 1UL)
#define MON_BITMASK_ALL    (~(0UL))

#define MON_MIN_VARS  1
#define MON_MAX_VARS  (FLINT_BITS / MON_BITS_PER_EXP)

/* Initialization ************************************************************/

#define mon_init(x)  ((x) = 0)
#define mon_clear(x)
#define mon_set(x, y)  ((x) = (y))
#define mon_swap(x, y)  \
    do { mon_t xxx = (x); (x) = (y); (y) = xxx; } while(0)
#define mon_one(x)  ((x) = 0)

/* Access ********************************************************************/

#define mon_get_exp(x, i)  \
    ((exp_t) (((x) >> ((i) * MON_BITS_PER_EXP)) & MON_BITMASK_BLOCK))

#define mon_set_exp(x, i, e)  \
    ((x) = (((x) & (MON_BITMASK_ALL - (MON_BITMASK_BLOCK << ((i) * MON_BITS_PER_EXP)))) \
            | (((mon_t)(e)) << ((i) * MON_BITS_PER_EXP))))

#define mon_inc_exp(x, i, e)  \
    ((x) = ((x) + (((mon_t)(e)) << ((i) * MON_BITS_PER_EXP))))

#define mon_dec_exp(x, i, e)  \
    ((x) = ((x) - (((mon_t)(e)) << ((i) * MON_BITS_PER_EXP))))

/* Randomisation *************************************************************/

mon_t _mon_randtest(flint_rand_t state, int n, exp_t k);

#define mon_randtest(x, state, n, k) \
    ((x) = _mon_randtest((state), (n), (k)))

/* Comparison ****************************************************************/

#define mon_cmp_invlex(x, y)  (((x) < (y)) ? -1 : ((x) > (y) ? 1 : 0))

static __inline__
int mon_cmp(const mon_t x, const mon_t y)
{
    return mon_cmp_invlex(x, y);
}

#define mon_is_one(x)  ((x) == 0)

#define mon_equal(x, y)  ((x) == (y))

/* Multiplication and division ***********************************************/

#define mon_mul(x, y, z)  ((x) = (y) + (z))

#define mon_div(x, y, z)  ((x) = (y) - (z))

int mon_divides(mon_t x, mon_t y);

/* Monomial parameters *******************************************************/

int mon_degree(mon_t x);

/* Input and output **********************************************************/

static int mon_exp_len(exp_t e)
{
    if (e < 10)
        return 1;
    else if (e < 100)
        return 2;
    else 
        return 3;
}

char * mon_get_str(mon_t x, int n);

char * mon_get_str_pretty(mon_t x, int n, const char * vars);

mon_t _mon_set_str(char * str);

#define mon_set_str(x, str)  ((x) = _mon_set_str(str))

static int mon_print(mon_t x, int n)
{
    char *str;

    str = mon_get_str(x, n);
    printf("%s", str);
    free(str);
    return 1;
}

static int mon_print_pretty(mon_t x, int n, const char * vars)
{
    char *str;

    str = mon_get_str_pretty(x, n, vars);
    printf("%s", str);
    free(str);
    return 1;
}

/* Enumeration  **************************************************************/

mon_t * mon_generate_by_degree(long * len, int n, int d);

mon_t * mon_generate_by_degree_invlex(long * len, int n, int d);

/* Auxiliary functions *******************************************************/

static unsigned long mon_binom(unsigned long n, unsigned long k)
{
    unsigned long i, d, res;

    if (n < k)
        return 0;
    if (k > n / 2)
        k = n - k;
    if (k == 0)
        return 1;
    d = n - k;
    res = d + 1;
    for (i = 2; i <= k; i++)
    {
        res *= d + i;
        res /= i;
    }
    return res;
}

#endif

