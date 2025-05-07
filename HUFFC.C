/* ------------------- huffc.c -------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "htree.h"

static void compress(FILE *, int, int);
static void outbit(FILE *fo, int bit);

void main(int argc, char *argv[])
{
    FILE *fi, *fo;
    int c;
    BYTECOUNTER bytectr = 0;
    int freqctr = 0;

    if (argc < 3)   {
        printf("\nusage: huffc infile outfile");
        exit(1);
    }

    if ((fi = fopen(argv[1], "rb")) == NULL)    {
        printf("\nCannot open %s", argv[1]);
        exit(1);
    }
    if ((fo = fopen(argv[2], "wb")) == NULL)    {
        printf("\nCannot open %s", argv[2]);
        fclose(fi);
        exit(1);
    }

	ht = calloc(256, sizeof(struct htree));

    /* - read the input file and count character frequency - */
    while ((c = fgetc(fi)) != EOF)   {
        c &= 255;
        if (ht[c].cnt == 0)   {
            freqctr++;
            ht[c].ch = c;
        }
        ht[c].cnt++;
        bytectr++;
    }

    /* --- write the byte count to the output file --- */
    fwrite(&bytectr, sizeof bytectr, 1, fo);

    /* --- write the frequency count to the output file --- */
    fwrite(&freqctr, sizeof freqctr, 1, fo);

    /* -- write the frequency array to the output file -- */
    for (c = 0; c < 256; c++)   {
        if (ht[c].cnt > 0)    {
            fwrite(&ht[c].ch, sizeof(char), 1, fo);
            fwrite(&ht[c].cnt, sizeof(BYTECOUNTER), 1, fo);
        }
    }

    /* ---- build the huffman tree ---- */
    buildtree();

    /* ------ compress the file ------ */
    fseek(fi, 0L, 0);
    while ((c = fgetc(fi)) != EOF)
        compress(fo, (c & 255), 0);
    outbit(fo, -1);
    fclose(fi);
    fclose(fo);
}

/* ---- compress a character value into a bit stream ---- */
static void compress(FILE *fo, int h, int child)
{
    if (ht[h].parent != -1)
        compress(fo, ht[h].parent, h);
    if (child)  {
        if (child == ht[h].right)
            outbit(fo, 0);
        else if (child == ht[h].left)
            outbit(fo, 1);
    }
}

static char out8;
static int ct8;

/* -- collect and write bits to the compressed output file -- */
static void outbit(FILE *fo, int bit)
{
    if (ct8 == 8 || bit == -1)  {
		while (ct8 < 8)	{
			out8 <<= 1;
			ct8++;
		}
        fputc(out8, fo);
        ct8 = 0;
    }
    out8 = (out8 << 1) | bit;
    ct8++;
}
