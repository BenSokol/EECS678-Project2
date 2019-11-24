/** @file libscheduler.c
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"

#include "../libpriqueue/libpriqueue.h"


typedef struct _job_t {
  int id;
  float arrival_time;
  float running_time;
  float remaining_time;
  int priority;
} job_t;


static float total_waiting_time;
static float total_response_time;
static float total_turnaround_time;
static unsigned int total_finished_jobs;
static unsigned int cores;
static job_t **core_arr;
static scheme_t scheme;
static priqueue_t queue;


int fcfs(const void *a, const void *b) {
  (void)a;
  (void)b;
  return 1;
}


int sjf(const void *a, const void *b) {
  job_t const *lhs = (job_t *)a;
  job_t const *rhs = (job_t *)b;

  if (lhs->running_time != rhs->running_time) {
    return lhs->running_time - rhs->running_time;
  }
  else {
    return lhs->arrival_time - rhs->arrival_time;
  }
}


int psjf(const void *a, const void *b) {
  job_t const *lhs = (job_t *)a;
  job_t const *rhs = (job_t *)b;

  if (lhs->remaining_time != rhs->remaining_time) {
    return lhs->remaining_time - rhs->remaining_time;
  }
  else {
    return lhs->arrival_time - rhs->arrival_time;
  }
}


int pri(const void *a, const void *b) {
  job_t const *lhs = (job_t *)a;
  job_t const *rhs = (job_t *)b;

  if (lhs->priority != rhs->priority) {
    return lhs->priority - rhs->priority;
  }
  else {
    return lhs->arrival_time - rhs->arrival_time;
  }
}


int ppri(const void *a, const void *b) {
  return pri(a, b);
}


int rr(const void *a, const void *b) {
  (void)a;
  (void)b;
  return 1;
}


/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler.
   These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int _cores, scheme_t _scheme) {
  assert(_cores > 0);
  cores = (unsigned int)_cores;
  scheme = _scheme;

  int (*comparer)(const void *, const void *) = NULL;

  switch (scheme) {
    case FCFS:
      comparer = fcfs;
      break;
    case SJF:
      comparer = sjf;
      break;
    case PSJF:
      comparer = psjf;
      break;
    case PRI:
      comparer = pri;
      break;
    case PPRI:
      comparer = ppri;
      break;
    case RR:
      comparer = rr;
      break;
    default:
      assert(false);
      break;
  }

  priqueue_init(&queue, comparer);
  core_arr = (job_t **)malloc(cores * sizeof(job_t *));
}


/**
  Called when a new job arrives.

  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumption:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority) {
  (void)job_number;
  (void)time;
  (void)running_time;
  (void)priority;
  return -1;
}


/**
  Called when a job has completed execution.

  The core_id, job_number and time parameters are provided for convenience.
    You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.

  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time) {
  (void)core_id;
  (void)job_number;
  (void)time;
  return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.

  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time) {
  (void)core_id;
  (void)time;
  return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
      (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time() {
  if (total_finished_jobs == 0) {
    return 0.0;
  }
  else {
    return total_waiting_time / (float)total_finished_jobs;
  }
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
      (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time() {
  if (total_finished_jobs == 0) {
    return 0.0;
  }
  else {
    return total_turnaround_time / (float)total_finished_jobs;
  }
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
      (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time() {
  if (total_finished_jobs == 0) {
    return 0.0;
  }
  else {
    return total_response_time / (float)total_finished_jobs;
  }
}


/**
  Free any memory associated with your scheduler.

  Assumption:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up() {
  priqueue_destroy(&queue);

  for (unsigned int i = 0; i < cores; ++i) {
    if (core_arr[i] != NULL) {
      free(core_arr[i]);
      core_arr[i] = NULL;
    }
  }

  free(core_arr);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the
    jobs in the order they are to be scheduled. Furthermore, we have also
    listed the current state of the job (either running on a given core or idle).
    For example, if we have a non-preemptive algorithm and job(id=4) has began running,
    job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority,
    the output in our sample output will be:

    2(-1) 4(0) 1(-1)

  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue() {
}
