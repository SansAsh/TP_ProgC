#include <stdio.h>

struct Couleur {
    unsigned char rouge;
    unsigned char vert;
    unsigned char bleu;
    unsigned char alpha;
};

int main(void) {
    const struct Couleur couleurs[10] = {
        {0xef, 0x78, 0x12, 0xff},
        {0x2c, 0xc8, 0x64, 0xff},
        {0xff, 0x00, 0x00, 0xff},
        {0x00, 0xff, 0x00, 0xff},
        {0x00, 0x00, 0xff, 0xff},
        {0xff, 0xff, 0x00, 0xff},
        {0xff, 0x00, 0xff, 0xff},
        {0x00, 0xff, 0xff, 0xff},
        {0x80, 0x80, 0x80, 0x80},
        {0x12, 0x34, 0x56, 0x78}
    };

    for (size_t i = 0; i < sizeof couleurs / sizeof couleurs[0]; ++i) {
        printf("Couleur %zu :\n", i + 1);
        printf("Rouge : %u\n", (unsigned int)couleurs[i].rouge);
        printf("Vert : %u\n", (unsigned int)couleurs[i].vert);
        printf("Bleu : %u\n", (unsigned int)couleurs[i].bleu);
        printf("Alpha : %u\n\n", (unsigned int)couleurs[i].alpha);
    }
    return 0;
}