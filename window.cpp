#include "window.h"
#include <iostream>
#include <cstdlib>



void* window::gui_thread(void* arg)
{
  if(arg == NULL)
  {exit (1);}
  
  window* app = (static_cast<window*>(arg));
  field plot(app, *app->bbw, app->bbw->fieldSize, app->bbw->cellSize);
  gtk_widget_show(app->mainWindow);
  gtk_main();

  return 0;
}



/* Draw a rectangle on the screen */

void field::draw_brush(gdouble x_in, gdouble y_in, bool st)
{
  GdkRectangle update_rect;
  
  update_rect.x = x_in - brush_width;
  update_rect.y = y_in - brush_width;
  update_rect.width = brush_width;
  update_rect.height = brush_width;
  
  
  switch(worldRef.game)
  {
    case entity::game::GOL:
    if(worldRef.get(x, y) == gol::state::ALIVE)
    {
      gdk_draw_rectangle (pixMap, drawingArea->style->white_gc, TRUE, 
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
    }
    else
    {
      gdk_draw_rectangle (pixMap, drawingArea->style->black_gc, TRUE,
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
    }
    break;
    
    case entity::game::BB:
//     std::cout << __func__ << ": bb::state: " << worldRef.get(x, y) << std::endl;
    if(worldRef.get(x, y) == bb::state::ALVA)
    {
        gdk_draw_rectangle (pixMap, drawingArea->style->dark_gc[0], TRUE, 
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
    }
    else
    if(worldRef.get(x, y) == bb::state::ALVB)
    {
      gdk_draw_rectangle (pixMap, drawingArea->style->light_gc[3], TRUE,
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
    }
    else
    {
      gdk_draw_rectangle (pixMap, drawingArea->style->black_gc, TRUE,
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
    }
    break;
  }
  gtk_widget_queue_draw_area (drawingArea, update_rect.x, update_rect.y,
                              update_rect.width, update_rect.height);
}


void field::draw_grid()
{
  if(0 == brush_width)
  { return; }
 
  for(int i = 0; i <= field_width; i = i + brush_width + 1)
  {
    gdk_draw_line (pixMap, drawingArea->style->dark_gc[10], i, 0, i, field_width - 1);
    gdk_draw_line (pixMap, drawingArea->style->dark_gc[10], 0, i, field_width - 1, i);
  }
}



bool field::map_to_grid(gdouble x_in, gdouble y_in, bool st)
{
  x = static_cast<int>(x_in);
  y = static_cast<int>(y_in);
  
  x = x/(brush_width + 1);
  y = y/(brush_width + 1);
  
  if(x > worldRef.fieldSize - 1 || y > worldRef.fieldSize - 1)
  { return false; }
  
  switch(worldRef.game)
  {
    case entity::game::GOL:
      if(worldRef.get(x, y) == gol::state::ALIVE)
      { worldRef.set(x, y, gol::state::DEAD); }
      else
      { worldRef.set(x, y, gol::state::ALIVE); }
      break;
      
    case entity::game::BB:
      if(st)
      { /* left mouse click */
        if(worldRef.get(x, y) > bb::state::DEAD)
        { worldRef.set(x, y, bb::state::DEAD); }
        else
        { worldRef.set(x, y, bb::state::ALVA); }
      }
      else
      { /* right mouse click */
        if(worldRef.get(x, y) > bb::state::DEAD)
        { worldRef.set(x, y, bb::state::DEAD); }
        else
        { worldRef.set(x, y, bb::state::ALVB); }
      }
      break;
  }
    
  x_m = static_cast<gdouble>(x*(brush_width + 1) + brush_width + 1);
  y_m = static_cast<gdouble>(y*(brush_width + 1) + brush_width + 1);
  
  return true;
}


static int currently_drawing = 3;
static gboolean first_execution = TRUE;


/***************************     events       **********************************/

gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer fp)
{
  field& obj = *((field*)fp);
  
  if (NULL == fp)
  {return FALSE; }
  
  gdk_draw_drawable(widget->window,
                    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                    obj.pixMap,
                    event->area.x, event->area.y,
                    event->area.x, event->area.y,
                    event->area.width, event->area.height);
  return FALSE;
}


gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer fp)
{
  field& obj = *((field*)fp);
  
  if (NULL == fp)
  { g_object_unref(obj.pixMap); }
  
  if(!event){}
  
  obj.pixMap = gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1);
  gdk_draw_rectangle (obj.pixMap, widget->style->black_gc, TRUE, 0, 0,
                      widget->allocation.width, widget->allocation.height);
  obj.draw_grid();
 
  return TRUE;
}



gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer fp)
{
  field& obj = *((field*)fp);
 
  if (NULL == fp || NULL == obj.pixMap)
  { return FALSE; }
  
  if(!widget){}
     
  if(event->button == 1)
  { 
    /* left mouse button */
    if(!obj.map_to_grid(event->x, event->y, true))
    { return FALSE; }
    obj.draw_brush (obj.x_m, obj.y_m, true); 
  }
  
  if(event->button == 3)
  {
    /* right mouse button */
    if(!obj.map_to_grid(event->x, event->y, false))
    { return FALSE; }
    obj.draw_brush(obj.x_m, obj.y_m, false);
  }
  return TRUE;
}



gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer fp)
{
  int x, y;
  GdkModifierType   state;
  field& obj = *((field*)fp);
  
  if (NULL == fp || NULL == obj.pixMap)
  { return FALSE; }
  
  if(!widget){}
  
  if (event->is_hint)
  { gdk_window_get_pointer (event->window, &x, &y, &state); }
  else
  {
    x = event->x;
    y = event->y;
    state = (GdkModifierType)event->state;
  }
  
  if(state & GDK_BUTTON1_MASK)
  { 
    obj.map_to_grid(event->x, event->y, true);
    obj.draw_brush (obj.x_m, obj.y_m, true);
  }
  
  if(state & GDK_BUTTON3_MASK)
  { 
    obj.map_to_grid(event->x, event->y, false);
    obj.draw_brush (obj.x_m, obj.y_m, false);
  }
  
  return TRUE;
}

