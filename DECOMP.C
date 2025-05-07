/* ------------------- decomp.c -------------------- */

/*
 * Decompress the application.HLP file
 * or load the application.TXT file if the .HLP file
 * does not exist
 */

#include "dflat.h"
#include "htree.h"

static int in8;
static int ct8 = 8;

static FILE *fi;
static BYTECOUNTER bytectr;

static BOOL LoadingASCII;

FILE *OpenHelpFile(void)
{
    unsigned char c;
    int freqctr;
	char path[65];

	BuildFileName(path, ".hlp");

    if ((fi = fopen(path, "rb")) == NULL)	{
		BuildFileName(path, ".txt");
	    if ((fi = fopen(path, "rt")) == NULL)
			return NULL;
		LoadingASCII = TRUE;
	}

	if (!LoadingASCII && ht == NULL)	{
		if ((ht = calloc(256, sizeof(struct htree))) != NULL)	{
    		/* ----- read the byte count ------ */
    		fread(&bytectr, sizeof bytectr, 1, fi);
    		/* ----- read the frequency count ------ */
    		fread(&freqctr, sizeof freqctr, 1, fi);
			/* -------- read the characters ---------- */
    		while (freqctr--)   {
        		fread(&c, sizeof(char), 1, fi);
	        	ht[c].ch = c;
    	    	fread(&ht[c].cnt, sizeof(BYTECOUNTER), 1, fi);
    		}
    		/* ---- build the huffman tree ----- */
    		buildtree();
		}
	}
	return fi;
}

void *GetHelpLine(char *line)
{
	int h;
	if (LoadingASCII)
		return fgets(line, 160, fi);
	*line = '\0';
	while (TRUE)	{
    	/* ----- decompress a line from the file ------ */
		h = root;
		/* ----- first get a character ----- */
    	while (ht[h].right != -1)	{
    		if (ct8 == 8)   {
        		if ((in8 = fgetc(fi)) == EOF)	{
					*line = '\0';
					return NULL;
				}
        		ct8 = 0;
    		}
    		if (in8 & 0x80)
            	h = ht[h].left;
        	else
            	h = ht[h].right;
    		in8 <<= 1;
    		ct8++;
		}
	    if ((*line = ht[h].ch) == '\r')
			continue;
		if (*line == '\n')
			break;
		line++;
	}
	*++line = '\0';
	return line;
}

void HelpFilePosition(long *offset, int *bit)
{
	*offset = ftell(fi);
	if (LoadingASCII)
		*bit = 0;
	else	{
		if (ct8 < 8)
			--*offset;
		*bit = ct8;
	}
}

void SeekHelpLine(long offset, int bit)
{
	int i;
	fseek(fi, offset, 0);
	if (!LoadingASCII)	{
		ct8 = bit;
		if (ct8 < 8)	{
			in8 = fgetc(fi);
			for (i = 0; i < bit; i++)
    			in8 <<= 1;
		}
	}
}


