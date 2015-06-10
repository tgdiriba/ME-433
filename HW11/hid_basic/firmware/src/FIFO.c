#include "FIFO.h"

int FIFO_Buffer[NUM_FIFOS][FIFO_SIZE];
int FIFO_Head[NUM_FIFOS];
int FIFO_Tail[NUM_FIFOS];

int FIFO_Reset(int fifo_id)
{
    FIFO_Head[fifo_id] = 0;
    FIFO_Tail[fifo_id] = 0;
}

int FIFO_Enqueue(int fifo_id, int element)
{
  if((FIFO_Tail[fifo_id]+2)%FIFO_SIZE != FIFO_Head[fifo_id]) {
    FIFO_Buffer[fifo_id][FIFO_Tail[fifo_id]] = element;
    FIFO_Tail[fifo_id] = (FIFO_Tail[fifo_id]+1)%FIFO_SIZE;
    return 1;
  } else return 0;
}

int FIFO_Dequeue(int fifo_id)
{
  if(FIFO_Head[fifo_id] != FIFO_Tail[fifo_id]) {
    int t_head = FIFO_Head[fifo_id];
    FIFO_Head[fifo_id] = (FIFO_Head[fifo_id]+1)%FIFO_SIZE;
    return FIFO_Buffer[fifo_id][t_head];
  } else return 0;
}

int FIFO_Get(int fifo_id, int index)
{
    int fifo_index = ((FIFO_Head[fifo_id]+index)%FIFO_SIZE);
    if(index < FIFO_SIZE && fifo_index < FIFO_Tail[fifo_id]) {
        return FIFO_Buffer[fifo_id][fifo_index];
    } else return 0;
}

int FIFO_GetReverse(int fifo_id, int index)
{
    int fifo_index = ((FIFO_Tail[fifo_id]-index-1)%FIFO_SIZE);
    if(index < FIFO_SIZE && fifo_index >= FIFO_Head[fifo_id]) {
        return FIFO_Buffer[fifo_id][fifo_index];
    } else return 0;
}