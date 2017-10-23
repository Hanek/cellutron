#include <iostream>
#include <pthread.h>


#ifndef  _ENTITY_H
#define _ENTITY_H


class entity
{
public:
  struct game
  { enum gm { GOL, BB }; };
  
  int game;
  int gen;
  int size;
  int*    cell;
  int*    swap;
  int*    swac;
  
  entity(int);
  virtual ~entity();
  
  inline const int& get(int x, int y) 
  { return cell[x + fieldSize*y]; }
  
  inline const int& swget(int x, int y) 
  { return swac[x + fieldSize*y]; }
  
  inline void set(int x, int y, int state)
  { cell[x + fieldSize*y] = state; }
  
  virtual bool generate_single(int x, int y) = 0;
  virtual int  define_state(int) = 0;
  
  void make_swap(); 
  void make_swac();
  void clear(); 
  int      fieldSize;
  int      cellSize;
};

class gol: public entity
{
public:
  struct state
  { enum st { DEAD, ALIVE }; };
  gol(int size): entity(size) { game = game::GOL; }
  virtual ~gol(){}
  
  /* true if color update, false otherwise */
  bool generate_single(int x, int y);
  int  define_state(int);
  
};

class bb: public entity
{
public:
  struct state
  { enum st { DEAD, ALVA, ALVB }; };
  
  int acount;
  int bcount;
  bb(int size): entity(size) { game = game::BB; }
  virtual ~bb() {}
  
  /* true if color update, false otherwise */
  bool generate_single(int x, int y);
  int  define_state(int);
  
};

#endif