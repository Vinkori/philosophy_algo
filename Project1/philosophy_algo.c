#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>

// Кількість філософів (і виделок)
#define N 5 

pthread_mutex_t forks[N];

// Глобальний прапорець для безпечного завершення роботи потоків
volatile int keep_running = 1;

typedef struct
{
    int id;
} PhilosopherArgs;

void* philosopher_routine(void* arg)
{
    PhilosopherArgs* args = (PhilosopherArgs*)arg;
    int id = args->id;

    unsigned int seed = id ^ time(NULL);

    int left_fork = id;
    int right_fork = (id + 1) % N;

    int first_fork = (left_fork < right_fork) ? left_fork : right_fork;
    int second_fork = (left_fork > right_fork) ? left_fork : right_fork;

    while (keep_running)
    {
        printf("Філософ %d розмірковує...\n", id);
        usleep(500000 + (rand_r(&seed) % 1000000));

        // Перед тим як почати брати виделки, перевіряємо, чи не час закінчувати
        if (!keep_running) break;

        printf("Філософ %d зголоднів. Намагається взяти виделку %d.\n", id, first_fork);

        pthread_mutex_lock(&forks[first_fork]);
        printf("Філософ %d ВЗЯВ першу виделку %d.\n", id, first_fork);

        printf("Філософ %d намагається взяти другу виделку %d.\n", id, second_fork);
        pthread_mutex_lock(&forks[second_fork]);
        printf("Філософ %d ВЗЯВ другу виделку %d.\n", id, second_fork);

        printf(">>> Філософ %d ЇСТЬ! <<<\n", id);
        usleep(500000 + (rand_r(&seed) % 1000000));

        pthread_mutex_unlock(&forks[second_fork]);
        pthread_mutex_unlock(&forks[first_fork]);

        printf("Філософ %d поклав обидві виделки (%d та %d) і знову ситий.\n", id, first_fork, second_fork);
    }

    printf("Філософ %d завершив роботу.\n", id);
    return NULL;
}

int main()
{
    setlocale(LC_ALL, "");
    pthread_t philosophers[N];
    PhilosopherArgs args[N];

    for (int i = 0; i < N; i++)
    {
        if (pthread_mutex_init(&forks[i], NULL) != 0) {
            fprintf(stderr, "Помилка ініціалізації м'ютекса для виделки %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < N; i++)
    {
        args[i].id = i;
        if (pthread_create(&philosophers[i], NULL, philosopher_routine, &args[i]) != 0) {
            fprintf(stderr, "Помилка створення потоку для філософа %d\n", i);
            return 1;
        }
    }

    sleep(30);
    printf("Час вийшов! Даємо команду філософам доїсти і завершити роботу...\n");

    keep_running = 0;

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }

    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    printf("Програма успішно завершена.\n");
    return 0;
}