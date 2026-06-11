/* coffops.h  --  Special purpose exports of load_coff.c
                  General purpose exports are mentioned in loadfile.h  */

extern int find_coff_address (void * data, const char *labelname);
extern int  display_coff_symbols (void * data);
