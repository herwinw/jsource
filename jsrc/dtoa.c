/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/* Jsoftware Copyright applies only to changes made by Jsoftware           */
/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991, 2000, 2001 by Lucent Technologies.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/

/* Please send bug reports to David M. Gay (dmg at acm dot org,
 * with " at " changed at "@" and " dot " changed to "."). */

/* On a machine with IEEE extended-precision registers, it is
 * necessary to specify double-precision (53-bit) rounding precision
 * before invoking strtod or dtoa.  If the machine uses (the equivalent
 * of) Intel 80x87 arithmetic, the call
 * _control87(PC_53, MCW_PC);
 * does this with many compilers.  Whether this or another call is
 * appropriate depends on the compiler; for this to work, it may be
 * necessary to #include "float.h" or another system-dependent header
 * file.
 */

/* strtod for IEEE-arithmetic machines.
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets errno to ERANGE).  With IEEE arithmetic, ties are
 * broken by the IEEE round-even rule.  Otherwise ties are broken by
 * biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 92-101].
 *
 * Modifications:
 *
 * 1. We only require IEEE double-precision
 *  arithmetic (not IEEE double-extended).
 * 2. We get by with floating-point arithmetic in a case that
 *  Clinger missed -- when we're computing d * 10^n
 *  for a small integer d and the integer n is not too
 *  much larger than 22 (the maximum integer k for which
 *  we can represent 10^k exactly), we may be able to
 *  compute (d*10^k) * 10^(e-k) with just one roundoff.
 * 3. Rather than a bit-at-a-time adjustment of the binary
 *  result in the hard case, we use floating-point
 *  arithmetic to determine the adjustment to within
 *  one bit; only in really hard cases do we need to
 *  compute a second residual.
 * 4. Because of 3., we don't need a large table of powers of 10
 *  for ten-to-e (just some small tables, e.g. of 10^k
 *  for 0 <= k <= 22).
 */

/* Options for use with J */
#define PRIVATE_MEM 8000

#ifdef DEBUG
#include "stdio.h"
#define Bug(x)                      \
    {                               \
        fprintf(stderr, "%s\n", x); \
        exit(1);                    \
    }
#endif

#include "stdlib.h"
#include "string.h"

#define PRIVATE_mem (long)((PRIVATE_MEM + sizeof(double) - 1) / sizeof(double))

#include "float.h"

#ifndef __MATH_H__
#include "math.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    double d;
    unsigned int L[2];
} U;

#define word0(x) ((U *)&x)->L[1]
#define word1(x) ((U *)&x)->L[0]
#define dval(x) ((U *)&x)->d

#define Exp_shift 20
#define Exp_shift1 20
#define Exp_msk1 0x100000
#define Exp_mask 0x7ff00000
#define P 53
#define Bias 1023
#define Exp_11 0x3ff00000
#define Frac_mask 0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask 0xfffff
#define Sign_bit 0x80000000
#define Log2P 1
#define Quick_max 14
#define Int_max 14

extern double rnd_prod(double, double), rnd_quot(double, double);

#define FFFFFFFF 0xffffffffUL

struct Bigint {
    struct Bigint *next;
    int k, maxwds, sign, wds;
    unsigned int x[1];
};

typedef struct Bigint Bigint;
#define HAVE_BIGINT
#include "dtoa.h"

static void *d2a_Malloc(struct dtoa_info *d2a, int k);
static Bigint *d2a_Balloc(struct dtoa_info *d2a, int k);
static void d2a_Bfree(struct dtoa_info *d2a, Bigint *v);

#define p5s (d2a->_p5s)
#define private_mem (d2a->_private_mem)
#define pmem_next (d2a->_pmem_next)
#define freelist (d2a->_freelist)

#define MALLOC(n) d2a_Malloc(d2a, n)
#define Balloc(k) d2a_Balloc(d2a, k)
#define Bfree(v) d2a_Bfree(d2a, v)
#define multadd(b, m, a) d2a_multadd(d2a, b, m, a)
#define i2b(i) d2a_i2b(d2a, i)
#define mult(a, b) d2a_mult(d2a, a, b)
#define nrv_alloc(s, rve, n) d2a_nrv_alloc(d2a, s, rve, n)
#define rv_alloc(n) d2a_rv_alloc(d2a, n)
#define d2b(d, e, b) d2a_d2b(d2a, d, e, b)
#define pow5mult(b, k) d2a_pow5mult(d2a, b, k)
#define diff(x, y) d2a_diff(d2a, x, y)
#define lshift(x, y) d2a_lshift(d2a, x, y)

