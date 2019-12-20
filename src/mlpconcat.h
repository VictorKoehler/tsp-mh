#ifndef __MLPCONCAT_CLASS__
#define __MLPCONCAT_CLASS__

#include <vector>

struct Concatenation {
    double t, c; // Duration, Cost
    int w; // Delay
};
#define matapl(a, b) mat[(*mat_at)[a]][(*mat_at)[b]]

#define subconc(s, ei) subseq[s][ei]
#define subt(s, ei) subconc(s, ei).t
#define subc(s, ei) subconc(s, ei).c
#define subw(s, ei) subconc(s, ei).w
#define subcint(s, ei) fromConcatenation(subconc(s, ei), s, ei)

#define concat_tp(lt, rt, matapl_lei_rs) lt + matapl_lei_rs + rt
#define concat_cp(lt, lc, rc, rw, matapl_lei_rs) lc + rw*(lt + matapl_lei_rs) + rc
#define concat_wp(lw, rw) lw + rw

#define concat_t(ls, lei, rs, rei) concat_tp(subt(ls, lei), matapl(lei, rs), subt(rs, rei))
#define concat_c(ls, lei, rs, rei) concat_cp(subt(ls, lei), subc(ls, lei), subc(rs, rei), subw(rs, rei), matapl(lei, rs))
#define concat_w(ls, lei, rs, rei) concat_wp(subw(ls, lei), subw(rs, rei))

#define concat_fulli(l, r) l._add(r, matapl(l.e, r.s))
#define concat_full(ls, le, rs, re) concat_fulli(subcint(ls, le), subcint(rs, re))
#define concat_fullb(ls, le, rs, re) concat_mergelr(subconc(ls, le), subconc(rs, re), matapl(le, rs))


struct ConcatenationInterv {
    double t, c;  // Duration, Cost
    int w, s, e;  // Delay, start, end (inclusive)

    inline ConcatenationInterv _add(ConcatenationInterv const &r, double matapl_lei_rs) {
        return ConcatenationInterv{
            concat_tp(t, r.t, matapl_lei_rs),
            concat_cp(t, c, r.c, r.w, matapl_lei_rs),
            concat_wp(w, r.w),
            s,
            r.e
        };
    }

    inline ConcatenationInterv _add(ConcatenationInterv const &r, double **mat, std::vector<int> *sol) {
        return _add(r, mat[*(sol->begin() + e)][*(sol->begin() + r.s)]);
    }
};

inline Concatenation concat_mergelr(Concatenation const &l, Concatenation const &r, double matapl_lei_rs) {
    return Concatenation{concat_tp(l.t, r.t, matapl_lei_rs),
                         concat_cp(l.t, l.c, r.c, r.w, matapl_lei_rs),
                         concat_wp(l.w, r.w)};
}

inline ConcatenationInterv fromConcatenation(Concatenation &c, int s, int ei) {
    return {c.t, c.c, c.w, s, ei};
}

#endif