void* do_clean(void* p)
{ 
  window* pWin = (window*)p;
  entity& worldRef = *pWin->worldRef;
  pWin->worldRef->clear();
  
  gdouble x_m;  
  gdouble y_m;
  GdkRectangle update_rect;
  int brush_width = pWin->worldField->brush_width;
  GdkPixmap* pixMap = pWin->worldField->pixMap;
  GtkWidget* drawingArea = pWin->worldField->drawingArea;
  
  update_rect.width = brush_width;
  update_rect.height = brush_width;
  
  char str[10];
  sprintf(str, "%d", pWin->worldRef->gen);  
  gtk_entry_set_text(GTK_ENTRY(pWin->genNum), str);

  std::cout << __func__ << std::endl;  
  int x, y;
  for(y = 0; y < worldRef.fieldSize; y++)
  {
    for(x = 0; x < worldRef.fieldSize; x++)
    {
      gdk_threads_enter();
      
      x_m = static_cast<gdouble>(x*(brush_width + 1) + brush_width + 1);
      y_m = static_cast<gdouble>(y*(brush_width + 1) + brush_width + 1);
      update_rect.x = x_m - brush_width;
      update_rect.y = y_m - brush_width;

      gdk_draw_rectangle (pixMap, drawingArea->style->black_gc, TRUE,
                          update_rect.x, update_rect.y, update_rect.width, update_rect.height);
   
      gtk_widget_queue_draw_area (drawingArea, update_rect.x, update_rect.y,
                                  update_rect.width, update_rect.height);
      
      gdk_threads_leave();
    }
  }
  return NULL;
}

