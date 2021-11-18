#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t m1, m2;
int x = 1;
int *p;
int y = 2;
int *q = &x;

void *thread1(void *arg)
{
    // pthread_mutex_lock(&m1);
    p = &x;
    *q = 3;
    x = 2;
    // pthread_mutex_unlock(&m1);
    return nullptr;
}

void *thread2(void *arg)
{
    // pthread_mutex_lock(&m1);
    *p = 2;
    int y = *q;
    *q = 4;
    // pthread_mutex_unlock(&m1);
    return nullptr;
}

void *thread3(void *arg)
{
    *p = 2;
    return nullptr;
}

int main()
{
    pthread_t t1, t2, t3;
    pthread_mutex_init(&m1, nullptr);
    pthread_mutex_init(&m2, nullptr);
    pthread_create(&t1, nullptr, thread1, nullptr);
    // pthread_create(&t3, nullptr, thread3, nullptr);
    pthread_create(&t2, nullptr, thread2, nullptr);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    // pthread_join(t3, nullptr);
    int c = y;
    return 0;
}
