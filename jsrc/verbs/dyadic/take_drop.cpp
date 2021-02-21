
#include <algorithm>
#include <iterator>

#include "array.hpp"

/** @file */

A
jtbehead(J jt, A w) {
    FPREFIP;
    return jtdrop(jtinplace, zeroionei(1), w);
}
A
jtcurtail(J jt, A w) {
    FPREFIP;
    return jtdrop(jtinplace, num(-1), w);
}

A
jtshift1(J jt, A w) {
    return jtdrop(jt, num(-1), jtover(jt, num(1), w));
}

static A
jttk0(J jt, B b, A a, A w) {
    A z;
    I k, m = 0, n, p, r, *s, *u;
    r = AR(w);
    n = AN(a);
    u = AV(a);
    if (!b) {
        PRODX(m, n, u, 1) ASSERT(m > IMIN, EVLIMIT);
        PRODX(m, r - n, n + AS(w), ABS(m))
    }
    GA(z, AT(w), m, r, AS(w));
    s = AS(z);
    DO(n, p = u[i]; ASSERT(p > IMIN, EVLIMIT); *s++ = ABS(p););
    if (m) {
        k = bpnoun(AT(w));
        mvc(k * m, AV(z), k, jt->fillv);
    }
    return z;
}

static array
jttks(J jt, array a, array w) { // take_sparse
    PROLOG(0092);
    array x, y, z;
    I an, r, *s, *u, *v;
    P *wp, *zp;
    an = AN(a);
    u  = AV(a);
    r  = AR(w);
    s  = AS(w);
    GASPARSE(z, AT(w), 1, r, s);
    v = AS(z);
    DO(an, v[i] = ABS(u[i]););
    zp = PAV(z);
    wp = PAV(w); // pointer to array values

    if (an <= r) {
        RZ(a = vec(INT, r, s));
        MCISH(AV(a), u, an);
    }  // vec is not virtual

    auto [m, q] = [&] {
        array const a1 = SPA(wp, a);
        return std::pair{AN(a1), jtpaxis(jt, r, a1)};
    } ();

    RZ(a = jtfrom(jt, q, a));
    u = AV(a);
    RZ(y = jtfrom(jt, q, shape(jt, w)));
    s = AV(y);

    // TODO: rename b when we figure out what it is doing
    auto const b = std::mismatch(u + m, u + r, s + m).first != u + r; 

    if (b) {
        jt->fill = SPA(wp, e);
        x        = irs2(vec(INT, r - m, m + u), SPA(wp, x), 0L, 1L, -1L, reinterpret_cast<AF>(jttake));
        jt->fill = 0;
        RZ(x);
    }  // fill cannot be virtual
    else
        x = SPA(wp, x);
    
    // TODO: rename c when we figure out what it is doing
    if (auto const c = std::mismatch(u, u + m, s).first != u + m; c) {
        A j;
        C *xv, *yv;
        I d, i, *iv, *jv, k, n, t;
        d = 0;
        t = AT(x);
        k = jtaii(jt, x) << bplg(t);
        q = SPA(wp, i);
        SETIC(q, n);
        GATV(j, INT, AN(q), AR(q), AS(q));
        jv = AV(j);
        iv = AV(q);
        GA(y, t, AN(x), AR(x), AS(x));
        yv = CAV(y);
        xv = CAV(x);
        for (i = 0; i < n; ++i) {
            
            // this is std::mismatch3 (or std::zip_find3)
            bool cc = 0;
            for (int64_t i = 0; i < m; ++i) {
                t = u[i]; 
                if (0 > t ? iv[i] < t + s[i] : iv[i] >= t) {
                    cc = true;
                    break;
                }
            }

            if (!cc) {
                ++d;
                memcpy(yv, xv, k);
                yv += k;
                // TODO: use algorithm created above
                DO(m, t = u[i]; *jv++ = 0 > t ? iv[i] - (t + s[i]) : iv[i];);
            }
            iv += m;
            xv += k;
        }
        SPB(zp, i, d < n ? jttake(jt, jtsc(jt, d), j) : j);
        SPB(zp, x, d < n ? jttake(jt, jtsc(jt, d), y) : y);
    } else {
        SPB(zp, i, jtca(jt, SPA(wp, i)));
        SPB(zp, x, b ? x : jtca(jt, x));
    }
    SPB(zp, a, jtca(jt, SPA(wp, a)));
    SPB(zp, e, jtca(jt, SPA(wp, e)));
    EPILOG(z);
} /* take on sparse array w */

