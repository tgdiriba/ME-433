/*
 * File:   FIFO.h
 * Author: tgdiriba
 *
 * Created on June 2, 2015, 10:29 PM
 */

#ifndef FIFO_H
#define	FIFO_H

#define FIFO_SIZE 100
#define NUM_FIFOS 3

int FIFO_Enqueue(int fifo_id, int element);
int FIFO_Dequeue(int fifo_id);
int FIFO_Get(int fifo_id, int index);
int FIFO_GetReverse(int fifo_id, int index);

#endif	/* FIFO_H */
