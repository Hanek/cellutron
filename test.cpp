#include "entity.h"
#include "window.h"

#include <gtk/gtk.h>
#include <pthread.h>
#include <locale.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>


using namespace std;



void* gui_thread(void* arg)
{
  if(arg == NULL)
  {exit (1);}
  
  window app;
  entity& world =  *(static_cast<entity*>(arg));
  field plot(&app, world, world.fieldSize, world.cellSize);

  std::cout << "fs: " << world.fieldSize << " cs: " << world.cellSize << std::endl;
  
  gtk_widget_show(app.mainWindow);
  gtk_main();

  return 0;
}

void print_help()
{
  std::cout << "Usage: ./cellutron [field size] [cell size]\n\n";
}


int main(int argc, char *argv[])
{
  if(argc < 3) { print_help(); return 0; } 
  int x;
  pthread_t gui;
  stringstream ss;
  
  ss << argv[1]; 
  ss >> x;

  if(ss.fail())
  { 
    std::cout << "Invalid field size" << std::endl;
    print_help();
    return 0;
  }
  
  entity world(x);
  ss.clear(); 
  ss << argv[2];
  ss >> x;   
  
  if(ss.fail())
  { 
    std::cout << "Invalid cell size" << std::endl; 
    print_help();
    return 0;
  }
  world.cellSize = x; 

  if(0 != pthread_create(&gui, NULL, gui_thread, &world))
  {
    std::cout << "pthread_create() failure" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  while(true)
  {
    sleep(1);
  }

    
  return 0;
}