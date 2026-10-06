#include <stdio.h>

static void afficher_etat(const char *moment,
                          const char *nom_c, const char *nom_sc,
                          const char *nom_s, const char *nom_i,
                          const char *nom_l, const char *nom_ll,
                          const char *nom_f, const char *nom_d,
                          const char *nom_ld,
                          const char *c, const signed char *sc,
                          const short *s, const int *i,
                          const long int *l, const long long int *ll,
                          const float *f, const double *d,
                          const long double *ld) {
    printf("%s\n", moment);
    printf("%s : adresse %p, valeur 0x%llx\n", nom_c, (const void *)c,
           (unsigned long long)(unsigned char)*c);
    printf("%s : adresse %p, valeur 0x%llx\n", nom_sc, (const void *)sc,
           (unsigned long long)(unsigned char)*sc);
    printf("%s : adresse %p, valeur 0x%llx\n", nom_s, (const void *)s,
           (unsigned long long)(unsigned short)*s);
    printf("%s : adresse %p, valeur 0x%llx\n", nom_i, (const void *)i,
           (unsigned long long)(unsigned int)*i);
    printf("%s : adresse %p, valeur 0x%llx\n", nom_l, (const void *)l,
           (unsigned long long)(unsigned long)*l);
    printf("%s : adresse %p, valeur 0x%llx\n", nom_ll, (const void *)ll,
           (unsigned long long)*ll);
    printf("%s : adresse %p, valeur %a\n", nom_f, (const void *)f, (double)*f);
    printf("%s : adresse %p, valeur %a\n", nom_d, (const void *)d, *d);
    printf("%s : adresse %p, valeur %La\n\n", nom_ld, (const void *)ld, *ld);
}

int main(void) {
    char c = 'A';
    signed char sc = -65;
    short s = -32000;
    int i = -100000;
    long int l = -2000000000L;
    long long int ll = -9000000000000000000LL;
    float f = 3.14159f;
    double d = 2.718281828459;
    long double ld = 1.6180339887498948482L;

    char *pc = &c;
    signed char *psc = &sc;
    short *ps = &s;
    int *pi = &i;
    long int *pl = &l;
    long long int *pll = &ll;
    float *pf = &f;
    double *pd = &d;
    long double *pld = &ld;

    afficher_etat("Avant la manipulation :",
                  "char", "signed char", "short", "int", "long int",
                  "long long int", "float", "double", "long double",
                  pc, psc, ps, pi, pl, pll, pf, pd, pld);

    *pc = 'B';
    *psc = -64;
    *ps = -31999;
    *pi = -99999;
    *pl = -1999999999L;
    *pll = -8999999999999999999LL;
    *pf = 1.0f;
    *pd = 1.5;
    *pld = 2.0L;

    afficher_etat("Apres la manipulation :",
                  "char", "signed char", "short", "int", "long int",
                  "long long int", "float", "double", "long double",
                  pc, psc, ps, pi, pl, pll, pf, pd, pld);
    return 0;
}