static Bigint *
d2a_Balloc(struct dtoa_info *d2a, int k) {
    int x;
    Bigint *rv;
    unsigned int len;

    if (rv = freelist[k]) {
        freelist[k] = rv->next;
    } else {
        x   = 1 << k;
        len = (sizeof(Bigint) + (x - 1) * sizeof(unsigned int) + sizeof(double) - 1) / sizeof(double);
        if (pmem_next - private_mem + len <= PRIVATE_mem) {
            rv = (Bigint *)pmem_next;
            pmem_next += len;
        } else
            rv = (Bigint *)MALLOC(len * sizeof(double));
        rv->k      = k;
        rv->maxwds = x;
    }
    rv->sign = rv->wds = 0;
    return rv;
}

static void
d2a_Bfree(struct dtoa_info *d2a, Bigint *v) {
    if (v) {
        v->next        = freelist[v->k];
        freelist[v->k] = v;
    }
}

#define Bcopy(x, y) memcpy((char *)&x->sign, (char *)&y->sign, y->wds * sizeof(int) + 2 * sizeof(int))

static Bigint *
d2a_multadd(struct dtoa_info *d2a, Bigint *b, int m, int a) /* multiply by m and add a */
{
    int i, wds;
    unsigned int *x;
    unsigned long long carry, y;
    Bigint *b1;

    wds   = b->wds;
    x     = b->x;
    i     = 0;
    carry = a;
    do {
        y     = *x * (unsigned long long)m + carry;
        carry = y >> 32;
        *x++  = (unsigned int)(y & FFFFFFFF);
    } while (++i < wds);
    if (carry) {
        if (wds >= b->maxwds) {
            b1 = Balloc(b->k + 1);
            Bcopy(b1, b);
            Bfree(b);
            b = b1;
        }
        b->x[wds++] = (unsigned int)carry;
        b->wds      = wds;
    }
    return b;
}

static int
hi0bits(register unsigned int x) {
    register int k = 0;

    if (!(x & 0xffff0000)) {
        k = 16;
        x <<= 16;
    }
    if (!(x & 0xff000000)) {
        k += 8;
        x <<= 8;
    }
    if (!(x & 0xf0000000)) {
        k += 4;
        x <<= 4;
    }
    if (!(x & 0xc0000000)) {
        k += 2;
        x <<= 2;
    }
    if (!(x & 0x80000000)) {
        k++;
        if (!(x & 0x40000000)) return 32;
    }
    return k;
}

static int
lo0bits(unsigned int *y) {
    register int k;
    register unsigned int x = *y;

    if (x & 7) {
        if (x & 1) return 0;
        if (x & 2) {
            *y = x >> 1;
            return 1;
        }
        *y = x >> 2;
        return 2;
    }
    k = 0;
    if (!(x & 0xffff)) {
        k = 16;
        x >>= 16;
    }
    if (!(x & 0xff)) {
        k += 8;
        x >>= 8;
    }
    if (!(x & 0xf)) {
        k += 4;
        x >>= 4;
    }
    if (!(x & 0x3)) {
        k += 2;
        x >>= 2;
    }
    if (!(x & 1)) {
        k++;
        x >>= 1;
        if (!x) return 32;
    }
    *y = x;
    return k;
}

static Bigint *
d2a_i2b(struct dtoa_info *d2a, int i) {
    Bigint *b;

    b       = Balloc(1);
    b->x[0] = i;
    b->wds  = 1;
    return b;
}

static Bigint *
d2a_mult(struct dtoa_info *d2a, Bigint *a, Bigint *b) {
    Bigint *c;
    int k, wa, wb, wc;
    unsigned int *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
    unsigned int y;
    unsigned long long carry, z;

    if (a->wds < b->wds) {
        c = a;
        a = b;
        b = c;
    }
    k  = a->k;
    wa = a->wds;
    wb = b->wds;
    wc = wa + wb;
    if (wc > a->maxwds) k++;
    c = Balloc(k);
    for (x = c->x, xa = x + wc; x < xa; x++) *x = 0;
    xa  = a->x;
    xae = xa + wa;
    xb  = b->x;
    xbe = xb + wb;
    xc0 = c->x;
    for (; xb < xbe; xc0++) {
        if (y = *xb++) {
            x     = xa;
            xc    = xc0;
            carry = 0;
            do {
                z     = *x++ * (unsigned long long)y + *xc + carry;
                carry = z >> 32;
                *xc++ = (unsigned int)(z & FFFFFFFF);
            } while (x < xae);
            *xc = (unsigned int)carry;
        }
    }
    for (xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc)
        ;
    c->wds = wc;
    return c;
}