// general take routine.  a is result frame followed by signed take values i. e. shape of result, w is array
static A
jttk(J jt, A a, A w) {
    PROLOG(0093);
    A y, z;
    B b = 0;
    C *yv, *zv;
    I c, d, dy, dz, e, i, k, m, n, p, q, r, *s, t, *u;
    n = AN(a);
    u = AV(a);
    r = AR(w);
    s = AS(w);
    t = AT(w);
    if ((t & SPARSE) != 0) return jttks(jt, a, w);
    DO(
      n, if (!u[i]) {
          b = 1;
          break;
      });
    if (!b)
        DO(
          r - n, if (!s[n + i]) {
              b = 1;
              break;
          });                                         // if empty take, or take from empty cell, set b
    if (((b - 1) & AN(w)) == 0) return tk0(b, a, w);  // this handles empty w, so PROD OK below   b||!AN(w)
    k = bpnoun(t);
    z = w;
    c = q = 1;  // c will be #cells for this axis
    // process take one axis at a time
    for (i = 0; i < n; ++i) {
        I itemsize;
        c *= q;
        p = u[i];
        q = ABS(p);
        m = s[i];                                  // q=length of take can be IMIN out of this   m=length of axis
        if (q != m) {                              // if axis unchanged, skip it.  This includes the first axis
            PROD(itemsize, r - i - 1, s + i + 1);  // size of item of cell
            DPMULDE(c * itemsize, q, d);
            GA(y, t, d, r, AS(z));
            AS(y)[i] = q;  // this catches q=IMIN: mult error or GA error   d=#cells*itemsize*#taken items
            if (q > m) mvc(k * AN(y), CAV(y), k, jt->fillv);  // overtake - fill the whole area
            itemsize *= k;
            e  = itemsize * MIN(m, q);  //  itemsize=in bytes; e=total bytes moved per item
            dy = itemsize * q;
            yv = CAV(y);
            dz = itemsize * m;
            zv = CAV(z);
            m -= q;
            I yzdiff = dy - dz;
            yv += REPSGN(p & m) & yzdiff;
            zv -= REPSGN(p & -m) & yzdiff;
            DQ(c, memcpy(yv, zv, e); yv += dy; zv += dz;);
            z = y;
        }
    }
    EPILOG(z);
}

