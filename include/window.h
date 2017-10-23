#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "entity.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>



#ifndef _WINDOW_H
#define _WINDOW_H

struct window;

class field
{
public:
  struct timespec tts, rts;
  GtkWidget  *panel;
  GtkWidget  *drawingArea;
  GdkPixmap  *pixMap;
  entity          &worldRef;
  
  gdouble     x_m, y_m;
  int              x, y;
  int              brush_width;
  int              field_width;
  
  field(window* pWin, entity& wRef, int fw, int bw);
  void draw_grid();
  
  /* separate logic for different games */
  void draw_brush(gdouble x, gdouble y, bool st);
  bool map_to_grid(gdouble x, gdouble y, bool st);
};



struct window
{
  GtkSettings *settings;
  GtkWidget *mainWindow;
  GtkWidget *hbox;
  GtkWidget *panel;
  GtkWidget *genNum;
  GtkWidget *stopButton;
  GtkWidget *startButton;
  GtkObject *timeAdjust;
  GtkWidget *adjustScale;
  GtkWidget *clearButton;
  GtkWidget *fieldFrame;
  GtkWidget *fieldAlignment;
  GtkWidget *fieldLabel;
  GtkWidget *comboBox;
  
  entity* worldRef;
  field*  worldField;
  gol*    golw;
  bb*     bbw;
  static void* gui_thread(void* arg);
    
  window();
};




#endif