/*
  For android, sgf files and other functionality is not used.
  So Add empty implementation for link.
*/


#include "ttsgf.h"

int analyzerflag=0;

int sgf_write_line(const char * line, ...){ return 0; }
void sgf_move_made(int i, int j, int who, int value){}
void sgf_set_stone(int i, int j, int who){}
void sgf_dragon_status(int i, int j, int status){}
int writesgf(SGFNodeP root, const char *filename,int seed){ return 0; }

SGFNodeP sgfAddComment(SGFNodeP pr, const char *comment){ return NULL; }
void sgfAddPropertyInt(SGFNodeP n, const char *name, long v){}
SGFNodeP sgfStartVariant(SGFNodeP pr){ return NULL; }
SGFNodeP sgfTerritory(SGFNodeP pr,int i,int j,int color){ return NULL; }
SGFNodeP sgfBoardText(SGFNodeP pr,int i,int j, const char *text){ return NULL; }
SGFNodeP sgfBoardNumber(SGFNodeP pr,int i,int j, int number){ return NULL; }

