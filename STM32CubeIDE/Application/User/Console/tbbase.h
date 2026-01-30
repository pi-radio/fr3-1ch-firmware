/*
 * tbbase.h
 *
 *  Created on: Jan 30, 2026
 *      Author: zapman
 */

#ifndef APPLICATION_USER_CONSOLE_TBBASE_H_
#define APPLICATION_USER_CONSOLE_TBBASE_H_

struct tbbase {
  struct tbbase *parent;
  struct tbbase *first, *last; // Doubly linked list of children
};

static inline struct tbase *tbase_get_parent(struct tbbase *b) {
  return b->parent;
}


#endif /* APPLICATION_USER_CONSOLE_TBBASE_H_ */
