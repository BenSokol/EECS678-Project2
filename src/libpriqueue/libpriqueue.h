/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct node_t;
struct priqueue_t;

/** @struct node_t
 *  @brief priqueue_t node structure
 *  @var node_t::data
 *  Member 'data' contains a pointer to data contained within this node.
 *  @var node_t::next
 *  Member 'next' contains a pointer to the next node.
 */
typedef struct node_t {
  void *data;
  struct node_t *next;
} node_t;


/** @struct priqueue_t
 *  @brief Priority Queue Structure
 *  @var priqueue_t::root
 *  Member 'root' contains a pointer to the root node of the priority queue.
 *  @var priqueue_t::size
 *  Member 'size' contains the size of the priority queue.
 *  @var priqueue_t::comparer
 *  Member 'comparer' contains a function to compare two node_t::data values. See @ref comparer-page
 */
typedef struct priqueue_t {
  node_t *root;
  unsigned int size;
  int (*comparer)(const void *, const void *);
} priqueue_t;


void priqueue_init(priqueue_t *q, int (*comparer)(const void *, const void *));

unsigned int priqueue_offer(priqueue_t *q, void *ptr);
void *priqueue_peek(priqueue_t *q);
void *priqueue_poll(priqueue_t *q);
void *priqueue_at(priqueue_t *q, unsigned int index);
unsigned int priqueue_remove(priqueue_t *q, void *ptr);
void *priqueue_remove_at(priqueue_t *q, unsigned int index);
unsigned int priqueue_size(priqueue_t *q);

void priqueue_destroy(priqueue_t *q);

void priqueue_print(priqueue_t *q, char *str);

#ifdef __cplusplus
}
#endif

#endif /* LIBPQUEUE_H_ */
