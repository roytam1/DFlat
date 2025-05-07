/* ------------------- htree.c -------------------- */

#include "dflat.h"
#include "htree.h"

struct htree *ht;
int root;
int treect;

/* ------ build a Huffman tree from a frequency array ------ */
void buildtree(void)
{
    int i;

	treect = 256;
	for (i = 0; i < treect; i++)	{
		ht[i].parent = -1;
		ht[i].right  = -1;
		ht[i].left   = -1;
	}
    /* ---- build the huffman tree ----- */
    while (1)   {
        int h1 = -1, h2 = -1;
        /* ---- find the two smallest frequencies ---- */
        for (i = 0; i < treect; i++)   {
            if (i != h1) {
				struct htree *htt = ht+i;
                if (htt->cnt > 0 && htt->parent == -1)   {
                    if (h1 == -1 || htt->cnt < ht[h1].cnt) {
                        if (h2 == -1 || ht[h1].cnt < ht[h2].cnt)
                            h2 = h1;
                        h1 = i;
                    }
                    else if (h2 == -1 || htt->cnt < ht[h2].cnt)
                        h2 = i;
                }
            }
        }
        if (h2 == -1) {
            root = h1;
            break;
        }
        /* --- combine two nodes and add one --- */
        ht[h1].parent = treect;
        ht[h2].parent = treect;
		if ((ht = realloc(ht, (treect+1) * sizeof(struct htree))) == NULL)
			break;
        ht[treect].cnt = ht[h1].cnt + ht[h2].cnt;
        ht[treect].right = h1;
        ht[treect].left = h2;
		ht[treect].parent = -1;
        treect++;
    }
}