A
jttake(J jt, A a, A w) {
    A s;
    I acr, af, ar, n, *v, wcr, wf, wr;
    FPREFIP;
    if (!(a && w)) return 0;
    I wt = AT(w);  // wt=type of w
    if ((SPARSE & AT(a)) != 0) RZ(a = jtdenseit(jt, a));
    if (!(SPARSE & wt)) RZ(w = jtsetfv(jt, w, w));
    ar  = AR(a);
    acr = jt->ranks >> RANKTX;
    acr = ar < acr ? ar : acr;
    af  = ar - acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
    wr  = AR(w);
    wcr = (RANKT)jt->ranks;
    wcr = wr < wcr ? wr : wcr;
    wf  = wr - wcr;
    RESETRANK;
    if (((af - 1) & (acr - 2)) >= 0) {
        s = rank2ex(a, w, UNUSED_VALUE, MIN(acr, 1), wcr, acr, wcr, reinterpret_cast<AF>(jttake));  // if multiple x values, loop over them
                                                                              // af>0 or acr>1
        // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication (if there
        // was fill), so we don't pass pristinity through  We overwrite w because it is no longer in use
        PRISTCLRF(w)
        return s;
    }
    // canonicalize x
    n = AN(a);                              // n = #axes in a
    ASSERT(BETWEENC(n, 1, wcr), EVLENGTH);  // if y is not atomic, a must not have extra axes  wcr==0 is always true
    I* RESTRICT ws = AS(w);                 // ws->shape of w
    RZ(s =
         jtvib(jt, a));  // convert input to integer, auditing for illegal values; and convert infinities to IMAX/-IMAX
    // if the input was not INT/bool, we go through and replace any infinities with the length of the axis.  If we do
    // this, we have to clone the area, because vib might return a canned value
    if (!(AT(a) & (B01 + INT))) {
        I i;
        for (i = 0; i < AN(s); ++i) {
            I m  = IAV(s)[i];
            I ms = REPSGN(m);
            if ((m ^ ms) - ms == IMAX) break;
        }  // see if there are infinities.  They are IMAX/-IMAX, so take abc & see if result is IMAX
        if (i < AN(s)) {
            s = jtca(jt, s);
            if (!(AT(a) & FL))
                RZ(a = jtcvt(
                     jt, FL, a));  // copy area we are going to change; put a in a form where we can recognize infinity
            for (; i < AN(s); ++i) {
                if (DAV(a)[i] == IMIN)
                    IAV(s)[i] = IMIN;
                else if (INF(DAV(a)[i]))
                    IAV(s)[i] = wcr ? ws[wf + i] : 1;
            }  // kludge.  The problem is which huge values to consider infinite.  This is how it was done
        }
    }
    a = s;
    // correct if(!(ar|wf|(SPARSE&wt)|!wcr|(AFLAG(w)&(AFNJA)))){  // if there is only 1 take axis, w has no frame and is
    // not atomic
    if (!(ar | wf | ((NOUN & ~(DIRECT | RECURSIBLE)) & wt) | !wcr |
          (AFLAG(w) & (AFNJA)))) {  // if there is only 1 take axis, w has no frame and is not atomic  NJAwhy
        // if the length of take is within the bounds of the first axis
        I tklen   = IAV(a)[0];                    // get the one number in a, the take amount
        I tkasign = REPSGN(tklen);                // 0 if tklen nonneg, ~0 if neg
        I nitems  = ws[0];                        // number of items of w
        I tkabs   = (tklen ^ tkasign) - tkasign;  // ABS(tklen)
        if ((UI)tkabs <= (UI)nitems) {  // if this is not an overtake...  (unsigned to handle overflow, if tklen=IMIN).
            // calculate offset
            I woffset = tkasign & (tklen + nitems);  // x+#y if x neg, 0 if x pos
            // get length of a cell of w
            I wcellsize;
            PROD(wcellsize, wr - 1, ws + 1);  // size of a cell in atoms of w
            I offset = woffset * wcellsize;   // offset in atoms of the virtual data
            // allocate virtual block, passing in the in-place status from w
            RZ(s = virtualip(w, offset, wr));  // allocate block
            // fill in shape.  Note that s and w may be the same block, so ws is destroyed
            I* RESTRICT ss = AS(s);
            ss[0]          = tkabs;
            DO(wr - 1, ss[i + 1] = ws[i + 1];);  // shape of virtual matches shape of w except for #items
            AN(s) = tkabs * wcellsize;           // install # atoms
            // virtual block does not affect pristinity of w
            return s;
        }
    }
    // full processing for more complex a
    if ((-wcr & (wf - 1)) >= 0) {  // if y is an atom, or y has multiple cells:
        RZ(s = vec(INT, wf + n, AS(w)));
        v = wf + AV(s);  // s is a block holding shape of a cell of input to the result: w-frame followed by #$a axes,
                         // all taken from w.  vec is never virtual
        if (!wcr) {
            DO(n, v[i] = 1;);
            RZ(w = jtreshape(jt, s, w));
        }                    // if w is an atom, change it to a singleton of rank #$a
        MCISH(v, AV(a), n);  // whether w was an atom or not, replace the axes of w-cell with values from a.  This
                             // leaves s with the final shape of the result
    }
    s = jttk(jt, s, w);  // go do the general take/drop
    // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication (if there was
    // fill), so we don't pass pristinity through  We overwrite w because it is no longer in use
    PRISTCLRF(w)
    return s;
}