void* do_draw(void* p)
{
  g_atomic_int_set(&currently_drawing, 1);
  window* pWin = (window*)p;
  entity& worldRef = *pWin->worldRef;
  
  GdkRectangle update_rect;
  gdouble x_m;  
  gdouble y_m;
  int brush_width = pWin->worldField->brush_width;
  GdkPixmap* pixMap = pWin->worldField->pixMap;
  GtkWidget* drawingArea = pWin->worldField->drawingArea;
  
  update_rect.width = brush_width;
  update_rect.height = brush_width;
  
  while(true)
  {
    int x, y;
    nanosleep(&pWin->worldField->tts, &pWin->worldField->rts);
    char str[10];
    sprintf(str, "%d", pWin->worldRef->gen);  
    gdk_threads_enter();
    gtk_entry_set_text(GTK_ENTRY(pWin->genNum), str);
  
    int draws = 0;
    for(y = 0; y < worldRef.fieldSize; y++)
    {
      for(x = 0; x < worldRef.fieldSize; x++)
      { 
        /* dont't update if color is the same */
        if(!worldRef.generate_single(x, y))
        { continue; }
        draws++; 
        x_m = static_cast<gdouble>(x*(brush_width + 1) + brush_width + 1);
        y_m = static_cast<gdouble>(y*(brush_width + 1) + brush_width + 1);
        update_rect.x = x_m - brush_width;
        update_rect.y = y_m - brush_width;
        
        switch(worldRef.game)
        {
          case entity::game::GOL:
          if(worldRef.get(x, y) == gol::state::ALIVE)
          {
            gdk_draw_rectangle (pixMap, drawingArea->style->white_gc, TRUE, 
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
          }
          else
          {
            gdk_draw_rectangle (pixMap, drawingArea->style->black_gc, TRUE,
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
          }
          break;
         
          case entity::game::BB:
          if(worldRef.get(x, y) == bb::state::ALVA)
          {
            gdk_draw_rectangle (pixMap, drawingArea->style->dark_gc[0], TRUE, 
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
          }
          else
          if(worldRef.get(x, y) == bb::state::ALVB)
          {
            gdk_draw_rectangle (pixMap, drawingArea->style->light_gc[3], TRUE,
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
          }
          else
          {
            gdk_draw_rectangle (pixMap, drawingArea->style->black_gc, TRUE,
                        update_rect.x, update_rect.y, update_rect.width, update_rect.height);
          }
          break;
        }
        gtk_widget_queue_draw_area (drawingArea, update_rect.x, update_rect.y,
                              update_rect.width, update_rect.height);
      }
    }
    gdk_threads_leave();
    worldRef.make_swap();
    if(3 == g_atomic_int_get(&currently_drawing)) 
    {
      first_execution = TRUE;
      return NULL; 
    }
  }
}




/***************************     callbacks        **********************************/


gboolean delete_event()
{ return FALSE; }


void destroy()
{ gtk_main_quit (); }


void callback_start(GtkWidget *widget, gpointer data)
{
  if(NULL == widget || NULL == data)
  {return;}
  
  window* pWin = ((window*)data);
  g_atomic_int_set(&currently_drawing, 0);

  if(first_execution)
  {
    static pthread_t drawing_thread;
    pthread_create(&drawing_thread, NULL, do_draw, pWin);
  }
  
  first_execution = FALSE;
 
}

void callback_stop(GtkWidget *widget, gpointer data)
{
  if(NULL == widget || NULL == data)
  {return;}
  
  g_atomic_int_set(&currently_drawing, 3); 
  g_print ("stop\n");
}

void callback_clear(GtkWidget *widget, gpointer data)
{
  if(NULL == widget || NULL == data)
  {return;}
  
  window* pWin = ((window*)data);
  g_atomic_int_set(&currently_drawing, 3);
  
  static pthread_t cleansing_thread;
  pthread_create(&cleansing_thread, NULL, do_clean, pWin);
  
  g_print ("clear\n");
}


void callback_cb_changed (GtkComboBox *widget, gpointer data)
{
  if(NULL == widget || NULL == data)
  {return;}
 
  GtkComboBox *comboBox = widget;
  
  window* pWin = ((window*)data);
  
  if(gtk_combo_box_get_active (comboBox) != 0) 
  {
    gchar *distro = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT(comboBox));
    if(strstr((const char*)distro, "Game of Life"))
    {
      g_atomic_int_set(&currently_drawing, 3);
      static pthread_t cleansing_thread;
      pthread_create(&cleansing_thread, NULL, do_clean, pWin);
      pWin->worldRef = pWin->golw;
      pWin->worldField->worldRef = *pWin->golw;
      std::cout << distro << ": " << pWin->worldRef->game << std::endl;
    }
    
    if(strstr((const char*)distro, "Binary Battle"))
    {
      g_atomic_int_set(&currently_drawing, 3);
      static pthread_t cleansing_thread;
      pthread_create(&cleansing_thread, NULL, do_clean, pWin);
      pWin->worldRef = pWin->bbw;
      pWin->worldField->worldRef = *pWin->bbw;
      std::cout << distro << ": " << pWin->worldRef->game << std::endl;
    }
    
    g_free (distro);
  }
}



void scale_changed(GtkAdjustment* adj, gpointer p)
{
  if(NULL == adj || NULL == p)
  {return;} 
  
  window* pWin = ((window*)p);
  
  size_t scale = adj->value*adj->value*adj->value;
  if(scale >= 1000000000)
  { pWin->worldField->tts.tv_nsec = 999999999; }
  else
  { pWin->worldField->tts.tv_nsec = scale; }
}


void changed(GtkAdjustment* adj, gpointer p)
{
  if(NULL == adj || NULL == p)
  {return;}
  
  adj->value = 0.0;
  gtk_adjustment_set_value(adj, 0.0);
}



gboolean time_handler(window* pWin)
{
  if(NULL == pWin)
  {return false;}
  
  
  return true;
}



/******************************************************************************/


window::window()
{
  gtk_disable_setlocale();
  gdk_threads_init();
  gdk_threads_enter();
  gtk_init(0, 0);
  
  /*   Window   */
  mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_container_set_border_width (GTK_CONTAINER (mainWindow), 2);
  gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_title (GTK_WINDOW (mainWindow), "life");
  gtk_window_set_resizable (GTK_WINDOW (mainWindow), FALSE);
  settings = gtk_settings_get_default();
  g_object_set(G_OBJECT(settings), "gtk-font-name", "Sans 8", NULL);
  
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_container_add (GTK_CONTAINER (mainWindow), hbox);
  
  panel = gtk_fixed_new ();
  gtk_widget_show (panel);
  gtk_box_pack_start (GTK_BOX (hbox), panel, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (panel), 8);
  
  genNum = gtk_entry_new ();
  gtk_widget_show (genNum);
  gtk_fixed_put (GTK_FIXED (panel), genNum, 0, 10);
  gtk_widget_set_size_request (genNum, 125, 25);
  
  startButton = gtk_button_new_with_mnemonic ("start");
  gtk_widget_show (startButton);
  gtk_fixed_put (GTK_FIXED (panel), startButton, 0, 40);
  gtk_widget_set_size_request (startButton, 125, 27);
  
  stopButton = gtk_button_new_with_mnemonic ("stop");
  gtk_widget_show (stopButton);
  gtk_fixed_put (GTK_FIXED (panel), stopButton, 0, 70);
  gtk_widget_set_size_request (stopButton, 125, 27);
  
  clearButton = gtk_button_new_with_mnemonic ("clear");
  gtk_widget_show (clearButton);
  gtk_fixed_put (GTK_FIXED (panel), clearButton, 0, 100);
  gtk_widget_set_size_request (clearButton, 125, 27);
  
  /* value, lower, upper, step_increment, page_increment, page_size */
  timeAdjust = gtk_adjustment_new (0.0, 0.0, 1000.0, 1.0, 0.0, 0.0);
  adjustScale = gtk_hscale_new (GTK_ADJUSTMENT (timeAdjust));
  gtk_widget_set_size_request (adjustScale, 125, 36);
  gtk_fixed_put (GTK_FIXED (panel), adjustScale, 0, 136);
  gtk_widget_show (adjustScale);
  
  
  comboBox = gtk_combo_box_text_new ();
  const char* distros[] = {"Game selection", "Game of Life", "Binary Battle"};
  for (int i = 0; i < 3; i++)
  { gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (comboBox), distros[i]); }
  gtk_combo_box_set_active (GTK_COMBO_BOX (comboBox), 0);
  gtk_widget_set_size_request (comboBox, 125, 27);
  gtk_widget_show (comboBox);
  gtk_fixed_put (GTK_FIXED (panel), comboBox, 0, 180);
  
  
  fieldFrame = gtk_frame_new (NULL);
  gtk_widget_show (fieldFrame);
  gtk_box_pack_start (GTK_BOX (hbox), fieldFrame, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (fieldFrame), GTK_SHADOW_ETCHED_OUT);
  
  fieldAlignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (fieldAlignment);
  gtk_container_add (GTK_CONTAINER (fieldFrame), fieldAlignment);
  gtk_alignment_set_padding (GTK_ALIGNMENT (fieldAlignment), 0, 0, 12, 0);
   
  fieldLabel = gtk_label_new ("field");
  gtk_widget_show (fieldLabel);
  gtk_frame_set_label_widget (GTK_FRAME (fieldFrame), fieldLabel);
  gtk_label_set_use_markup (GTK_LABEL (fieldLabel), TRUE);
  
  
  g_signal_connect (GTK_OBJECT(timeAdjust), "changed", GTK_SIGNAL_FUNC(changed), (gpointer) this);
  g_signal_connect (GTK_OBJECT(timeAdjust), "value_changed", GTK_SIGNAL_FUNC(scale_changed), (gpointer)this);
  g_signal_connect (GTK_OBJECT(mainWindow), "delete_event", GTK_SIGNAL_FUNC(delete_event), NULL);
  g_signal_connect (GTK_OBJECT(mainWindow), "destroy", GTK_SIGNAL_FUNC(destroy), NULL);
  g_signal_connect (GTK_OBJECT(startButton), "clicked", GTK_SIGNAL_FUNC(callback_start), gpointer(this));
  g_signal_connect (GTK_OBJECT(stopButton), "clicked", GTK_SIGNAL_FUNC(callback_stop), gpointer(this));
  g_signal_connect (GTK_OBJECT(clearButton), "clicked", GTK_SIGNAL_FUNC(callback_clear), gpointer(this));
  g_signal_connect (GTK_OBJECT(comboBox), "changed", G_CALLBACK(callback_cb_changed), gpointer(this));
  g_timeout_add(200, (GSourceFunc)time_handler, (gpointer) this);
  
}




field::field(window* pWin, entity& wRef, int fw, int bw): worldRef(wRef), brush_width(bw)
{
  if(brush_width < 0)
  { exit(0); }
  
  tts.tv_sec = 0;
  tts.tv_nsec = 0;
  pWin->worldRef = &wRef; 
  pWin->worldField = this;
  field_width = bw*fw + fw + 1; 

  drawingArea = gtk_drawing_area_new();
  gtk_drawing_area_size(GTK_DRAWING_AREA (drawingArea), field_width, field_width);
  panel = gtk_vbox_new(FALSE, 0);
  gtk_container_add (GTK_CONTAINER (pWin->fieldAlignment), panel);
  
  gtk_box_pack_start (GTK_BOX(panel), drawingArea, TRUE, TRUE, 0);
  gtk_widget_show (drawingArea);
  gtk_widget_show (panel);
  
  gtk_widget_set_app_paintable(pWin->mainWindow, TRUE);
//   gtk_widget_set_double_buffered(pWin->mainWindow, FALSE);
 
  gtk_signal_connect (GTK_OBJECT (drawingArea), "expose_event", (GtkSignalFunc) expose_event,  (gpointer)this);
  gtk_signal_connect (GTK_OBJECT(drawingArea),"configure_event", (GtkSignalFunc) configure_event, (gpointer)this);
  gtk_signal_connect (GTK_OBJECT (drawingArea), "button_press_event", (GtkSignalFunc) button_press_event, (gpointer)this);
  gtk_signal_connect (GTK_OBJECT (drawingArea), "motion_notify_event", (GtkSignalFunc) motion_notify_event, (gpointer)this);  

  gtk_widget_set_events (drawingArea, GDK_EXPOSURE_MASK
  | GDK_LEAVE_NOTIFY_MASK
  | GDK_BUTTON_PRESS_MASK
  | GDK_POINTER_MOTION_MASK
  | GDK_POINTER_MOTION_HINT_MASK);
}