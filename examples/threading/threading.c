#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // JDH begin
    int rc;
    unsigned int wait_value;
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    //printf("### threadfunc() enter : raw\n");
    //printf("###       (struct thread_data *)thread_param->m_wait_to_obtain_ms %d\n",((struct thread_data *)thread_param)->m_wait_to_obtain_ms);
    //printf("###       (struct thread_data *)thread_param->m_wait_to_release_ms %d\n",((struct thread_data *)thread_param)->m_wait_to_release_ms);
    //printf("###       (struct thread_data *)thread_param->thread_complete_success %d\n",((struct thread_data *)thread_param)->thread_complete_success);

    //printf("### threadfunc() enter : local \n");
    //printf("###       thread_func_args->m_wait_to_obtain_ms %d\n",thread_func_args->m_wait_to_obtain_ms);
    //printf("###       thread_func_args->m_wait_to_release_ms %d\n",thread_func_args->m_wait_to_release_ms);
    //printf("###       thread_func_args->thread_complete_success %d\n",thread_func_args->thread_complete_success);


    // wait arg ms
    wait_value = (unsigned int)thread_func_args->m_wait_to_obtain_ms;
    printf("threadfunc() usleep1(%u) before\n",wait_value);
    int usleep_rc = usleep(wait_value*1000);	// from the test code
    printf("threadfunc() usleep1(%u) after : usleep_rc : %d\n",wait_value, usleep_rc);

    // obtain the mutex
    rc = pthread_mutex_lock(thread_func_args->m_mutex);
    printf("threadfunc(): immediately after pthread_mutex_lock()\n");
    if (rc != 0)
    {
       printf("threadfunc() failed to lock rc:%d\n", rc);
       thread_func_args->thread_complete_success = false;
       // JDH THU assumption above and below is that thread_param gets updated as thread_func_args gets updated
       return thread_param;
    }
    printf("threadfunc(): after pthread_mutex_lock() succeeded\n");

    // wait to release mutex : this wait appears to mimic a critical section
    wait_value = thread_func_args->m_wait_to_release_ms;
    printf("threadfunc() usleep2(%u) before\n",wait_value);
    usleep_rc = usleep(wait_value*1000); // JDH WED added
    printf("threadfunc() usleep2(%u) after : usleep_rc : %d\n",wait_value, usleep_rc);

    // now unlock the mutex
    rc = pthread_mutex_unlock(thread_func_args->m_mutex);
    if (rc != 0)
    {
       printf("threadfunc() failed to unlock rc:%d\n",rc);
       thread_func_args->thread_complete_success = false; // JDH see JDH THU assumption above
       return thread_param;
    }

    // JDH end
    thread_func_args->thread_complete_success = true; // JDH see JDH THU assumption above : we made it this far so it worked
    printf("### threadfunc() exit thread_func_args->thread_complete_success : %d\n", thread_func_args->thread_complete_success);
    printf("### threadfunc() exit thread_param->thread_complete_success : %d\n", ((struct thread_data *)thread_param)->thread_complete_success);
    printf("### threadfunc() exit : true\n");
    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    // JDH begin
    int rc;
    // JDH causing issues because it's on the stack? struct thread_data my_thread_data;
    struct thread_data* my_thread_data = malloc(sizeof(struct thread_data)); // JDH but dealloc too
    my_thread_data->m_wait_to_obtain_ms = wait_to_obtain_ms;
    my_thread_data->m_wait_to_release_ms = wait_to_release_ms;
    my_thread_data->thread_complete_success = false; // let's initialize to false
    //printf("$$$ arg: wait_to_obtain_ms %d\n", wait_to_obtain_ms);
    //printf("$$$ arg: wait_to_release_ms %d\n", wait_to_release_ms);
    //printf("$$$ value: thread_complete_success %d\n", my_thread_data->thread_complete_success);
    //printf("$$$ my_thread_data : wait_to_obtain_ms %d\n", my_thread_data->m_wait_to_obtain_ms);
    //printf("$$$ my_thread_data : wait_to_release_ms %d\n", my_thread_data->m_wait_to_release_ms);
    //printf("$$$ my_thread_data : thread_complete_success %d\n", my_thread_data->thread_complete_success);

    printf("start_thread_obtaining_mutex() enter\n");

    // create a mutex
    rc = pthread_mutex_init(mutex, NULL);
    if (rc !=0)
    {
        printf("start_thread_obtaining_mutex() pthread_mutex_init() rc:%d\n",rc);
	return false;
    }
    // JDH WED : save the mutex even though it was passed in as an arg to this function
    my_thread_data->m_mutex = mutex;

    // call the thread function
    // JDH stack related issues? rc = pthread_create(thread, NULL, threadfunc, &my_thread_data);
    rc = pthread_create(thread, NULL, threadfunc, my_thread_data);

    // delete the mutex? It might be that the test code cleans it up
    if (rc ==0)
    {
        printf("start_thread_obtaining_mutex() returns true\n");
	return true;
    }

    // wait for thread to finish
    pthread_join(*thread, NULL); // JDH WED : without it maybe main exits and we get a crash?
    free(my_thread_data);	// JDH WED : to prevent memory leak
 
    // JDH end
    return false;
}

