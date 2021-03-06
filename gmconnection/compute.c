/******************************************************************************

    Copyright (C) 2013 Sebastian Pancratz
 
******************************************************************************/
/******************************************************************************

    Copyright (C) 2013 Sebastian Pancratz
 
******************************************************************************/

#include "gmconnection.h"

/*
    Sets the polynomial \code{rop} to the derivative of \code{op} 
    with respect to the indeterminate $t$ of the base field.
 */
static void mpoly_tderivative(mpoly_t rop, const mpoly_t op, const ctx_t ctx)
{
    mpoly_t temp;
    
    mpoly_iter_t iter;
    mpoly_term t;
    
    if (mpoly_is_zero(op, ctx))
    {
        mpoly_zero(rop, ctx);
        rop->n = op->n;
        return;
    }
    
    mpoly_init(temp, op->n, ctx);
    
    mpoly_iter_init(iter, op);
    while ((t = mpoly_iter_next(iter)))
    {
        char *c;

        c = malloc(ctx->size);
        ctx->init(ctx, c);
        ctx->derivative(ctx, c, t->val);

        if (ctx->is_zero(ctx, c))
        {
            ctx->clear(ctx, c);
            free(c);
        }
        else
        {
            mon_t m2;
            void *c2;

            /* Note that no entry with this key is present in temp yet */
            RBTREE_INSERT(mpoly, &m2, &c2, temp->dict, t->key, c, &mon_cmp);
        }
    }
    mpoly_iter_clear(iter);
    
    mpoly_swap(rop, temp, ctx);
    mpoly_clear(temp, ctx);
}

void gmc_compute(mat_t M, mon_t **rows, mon_t **cols, 
                 const mpoly_t P, const ctx_t ctx)
{
    long i, j, k, colk, rowk;
    mpoly_t *dP, dPdt;

    mon_t *B;
    long *iB, l, u, lenB;

    const long n = P->n - 1;
    const long d = mpoly_degree(P, -1, ctx);
    
    /*
        Arrays for the auxiliary matrices;  the arrays are of length u + 1 
        but they are NULL when k < l
     */
    mat_csr_t *aux;
    mon_t **aux_rows, **aux_cols;
    long **aux_p;
    mat_csr_solve_t *aux_s;
    
    /*
        Compute all partial derivatives of P, and the derivative of P with 
        respect to t, the variable of the coefficient field
     */
    dP = malloc((n + 1) * sizeof(mpoly_t));
    for (i = 0; i <= n; i++)
        mpoly_init(dP[i], n + 1, ctx);
    gmc_derivatives(dP, P, ctx);
    mpoly_init(dPdt, n + 1, ctx);
    mpoly_tderivative(dPdt, P, ctx);
    
    /* Construct the index set B */
    gmc_basis_sets(&B, &iB, &lenB, &l, &u, n, d);
    
    /*
        Construct the auxiliary matrices

        Note that a priori we might need them for k = 1, ..., n, n+1. 
        The rows and columns sets are only non-empty if (k-1)*d >= n.

        To ease indexing, we include the empty set in the case k = 0.
     */
    aux      = malloc((n + 2) * sizeof(mat_csr_t));
    aux_rows = malloc((n + 2) * sizeof(mon_t *));
    aux_cols = malloc((n + 2) * sizeof(mon_t *));
    aux_p    = malloc((n + 2) * sizeof(long *));
    aux_s    = malloc((n + 2) * sizeof(mat_csr_solve_t));

    for (k = (n + (d - 1)) / d + 1; k <= u + 1; k++)
    {
        aux_p[k] = malloc((n + 2) * sizeof(long));

        gmc_init_auxmatrix(aux[k], aux_rows + k, aux_cols + k, aux_p[k], 
                           P, k, ctx);

        mat_csr_solve_init(aux_s[k], aux[k], ctx);
    }
    
    /* Construct the Gauss--Manin connection matrix */

    for (colk = l; colk <= u; colk++)
    {
        for (j = iB[colk]; j < iB[colk + 1]; j++)
        {
            mpoly_t Q, *R;

            mpoly_init(Q, n + 1, ctx);
            R = malloc((n + 2) * sizeof(mpoly_t));
            for (i = 0; i <= n + 1; i++)
                mpoly_init(R[i], n + 1, ctx);

            /* Set Q to -colk B[j] dPdt, and then reduce */
            mpoly_mul_mon(Q, dPdt, B[j], ctx);
            mpoly_scalar_mul_si(Q, Q, -colk, ctx);
            
            gmc_reduce(R, Q, colk + 1, d, dP, 
                       aux_s, aux_rows, aux_cols, aux_p, l, u, ctx);

            /* Extract the column vector */
            
            /* Iterate over all rows.. */
            
            for (rowk = l; rowk <= FLINT_MIN(u, colk + 1); rowk++)
            {
                for (i = iB[rowk]; i < iB[rowk + 1]; i++)
                {
                    mpoly_get_coeff(mat_entry(M, i, j, ctx), 
                                    R[rowk], B[i], ctx);
                }
            }

            mpoly_clear(Q, ctx);
            for (i = 0; i <= n + 1; i++)
                mpoly_clear(R[i], ctx);
            free(R);
        }
    }

    /* Copy row and column index sets */

    *rows = malloc(lenB * sizeof(mon_t));
    *cols = malloc(lenB * sizeof(mon_t));

    for (i = 0; i < lenB; i++)
    {
        (*rows)[i] = B[i];
        (*cols)[i] = B[i];
    }

    /* Clean up */

    k = (n + (d - 1)) / d + 1;  /* k = ceil(n / d) + 1 */

    for ( ; k <= u + 1; k++)
    {
        free(aux_rows[k]);
        free(aux_cols[k]);
        free(aux_p[k]);
        mat_csr_clear(aux[k], ctx);
        mat_csr_solve_clear(aux_s[k], ctx);
    }

    free(aux);
    free(aux_rows);
    free(aux_cols);
    free(aux_p);
    free(aux_s);

    free(B);
    free(iB);

    for (i = 0; i <= n; i++)
        mpoly_clear(dP[i], ctx);
    free(dP);
    mpoly_clear(dPdt, ctx);
}

