
#include <stdio.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

#include <assert.h>

#define ASSERT(x) assert(x)

typedef struct QueueItem
{
    struct QueueItem *next;

    void *arg;
    size_t size;
}   QueueItem;

typedef struct Queue
{
    QueueItem *head;
    pthread_mutex_t mutex;
    sem_t semaphore;
}   Queue;

static QueueItem* _queue_pop(Queue *queue)
{
    int err = pthread_mutex_lock(& queue->mutex);
    ASSERT(err == 0);

    QueueItem *item = queue->head;
    if (item)
    {
        //  remove from list
        queue->head = item->next;
        item->next = 0;
    }

    err = pthread_mutex_unlock(& queue->mutex);
    ASSERT(err == 0);

    return item;
}

static void _queue_put(Queue *queue, QueueItem *item)
{
    int err = pthread_mutex_lock(& queue->mutex);
    ASSERT(err == 0);

    QueueItem **head = & queue->head;
    for (; *head; head = &((*head)->next))
    {
        ;
    }

    item->next = 0;
    *head = item;

    err = pthread_mutex_unlock(& queue->mutex);
    ASSERT(err == 0);
}

void queue_put(Queue *queue, void *arg, size_t size)
{
    QueueItem *item = (QueueItem*) malloc(sizeof(QueueItem));
    memset(item, 0, sizeof(*item));
    item->arg = arg;
    item->size = size;
    // TODO make the thing bigger, memcpy the data ...
}

void queue_init(Queue *queue)
{
    int err = pthread_mutex_init(& queue->mutex, 0);
    ASSERT(err == 0);
    err = sem_init(& queue->semaphore, 0, 0);
    ASSERT(err == 0);
}

void queue_free(Queue *queue)
{
    int err = pthread_mutex_destroy(& queue->mutex);
    ASSERT(err == 0);
    err = sem_destroy(& queue->semaphore);
    ASSERT(err == 0);
}

int main(int argc, char ** argv)
{
    return 0;
}
