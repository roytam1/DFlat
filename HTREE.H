/* ------------------- htree.h -------------------- */

typedef unsigned int BYTECOUNTER;

/* ---- Huffman tree structure ---- */
struct htree    {
    unsigned char ch;       /* character value             */
    BYTECOUNTER cnt;        /* character frequency         */
    int parent;             /* offset to parent node       */
    int right;              /* offset to right child node  */
    int left;               /* offset to left child node   */
};

extern struct htree *ht;
extern int root;

void buildtree(void);
FILE *OpenHelpFile(void);
void HelpFilePosition(long *, int *);
void *GetHelpLine(char *);
void SeekHelpLine(long, int);

