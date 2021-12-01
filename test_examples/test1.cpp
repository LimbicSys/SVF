#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t m1, m2;
int x = 1;
int y = 2;

void *thread1(void *arg)
{
    // pthread_mutex_lock(&m1);
    int *p = (int*)arg;
    *p = 3;
    x = 2;
    // pthread_mutex_unlock(&m1);
    return nullptr;
}

void *thread2(void *arg)
{
    // pthread_mutex_lock(&m1);
    int *p = (int*)arg;
    *p = 2;
    p = &x;
    // pthread_mutex_unlock(&m1);
    return nullptr;
}

void *thread3(void *arg)
{
    int *p = (int*)arg;
    *p = 2;
    return nullptr;
}

int main()
{
    pthread_t t1, t2, t3;
    int c = 111;
    pthread_mutex_init(&m1, nullptr);
    pthread_mutex_init(&m2, nullptr);
    pthread_create(&t1, nullptr, thread1, &c);
    // pthread_create(&t3, nullptr, thread3, nullptr);
    pthread_create(&t2, nullptr, thread2, &c);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    // pthread_join(t3, nullptr);
    return 0;
}
