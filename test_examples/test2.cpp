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
    // p = &x;
    x = 2;
    // pthread_mutex_unlock(&m1);
    return nullptr;
}

void *thread2(void *arg)
{
    // pthread_mutex_lock(&m1);
    int *p = (int*)arg;
    *p = 2;
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
    int *p = &y;
    thread1(p);
    thread2(p);
    int c = y;
    return 0;
}
