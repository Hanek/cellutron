#include "entity.h"
#include "window.h"

#include <gtk/gtk.h>
#include <pthread.h>
#include <locale.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>


using namespace std;


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
  bb* bbw   = new bb(x);
  gol* golw = new gol(x);
  
  
  ss.clear(); 
  ss << argv[2];
  ss >> x;
  
  
  if(ss.fail())
  { 
    std::cout << "Invalid cell size" << std::endl; 
    print_help();
    return 0;
  }
  
  bbw->cellSize  = x;
  golw->cellSize = x;
  
  window app;

  app.golw = golw;
  app.bbw  = bbw;

  if(0 != pthread_create(&gui, NULL, window::gui_thread, &app))
  {
    std::cout << "pthread_create() failure" << std::endl;
    exit(EXIT_FAILURE);
  }
  
  while(true)
  {
    sleep(1);
  }
  
  delete bbw;
  delete golw;

    
  return 0;
}