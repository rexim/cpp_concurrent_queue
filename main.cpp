#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <pthread.h>

#if 1
static inline
void ec(int n)
{
    if (n) {
        fprintf(stderr, "pthread failed: %s\n", strerror(n));
        abort();
    }
}
#else
#define ec(e) e
#endif

template <typename T, size_t Capacity = 256>
struct Concurrent_Queue
{
    T elements[Capacity];
    size_t begin;
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;
    int is_done;

    void done()
    {
        ec(pthread_mutex_lock(&mutex));
        is_done = 1;
        ec(pthread_mutex_unlock(&mutex));
        pthread_cond_signal(&cond_empty);
    }

    void push(T element)
    {
        ec(pthread_mutex_lock(&mutex));
        while (size >= Capacity) {
            ec(pthread_cond_wait(&cond_full, &mutex));
        }

        elements[(begin + size) % Capacity] = element;
        size += 1;
        ec(pthread_mutex_unlock(&mutex));
        ec(pthread_cond_signal(&cond_empty));
    }

    int pop(T *element)
    {
        ec(pthread_mutex_lock(&mutex));
        while (size == 0) {
            if (is_done) {
                ec(pthread_mutex_unlock(&mutex));
                ec(pthread_cond_signal(&cond_empty));
                return 0;
            }
            ec(pthread_cond_wait(&cond_empty, &mutex));
        }

        if (element) *element = elements[begin];
        begin = (begin + 1) % Capacity;
        size -= 1;

        ec(pthread_mutex_unlock(&mutex));
        ec(pthread_cond_signal(&cond_full));

        return 1;
    }
};

const int ELEMENTS_COUNT = 100;

void *consumer(void *arg)
{
    Concurrent_Queue<int> *queue = (Concurrent_Queue<int> *)arg;

    int x = 0;
    while (queue->pop(&x)) {
        fprintf(stderr, "%lu: %d\n", pthread_self(), x);
    }

    return NULL;
}

Concurrent_Queue<int> queue;
const size_t THREAD_COUNT = 100;
pthread_t threads[THREAD_COUNT];

int main(int argc, char *argv[])
{
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        ec(pthread_create(&threads[i], NULL, consumer, &queue));
    }

    for (int i = 0; i < ELEMENTS_COUNT; ++i) {
        queue.push(i);
    }
    queue.done();

    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        ec(pthread_join(threads[i], NULL));
    }

    return 0;
}
