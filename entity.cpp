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



/*  
 *   0x00000000 - dead
 *   0x00000001 - alive
 */

bool entity::generate_single(int x, int y)
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
    return true;
  
  if(sw == cell[i])
    return false;
  
  return true;
}


void entity::make_swap()
{
  gen++;
  memcpy(swac, cell, fieldSize*fieldSize*sizeof(int));
  memcpy(cell, swap, fieldSize*fieldSize*sizeof(int));
}

void entity::make_swac()
{ memcpy(swac, cell, fieldSize*fieldSize*sizeof(int)); }


int entity::define_state(int i)
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
  
  if(cell[l] == 1){ counter++; }
  if(cell[r] == 1){ counter++; }
  if(cell[u] == 1){ counter++; }
  if(cell[d] == 1){ counter++; }
  if(cell[lu] == 1){ counter++; }
  if(cell[ru] == 1){ counter++; }
  if(cell[ld] == 1){ counter++; }
  if(cell[rd] == 1){ counter++; }
  
  return counter;
}

void entity::clear()
{
  gen = 0;
  memset(cell, 0x00, fieldSize*fieldSize*sizeof(int));
  memset(swac, 0x00, fieldSize*fieldSize*sizeof(int));
  memset(swap, 0x00, fieldSize*fieldSize*sizeof(int));
}