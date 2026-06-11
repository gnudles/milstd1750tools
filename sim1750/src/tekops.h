/* tekops.h  --  Special purpose exports of tekops.c
                 General purpose exports are mentioned in loadfile.h  */

extern void init_tekops (void * data);
extern long find_tek_address (const char *labelname);
extern char *find_tek_label (unsigned int address);
extern int  display_tek_symbols ();
