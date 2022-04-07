#ifndef _HELPERS_H_
#define _HELPERS_H
/*
    iLog, pow and root functions, taken from
	http://rosettacode.org/wiki/Nth_root#C
*/
float _fpow(float x, int e) {
    int i;
    float r = 1;
    for (i = 0; i < e; i++) {
        r *= x;
    }
    return r;
}

float _fsqrt(float x, int n) {
    float d, r = 1;
    if (!x) {
        return 0;
    }
    if (n < 1 || (x < 0 && !(n&1))) {
        return 0.0 / 0.0; /* NaN */
    }
    do {
        d = (x / _fpow(r, n - 1) - r) / n;
        r += d;
    }
    while (d >= 0.000010f * 10 || d <= -0.000010f * 10);
    return r;
}

#define round(a) (s16) (a+0.5) 

#endif
