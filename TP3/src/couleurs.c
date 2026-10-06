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
        printf("Couleur %zu : 0x%02x 0x%02x 0x%02x 0x%02x\n", i + 1,
               (unsigned int)couleurs[i].rouge,
               (unsigned int)couleurs[i].vert,
               (unsigned int)couleurs[i].bleu,
               (unsigned int)couleurs[i].alpha);
    }
    return 0;
}