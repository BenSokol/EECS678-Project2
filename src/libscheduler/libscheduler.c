/** @file libscheduler.c
 */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"

#include "../libpriqueue/libpriqueue.h"


/** @struct job_t
 *  @brief job_t node structure
 *  @var job_t::id
 *  Member 'id' contains the id of this job (constant).
 *  @var job_t::arrival_time
 *  Member 'arrival_time' contains the arrival time of this job (constant).
 *  @var job_t::running_time
 *  Member 'running_time' contains the total running time of this job (constant).
 *  @var job_t::remaining_time
 *  Member 'remaining_time' contains the current remaining time of this job.
 *  @var job_t::priority
 *  Member 'priority' contains the priority of this job (constant).
 *  @var job_t::core_number
 *  Member 'core_number' contains the current core running this job.
 *  @var job_t::start_time
 *  Member 'start_time' contains the first start time of this job. This is used to calculate the response time.
 *  @var job_t::last_updated_time
 *  Member 'last_updated_time' contains the last time this job's remaining time was updated.
 */
typedef struct job_t {
  int id;
  float arrival_time;
  float running_time;
  float remaining_time;
  int priority;
  int core_number;
  int start_time;
  int last_updated_time;
} job_t;

/**
 * \var static float total_waiting_time;
 * \brief Total waiting time
 */
static float total_waiting_time;

/**
 * \var static float total_response_time;
 * \brief Total response time
 */
static float total_response_time;

/**
 * \var static float total_turnaround_time;
 * \brief Total turnaround time
 */
static float total_turnaround_time;

/**
 * \var static unsigned int total_finished_jobs;
 * \brief Total finished jobs
 */
static unsigned int total_finished_jobs;

/**
 * \var static unsigned int cores;
 * \brief Number of cores this scheduler will use
 */
static unsigned int cores;

/**
 * \var static job_t **core_arr;
 * \brief Array of job_t pointers representing a processor with n cores
 */
static job_t **core_arr;

/**
 * \var static scheme_t scheme;
 * \brief What scheme to use
 */
static scheme_t scheme;

/**
 * \var static priqueue_t queue;
 * \brief Priority queue to hold jobs
 */
static priqueue_t queue;

/**
* Compare function for First Come First Serve (FCFS)
*
* @param a a pointer to the lhs job_t
* @param b a pointer to the rhs job_t
* @return -1
*
* See also @ref comparer-page
*/
int fcfs(const void *a, const void *b) {
  (void)a;
  (void)b;
  return -1;
}

/**
* Compare function for Shortest Job First (SJF)
*
* @param a a pointer to the lhs job_t
* @param b a pointer to the rhs job_t
* @return lhs->running_time - rhs->running_time, unless running times are the same, then lhs->arrival_time - rhs->arrival_time.
*
* See also @ref comparer-page
*/
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

/**
* Compare function for Preemptive Shortest Job First (PSJF)
*
* @param a a pointer to the lhs job_t
* @param b a pointer to the rhs job_t
* @return lhs->remaining_time - rhs->remaining_time, unless remaining times are the same, then lhs->arrival_time - rhs->arrival_time.
*
* See also @ref comparer-page
*/
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

/**
* Compare function for Priority (PRI)
*
* @param a a pointer to the lhs job_t
* @param b a pointer to the rhs job_t
* @return lhs->priority - rhs->priority, unless priorities are the same, then lhs->arrival_time - rhs->arrival_time.
*
* See also @ref comparer-page
*/
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

/**
* Compare function for Preemptive Priority (PPRI)
*
* @param a a pointer to the lhs job_t
* @param b a pointer to the rhs job_t
* @return lhs->priority - rhs->priority, unless priorities are the same, then lhs->arrival_time - rhs->arrival_time.
*
* See also @ref comparer-page
*/
int ppri(const void *a, const void *b) {
  return pri(a, b);
}

/**
* Compare function for First Come First Serve (FCFS)
*
* @param a a pointer to the lhs job_t
* @param b a pointer to the rhs job_t
* @return -1
*
* See also @ref comparer-page
*/
int rr(const void *a, const void *b) {
  (void)a;
  (void)b;
  return -1;
}


/**
  Initalizes the scheduler.

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param _cores the number of cores that is available by the scheduler.
   These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param _scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int _cores, scheme_t _scheme) {
  assert(_cores > 0);
  total_waiting_time = 0.0;
  total_response_time = 0.0;
  total_turnaround_time = 0.0;
  total_finished_jobs = 0;
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
  for (unsigned int i = 0; i < cores; ++i) {
    core_arr[i] = NULL;
  }
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
  @return index of core that the job should be scheduled on
  @return -1 if no scheduling changes should be made.

 */
