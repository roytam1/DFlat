/* ---------- direct.c --------- */

#include "dflat.h"

static char path[MAXPATH];
static char drive[MAXDRIVE] = " :";
static char dir[MAXDIR];
static char name[MAXFILE];
static char ext[MAXEXT];

/* ----- Create unambiguous path from file spec, filling in the
     drive and directory if incomplete. Optionally change to
     the new drive and subdirectory ------ */
void CreatePath(char *path,char *fspec,int InclName,int Change)
{
    int cm = 0;
    unsigned currdrive;
    char currdir[64];
    char *cp;

    if (!Change)    {
        /* ---- save the current drive and subdirectory ---- */
        currdrive = getdisk();
        getcwd(currdir, sizeof currdir);
        memmove(currdir, currdir+2, strlen(currdir+1));
        cp = currdir+strlen(currdir)-1;
        if (*cp == '\\')
            *cp = '\0';
    }
    *drive = *dir = *name = *ext = '\0';
    fnsplit(fspec, drive, dir, name, ext);
    if (!InclName)
        *name = *ext = '\0';
    *drive = toupper(*drive);
    if (*ext)
        cm |= EXTENSION;
    if (InclName && *name)
        cm |= FILENAME;
    if (*dir)
        cm |= DIRECTORY;
    if (*drive)
        cm |= DRIVE;
    if (cm & DRIVE)
        setdisk(*drive - 'A');
    else     {
        *drive = getdisk();
        *drive += 'A';
    }
    if (cm & DIRECTORY)    {
        cp = dir+strlen(dir)-1;
        if (*cp == '\\')
            *cp = '\0';
        chdir(dir);
    }
    getcwd(dir, sizeof dir);
    memmove(dir, dir+2, strlen(dir+1));
    if (InclName)    {
        if (!(cm & FILENAME))
            strcpy(name, "*");
        if (!(cm & EXTENSION) && strchr(fspec, '.') != NULL)
            strcpy(ext, ".*");
    }
    else
        *name = *ext = '\0';
    if (dir[strlen(dir)-1] != '\\')
        strcat(dir, "\\");
    memset(path, 0, sizeof path);
    fnmerge(path, drive, dir, name, ext);
    if (!Change)    {
        setdisk(currdrive);
        chdir(currdir);
    }
}

static int dircmp(const void *c1, const void *c2)
{
    return stricmp(*(char **)c1, *(char **)c2);
}

BOOL DlgDirList(WINDOW wnd, char *fspec,
                enum commands nameid, enum commands pathid,
                unsigned attrib)
{
    int ax, i = 0, criterr = 1;
    struct ffblk ff;
    CTLWINDOW *ct = FindCommand(wnd->extension,nameid,LISTBOX);
    WINDOW lwnd;
    char **dirlist = NULL;

    CreatePath(path, fspec, TRUE, TRUE);
    if (ct != NULL)    {
        lwnd = ct->wnd;
        SendMessage(ct->wnd, CLEARTEXT, 0, 0);

        if (attrib & 0x8000)    {
            union REGS regs;
            char drname[15];
            unsigned int cd, dr;

            cd = getdisk();
            for (dr = 0; dr < 26; dr++)    {
                unsigned ndr;
                setdisk(dr);
                ndr = getdisk();
                if (ndr == dr)    {
                    /* ----- test for remapped B drive ----- */
                    if (dr == 1)    {
                        regs.x.ax = 0x440e; /* IOCTL func 14 */
                        regs.h.bl = dr+1;
                        int86(DOS, &regs, &regs);
                        if (regs.h.al != 0)
                            continue;
                    }

                    sprintf(drname, "[%c:]", dr+'A');

                    /* ---- test for network or RAM disk ---- */
                    regs.x.ax = 0x4409;     /* IOCTL func 9 */
                    regs.h.bl = dr+1;
                    int86(DOS, &regs, &regs);
                    if (!regs.x.cflag)    {
                        if (regs.x.dx & 0x1000)
                            strcat(drname, " (Network)");
                        else if (regs.x.dx == 0x0800)
                            strcat(drname, " (RAMdisk)");
                    }
                    SendMessage(lwnd,ADDTEXT,(PARAM)drname,0);
                }
            }
            setdisk(cd);
        }
        while (criterr == 1)    {
            ax = findfirst(path, &ff, attrib & 0x3f);
            criterr = TestCriticalError();
        }
        if (criterr)
            return FALSE;
        while (ax == 0)    {
            if (!((attrib & 0x4000) &&
                    (ff.ff_attrib & (attrib & 0x3f)) == 0) &&
                        strcmp(ff.ff_name, "."))    {
                char fname[15];
                sprintf(fname, (ff.ff_attrib & 0x10) ?
                                "[%s]" : "%s" , ff.ff_name);
                dirlist = DFrealloc(dirlist,
                                    sizeof(char *)*(i+1));
                dirlist[i] = DFmalloc(strlen(fname)+1);
                if (dirlist[i] != NULL)
                    strcpy(dirlist[i], fname);
                i++;
            }
            ax = findnext(&ff);
        }
        if (dirlist != NULL)    {
            int j;
            /* -- sort file/drive/directory list box data -- */
            qsort(dirlist, i, sizeof(void *), dircmp);

            /* ---- send sorted list to list box ---- */
            for (j = 0; j < i; j++)    {
                SendMessage(lwnd,ADDTEXT,(PARAM)dirlist[j],0);
                free(dirlist[j]);
            }
            free(dirlist);
        }
        SendMessage(lwnd, SHOW_WINDOW, 0, 0);
    }
    if (pathid)    {
        fnmerge(path, drive, dir, NULL, NULL);
        PutItemText(wnd, pathid, path);
    }
    return TRUE;
}