static Bigint *
d2a_pow5mult(struct dtoa_info *d2a, Bigint *b, int k) {
    Bigint *b1, *p5, *p51;
    int i;
    static const int p05[3] = {5, 25, 125};

    if (i = k & 3) b = multadd(b, p05[i - 1], 0);

    if (!(k >>= 2)) return b;
    if (!(p5 = p5s)) {
        /* first time */
        p5 = p5s = i2b(625);
        p5->next = 0;
    }
    for (;;) {
        if (k & 1) {
            b1 = mult(b, p5);
            Bfree(b);
            b = b1;
        }
        if (!(k >>= 1)) break;
        if (!(p51 = p5->next)) {
            p51 = p5->next = mult(p5, p5);
            p51->next      = 0;
        }
        p5 = p51;
    }
    return b;
}

static Bigint *
d2a_lshift(struct dtoa_info *d2a, Bigint *b, int k) {
    int i, k1, n, n1;
    Bigint *b1;
    unsigned int *x, *x1, *xe, z;

    n  = k >> 5;
    k1 = b->k;
    n1 = n + b->wds + 1;
    for (i = b->maxwds; n1 > i; i <<= 1) k1++;
    b1 = Balloc(k1);
    x1 = b1->x;
    for (i = 0; i < n; i++) *x1++ = 0;
    x  = b->x;
    xe = x + b->wds;
    if (k &= 0x1f) {
        k1 = 32 - k;
        z  = 0;
        do {
            *x1++ = *x << k | z;
            z     = *x++ >> k1;
        } while (x < xe);
        if (*x1 = z) ++n1;
    } else
        do *x1++ = *x++;
        while (x < xe);
    b1->wds = n1 - 1;
    Bfree(b);
    return b1;
}

static int
cmp(Bigint *a, Bigint *b) {
    unsigned int *xa, *xa0, *xb, *xb0;
    int i, j;

    i = a->wds;
    j = b->wds;
#ifdef DEBUG
    if (i > 1 && !a->x[i - 1]) Bug("cmp called with a->x[a->wds-1] == 0");
    if (j > 1 && !b->x[j - 1]) Bug("cmp called with b->x[b->wds-1] == 0");
#endif
    if (i -= j) return i;
    xa0 = a->x;
    xa  = xa0 + j;
    xb0 = b->x;
    xb  = xb0 + j;
    for (;;) {
        if (*--xa != *--xb) return *xa < *xb ? -1 : 1;
        if (xa <= xa0) break;
    }
    return 0;
}

static Bigint *
d2a_diff(struct dtoa_info *d2a, Bigint *a, Bigint *b) {
    Bigint *c;
    int i, wa, wb;
    unsigned int *xa, *xae, *xb, *xbe, *xc;
    unsigned long long borrow, y;

    i = cmp(a, b);
    if (!i) {
        c       = Balloc(0);
        c->wds  = 1;
        c->x[0] = 0;
        return c;
    }
    if (i < 0) {
        c = a;
        a = b;
        b = c;
        i = 1;
    } else
        i = 0;
    c       = Balloc(a->k);
    c->sign = i;
    wa      = a->wds;
    xa      = a->x;
    xae     = xa + wa;
    wb      = b->wds;
    xb      = b->x;
    xbe     = xb + wb;
    xc      = c->x;
    borrow  = 0;
    do {
        y      = (unsigned long long)*xa++ - *xb++ - borrow;
        borrow = y >> 32 & (unsigned int)1;
        *xc++  = (unsigned int)(y & FFFFFFFF);
    } while (xb < xbe);
    while (xa < xae) {
        y      = *xa++ - borrow;
        borrow = y >> 32 & (unsigned int)1;
        *xc++  = (unsigned int)(y & FFFFFFFF);
    }
    while (!*--xc) wa--;
    c->wds = wa;
    return c;
}

