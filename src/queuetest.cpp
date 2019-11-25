/** @file queuetest.c
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "libpriqueue/libpriqueue.h"

#define CATCH_CONFIG_DEFAULT_REPORTER "compact"
#define CATCH_CONFIG_MAIN
#include "../external/Catch2/single_include/catch2/catch.hpp"

int compare1(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}


TEST_CASE("Correct return values when queue is empty",
          "[priqueue_size][priqueue_poll][priqueue_peek][priqueue_at][priqueue_remove][priqueue_remove_at]") {
  priqueue_t q;
  priqueue_init(&q, compare1);
  REQUIRE(priqueue_size(&q) == 0);
  REQUIRE(priqueue_poll(&q) == NULL);
  REQUIRE(priqueue_peek(&q) == NULL);
  REQUIRE(priqueue_at(&q, 0) == NULL);
  REQUIRE(priqueue_remove(&q, NULL) == 0);
  REQUIRE(priqueue_remove_at(&q, 0) == NULL);
  priqueue_destroy(&q);
}


TEST_CASE("priqueue_offer works when queue is empty", "[priqueue_size][priqueue_offer][priqueue_peek]") {
  priqueue_t q;
  int value = 0;
  priqueue_init(&q, compare1);
  REQUIRE(priqueue_size(&q) == 0);
  REQUIRE(priqueue_offer(&q, &value) == 0);
  REQUIRE(priqueue_size(&q) == 1);
  REQUIRE(priqueue_peek(&q) == &value);
  priqueue_destroy(&q);
}

TEST_CASE("priqueue_offer works when queue is empty (add 2, without swap)",
          "[priqueue_size][priqueue_offer][priqueue_at]") {
  priqueue_t q;
  int value0 = 0;
  int value1 = 1;
  priqueue_init(&q, compare1);
  REQUIRE(priqueue_size(&q) == 0);
  REQUIRE(priqueue_offer(&q, &value0) == 0);
  REQUIRE(priqueue_offer(&q, &value1) == 1);
  REQUIRE(priqueue_size(&q) == 2);
  REQUIRE(priqueue_at(&q, 0) == &value0);
  REQUIRE(priqueue_at(&q, 1) == &value1);
  priqueue_destroy(&q);
}


TEST_CASE("priqueue_offer works when queue is empty (add 2, with swap)",
          "[priqueue_size][priqueue_offer][priqueue_at]") {
  priqueue_t q;
  int value0 = 0;
  int value1 = 1;
  priqueue_init(&q, compare1);
  REQUIRE(priqueue_size(&q) == 0);
  REQUIRE(priqueue_offer(&q, &value1) == 0);
  REQUIRE(priqueue_offer(&q, &value0) == 0);
  REQUIRE(priqueue_size(&q) == 2);
  REQUIRE(priqueue_at(&q, 0) == &value0);
  REQUIRE(priqueue_at(&q, 1) == &value1);
  priqueue_destroy(&q);
}

TEST_CASE("priqueue_remove_at works with 1 value in queue",
          "[priqueue_size][priqueue_offer][priqueue_peek][priqueue_remove_at]") {
  priqueue_t q;
  int value = 0;
  priqueue_init(&q, compare1);
  REQUIRE(priqueue_size(&q) == 0);
  REQUIRE(priqueue_offer(&q, &value) == 0);
  REQUIRE(priqueue_size(&q) == 1);
  REQUIRE(priqueue_peek(&q) == &value);
  REQUIRE(priqueue_remove_at(&q, 0) == &value);
  REQUIRE(priqueue_size(&q) == 0);
  REQUIRE(priqueue_peek(&q) == NULL);
  priqueue_destroy(&q);
}

TEST_CASE("priqueue_offer and priqueue_remove_at works with 100 values (without swapping)",
          "[priqueue_size][priqueue_offer][priqueue_at][priqueue_remove_at]") {
  int *values = new int[100];

  priqueue_t q;
  for (unsigned int i = 0; i < 100; i++) {
    values[i] = i;
  }
  priqueue_init(&q, compare1);
  for (unsigned int j = 0; j < 100; ++j) {
    REQUIRE(priqueue_offer(&q, &values[j]) == j);
  }
  REQUIRE(priqueue_size(&q) == 100);
  for (unsigned int j = 0; j < 100; ++j) {
    REQUIRE(priqueue_at(&q, values[j]) == &values[j]);
  }
  for (unsigned int j = 100; j > 0; --j) {
    REQUIRE(priqueue_remove_at(&q, values[j - 1]) == &values[j - 1]);
  }
  REQUIRE(priqueue_size(&q) == 0);
  priqueue_destroy(&q);

  delete[] values;
}

TEST_CASE("priqueue_offer and priqueue_remove_at works with 100 values (with swapping)",
          "[priqueue_size][priqueue_offer][priqueue_at][priqueue_remove_at]") {
  int *values = new int[100];

  priqueue_t q;
  for (unsigned int i = 0; i < 100; i++) {
    values[i] = i;
  }
  priqueue_init(&q, compare1);
  for (unsigned int j = 100; j > 0; --j) {
    REQUIRE(priqueue_offer(&q, &values[j - 1]) == 0);
  }
  REQUIRE(priqueue_size(&q) == 100);
  for (unsigned int j = 0; j < 100; ++j) {
    REQUIRE(priqueue_at(&q, values[j]) == &values[j]);
  }
  for (unsigned int j = 100; j > 0; --j) {
    REQUIRE(priqueue_remove_at(&q, values[j - 1]) == &values[j - 1]);
  }
  REQUIRE(priqueue_size(&q) == 0);
  priqueue_destroy(&q);

  delete[] values;
}

TEST_CASE("priqueue_remove works with 100 values", "[priqueue_size][priqueue_offer][priqueue_at][priqueue_remove]") {
  int *values = new int[100];

  priqueue_t q;
  for (unsigned int i = 0; i < 100; i++) {
    values[i] = i;
  }
  priqueue_init(&q, compare1);
  for (unsigned int j = 0; j < 100; ++j) {
    REQUIRE(priqueue_offer(&q, &values[j]) == j);
  }
  REQUIRE(priqueue_size(&q) == 100);
  for (unsigned int j = 0; j < 100; ++j) {
    REQUIRE(priqueue_at(&q, values[j]) == &values[j]);
  }
  REQUIRE(priqueue_remove(&q, &values[23]) == 1);
  REQUIRE(priqueue_size(&q) == 99);
  REQUIRE(priqueue_remove(&q, &values[34]) == 1);
  REQUIRE(priqueue_size(&q) == 98);
  REQUIRE(priqueue_remove(&q, &values[85]) == 1);
  REQUIRE(priqueue_size(&q) == 97);
  priqueue_destroy(&q);

  delete[] values;
}
