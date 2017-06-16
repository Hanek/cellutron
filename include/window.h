#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "entity.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>



#ifndef _WINDOW_H
#define _WINDOW_H

class window;

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
  
  void draw_brush(gdouble x, gdouble y);
  void draw_grid();
  void map_to_grid(gdouble x, gdouble y);
  
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
  
  entity* worldRef;
  field*  worldField;
  
  window();
};




#endif