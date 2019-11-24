/** @file queuetest.c
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "libpriqueue/libpriqueue.h"

int compare1(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

int compare2(const void *a, const void *b) {
  return (*(int *)b - *(int *)a);
}


void test_suite() {
  int *values = malloc(100 * sizeof(int));

  for (unsigned int i = 0; i < 100; i++) {
    values[i] = i;
  }

  for (unsigned int i = 1; i <= 11; ++i) {
    fprintf(stdout, "Test %u: ", i);
    priqueue_t q;
    bool success = true;
    switch (i) {
      case 1:
        fprintf(stdout, "priqueue_size returns 0 when empty");
        priqueue_init(&q, compare1);
        success = success && priqueue_size(&q) == 0;
        priqueue_destroy(&q);
        break;

      case 2:
        fprintf(stdout, "priqueue_poll returns NULL when empty");
        priqueue_init(&q, compare1);
        success = success && priqueue_poll(&q) == NULL;
        priqueue_destroy(&q);
        break;

      case 3:
        fprintf(stdout, "priqueue_peek returns NULL when empty");
        priqueue_init(&q, compare1);
        success = success && priqueue_peek(&q) == NULL;
        priqueue_destroy(&q);
        break;

      case 4:
        fprintf(stdout, "priqueue_at returns NULL when empty");
        priqueue_init(&q, compare1);
        success = success && priqueue_at(&q, 0) == NULL;
        priqueue_destroy(&q);
        break;

      case 5:
        fprintf(stdout, "priqueue_remove returns 0 when empty");
        priqueue_init(&q, compare1);
        success = success && priqueue_remove(&q, &values[0]) == 0;
        priqueue_destroy(&q);
        break;

      case 6:
        fprintf(stdout, "priqueue_remove_at returns NULL when empty");
        priqueue_init(&q, compare1);
        success = success && priqueue_remove_at(&q, 0) == NULL;
        priqueue_destroy(&q);
        break;

      case 7:
        fprintf(stdout, "priqueue_remove_at returns NULL when out of range");
        priqueue_init(&q, compare1);
        success = success && priqueue_remove_at(&q, 100) == NULL;
        priqueue_destroy(&q);
        break;

      case 8:
        fprintf(stdout, "priqueue_offer 1 time");
        priqueue_init(&q, compare1);
        success = success && priqueue_offer(&q, &values[0]) == 0;
        success = success && priqueue_size(&q) == 1;
        success = success && priqueue_peek(&q) == &values[0];
        priqueue_destroy(&q);
        break;

      case 9:
        fprintf(stdout, "priqueue_offer 2 times (without swap)");
        priqueue_init(&q, compare1);
        success = success && priqueue_offer(&q, &values[0]) == 0;
        success = success && priqueue_offer(&q, &values[1]) == 1;
        success = success && priqueue_size(&q) == 2;
        success = success && priqueue_at(&q, 0) == &values[0];
        success = success && priqueue_at(&q, 1) == &values[1];
        priqueue_destroy(&q);
        break;

      case 10:
        fprintf(stdout, "priqueue_offer 2 times (with swap)");
        priqueue_init(&q, compare1);
        success = success && priqueue_offer(&q, &values[1]) == 0;
        success = success && priqueue_at(&q, 0) == &values[1];
        success = success && priqueue_offer(&q, &values[0]) == 0;
        success = success && priqueue_size(&q) == 2;
        success = success && priqueue_at(&q, 0) == &values[0];
        success = success && priqueue_at(&q, 1) == &values[1];
        priqueue_destroy(&q);
        break;

      case 11:
        fprintf(stdout, "priqueue_size returns correct after adding and removing 1 value");
        priqueue_init(&q, compare1);
        success = success && priqueue_offer(&q, &values[0]) == 0;
        success = success && priqueue_remove_at(&q, 0) == &values[0];
        success = success && priqueue_size(&q) == 0;
        success = success && priqueue_peek(&q) == NULL;
        priqueue_destroy(&q);
        break;

      case 12:
        break;
      case 13:
        break;
      case 14:
        break;
      case 15:
        break;
      case 16:
        break;
      case 17:
        break;
      case 18:
        break;
      case 19:
        break;
      case 20:
        break;
      default:
        assert(false);
    }

    if (success) {
      fprintf(stdout, ": \033[32mPASS\033[0m\n");
    }
    else {
      fprintf(stdout, ": \033[31mFAIL\033[0m\n");
    }
    fflush(stdout);
  }

  free(values);
}


int main() {
  test_suite();

  return 0;
}
