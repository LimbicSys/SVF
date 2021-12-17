#include <vector>
#include <stdio.h>
#include <pthread.h>

using namespace std;


struct Node {
    vector<int> v;
};

void accessNode(const Node* n) {
    size_t sz = n->v.size();
    printf("%lu\n", sz);
}

void *thread_func(void *arg) {
    const Node *n = (const Node*)arg;
    accessNode(n);
    return nullptr;
}


int main() {
    pthread_t t1, t2;
    Node *node = new Node();
    pthread_create(&t1, nullptr, thread_func, node);
    pthread_create(&t2, nullptr, thread_func, node);
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    delete node;
    return 0;
}
