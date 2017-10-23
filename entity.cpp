#include <stdlib.h>
#include <string.h>
#include "entity.h"

entity::entity(int size): gen(0), fieldSize(size)
{
  if((cell = (int*)malloc(fieldSize*fieldSize*sizeof(int))) == NULL)
  { 
    std::cerr << "failed to allocate memory\n";
    exit(1);
  }
   
  
  if((swap = (int*)malloc(fieldSize*fieldSize*sizeof(int))) == NULL)
  { 
    std::cerr << "failed to allocate memory\n";
    exit(1);
  }
  
  if((swac = (int*)malloc(fieldSize*fieldSize*sizeof(int))) == NULL)
  { 
    std::cerr << "failed to allocate memory\n";
    exit(1);
  }
}



entity::~entity()
{
  free(cell);
  free(swap);
  free(swac);
}



void entity::make_swap()
{
  gen++;
  memcpy(swac, cell, fieldSize*fieldSize*sizeof(int));
  memcpy(cell, swap, fieldSize*fieldSize*sizeof(int));
}

void entity::make_swac()
{ memcpy(swac, cell, fieldSize*fieldSize*sizeof(int)); }



void entity::clear()
{
  gen = 0;
  memset(cell, 0x00, fieldSize*fieldSize*sizeof(int));
  memset(swac, 0x00, fieldSize*fieldSize*sizeof(int));
  memset(swap, 0x00, fieldSize*fieldSize*sizeof(int));
}


/*************************   game of life   *************************/


/*  
 *   0x00000000 - dead
 *   0x00000001 - alive
 */

bool gol::generate_single(int x, int y)
{
  int i   = x + fieldSize*y;
  int sw  = swac[i]; 
  int res = define_state(i);
  
  /* w - b */  
  if(res < 2 && cell[i] == 1)
  { swap[i] = 0x00000000; }
  
  /* w - w */
  if((res == 2 || res == 3) && cell[i] == 1)
  { swap[i] = 0x00000001; }
  
  /* w - b */
  if(res > 3 && cell[i] == 1)
  { swap[i] = 0x00000000; }
  
  /* b - w */
  if(res == 3 && cell[i] == 0)
  { swap[i] = 0x00000001; }
  
  
  if((sw && !cell[i]) || (cell[i] && !sw))
  { return true; }
  
  if(sw == cell[i])
  { return false; }
  
  return true;
}



int gol::define_state(int i)
{ 
  int counter = 0;
  int len = fieldSize*fieldSize;
  
  int u = (i < fieldSize) ? len - fieldSize + i : i - fieldSize;
  int d = (i > len - fieldSize - 1) ? i - (len -fieldSize) : i + fieldSize; 
  
  int l, lu, ld, r, ru, rd;
  
  if(i%fieldSize == 0)
  {
    l  = i + fieldSize - 1;
    lu = u + fieldSize - 1;
    ld = d + fieldSize - 1;
  }
  else
  {
    l  = i - 1;
    lu = u - 1;
    ld = d - 1;
  }
  
  if((i + 1)%fieldSize == 0)
  {
    r  = i + 1 - fieldSize;
    ru = u + 1 - fieldSize;
    rd = d + 1 - fieldSize;
  }
  else
  {
    r  = i + 1;
    ru = u + 1;
    rd = d + 1;
  }
  
  if(state::ALIVE == cell[l]){ counter++; }
  if(state::ALIVE == cell[r]){ counter++; }
  if(state::ALIVE == cell[u]){ counter++; }
  if(state::ALIVE == cell[d]){ counter++; }
  if(state::ALIVE == cell[lu]){ counter++; }
  if(state::ALIVE == cell[ru]){ counter++; }
  if(state::ALIVE == cell[ld]){ counter++; }
  if(state::ALIVE == cell[rd]){ counter++; }
  
  return counter;
}


/*************************   binary battle   *************************/


bool bb::generate_single(int x, int y)
{
  int i   = x + fieldSize*y;
  int sw  = swac[i]; 
  int res = define_state(i);

  /*
   * Any live cell with seven live neighbours dies due to overpopulation;
   * Any live cell with less then seven live neighbours and with more live    
   * neighbours of opposite race then its own, becomes dead due to struggle for 
   * survival;
   * Any dead cell becomes alive if it has at least one live neighbour and the
   * newborn cell acquire a race of majority, if black neighbours equals whith
   * neighbours then cell remains dead;
   */
  
  if(cell[i] != state::DEAD)
  {
    if(7 == acount + bcount)
    { swap[i] = state::DEAD; }
    
    if(7 > res && ((cell[i] == state::ALVA && acount + 1 < bcount) ||
                   (cell[i] == state::ALVB && bcount + 1 < acount)))
  { swap[i] = state::DEAD; }
  }
  else
  {
    if(acount > bcount)
    { swap[i] = state::ALVA; }
    
    if(acount < bcount)
    { swap[i] = state::ALVB; }  
  }
  
  if((sw && !cell[i]) || (cell[i] && !sw))
    return true;
  
  if(sw == cell[i])
    return false;
  
  return true;
}



int bb::define_state(int i)
{ 
  acount = 0;
  bcount = 0;
  int counter = 0;
  int len = fieldSize*fieldSize;
  
  int u = (i < fieldSize) ? len - fieldSize + i : i - fieldSize;
  int d = (i > len - fieldSize - 1) ? i - (len -fieldSize) : i + fieldSize; 
  
  int l, lu, ld, r, ru, rd;
  
  if(i%fieldSize == 0)
  {
    l  = i + fieldSize - 1;
    lu = u + fieldSize - 1;
    ld = d + fieldSize - 1;
  }
  else
  {
    l  = i - 1;
    lu = u - 1;
    ld = d - 1;
  }
  
  if((i + 1)%fieldSize == 0)
  {
    r  = i + 1 - fieldSize;
    ru = u + 1 - fieldSize;
    rd = d + 1 - fieldSize;
  }
  else
  {
    r  = i + 1;
    ru = u + 1;
    rd = d + 1;
  }
  
  if(state::ALVA == cell[l])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[l])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[r])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[r])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[u])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[u])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[d])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[d])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[lu])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[lu])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[ru])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[ru])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[ld])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[ld])
  { counter++; bcount++; }
  
  if(state::ALVA == cell[rd])
  { counter++; acount++; }
  else
  if(state::ALVB == cell[rd])
  { counter++; bcount++; }
  
  return counter;
}