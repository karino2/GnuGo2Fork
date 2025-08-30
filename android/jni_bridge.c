#include <jni.h>

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "liberty.h"
#include "interface.h"

static char g_output_line[1024];
static int g_setting = 1;

static void ensureStart()
{
  if (g_setting)
  {
    g_setting = 0;
    set_movenumber(0);
    set_tomove(BLACK);
  }
}



static void hash_alloc(int memory)
{
#if HASHING
  {
    float nodes;
    
    nodes = (float)( (memory * 1024 * 1024)
	     / (sizeof(Hashnode) + sizeof(Read_result) * 1.4));
    movehash = hashtable_new((int) (1.5 * nodes), (int)(nodes), (int)(nodes * 1.4) );
  } 
#endif
}

void
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_initNative (
	JNIEnv*	env,
	jclass clasz
	)
{
  hash_alloc(8);
  hash_init();
  init_board();
  init_ginfo();
  init_gopt();
  g_setting = 1;
}

void
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_setKomi (
	JNIEnv*	env,
	jclass clasz,
	jfloat komi
	)
{
  set_komi(komi);
  g_setting = 1;
}

extern int depth;

void
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_setDepth (
	JNIEnv*	env,
	jclass clasz,
	jint d
	)
{
  depth = d;  
}


void
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_clearBoard (
	JNIEnv*	env,
	jclass clasz
	)
{
  clear_board(NULL);
  g_setting = 1;
}

/*
  x, y is start from upperleft, 0 origin.

  A3 means
  x = 0
  y = BOARD_SIZE-3
*/
jboolean
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_doMove (
	JNIEnv*	env,
	jclass clasz,
  jint x, 
  jint y
	)
{
  ensureStart();
  if (!legal(y, x, get_tomove()))
    return JNI_FALSE;
  
  inc_movenumber();
  updateboard(y, x, get_tomove());
  switch_tomove();
  return JNI_TRUE;
}

void
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_doPass (
	JNIEnv*	env,
	jclass clasz
	)
{
  ensureStart();
  updateboard(get_boardsize(), get_boardsize(), get_tomove());
  inc_movenumber();
  switch_tomove();
}

/*
  x | (y<<16),
  pass is -1.
*/
jint
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_genMoveInternal (
	JNIEnv*	env,
	jclass clasz
	)
{
  ensureStart();
  int cur_color = get_tomove();
  int i, j;
  int move_val;
  move_val = get_move(&i, &j, cur_color);

  if (move_val) {
    switch_tomove();
    return j | (i<<16);
  }
  else {
    switch_tomove();
    // PASS
    return -1;
  }
}


void
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_setBoardSize (
	JNIEnv*	env,
	jclass clasz,
	jint boardSize
	)
{
  if (!g_setting)
  {
    g_setting = 1;
    clear_board(NULL);
  }
  set_boardsize( boardSize );
}

static int
oprintf(const char *format_string, ...)
{
  va_list arguments;
  int result;
  char buf[256];

  va_start(arguments, format_string);
  result = vsprintf(buf, format_string, arguments);
  va_end(arguments);

  strcat(g_output_line, buf);

  return result;
}


jstring
Java_io_github_karino2_goengine_gnugo2_GnuGo2Native_debugInfo (
	JNIEnv*	env,
	jclass clasz
	)
{
  g_output_line[0] = '\0';
  oprintf("komi=%d\n", get_komi());
  oprintf("boardsize=%d\n", get_boardsize());
  oprintf("movenumber=%d\n", get_movenumber());
  oprintf("depth=%d\n", depth);
  return (*env)->NewStringUTF (env, g_output_line);
}