static Bigint *
d2a_d2b(struct dtoa_info *d2a, double d, int *e, int *bits) {
    Bigint *b;
    int de, k;
    unsigned int *x, y, z;
    int i;

#define d0 word0(d)
#define d1 word1(d)

    b = Balloc(1);
    x = b->x;

    z = d0 & Frac_mask;
    d0 &= 0x7fffffff; /* clear sign bit, which we ignore */
    if (de = (int)(d0 >> Exp_shift)) z |= Exp_msk1;
    if (y = d1) {
        if (k = lo0bits(&y)) {
            x[0] = y | z << (32 - k);
            z >>= k;
        } else
            x[0] = y;
#ifndef Sudden_Underflow
        i =
#endif
          b->wds = (x[1] = z) ? 2 : 1;
    } else {
#ifdef DEBUG
        if (!z) Bug("Zero passed to d2b");
#endif
        k    = lo0bits(&z);
        x[0] = z;
#ifndef Sudden_Underflow
        i =
#endif
          b->wds = 1;
        k += 32;
    }
    if (de) {
        *e    = de - Bias - (P - 1) + k;
        *bits = P - k;
    } else {
        *e    = de - Bias - (P - 1) + 1 + k;
        *bits = 32 * i - hi0bits(x[i - 1]);
    }
    return b;
}
#undef d0
#undef d1

