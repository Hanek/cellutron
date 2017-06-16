#include <iostream>
#include <pthread.h>


#ifndef  _ENTITY_H
#define _ENTITY_H


class entity
{
public:
  int gen;
  int size;
  int*    cell;
  int*    swap;
  int*    swac;
  
  entity(int);
  ~entity();
  
  inline const int& get(int x, int y) 
  { return cell[x + fieldSize*y]; }
  
  inline const int& swget(int x, int y) 
  { return swac[x + fieldSize*y]; }
  
  inline void set(int x, int y, int state)
  { cell[x + fieldSize*y] = (0 == state) ? 0 : 1; }
  
  /* 
   * true if color update
   * false otherwise
   */
  bool generate_single(int x, int y);
  void make_swap(); 
  void make_swac();
  
  int define_state(int);
  void clear(); 
  int      fieldSize;
  int      cellSize;
};



#endif