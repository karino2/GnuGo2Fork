/*
  For android, sgf files and other functionality is not used.
  So Add empty implementation for link.
*/


#include "ttsgf.h"

int sgf_write_line(const char * line, ...){ return 0; }
void sgfAddPropertyInt(SGFNodeP n, const char *name, long v){}
SGFNodeP sgfBoardText(SGFNodeP pr,int i,int j, const char *text){ return NULL; }