A
jtdrop(J jt, A a, A w) {
    A s;
    I acr, af, ar, d, m, n, *u, *v, wcr, wf, wr;
    FPREFIP;
    RZ((a = jtvib(jt, a)) && w);  // convert & audit a
    ar  = AR(a);
    acr = jt->ranks >> RANKTX;
    acr = ar < acr ? ar : acr;
    af  = ar - acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
    wr  = AR(w);
    wcr = (RANKT)jt->ranks;
    wcr = wr < wcr ? wr : wcr;
    wf  = wr - wcr;
    RESETRANK;
    I wt = AT(w);
    // special case: if a is atomic 0, and cells of w are not atomic
    if ((-wcr & (ar - 1)) < 0 && (IAV(a)[0] == 0)) return w;  // 0 }. y, return y
    if (((af - 1) & (acr - 2)) >= 0) {
        s = rank2ex(a, w, UNUSED_VALUE, MIN(acr, 1), wcr, acr, wcr, reinterpret_cast<AF>(jtdrop));  // if multiple x values, loop over them
                                                                              // af>0 or acr>1
        // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication, so we
        // don't pass pristinity through  We overwrite w because it is no longer in use
        PRISTCLRF(w)
        return s;
    }
    n = AN(a);
    u = AV(a);  // n=#axes to drop, u->1st axis
    // virtual case: scalar a
    // correct if(!(ar|wf|(SPARSE&wt)|!wcr|(AFLAG(w)&(AFNJA)))){  // if there is only 1 take axis, w has no frame and is
    // not atomic
    if (!(ar | wf | ((NOUN & ~(DIRECT | RECURSIBLE)) & wt) | !wcr |
          (AFLAG(w) & (AFNJA)))) {   // if there is only 1 take axis, w has no frame and is not atomic  BJAwhy
        I* RESTRICT ws = AS(w);      // ws->shape of w
        I droplen      = IAV(a)[0];  // get the one number in a, the take amount
        I dropabs      = droplen < 0 ? -droplen : droplen;  // ABS(droplen), but may be as high as IMIN
        I remlen       = ws[0] - dropabs;
        remlen         = remlen < 0 ? 0 : remlen;  // length remaining after drop: (#y)-abs(x), 0 if overdrop
        // calculate offset
        I woffset =
          droplen < 0
            ? 0
            : droplen;  // x if x pos, 0 if x neg.  May be out of bounds if overdrop, but there will be no elements
        // get length of a cell of w
        I wcellsize;
        PROD(wcellsize, wr - 1, ws + 1);  // size of a cell in atoms of w
        I offset = woffset * wcellsize;   // offset in bytes of the virtual data
        // allocate virtual block.  May use inplace w
        RZ(s = virtualip(w, offset, wr));  // allocate block
        // fill in shape.  s and w may be the same block, so ws is destroyed
        I* RESTRICT ss = AS(s);
        ss[0]          = remlen;
        DO(wr - 1, ss[i + 1] = ws[i + 1];);  // shape of virtual matches shape of w except for #items
        AN(s) = remlen * wcellsize;          // install # atoms
        // virtual block does not affect pristinity
        return s;
    }

    // length error if too many axes
    fauxblockINT(sfaux, 4, 1);
    if (wcr) {
        ASSERT(n <= wcr, EVLENGTH);
        RZ(s = shape(jt, w));
        v = wf + AV(s);
        DO(n, d = u[i]; m = v[i]; m = d < 0 ? m : -m; m += d; v[i] = m &= REPSGN(m ^ d););
    }  // nonatomic w-cell: s is (w frame),(values of a clamped to within size), then convert to equivalent take
    else {
        fauxINT(s, sfaux, wr + n, 1) v = AV(s);
        MCISH(v, AS(w), wf);
        v += wf;
        DO(n, v[i] = !u[i];);
        RZ(w = jtreshape(jt, s, w));
    }  // atomic w-cell: reshape w-cell  to result-cell shape, with axis length 0 or 1 as will be in result
    RZ(s = jttk(jt, s, w));
    // We extracted from w, so mark it (or its backer if virtual) non-pristine.  If w was pristine and inplaceable,
    // transfer its pristine status to the result.  We overwrite w because it is no longer in use
    PRISTXFERF(s, w)
    return s;
}

