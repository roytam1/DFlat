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

static int LoadingASCII;

struct htr *HelpTree;
static int root;

FILE *OpenHelpFile(void)
{
    char *cp;
    int treect, i;
	char helpname[65];

	BuildFileName(helpname, ".hlp");

    if ((fi = fopen(helpname, "rb")) == NULL)	{
		if ((cp = strrchr(helpname, '.')) != NULL)	{
			strcpy(cp, ".TXT");
		    fi = fopen(helpname, "rt");
		}
	    if (fi == NULL)
			return NULL;
		LoadingASCII = TRUE;
	}

	if (!LoadingASCII && HelpTree == NULL)	{
   		/* ----- read the byte count ------ */
   		fread(&bytectr, sizeof bytectr, 1, fi);
   		/* ----- read the frequency count ------ */
   		fread(&treect, sizeof treect, 1, fi);
   		/* ----- read the root offset ------ */
   		fread(&root, sizeof root, 1, fi);
		HelpTree = DFcalloc(treect-256, sizeof(struct htr));
		/* ---- read in the tree --- */
		for (i = 0; i < treect-256; i++)	{
       		fread(&HelpTree[i].left,  sizeof(int), 1, fi);
	        fread(&HelpTree[i].right, sizeof(int), 1, fi);
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
    	while (h > 255)	{
    		if (ct8 == 8)   {
        		if ((in8 = fgetc(fi)) == EOF)	{
					*line = '\0';
					return NULL;
				}
        		ct8 = 0;
    		}
    		if (in8 & 0x80)
            	h = HelpTree[h-256].left;
        	else
            	h = HelpTree[h-256].right;
    		in8 <<= 1;
    		ct8++;
		}
	    if (h == '\r')
			continue;
		*line++ = h;
		if (h == '\n')
			break;
	}
	*line = '\0';
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