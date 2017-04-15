/*
    mthread.c
*/
#include <mthread.h>
#include <pthread.h>



static pthread_t threads[64] = {0};
static int threads_count = 0;

mthread_thread mthread_thread_new (void* (*function) (void* argument), void* argument)
{
    pthread_t* thread = &threads[threads_count];
    int error = pthread_create (thread, NULL, function, argument);
    if(error!=0) return NULL;
    threads_count++;
    return thread;
}

void mthread_thread_join (mthread_thread thread) { pthread_join(*(pthread_t*)thread, NULL); }



static pthread_mutex_t mutexes[64] = {0};
static int mutexes_count = 0;

mthread_mutex mthread_mutex_new()
{
    pthread_mutex_t* mutex = &mutexes[mutexes_count];
    int error = pthread_mutex_init (mutex, NULL);
    if(error!=0) return NULL;
    mutexes_count++;
    return mutex;
}

bool mthread_mutex_lock   (mthread_mutex mutex, bool wait)
{
    if(wait) return (pthread_mutex_lock((pthread_mutex_t*)mutex)==0);
    else     return (pthread_mutex_trylock((pthread_mutex_t*)mutex)==0);
}

void mthread_mutex_unlock (mthread_mutex mutex) { pthread_mutex_unlock((pthread_mutex_t*)mutex); }

void mthread_mutex_free   (mthread_mutex mutex) { pthread_mutex_destroy((pthread_mutex_t*)mutex); }



#define USE_CUSTOM
#ifndef USE_CUSTOM

typedef struct _Signal
{   pthread_cond_t cond;
    pthread_mutex_t mutex;
} Signal;

static Signal signals[64] = {0};
static int signals_count = 0;

mthread_signal mthread_signal_new()
{
    Signal s = { PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER };
    Signal* signal = &signals[signals_count];
    *signal = s;
    return signal;
}

void mthread_signal_send (mthread_signal signal)
{
    Signal* s = (Signal*)signal;
    pthread_mutex_lock(&s->mutex);
    pthread_cond_signal(&s->cond);
    pthread_mutex_unlock(&s->mutex);
}

void mthread_signal_wait (mthread_signal signal)
{
    Signal* s = (Signal*)signal;
    pthread_mutex_lock(&s->mutex);
    pthread_cond_wait(&s->cond, &s->mutex);
    pthread_mutex_unlock(&s->mutex);
}

void mthread_signal_free (mthread_signal signal)
{
    Signal* s = (Signal*)signal;
    pthread_cond_destroy(&s->cond);
    pthread_mutex_destroy(&s->mutex);
}


#else


typedef struct _Signal
{   mthread_mutex mutex;
    bool signaled;
    bool acquired;
    bool waiting;
} Signal;

static Signal signals[64] = {0};
static int signals_count = 0;

mthread_signal mthread_signal_new()
{
    Signal s;
    s.mutex = mthread_mutex_new();
    mthread_mutex_lock(s.mutex,1);
    s.signaled = false;
    s.acquired = true;
    s.waiting = false;
    Signal* signal = &signals[signals_count];
    *signal = s;
    return signal;
}

void mthread_signal_send (mthread_signal signal)
{
    Signal* s = (Signal*)signal;
    bool waiting = s->waiting;
    s->signaled = true;
    if(s->acquired) mthread_mutex_unlock(s->mutex);
    if(waiting) { while(s->signaled); }
    if(!s->signaled)
    {   mthread_mutex_lock(s->mutex,1);
        s->acquired = true;
    } else s->acquired = false;
}

void mthread_signal_wait (mthread_signal signal)
{
    Signal* s = (Signal*)signal;
    s->waiting = true;
    while(!s->signaled)
    {
        mthread_mutex_lock(s->mutex,1);
        mthread_mutex_unlock(s->mutex);
    }
    s->signaled = false;
    s->waiting = false;
}

void mthread_signal_free (mthread_signal signal)
{
    Signal* s = (Signal*)signal;
    mthread_mutex_free(s->mutex); s->mutex = NULL;
}

#endif