static const double tens[] = {1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
                              1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

static const double bigtens[]  = {1e16, 1e32, 1e64, 1e128, 1e256};
static const double tinytens[] = {
  1e-16, 1e-32, 1e-64, 1e-128, 9007199254740992. * 9007199254740992.e-256
  /* = 2^106 * 1e-53 */
};
/* The factor of 2^53 in tinytens[4] helps us avoid setting the underflow */
/* flag unnecessarily.  It leads to a song and dance at the end of strtod. */
#define n_bigtens 5

static int
quorem(Bigint *b, Bigint *S) {
    int n;
    unsigned int *bx, *bxe, q, *sx, *sxe;
    unsigned long long borrow, carry, y, ys;

    n = S->wds;
#ifdef DEBUG
    /*debug*/ if (b->wds > n)
        /*debug*/ Bug("oversize b in quorem");
#endif
    if (b->wds < n) return 0;
    sx  = S->x;
    sxe = sx + --n;
    bx  = b->x;
    bxe = bx + n;
    q   = *bxe / (*sxe + 1); /* ensure q <= true quotient */
#ifdef DEBUG
    /*debug*/ if (q > 9)
        /*debug*/ Bug("oversized quotient in quorem");
#endif
    if (q) {
        borrow = 0;
        carry  = 0;
        do {
            ys     = *sx++ * (unsigned long long)q + carry;
            carry  = ys >> 32;
            y      = *bx - (ys & FFFFFFFF) - borrow;
            borrow = y >> 32 & (unsigned int)1;
            *bx++  = (unsigned int)(y & FFFFFFFF);
        } while (sx <= sxe);
        if (!*bxe) {
            bx = b->x;
            while (--bxe > bx && !*bxe) --n;
            b->wds = n;
        }
    }
    if (cmp(b, S) >= 0) {
        q++;
        borrow = 0;
        carry  = 0;
        bx     = b->x;
        sx     = S->x;
        do {
            ys     = *sx++ + carry;
            carry  = ys >> 32;
            y      = *bx - (ys & FFFFFFFF) - borrow;
            borrow = y >> 32 & (unsigned int)1;
            *bx++  = (unsigned int)(y & FFFFFFFF);
        } while (sx <= sxe);
        bx  = b->x;
        bxe = bx + n;
        if (!*bxe) {
            while (--bxe > bx && !*bxe) --n;
            b->wds = n;
        }
    }
    return q;
}

static char *
d2a_rv_alloc(struct dtoa_info *d2a, int i)
/* i is guarenteed i >= 0 by caller so the cast to unsigned below is safe */
{
    if (i > d2a->ndp) longjmp(d2a->_env, 2); /* this shouldn't happen? */
    return d2a->result;
}

static char *
d2a_nrv_alloc(struct dtoa_info *d2a, char *s, char **rve, int n) {
    char *rv, *t;

    t = rv = rv_alloc(n);
    /*while(*t = *s++) t++;*/
    while (*s) { *t++ = *s++; } /* don't copy the NUL */
    if (rve) *rve = t;
    return rv;
}

/* dtoa for IEEE arithmetic (dmg): convert double to ASCII string.
 *
 * Inspired by "How to Print Floating-Point Numbers Accurately" by
 * Guy L. Steele, Jr. and Jon L. White [Proc. ACM SIGPLAN '90, pp. 112-126].
 *
 * Modifications:
 * 1. Rather than iterating, we use a simple numeric overestimate
 *    to determine k = floor(log10(d)).  We scale relevant
 *    quantities using O(log2(k)) rather than O(k) multiplications.
 * 2. For some modes > 2 (corresponding to ecvt and fcvt), we don't
 *    try to generate digits strictly left to right.  Instead, we
 *    compute with fewer bits and propagate the carry if necessary
 *    when rounding the final digit up.  This is often faster.
 * 3. Under the assumption that input will be rounded nearest,
 *    mode 0 renders 1e23 as 1e23 rather than 9.999999999999999e22.
 *    That is, we allow equality in stopping tests when the
 *    round-nearest rule will give the same floating-point value
 *    as would satisfaction of the stopping test with strict
 *    inequality.
 * 4. We remove common factors of powers of 2 from relevant
 *    quantities.
 * 5. When converting floating-point integers less than 1e16,
 *    we use floating-point arithmetic rather than resorting
 *    to multiple-precision integers.
 * 6. When asked to produce fewer than 15 digits, we first try
 *    to get by with floating-point arithmetic; we resort to
 *    multiple-precision integer arithmetic only if we cannot
 *    guarantee that the floating-point calculation has given
 *    the correctly rounded result.  For k requested digits and
 *    "uniformly" distributed input, the probability is
 *    something like 10^(k-15) that we must resort to the int
 *    calculation.
 */

static char *
d2a_dtoa(struct dtoa_info *d2a, double d, int mode, int ndigits, int *decpt, int *sign, char **rve) {
    /* Arguments ndigits, decpt, sign are similar to those
    of ecvt and fcvt; trailing zeros are suppressed from
    the returned string.  If not null, *rve is set to point
    to the end of the return value.  If d is +-Infinity or NaN,
    then *decpt is set to 9999.

    mode:
     0 ==> shortest string that yields d when read in
      and rounded to nearest.
     1 ==> like 0, but with Steele & White stopping rule;
      e.g. with IEEE P754 arithmetic , mode 0 gives
      1e23 whereas mode 1 gives 9.999999999999999e22.
     2 ==> max(1,ndigits) significant digits.  This gives a
      return value similar to that of ecvt, except
      that trailing zeros are suppressed.
     3 ==> through ndigits past the decimal point.  This
      gives a return value similar to that from fcvt,
      except that trailing zeros are suppressed, and
      ndigits can be negative.
     4,5 ==> similar to 2 and 3, respectively, but (in
      round-nearest mode) with the tests of mode 0 to
      possibly return a shorter string that rounds to d.
     6-9 ==> Debugging modes similar to mode - 4:  don't try
      fast floating-point estimate (if applicable).

     Values of mode other than 0-9 are treated as mode 0.

     Sufficient space is allocated to the return value
     to hold the suppressed trailing zeros.
    */

    int bbits, b2, b5, be, dig, i, ieps, ilim, ilim0, ilim1, j, j1, k, k0, k_check, leftright, m2, m5, s2, s5,
      spec_case, try_quick;
    int L;
    int denorm;
    unsigned int x;
    Bigint *b, *b1, *delta, *mlo, *mhi, *S;
    double d2, ds, eps;
    char *s, *s0;

    if (word0(d) & Sign_bit) {
        /* set sign for everything, including 0's and NaNs */
        *sign = 1;
        word0(d) &= ~Sign_bit; /* clear sign bit */
    } else
        *sign = 0;

    if ((word0(d) & Exp_mask) == Exp_mask) {
        /* Infinity or NaN */
        *decpt = 9999;
        if (!word1(d) && !(word0(d) & 0xfffff)) return nrv_alloc("Infinity", rve, 8);
        return nrv_alloc("NaN", rve, 3);
    }
    if (!dval(d)) {
        *decpt = 1;
        return nrv_alloc("0", rve, 1);
    }

    b = d2b(dval(d), &be, &bbits);
    if (i = (int)(word0(d) >> Exp_shift1 & (Exp_mask >> Exp_shift1))) {
        dval(d2) = dval(d);
        word0(d2) &= Frac_mask1;
        word0(d2) |= Exp_11;

        /* log(x) ~=~ log(1.5) + (x-1.5)/1.5
         * log10(x)  =  log(x) / log(10)
         *  ~=~ log(1.5)/log(10) + (x-1.5)/(1.5*log(10))
         * log10(d) = (i-Bias)*log(2)/log(10) + log10(d2)
         *
         * This suggests computing an approximation k to log10(d) by
         *
         * k = (i - Bias)*0.301029995663981
         * + ( (d2-1.5)*0.289529654602168 + 0.176091259055681 );
         *
         * We want k to be too large rather than too small.
         * The error in the first-order Taylor series approximation
         * is in our favor, so we just round up the constant enough
         * to compensate for any error in the multiplication of
         * (i - Bias) by 0.301029995663981; since |i - Bias| <= 1077,
         * and 1077 * 0.30103 * 2^-52 ~=~ 7.2e-14,
         * adding 1e-13 to the constant term more than suffices.
         * Hence we adjust the constant term to 0.1760912590558.
         * (We could get a more accurate k by invoking log10,
         *  but this is probably not worthwhile.)
         */

        i -= Bias;
        denorm = 0;
    } else {
        /* d is denormalized */

        i        = bbits + be + (Bias + (P - 1) - 1);
        x        = i > 32 ? word0(d) << (64 - i) | word1(d) >> (i - 32) : word1(d) << (32 - i);
        dval(d2) = x;
        word0(d2) -= 31 * Exp_msk1; /* adjust exponent */
        i -= (Bias + (P - 1) - 1) + 1;
        denorm = 1;
    }
    ds = (dval(d2) - 1.5) * 0.289529654602168 + 0.1760912590558 + i * 0.301029995663981;
    k  = (int)ds;
    if (ds < 0. && ds != k) k--; /* want k = floor(ds) */
    k_check = 1;
    if (k >= 0 && k <= Ten_pmax) {
        if (dval(d) < tens[k]) k--;
        k_check = 0;
    }
    j = bbits - i - 1;
    if (j >= 0) {
        b2 = 0;
        s2 = j;
    } else {
        b2 = -j;
        s2 = 0;
    }
    if (k >= 0) {
        b5 = 0;
        s5 = k;
        s2 += k;
    } else {
        b2 -= k;
        b5 = -k;
        s5 = 0;
    }
    if (mode < 0 || mode > 9) mode = 0;

    try_quick = 1;

    if (mode > 5) {
        mode -= 4;
        try_quick = 0;
    }
    leftright = 1;
    switch (mode) {
        case 0:
        case 1:
            ilim = ilim1 = -1;
            i            = 18;
            ndigits      = 0;
            break;
        case 2:
            leftright = 0;
            /* no break */
        case 4:
            if (ndigits <= 0) ndigits = 1;
            ilim = ilim1 = i = ndigits;
            break;
        case 3:
            leftright = 0;
            /* no break */
        case 5:
            i     = ndigits + k + 1;
            ilim  = i;
            ilim1 = i - 1;
            if (i <= 0) i = 1;
    }
    s = s0 = rv_alloc(i);

    if (ilim >= 0 && ilim <= Quick_max && try_quick) {
        /* Try to get by with floating-point arithmetic. */

        i        = 0;
        dval(d2) = dval(d);
        k0       = k;
        ilim0    = ilim;
        ieps     = 2; /* conservative */
        if (k > 0) {
            ds = tens[k & 0xf];
            j  = k >> 4;
            if (j & Bletch) {
                /* prevent overflows */
                j &= Bletch - 1;
                dval(d) /= bigtens[n_bigtens - 1];
                ieps++;
            }
            for (; j; j >>= 1, i++)
                if (j & 1) {
                    ieps++;
                    ds *= bigtens[i];
                }
            dval(d) /= ds;
        } else if (j1 = -k) {
            dval(d) *= tens[j1 & 0xf];
            for (j = j1 >> 4; j; j >>= 1, i++)
                if (j & 1) {
                    ieps++;
                    dval(d) *= bigtens[i];
                }
        }
        if (k_check && dval(d) < 1. && ilim > 0) {
            if (ilim1 <= 0) goto fast_failed;
            ilim = ilim1;
            k--;
            dval(d) *= 10.;
            ieps++;
        }
        dval(eps) = ieps * dval(d) + 7.;
        word0(eps) -= (P - 1) * Exp_msk1;
        if (ilim == 0) {
            S = mhi = 0;
            dval(d) -= 5.;
            if (dval(d) > dval(eps)) goto one_digit;
            if (dval(d) < -dval(eps)) goto no_digits;
            goto fast_failed;
        }
        if (leftright) {
            /* Use Steele & White method of only
             * generating digits needed.
             */
            dval(eps) = 0.5 / tens[ilim - 1] - dval(eps);
            for (i = 0;;) {
                L = (int)dval(d);
                dval(d) -= L;
                *s++ = '0' + (int)L;
                if (dval(d) < dval(eps)) goto ret1;
                if (1. - dval(d) < dval(eps)) goto bump_up;
                if (++i >= ilim) break;
                dval(eps) *= 10.;
                dval(d) *= 10.;
            }
        } else {
            /* Generate ilim digits, then fix them up. */
            dval(eps) *= tens[ilim - 1];
            for (i = 1;; i++, dval(d) *= 10.) {
                L = (int)(dval(d));
                if (!(dval(d) -= L)) ilim = i;
                *s++ = '0' + (int)L;
                if (i == ilim) {
                    if (dval(d) > 0.5 + dval(eps))
                        goto bump_up;
                    else if (dval(d) < 0.5 - dval(eps)) {
                        while (*--s == '0')
                            ;
                        s++;
                        goto ret1;
                    }
                    break;
                }
            }
        }
    fast_failed:
        s       = s0;
        dval(d) = dval(d2);
        k       = k0;
        ilim    = ilim0;
    }

    /* Do we have a "small" integer? */

    if (be >= 0 && k <= Int_max) {
        /* Yes. */
        ds = tens[k];
        if (ndigits < 0 && ilim <= 0) {
            S = mhi = 0;
            if (ilim < 0 || dval(d) <= 5 * ds) goto no_digits;
            goto one_digit;
        }
        for (i = 1;; i++, dval(d) *= 10.) {
            L = (int)(dval(d) / ds);
            dval(d) -= L * ds;
            *s++ = '0' + (int)L;
            if (!dval(d)) { break; }
            if (i == ilim) {
                dval(d) += dval(d);
                if (dval(d) > ds || dval(d) == ds && L & 1) {
                bump_up:
                    while (*--s == '9')
                        if (s == s0) {
                            k++;
                            *s = '0';
                            break;
                        }
                    ++*s++;
                }
                break;
            }
        }
        goto ret1;
    }

    m2  = b2;
    m5  = b5;
    mhi = mlo = 0;
    if (leftright) {
        i = denorm ? be + (Bias + (P - 1) - 1 + 1) : 1 + P - bbits;
        b2 += i;
        s2 += i;
        mhi = i2b(1);
    }
    if (m2 > 0 && s2 > 0) {
        i = m2 < s2 ? m2 : s2;
        b2 -= i;
        m2 -= i;
        s2 -= i;
    }
    if (b5 > 0) {
        if (leftright) {
            if (m5 > 0) {
                mhi = pow5mult(mhi, m5);
                b1  = mult(mhi, b);
                Bfree(b);
                b = b1;
            }
            if (j = b5 - m5) b = pow5mult(b, j);
        } else
            b = pow5mult(b, b5);
    }
    S = i2b(1);
    if (s5 > 0) S = pow5mult(S, s5);

    /* Check for special case that d is a normalized power of 2. */

    spec_case = 0;
    if ((mode < 2 || leftright)) {
        if (!word1(d) && !(word0(d) & Bndry_mask) && word0(d) & (Exp_mask & ~Exp_msk1)) {
            /* The special case */
            b2 += Log2P;
            s2 += Log2P;
            spec_case = 1;
        }
    }

    /* Arrange for convenient computation of quotients:
     * shift left if necessary so divisor has 4 leading 0 bits.
     *
     * Perhaps we should just compute leading 28 bits of S once
     * and for all and pass them and a shift to quorem, so it
     * can do shifts and ors to compute the numerator for q.
     */
    if (i = ((s5 ? 32 - hi0bits(S->x[S->wds - 1]) : 1) + s2) & 0x1f) i = 32 - i;
    if (i > 4) {
        i -= 4;
        b2 += i;
        m2 += i;
        s2 += i;
    } else if (i < 4) {
        i += 28;
        b2 += i;
        m2 += i;
        s2 += i;
    }
    if (b2 > 0) b = lshift(b, b2);
    if (s2 > 0) S = lshift(S, s2);
    if (k_check) {
        if (cmp(b, S) < 0) {
            k--;
            b = multadd(b, 10, 0); /* we botched the k estimate */
            if (leftright) mhi = multadd(mhi, 10, 0);
            ilim = ilim1;
        }
    }
    if (ilim <= 0 && (mode == 3 || mode == 5)) {
        if (ilim < 0 || cmp(b, S = multadd(S, 5, 0)) <= 0) {
            /* no digits, fcvt style */
        no_digits:
            k = -1 - ndigits;
            goto ret;
        }
    one_digit:
        *s++ = '1';
        k++;
        goto ret;
    }
    if (leftright) {
        if (m2 > 0) mhi = lshift(mhi, m2);

        /* Compute mlo -- check for special case
         * that d is a normalized power of 2.
         */

        mlo = mhi;
        if (spec_case) {
            mhi = Balloc(mhi->k);
            Bcopy(mhi, mlo);
            mhi = lshift(mhi, Log2P);
        }

        for (i = 1;; i++) {
            dig = quorem(b, S) + '0';
            /* Do we yet have the shortest decimal string
             * that will round to d?
             */
            j     = cmp(b, mlo);
            delta = diff(S, mhi);
            j1    = delta->sign ? 1 : cmp(b, delta);
            Bfree(delta);
            if (j1 == 0 && mode != 1 && !(word1(d) & 1)) {
                if (dig == '9') goto round_9_up;
                if (j > 0) dig++;
                *s++ = dig;
                goto ret;
            }
            if (j < 0 || j == 0 && mode != 1 && !(word1(d) & 1)) {
                if (!b->x[0] && b->wds <= 1) { goto accept_dig; }
                if (j1 > 0) {
                    b  = lshift(b, 1);
                    j1 = cmp(b, S);
                    if ((j1 > 0 || j1 == 0 && dig & 1) && dig++ == '9') goto round_9_up;
                }
            accept_dig:
                *s++ = dig;
                goto ret;
            }
            if (j1 > 0) {
                if (dig == '9') { /* possible if i == 1 */
                round_9_up:
                    *s++ = '9';
                    goto roundoff;
                }
                *s++ = dig + 1;
                goto ret;
            }
            *s++ = dig;
            if (i == ilim) break;
            b = multadd(b, 10, 0);
            if (mlo == mhi)
                mlo = mhi = multadd(mhi, 10, 0);
            else {
                mlo = multadd(mlo, 10, 0);
                mhi = multadd(mhi, 10, 0);
            }
        }
    } else
        for (i = 1;; i++) {
            *s++ = dig = quorem(b, S) + '0';
            if (!b->x[0] && b->wds <= 1) { goto ret; }
            if (i >= ilim) break;
            b = multadd(b, 10, 0);
        }

    /* Round off last digit */
    b = lshift(b, 1);
    j = cmp(b, S);
    if (j > 0 || j == 0 && dig & 1) {
    roundoff:
        while (*--s == '9')
            if (s == s0) {
                k++;
                *s++ = '1';
                goto ret;
            }
        ++*s++;
    } else {
        /* trimzeros: */ /* since it's unreferenced */
        while (*--s == '0')
            ;
        s++;
    }
ret:
    Bfree(S);
    if (mhi) {
        if (mlo && mlo != mhi) Bfree(mlo);
        Bfree(mhi);
    }
ret1:
    Bfree(b);
    /* *s = 0; */ /* don't NUL terminate */
    *decpt = k + 1;
    if (rve) *rve = s;
    return s0;
}

#undef diff
#undef mult
#undef P
#undef MALLOC
#include "j.h"

static void *
d2a_Malloc(struct dtoa_info *di, int n) {
    A z;
    J jt = (J)di->jt;

    z = jtga(jt, LIT, n, 1, 0);
    if (!z || jt->jerr) longjmp(di->_env, 1);
    return AV(z);
}

B
jtecvtinit(J jt) {
    A x;
    struct dtoa_info *di;
    if (jt->dtoa) return 1;
    GATV0(x, LIT, sizeof(struct dtoa_info), 1);
    di             = (struct dtoa_info *)AV(x);
    di->_p5s       = 0;
    di->_pmem_next = di->_private_mem;
    memset(di->_private_mem, 0, sizeof(di->_private_mem));
    memset(di->_freelist, 0, sizeof(di->_freelist));
    di->jt = jt;
    ras(x);
    jt->dtoa = di;
    return 1;
}

/* uses dtoa and behaves like ecvt (well, ecvt_r) */
/* this writes exactly ndp bytes at dest */
B
jtecvt(J jt, D dw, I ndp, int *decpt, int *sign, C *dest) {
    struct dtoa_info *di = (struct dtoa_info *)jt->dtoa;
    C *y, *z = 0;

    ASSERTSYS(ndp <= INT_MAX, "jtecvt: too long");
    di->ndp    = (int)ndp;
    di->result = dest;
    y          = d2a_dtoa(di, dw, 2, (int)ndp, decpt, sign, (char **)&z);
    RZ(y && z);
    memset(z, '0', ndp - (z - y));
    return 1;
}

#ifdef __cplusplus
}
#endif
