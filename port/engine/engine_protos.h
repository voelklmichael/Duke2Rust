// PORT: prototypes for the Build engine (engine.c with ves2.h, cache1d.c, mmulti.c).
// The original code relied on implicit declarations (K&R style), which modern
// compilers reject when parameters are narrower than int (short, char).
// Functions without a return type became int, or void if they never return a value.
// Generated with gcc -aux-info from the function definitions. a.c has its own header (a.h).
#ifndef __ENGINE_PROTOS_H
#define __ENGINE_PROTOS_H

#include <stddef.h>

// engine.c
int setvesa (long int x, long int y);  // ves2.h
void setactivepage (long int dapagenum);  // ves2.h
void setvisualpage (long int dapagenum);  // ves2.h
void uninitvesa (void);  // ves2.h
void VBE_setPalette (long int start, long int num, char *dapal);  // ves2.h
void VBE_getPalette (long int start, long int num, char *dapal);  // ves2.h
void *kmalloc (size_t size);
void kfree (void *buffer);
void loadvoxel (long int voxindex);
void drawrooms (long int daposx, long int daposy, long int daposz, short int daang, long int dahoriz, short int dacursectnum);
void scansector (short int sectnum);
int wallfront (long int l1, long int l2);
int spritewallfront (spritetype *s, long int w);
int bunchfront (long int b1, long int b2);
void drawalls (long int bunch);
void prepwall (long int z, walltype *wal);
void ceilscan (long int x1, long int x2, long int sectnum);
void florscan (long int x1, long int x2, long int sectnum);
void wallscan (long int x1, long int x2, short int *uwal, short int *dwal, long int *swal, long int *lwal);
void maskwallscan (long int x1, long int x2, short int *uwal, short int *dwal, long int *swal, long int *lwal);
void transmaskvline (long int x);
void transmaskvline2 (long int x);
void transmaskwallscan (long int x1, long int x2);
int loadboard (char *filename, long int *daposx, long int *daposy, long int *daposz, short int *daang, short int *dacursectnum);
int saveboard (char *filename, long int *daposx, long int *daposy, long int *daposz, short int *daang, short int *dacursectnum);
void loadtables (void);
void loadpalette (void);
int setgamemode (char davidoption, long int daxdim, long int daydim);
void hline (long int xr, long int yp);
void slowhline (long int xr, long int yp);
void initengine (void);
void uninitengine (void);
void nextpage (void);
void loadtile (short int tilenume);
int allocatepermanenttile (short int tilenume, long int xsiz, long int ysiz);
int loadpics (char *filename);
void qloadkvx (long int voxindex, char *filename);
int clipinsidebox (long int x, long int y, short int wallnum, long int walldist);
int clipinsideboxline (long int x, long int y, long int x1, long int y1, long int x2, long int y2, long int walldist);
int readpixel16 (long int p);
int screencapture (char *filename, char inverseit);
int inside (long int x, long int y, short int sectnum);
int getangle (long int xvect, long int yvect);
int ksqrt (long int num);
int krecip (long int num);
void initksqrt (void);
void copytilepiece (long int tilenume1, long int sx1, long int sy1, long int xsiz, long int ysiz, long int tilenume2, long int sx2, long int sy2);
void drawmasks (void);
void drawmaskwall (short int damaskwallcnt);
void drawsprite (long int snum);
void drawvox (long int dasprx, long int daspry, long int dasprz, long int dasprang, long int daxscale, long int dayscale, char daindex, signed char dashade, char dapal, long int *daumost, long int *dadmost);
void ceilspritescan (long int x1, long int x2);
void ceilspritehline (long int x2, long int y);
int setsprite (short int spritenum, long int newx, long int newy, long int newz);
int animateoffs (short int tilenum, short int fakevar);
void initspritelists (void);
int insertsprite (short int sectnum, short int statnum);
int insertspritesect (short int sectnum);
int insertspritestat (short int statnum);
int deletesprite (short int spritenum);
int deletespritesect (short int deleteme);
int deletespritestat (short int deleteme);
int changespritesect (short int spritenum, short int newsectnum);
int changespritestat (short int spritenum, short int newstatnum);
int nextsectorneighborz (short int sectnum, long int thez, short int topbottom, short int direction);
int cansee (long int x1, long int y1, long int z1, short int sect1, long int x2, long int y2, long int z2, short int sect2);
int hitscan (long int xs, long int ys, long int zs, short int sectnum, long int vx, long int vy, long int vz, short int *hitsect, short int *hitwall, short int *hitsprite, long int *hitx, long int *hity, long int *hitz, long unsigned int cliptype);
int neartag (long int xs, long int ys, long int zs, short int sectnum, short int ange, short int *neartagsector, short int *neartagwall, short int *neartagsprite, long int *neartaghitdist, long int neartagrange, char tagsearch);
int lintersect (long int x1, long int y1, long int z1, long int x2, long int y2, long int z2, long int x3, long int y3, long int x4, long int y4, long int *intx, long int *inty, long int *intz);
int rintersect (long int x1, long int y1, long int z1, long int vx, long int vy, long int vz, long int x3, long int y3, long int x4, long int y4, long int *intx, long int *inty, long int *intz);
void dragpoint (short int pointhighlight, long int dax, long int day);
int lastwall (short int point);
int clipmove (long int *x, long int *y, long int *z, short int *sectnum, long int xvect, long int yvect, long int walldist, long int ceildist, long int flordist, long unsigned int cliptype);
void keepaway (long int *x, long int *y, long int w);
int raytrace (long int x3, long int y3, long int *x4, long int *y4);
int pushmove (long int *x, long int *y, long int *z, short int *sectnum, long int walldist, long int ceildist, long int flordist, long unsigned int cliptype);
void updatesector (long int x, long int y, short int *sectnum);
void rotatepoint (long int xpivot, long int ypivot, long int x, long int y, short int daang, long int *x2, long int *y2);
int initmouse (void);
void getmousevalues (short int *mousx, short int *mousy, short int *bstatus);
void printscreeninterrupt (void);
void drawline256 (long int x1, long int y1, long int x2, long int y2, char col);
void drawline16 (long int x1, long int y1, long int x2, long int y2, char col);
void qsetmode640350 (void);
void qsetmode640480 (void);
void clear2dscreen (void);
void draw2dgrid (long int posxe, long int posye, short int ange, long int zoome, short int gride);
void draw2dscreen (long int posxe, long int posye, short int ange, long int zoome, short int gride);
void printext16 (long int xpos, long int ypos, short int col, short int backcol, char *name, char fontsize);
void printext256 (long int xpos, long int ypos, short int col, short int backcol, char *name, char fontsize);
int krand (void);
void getzrange (long int x, long int y, long int z, short int sectnum, long int *ceilz, long int *ceilhit, long int *florz, long int *florhit, long int walldist, long unsigned int cliptype);
void setview (long int x1, long int y1, long int x2, long int y2);
void setaspect (long int daxrange, long int daaspect);
void dosetaspect (void);
void flushperms (void);
void rotatesprite (long int sx, long int sy, long int z, short int a, short int picnum, signed char dashade, char dapalnum, char dastat, long int cx1, long int cy1, long int cx2, long int cy2);
void dorotatesprite (long int sx, long int sy, long int z, short int a, short int picnum, signed char dashade, char dapalnum, char dastat, long int cx1, long int cy1, long int cx2, long int cy2);
int clippoly4 (long int cx1, long int cy1, long int cx2, long int cy2);
void makepalookup (long int palnum, char *remapbuf, signed char r, signed char g, signed char b, char dastat);
void initfastcolorlookup (long int rscale, long int gscale, long int bscale);
int getclosestcol (long int r, long int g, long int b);
void setbrightness (char dabrightness, char *dapal);
void drawmapview (long int dax, long int day, long int zoome, short int ang);
int clippoly (long int npoints, long int clipstat);
void fillpolygon (long int npoints);
void clearview (long int dacol);
void clearallviews (long int dacol);
void plotpixel (long int x, long int y, char col);
char getpixel (long int x, long int y);
void setviewtotile (short int tilenume, long int xsiz, long int ysiz);
void setviewback (void);
void squarerotatetile (short int tilenume);
void preparemirror (long int dax, long int day, long int daz, short int daang, long int dahoriz, short int dawall, short int dasector, long int *tposx, long int *tposy, short int *tang);
void completemirror (void);
int sectorofwall (short int theline);
int getceilzofslope (short int sectnum, long int dax, long int day);
int getflorzofslope (short int sectnum, long int dax, long int day);
void getzsofslope (short int sectnum, long int dax, long int day, long int *ceilz, long int *florz);
void alignceilslope (short int dasect, long int x, long int y, long int z);
void alignflorslope (short int dasect, long int x, long int y, long int z);
int owallmost (short int *mostbuf, long int w, long int z);
int wallmost (short int *mostbuf, long int w, long int sectnum, char dastat);
void grouscan (long int dax1, long int dax2, long int sectnum, char dastat);
int getpalookup (long int davis, long int dashade);
void parascan (long int dax1, long int dax2, long int sectnum, char dastat, long int bunch);
void stereohandler1 (void);
void stereohandler2 (void);
void stereonextpage (void);
void setstereo (long int dastereomode);
void *engconvalloc32 (long unsigned int size);
void installbistereohandlers (void *stereohan);
void uninstallbistereohandlers (void);
int loopnumofsector (short int sectnum, short int wallnum);
void setfirstwall (short int sectnum, short int newfirstwall);

