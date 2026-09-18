#include <stdio.h>

int main(void) {
    /* i. char */
    char c = 'A';
    signed char sc = -65;
    unsigned char uc = 200;

    /* ii. short */
    short s = -32000;
    signed short ss = -15000;
    unsigned short us = 65000;

    /* iii. int */
    int i = -100000;
    signed int si = -42;
    unsigned int ui = 3000000000U;

    /* iv. long int */
    long int li = -2000000000L;
    signed long int sli = -123456789L;
    unsigned long int uli = 4000000000UL;

    /* v. long long int */
    long long int lli = -9000000000000000000LL;
    signed long long int slli = -424242424242LL;
    unsigned long long int ulli = 18000000000000000000ULL;

    /* vi. float */
    float f = 3.14159f;

    /* vii. double */
    double d = 2.718281828459;

    /* viii. long double */
    long double ld = 1.6180339887498948482L;

    /* Affichage des valeurs */
    printf("=== AFFICHAGE DES VARIABLES ===\n\n");

    printf("--- char ---\n");
    printf("char : %c (code ASCII: %d)\n", c, c);
    printf("signed char : %d\n", sc);
    printf("unsigned char : %u\n\n", uc);

    printf("--- short ---\n");
    printf("short : %d\n", s);
    printf("signed short : %d\n", ss);
    printf("unsigned short : %u\n\n", us);

    printf("--- int ---\n");
    printf("int : %d\n", i);
    printf("signed int : %d\n", si);
    printf("unsigned int : %u\n\n", ui);

    printf("--- long int ---\n");
    printf("long int : %ld\n", li);
    printf("signed long int : %ld\n", sli);
    printf("unsigned long int : %lu\n\n", uli);

    printf("--- long long int ---\n");
    printf("long long int : %lld\n", lli);
    printf("signed long long int : %lld\n", slli);
    printf("unsigned long long int : %llu\n\n", ulli);

    printf("--- Types flottants ---\n");
    printf("float : %f\n", f);
    printf("double : %lf\n", d);
    printf("long double : %Lf\n", ld);

    return 0;
}