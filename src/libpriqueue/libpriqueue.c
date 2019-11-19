/** @file libpriqueue.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "libpriqueue.h"


void priqueue_print(priqueue_t *q, char *str);
void priqueue_print(priqueue_t *q, char *str) {
#ifdef DEBUG
  printf("Printing priqueue: %s\n", str);
  node_t *root = q->root;
  unsigned int i = 0;
  while (root != NULL) {
    printf("q[%d] = %d, (at = %p, next = %p)\n", i, (*(int *)root->data), root, root->next);
    i++;
    root = root->next;
  }
#else
  (void)q;
  (void)str;
#endif
}


/**
* Initializes the priqueue_t data structure.
*
* Assumptions
*    - You may assume this function will only be called once per instance of priqueue_t
*    - You may assume this function will be the first function called using an instance of priqueue_t.
*
* @param q a pointer to an instance of the priqueue_t data structure
* @param comparer a function pointer that compares two elements.
* See also @ref comparer-page
*/
void priqueue_init(priqueue_t *q, int (*comparer)(const void *, const void *)) {
  q->root = NULL;
  q->size = 0;
  q->comparer = comparer;
}


/**
  Insert the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
*/
unsigned int priqueue_offer(priqueue_t *q, void *ptr) {
  unsigned int index = 0;

  priqueue_print(q, "priqueue_offer, beg");

  // Create new node and assign data
  node_t *node = malloc(sizeof(node_t));
  node->data = ptr;
  node->next = NULL;

  node_t *temp = q->root;
  node_t *parent = NULL;

  // Determine location to insert node (ptr)
  while (temp != NULL && q->comparer(temp->data, node->data) < 0) {
    parent = temp;
    temp = temp->next;
    index++;
  }

  if (index == 0) {
    // Insert at front of priqueue
    node->next = q->root;
    q->root = node;
  }
  else {
    // Insert after parent
    parent->next = node;
    node->next = temp;
  }

  // Increment size and return index of new node
  q->size++;


  priqueue_print(q, "priqueue_offer, end");

  return index;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
*/
void *priqueue_peek(priqueue_t *q) {
  return (q->size == 0) ? NULL : q->root->data;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
*/
void *priqueue_poll(priqueue_t *q) {
  // Check if queue is empty, if so, return NULL
  if (q->size == 0) {
    return NULL;
  }

  priqueue_print(q, "priqueue_poll, beg");

  // Set temp to root of priqueue
  node_t *temp = q->root;

  // Set root of queue to next node (temp cannot be NULL, as size != 0)
  q->root = temp->next;

  // store data
  void *data = temp->data;

  // Delete temp (old root of queue)
  free(temp);

  priqueue_print(q, "priqueue_poll, end");

  // Decrease size
  q->size--;

  // Return data
  return data;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
*/
void *priqueue_at(priqueue_t *q, unsigned int index) {
  // Return NULL if index is out of bounds
  if (q->size <= index) {
    return NULL;
  }

  node_t *temp = q->root;
  unsigned int current_position = 0;

  while (current_position < index) {
    temp = temp->next;
    ++current_position;
  }

  return temp->data;
}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
*/
unsigned int priqueue_remove(priqueue_t *q, void *ptr) {
  unsigned int removed = 0;

  // Return 0 if priqueue is empty
  if (q->size == 0) {
    return removed;
  }

  node_t *temp = q->root;
  node_t *parent = NULL;

  priqueue_print(q, "priqueue_remove, beg");

  while (temp != NULL) {
    // Check if temp->data is equal to ptr
    if (q->comparer(temp->data, ptr) == 0) {
      if (parent == NULL) {
        // Looking at root node, thus set q->root to temp->next
        q->root = temp->next;
      }
      else {
        // Looking at node other than root. Set parent->next to temp->next
        parent->next = temp->next;
      }

      // Remove node
      free(temp);
      temp = NULL;

      // Decrement size and increment count of removed nodes
      q->size--;
      removed++;

      // Look at next node
      if (parent == NULL) {
        temp = q->root;
      }
      else {
        temp = parent->next;
      }
    }
    else {
      parent = temp;
      temp = temp->next;
    }
  }

  priqueue_print(q, "priqueue_remove, end");

  return removed;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
*/
void *priqueue_remove_at(priqueue_t *q, unsigned int index) {
  if (q->size <= index)
    return NULL;

  node_t *temp = q->root;
  node_t *parent = NULL;
  unsigned int current_position = 0;

  while (temp != NULL) {
    if (current_position == index) {
      if (parent == NULL) {
        // Looking at root node, thus set q->root to temp->next
        q->root = temp->next;
      }
      else {
        // Looking at node other than root. Set parent->next to temp->next
        parent->next = temp->next;
      }
      q->size--;
      break;
    }
    parent = temp;
    temp = temp->next;
    current_position++;
  }

  return temp;
}


/**
  Return the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
*/
unsigned int priqueue_size(priqueue_t *q) {
  return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
*/
void priqueue_destroy(priqueue_t *q) {
  while (q->size > 0) {
    void *temp = priqueue_remove_at(q, 0);
    free(temp);
  }
}
