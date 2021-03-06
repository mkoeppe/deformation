/******************************************************************************

    Copyright (C) 2010, 2011 Sebastian Pancratz

******************************************************************************/

#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "generics.h"
#include "mat.h"
#include "gmconnection.h"
#include "gmde.h"

#include "flint/flint.h"
#include "flint/fmpz_poly.h"
#include "flint/fmpq_poly.h"
#include "flint/fmpz_poly_q.h"
#include "flint/padic.h"
#include "flint/padic_mat.h"

int main(void)
{
    char *str;  /* String for the input polynomial P */
    mpoly_t P;  /* Input polynomial P */
    long n;     /* Number of variables minus one */
    long K;     /* Required t-adic precision */
    long N, Nw;
    long i, b;

    mat_t M;
    ctx_t ctxM;

    mon_t *rows, *cols;

    padic_mat_struct *C;
    fmpz_t p;

    printf("valuations... \n");
    fflush(stdout);

    /* Example 3-1-1 */
    /* str = "3  [3 0 0] [0 3 0] [0 0 3] (2  0 1)[1 1 1]"; */

    /* Example 4-4-2 */
    /* str = "4  [4 0 0 0] [0 4 0 0] [0 0 4 0] [0 0 0 4] (2  0 1)[3 1 0 0] (2  0 1)[1 0 1 2] (2  0 1)[0 1 0 3]"; */

    /* Example 3-3-6 */
    /* str = "3  [3 0 0] [0 3 0] [0 0 3] (2  0 314)[2 1 0] (2  0 42)[0 2 1] (2  0 271)[1 0 2] (2  0 -23)[1 1 1]"; */

    /* Example 3-3-2 */
    /* str = "3  [3 0 0] [0 3 0] [0 0 3] (2  0 1)[2 1 0] (2  0 1)[0 2 1] (2  0 1)[1 0 2]"; */

    /* Example ... */
    /* str = "4  [4 0 0 0] [0 4 0 0] [0 0 4 0] [0 0 0 4] (2  0 1)[1 1 1 1]"; */

    /* Cubic surface from AKR */
    str = "4  (1  3)[0 3 0 0] (2  0 3)[0 1 2 0] "
          "(2  0 -1)[1 1 1 0] (2  0 3)[1 1 0 1] "
          "(2  0 -1)[2 1 0 0] [0 0 3 0] (2  0 -1)[1 0 2 0] "
          "(1  2)[0 0 0 3] [3 0 0 0]";

    n  = atoi(str) - 1;
    N  = 10;
    Nw = 22;
    K  = 616;

    ctx_init_fmpz_poly_q(ctxM);

    mpoly_init(P, n + 1, ctxM);
    mpoly_set_str(P, str, ctxM);

    printf("P = "), mpoly_print(P, ctxM), printf("\n");

    b = gmc_basis_size(n, mpoly_degree(P, -1, ctxM));

    mat_init(M, b, b, ctxM);
    gmc_compute(M, &rows, &cols, P, ctxM);
    mat_print(M, ctxM);
    printf("\n");

    fmpz_init(p);
    fmpz_set_ui(p, 5);

    gmde_solve(&C, K, p, N, Nw, M, ctxM);

    printf("Valuations\n");

    for (i = 0; i < K; i++)
    {
        long v = padic_mat_val(C + i);

        if (v < LONG_MAX)
            printf("  i = %ld val = %ld val/log(i) = %f\n", i, v, 
                (i > 1) ? (double) v / log(i) : 0);
        else
            printf("  i = %ld val = +infty\n", i);
    }

    fmpz_clear(p);
    mpoly_clear(P, ctxM);
    mat_clear(M, ctxM);
    for (i = 0; i < K; i++)
        padic_mat_clear(C + i);
    free(C);
    ctx_clear(ctxM);

    return EXIT_SUCCESS;
}

