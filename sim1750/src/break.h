/* break.h  --  exports of break.c */

#ifndef _BREAK_H
#define _BREAK_H


#include "cpu_ctx.h"





extern int  si_brkset   (int argc, char *argv[]);
extern int  si_wtchset   (int argc, char *argv[]);
extern int  si_brklist  (int argc, char *argv[]);
extern int  si_brkclear (int argc, char *argv[]);
extern int  si_brksave  (int argc, char *argv[]);

#endif
