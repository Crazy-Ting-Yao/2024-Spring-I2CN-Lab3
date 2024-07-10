#include "header.h"
/*
 * @description
 * Here are some functions for you to use. 
 * you don't have to modify this part.
*/
bool packet_loss() {
    return ((double) rand() / (double) RAND_MAX) < PACKET_LOSS_RATE;
}