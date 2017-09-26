#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <thread>
#include <time.h>

using namespace std;

pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;

unsigned SumExo2 = 0;

typedef struct s_vect {
    int v1;
    int v2;
} vect;

typedef struct s_sum {
    int * all;
    int size;
} sum;

typedef struct s_thrWithID {
    unsigned ID;
    unsigned size;
    pthread_mutex_t *verroux;
} thrWithID;

void * DisplayLoop (void * p){
    pthread_t moi = pthread_self();
    for (unsigned i (0); i < 10; ++i)
        cout << "thread " << moi << " : " << i << endl;

    pthread_exit(NULL);
}

void * DisplayChar (void * p) {
    pthread_t moi = pthread_self();
    cout << "thread " << moi << " : " << (char *) p << endl;
}

void Question1 () {
   pthread_t t1, t2;

   if (pthread_create(&t1, NULL, DisplayLoop, NULL) != 0) cout << "creation error !" << endl;

   if (pthread_create(&t2, NULL, DisplayLoop, NULL) != 0) cout << "creation error !" << endl;

   pthread_join(t1, NULL);
   pthread_join(t2, NULL);
}

void Question4 () {
    char g = 'g';

    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, DisplayChar, &g) != 0) cout << "creation error !" << endl;

    if (pthread_create(&t2, NULL, DisplayChar, &g) != 0) cout << "creation error !" << endl;

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}

void * Addition (void * p){

    int * res = (int *) malloc(sizeof(int));
    *res = 0;

    pthread_mutex_lock(&verrou);

    for (unsigned i (0); i < ((sum *)p)->size; i++)
        *res += ((sum *)p)->all[i];

    pthread_mutex_unlock(&verrou);

    pthread_exit(res);

}

void * Multiplication (void * p){

    int * res = (int *) malloc(sizeof(int));

    *res = ((vect*)p)->v1 * ((vect*)p)->v2;

    cout << *res << endl;

    pthread_exit(res);

}

void Exo2MainThread(unsigned n){

    vect v[n];
    sum res;

    res.size = n;

    pthread_t mult [n], add;
    unsigned Sum (0);
    int * ptr;

    for (unsigned i (0); i < n; ++i) {
        v[i].v1 = rand() % 11;
        v[i].v2 = rand() % 11;
    }

    pthread_mutex_lock(&verrou);

    for (unsigned i (0); i < n; ++i) {
        if (pthread_create(&mult[i], NULL, Multiplication, &v[i]) != 0) cout << "creation error !" << endl;
    }

    if (pthread_create(&add, NULL, Addition, &res) != 0) cout << "creation error !" << endl;

    for (unsigned i (0); i < n; ++i) {
        pthread_join(mult[i], (void **) &ptr);
        res.all[i] = *ptr;
    }

    pthread_mutex_unlock(&verrou);

    pthread_join(add, (void **) &ptr);
    Sum = *ptr;

    cout << "Sum : " << Sum << endl;
}

void * MultiplicationQ4 (void * p){

    int * res = (int *) malloc(sizeof(int));

    *res = ((vect*)p)->v1 * ((vect*)p)->v2;

    pthread_mutex_lock(&verrou);

    cout << "adding " << *res << " to sum" << endl;
    SumExo2 += *res;

    pthread_mutex_unlock(&verrou);
}

void Exo2MainThread2(unsigned n){
    vect v[n];
    pthread_t mult [n];

    for (unsigned i (0); i < n; ++i) {
        v[i].v1 = rand() % 10 + 1;
        v[i].v2 = rand() % 10 + 1;
    }

    for (unsigned i (0); i < n; ++i) {
        if (pthread_create(&mult[i], NULL, MultiplicationQ4, &v[i]) != 0) cout << "creation error !" << endl;
    }

    for (unsigned i (0); i < n; ++i) {
        pthread_join(mult[i],NULL);
    }

    cout << "Sum : " << SumExo2 << endl;
}

unsigned TachePred;

void * Tache(void * p){

    pthread_mutex_unlock(&((thrWithID*)p)->verroux[((thrWithID*)p)->ID]);

    cout << "thread " << ((thrWithID*)p)->ID << " : Lock " << ((thrWithID*)p)->ID << " is locked" << endl;
    pthread_mutex_lock(&((thrWithID*)p)->verroux[((thrWithID*)p)->ID]);


    cout << "thread " << ((thrWithID*)p)->ID << " : computing" << endl;

    unsigned Calc = 0;

    while (Calc++ != 10000000){}

    pthread_mutex_unlock(&((thrWithID*)p)->verroux[((thrWithID*)p)->ID]);
    cout << "thread " << ((thrWithID*)p)->ID << " : Lock " << ((thrWithID*)p)->ID << " is unlocked" << endl;

    cout << "thread " << ((thrWithID*)p)->ID << " : waiting for other thread" << endl;

    for (unsigned i (0); i < ((thrWithID*)p)->size ; ++i)
        if (i != ((thrWithID*)p)->ID){
            cout << "thread " << ((thrWithID*)p)->ID << " : Lock " << i << " is locked" << endl;
            pthread_mutex_lock(&((thrWithID*)p)->verroux[i]);
            pthread_mutex_unlock(&((thrWithID*)p)->verroux[i]);
            cout << "thread " << ((thrWithID*)p)->ID << " : Lock " << i << " is unlocked" << endl;
        }


    cout << "thread " << ((thrWithID*)p)->ID << " : wait ended" << endl;
}

void Exo3MainThread (unsigned n){
    thrWithID threads[n];
    pthread_t tache[n];
    pthread_mutex_t verroux[n];

    for (unsigned i (0); i < n; ++i){
        verroux[n] = PTHREAD_MUTEX_INITIALIZER;
    }

    for (unsigned i (0); i < n; ++i){
        threads[i].ID = i;
        threads[i].size = n;
        threads[i].verroux = verroux;
    }


    for (unsigned i (0); i < n; ++i)
        if (pthread_create(&tache[i], NULL, Tache, &threads[i]) != 0)
            cout << "creation error !" << endl;

    for (unsigned i (0); i < n ; ++i)
        pthread_join(tache[i], NULL);

}


int main(){
    srand(time(NULL));

    /*cout << "EXO 1" << endl;

    cout << "Question 1 :  " << endl;

    Question1();

    cout << "Question 2 : Les threads ne s'executeront pas" << endl;
    cout << "Question 3 : Le programme entier s'arrete" << endl;
    cout << "Question 4 : " << endl;

    Question4();


    cout << "EXO 2" << endl;

    cout << "Question 3 :  " << endl;*/
    cout << "Nombre de taches : ";

    unsigned n;
    cin >> n;


    Exo3MainThread(n);




    return 0;
}