// create 1 cell of fill when head/tail of an array with no items (at the given rank)
static A
jtrsh0(J jt, A w) {
    A x, y;
    I wcr, wf, wr, *ws;
    wr  = AR(w);
    wcr = (RANKT)jt->ranks;
    wcr = wr < wcr ? wr : wcr;
    wf  = wr - wcr;
    RESETRANK;
    ws = AS(w);
    RZ(x = vec(INT, wr - 1, ws));
    MCISH(wf + AV(x), ws + wf + 1, wcr - 1);
    RZ(w = jtsetfv(jt, w, w));
    GA(y, AT(w), 1, 0, 0);
    memcpy(AV(y), jt->fillv, bp(AT(w)));
    return jtreshape(jt, x, y);
    // not pristine
}

A
jthead(J jt, A w) {
    I wcr, wf, wr;
    FPREFIP;
    wr  = AR(w);
    wcr = (RANKT)jt->ranks;
    wcr = wr < wcr ? wr : wcr;
    wf  = wr - wcr;  // no RESETRANK so that we can pass rank into other code
    if (!wcr ||
        AS(w)[wf]) {  // if cell is atom, or cell has items - which means it's safe to calculate the size of a cell
        if (((-wf) | ((AT(w) & (DIRECT | RECURSIBLE)) - 1) | (wr - 2)) >=
            0) {  // frame=0, and DIRECT|RECURSIBLE, and rank>1.  No gain in virtualizing an atom, and it messes up
                  // inplacing and allocation-size counting in the tests
            // just one cell (no frame).  Create a virtual block for it, at offset 0
            I wn = AN(w);
            wcr--;
            wcr = (wcr < 0) ? wr : wcr;  // wn=#atoms of w, wcr=rank of cell being created
            A z;
            RZ(z = virtualip(w, 0, wcr));  // allocate the cell.  Now fill in shape & #atoms
                                           // if w is empty we have to worry about overflow when calculating #atoms
            I zn;
            PROD(zn, wcr, AS(w) + 1)
            MCISH(AS(z), AS(w) + 1, wcr) AN(z) = zn;  // Since z and w may be the same, the copy destroys AS(w).  So
                                                      // calc zn first.  copy shape of CELL of w into z
            return z;
        } else {
            // frame not 0, or non-virtualable type, or cell is an atom.  Use from.  Note that jt->ranks is still set,
            // so this may produce multiple cells left rank is garbage, but since zeroionei(0) is an atom it doesn't
            // matter
            return jtfrom(jtinplace, zeroionei(0), w);  // could call jtfromi directly for non-sparse w
        }
    } else {
        return SPARSE & AT(w) ? irs2(num(0), jttake(jt, num(1), w), 0L, 0L, wcr, reinterpret_cast<AF>(jtfrom))
                              : jtrsh0(jt, w);  // cell of w is empty - create a cell of fills  jt->ranks is still set
                                                // for use in take.  Left rank is garbage, but that's OK
    }
    // pristinity from the called verb
}

A
jttail(J jt, A w) {
    I wcr, wf, wr;
    FPREFIP;
    wr  = AR(w);
    wcr = (RANKT)jt->ranks;
    wcr = wr < wcr ? wr : wcr;
    wf  = wr - wcr;  // no RESETRANK: rank is passed into from/take/rsh0.  Left rank is garbage but that's OK
    return !wcr || AS(w)[wf] ? jtfrom(jtinplace, num(-1), w)
                             :  // if cells are atoms, or if the cells are nonempty arrays, result is last cell(s) scaf
                                // should generate virtual block here for speed
             SPARSE & AT(w) ? irs2(num(0), jttake(jt, num(-1), w), 0L, 0L, wcr, reinterpret_cast<AF>(jtfrom))
                            : jtrsh0(jt, w);
    // pristinity from other verbs
}