// cache1d.c
void initcache (long int dacachestart, long int dacachesize);
void allocache (long int *newhandle, long int newbytes, char *newlockptr);
void suckcache (long int *suckptr);
void agecache (void);
void reportandexit (char *errormessage);
int initgroupfile (char *filename);
void uninitgroupfile (void);
int kopen4load (char *filename, char searchfirst);
int kread (long int handle, void *buffer, long int leng);
int klseek (long int handle, long int offset, long int whence);
int kfilelength (long int handle);
void kclose (long int handle);
void kdfread (void *buffer, size_t dasizeof, size_t count, long int fil);
void dfread (void *buffer, size_t dasizeof, size_t count, FILE *fil);
void dfwrite (void *buffer, size_t dasizeof, size_t count, FILE *fil);
int compress (char *lzwinbuf, long int uncompleng, char *lzwoutbuf);
int uncompress (char *lzwinbuf, long int compleng, char *lzwoutbuf);

// mmulti.c
void callcommit (void);
void initmultiplayers (char damultioption, char dacomrateoption, char dapriority);
void initcrc (void);
void setpackettimeout (long int datimeoutcount, long int daresendagaincount);
int getcrc (char *buffer, short int bufleng);
void uninitmultiplayers (void);
void sendlogon (void);
void sendlogoff (void);
int getoutputcirclesize (void);
void setsocket (short int newsocket);
void sendpacket (long int other, char *bufptr, long int messleng);
void dosendpackets (long int other);
short int getpacket (short int *other, char *bufptr);
void flushpackets (void);
void genericmultifunction (long int other, char *bufptr, long int messleng, long int command);
// Called by the engine, implemented by the game.
void faketimerhandler(void);

#endif
