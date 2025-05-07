/* ------------------- htree.h -------------------- */

#ifndef HTREE_H
#define HTREE_H

typedef unsigned short BYTECOUNTER;

/* ---- Huffman tree structure for building ---- */
struct htree    {
    BYTECOUNTER cnt;        /* character frequency         */
    short parent;             /* offset to parent node       */
    short right;              /* offset to right child node  */
    short left;               /* offset to left child node   */
};

/* ---- Huffman tree structure in compressed file ---- */
struct htr    {
    short right;              /* offset to right child node  */
    short left;               /* offset to left child node   */
};

extern struct htr *HelpTree;

void buildtree(void);
FILE *OpenHelpFile(void);
void HelpFilePosition(long *, short *);
void *GetHelpLine(char *);
void SeekHelpLine(long, short);

#endif


