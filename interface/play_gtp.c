/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * This is GNU GO, a Go program. Contact gnugo@gnu.org, or see   *
 * http://www.gnu.org/software/gnugo/ for more information.      *
 *                                                               *
 * Copyright 1999 by the Free Software Foundation.               *
 *                                                               *
 * This program is free software; you can redistribute it and/or *
 * modify it under the terms of the GNU General Public License   *
 * as published by the Free Software Foundation - version 2.     *
 *                                                               *
 * This program is distributed in the hope that it will be       *
 * useful, but WITHOUT ANY WARRANTY; without even the implied    *
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       *
 * PURPOSE.  See the GNU General Public License in file COPYING  *
 * for more details.                                             *
 *                                                               *
 * You should have received a copy of the GNU General Public     *
 * License along with this program; if not, write to the Free    *
 * Software Foundation, Inc., 59 Temple Place - Suite 330,       *
 * Boston, MA 02111, USA                                         *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */




#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>

#define BUILDING_GNUGO_ENGINE  /*FIXME ugly */
#include "liberty.h"
#include "interface.h"
#include "sgf.h"
#include "ttsgf.h"
#include "ttsgf_write.h"
#include "sgfana.h"


static void
failf(const char* format, ...)
{
  va_list args;
  
  printf("? ");
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n\n");
  fflush(stdout);
}


static void
replyf(const char* format, ...)
{
  va_list args;
  
  printf("= ");
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n\n");
  fflush(stdout);
}

static void
do_move(char *command, int *pass)
{
  char c;
  int d, i, j;
  sscanf(command, "%c%d", &c, &d);
  /* got a move... check if it's legal... */
  /* compensate for no 'I' on the board */
  c = toupper(c);
  if ((c - 'J') > (get_boardsize() - 9)
      || (d <= 0 || d > get_boardsize())) {
    failf("Invalid move: %c%d", c, d);
    return;
  }
  
  if (c > 'I')
    c--;
  i = get_boardsize() - d;
  j = c - 'A';
  
  if (!legal(i, j, get_tomove())) {
    if (c >= ('I'))
      c++;
    failf("Illegal move: %c%d", c, d);
    return;
  }
  else {
    *pass = 0;
    inc_movenumber();
    updateboard(i, j, get_tomove());
    switch_tomove();
  }
}


/* --------------------------------------------------------------*/
/* play a game as GTP client, fork from play_ascii                           */
/* --------------------------------------------------------------*/
void
play_gtp()
{
  char *known_commands="\nboardsize\ngenmove\nhelp\nknown_command"
  "\nkomi\nlist_commands\nname\nplay\nprotocol_version\nquit\nversion\nfinal_score\n";


  int pass = 0;  /* two passes and its over */
  char line[80];
  char *line_ptr = line;
  char *command;
  
  /*
    設定系とプレイの２つのループで適当に処理する。
  */

  int setting = 1;

  while(!time_to_die)
  {
    /* read the line of input */
    line_ptr = line;
    if (!fgets(line, 80, stdin))
      break; /* EOF or some error */
    
    line[strlen(line)-1] = 0;
    command = strtok(line_ptr, " \t\n");
    if (command == NULL) continue;          // ignore newline
    if (command[0] == '#') continue;        // ignore comment line

    // stateless commands.
    if (strcmp(command,"name") == 0) {
      replyf("gnugo-gtp");
      continue;
    } else if (strcmp(command, "version") == 0) {
      replyf("gnugo 2.6 gtp custom.");
      continue;
    } else if (strcmp(command, "protocol_version") == 0) {
      replyf("2");
      continue;
    } else if (strcmp(command, "list_commands") == 0) {
      replyf(known_commands);
      continue;
    } else if (strcmp(command, "help") == 0) {
      replyf(known_commands);
      continue;
    } else if (strcmp(command, "known_command") == 0) {
      char *cname = strtok(NULL, " \t\n");
      if (strstr(known_commands,cname) != NULL)
          replyf("true");
      else
          replyf("false");
      continue;
    } else if (strcmp(command, "quit") == 0) {
      break;
    }
    
    // setting commands
    if (strcmp(command, "clear_board") == 0) {
      // back to setting
      setting = 1;
      clear_board(NULL);
      set_movenumber(0);
      replyf("");
      continue;
    } else if (strcmp(command, "komi") == 0) {
      char *str = strtok(NULL, " \t\n");
      if (str == NULL) {
        failf("Error: invalid komi format.");
        continue;
      }
      float komi = (float) atof(str);
    	set_komi(komi);
      replyf("");
      continue;
    } else if (strcmp(command, "boardsize") == 0) {
      if (!setting)
      {
        // back to setting
        setting = 1;
        clear_board(NULL);
      }
      char *str = strtok(NULL, " \t\n");
      if (str == NULL) {
        failf("Error: invalid boardsize format.");
        continue;
      }
      int size =  atoi(str);
      set_boardsize(size);
      replyf("");
      continue;
    } else if (strcmp(command, "final_score") == 0) {
      int white_points,black_points;
      float score;
      evaluate_territory(&white_points,&black_points);
      score=black_points-white_points-black_captured+white_captured-get_komi();

      if (score>0) {
        replyf("B+%3.1f", score);
      }
      else if (score<0) {
        replyf("W+%3.1f", -score);
      }
      else {
        replyf("0");
      }

      continue;
    }



    /*
      playやgenmoveなら状態管理をする、プレー中のループを始める
    */
    if (strcmp(command,"play") == 0 || strcmp(command,"genmove") == 0) {
      if (setting)
      {
        setting = 0;
        pass = 0;
        set_movenumber(0);
        set_tomove(BLACK);
      }

    }

    if (strcmp(command, "play") == 0) {
      char* color = strtok(NULL, " \t\n");            // color is ignored
      char *str = strtok(NULL, " \t\n");

      if ((strcmp(str, "pass") == 0) || strcmp(str, "PASS") == 0){
        pass++;
        updateboard(get_boardsize(), get_boardsize(), get_tomove());
        inc_movenumber();
        switch_tomove();
      } else {
    	  do_move(str, &pass);
      }

      replyf("");
    } else if (strcmp(command, "genmove") == 0) {
      char* color = strtok(NULL, " \t\n");
      if (color == NULL) {
        failf("Invalid genmove syntax.");
        continue;        
      }
      int cur_color = get_tomove();
      if (color[0] == 'W' && cur_color == BLACK) {
        failf("Error: current move is BLACK but specify WHITE, illegal.");
        continue;
      }
      if (color[0] == 'B' && cur_color == WHITE) {
        failf("Error: current move is WHITE but specify BLACK, illegal.");
        continue;
      }

      int i, j;
      int move_val;
      move_val = get_move(&i, &j, cur_color);
      if (move_val) {
        switch_tomove();

        /* IはスキップするらしいのでI以上は1ずらす */
        replyf("%c%d", 'A'+j+(j >= 8), get_boardsize()-i);
        continue;
      } else {
        pass++;
        switch_tomove();

        replyf("pass");
        continue;
      }

    } else {
      failf("Error: Unknown command.");
      continue;
    }
  }
}

/*
 * Local Variables:
 * tab-width: 8
 * c-basic-offset: 2
 * End:
 */