int scheduler_new_job(int job_number, int time, int running_time, int priority) {
  job_t *toAdd = (job_t *)malloc(sizeof(job_t));

  toAdd->id = job_number;
  toAdd->arrival_time = time;
  toAdd->running_time = running_time;
  toAdd->remaining_time = running_time;
  toAdd->priority = priority;
  toAdd->core_number = -1;
  toAdd->start_time = -1;
  toAdd->last_updated_time = -1;

  if (scheme == PSJF) {
    // Preemptive Shortest Job First
    for (unsigned int i = 0; i < cores; ++i) {
      if (core_arr[i] != NULL) {
        core_arr[i]->remaining_time -= time - core_arr[i]->last_updated_time;
        core_arr[i]->last_updated_time = time;
      }
    }
  }

  unsigned int index = priqueue_offer(&queue, toAdd);

  if (index < cores) {
    // Attempt to add to core_arr, if available spot
    for (unsigned int i = 0; i < cores; ++i) {
      if (core_arr[i] == NULL) {
        toAdd->core_number = i;
        toAdd->start_time = time;
        toAdd->last_updated_time = time;
        core_arr[i] = toAdd;
        return i;
      }
    }

    // no cores are available, if preemptive try to add
    int core_to_run_on = -1;
    if (scheme == PSJF || scheme == PPRI) {
      if (scheme == PSJF) {
        // Preemptive Shortest Job First
        float longestTimeRemaining = core_arr[0]->remaining_time;
        core_to_run_on = 0;
        for (unsigned int i = 0; i < cores; ++i) {
          if (core_arr[i]->remaining_time > longestTimeRemaining) {
            longestTimeRemaining = core_arr[i]->remaining_time;
            core_to_run_on = i;
          }
        }
        if (longestTimeRemaining > toAdd->remaining_time) {
          core_arr[core_to_run_on]->core_number = -1;
          if (core_arr[core_to_run_on]->start_time == time) {
            core_arr[core_to_run_on]->start_time = -1;
          }
          toAdd->core_number = core_to_run_on;
          toAdd->start_time = time;
          toAdd->last_updated_time = time;
          core_arr[core_to_run_on] = toAdd;
        }
        else {
          core_to_run_on = -1;
        }
      }
      else if (scheme == PPRI) {
        // preemptive Priority
        int maxPriority = core_arr[0]->priority;
        core_to_run_on = 0;
        for (unsigned int i = 0; i < cores; ++i) {
          if (core_arr[i]->priority == maxPriority && core_arr[i]->start_time > core_arr[core_to_run_on]->start_time) {
            maxPriority = core_arr[i]->priority;
            core_to_run_on = i;
          }
          else if (core_arr[i]->priority > maxPriority) {
            maxPriority = core_arr[i]->priority;
            core_to_run_on = i;
          }
        }
        if (maxPriority > toAdd->priority) {
          core_arr[core_to_run_on]->core_number = -1;
          if (core_arr[core_to_run_on]->start_time == time) {
            core_arr[core_to_run_on]->start_time = -1;
          }
          toAdd->core_number = core_to_run_on;
          toAdd->start_time = time;
          toAdd->last_updated_time = time;
          core_arr[core_to_run_on] = toAdd;
        }
        else {
          core_to_run_on = -1;
        }
      }
    }

    return core_to_run_on;
  }

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
  int id = -1;
  core_arr[core_id] = NULL;

  for (unsigned int i = 0; i < priqueue_size(&queue); ++i) {
    if (((job_t *)priqueue_at(&queue, i))->id == job_number) {
      job_t *job = priqueue_remove_at(&queue, i);
      assert(job->arrival_time != -1);
      assert(job->running_time != -1);
      assert(job->start_time != -1);
      assert(job->last_updated_time != -1);
      total_waiting_time += time - job->arrival_time - job->running_time;
      total_response_time += job->start_time - job->arrival_time;
      total_turnaround_time += time - job->arrival_time;
      total_finished_jobs++;
      free(job);
    }
  }

  if (priqueue_size(&queue) != 0) {
    for (unsigned int i = 0; i < priqueue_size(&queue); ++i) {
      job_t *job = priqueue_at(&queue, i);
      if (job->core_number == -1) {
        job->core_number = core_id;
        if (job->start_time == -1) {
          job->start_time = time;
        }
        job->last_updated_time = time;
        id = job->id;
        core_arr[core_id] = job;
        break;
      }
    }
  }

  return id;
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
  int id = -1;

  for (unsigned int i = 0; i < priqueue_size(&queue); ++i) {
    if (((job_t *)priqueue_at(&queue, i))->id == core_arr[core_id]->id) {
      job_t *job = priqueue_remove_at(&queue, i);
      job->core_number = -1;
      core_arr[core_id] = NULL;
      priqueue_offer(&queue, job);
      break;
    }
  }

  for (unsigned int i = 0; i < priqueue_size(&queue); ++i) {
    if (((job_t *)priqueue_at(&queue, i))->core_number == -1) {
      job_t *job = priqueue_at(&queue, i);
      job->core_number = core_id;
      core_arr[core_id] = job;
      if (job->start_time == -1) {
        job->start_time = time;
      }
      job->last_updated_time = time;
      id = job->id;
      break;
    }
  }

  return id;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
      (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time() {
  return (total_finished_jobs == 0 ? 0.0 : total_waiting_time / (float)total_finished_jobs);
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
      (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time() {
  return (total_finished_jobs == 0 ? 0.0 : total_turnaround_time / (float)total_finished_jobs);
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete
      (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time() {
  return (total_finished_jobs == 0 ? 0.0 : total_response_time / (float)total_finished_jobs);
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
  for (unsigned int i = 0; i < priqueue_size(&queue); ++i) {
    job_t *job = priqueue_at(&queue, i);
    fprintf(stdout, "%u(%d) ", job->id, job->priority);
  }
}
