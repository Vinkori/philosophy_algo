#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Кількість філософів (і виделок)
#define N 5 

pthread_mutex_t forks[N];

typedef struct {
    int id;
} PhilosopherArgs;

void* philosopher_routine(void* arg) {
    PhilosopherArgs* args = (PhilosopherArgs*)arg;
    int id = args->id;

    unsigned int seed = id ^ time(NULL);

    int left_fork = id;
    int right_fork = (id + 1) % N;

    // спочатку беремо виделку з меншим номером
    int first_fork = (left_fork < right_fork) ? left_fork : right_fork;
    int second_fork = (left_fork > right_fork) ? left_fork : right_fork;

    while (1) {
        printf("Філософ %d розмірковує...\n", id);
        // Спить випадковий час від 0.5 до 1.5 секунди
        usleep(500000 + (rand_r(&seed) % 1000000));

        printf("Філософ %d зголоднів. Намагається взяти виделку %d.\n", id, first_fork);

        // Беремо виднлку 1 з меньшим індексом
        pthread_mutex_lock(&forks[first_fork]);
        printf("Філософ %d ВЗЯВ першу виделку %d.\n", id, first_fork);

        // Намагаємось взяти другу виделку
        printf("Філософ %d намагається взяти другу виделку %d.\n", id, second_fork);
        pthread_mutex_lock(&forks[second_fork]);
        printf("Філософ %d ВЗЯВ другу виделку %d.\n", id, second_fork);

        printf(">>> Філософ %d ЇСТЬ! <<<\n", id);
        usleep(500000 + (rand_r(&seed) % 1000000));

        pthread_mutex_unlock(&forks[second_fork]);
        pthread_mutex_unlock(&forks[first_fork]);

        printf("Філософ %d поклав обидві виделки (%d та %d) і знову ситий.\n", id, first_fork, second_fork);
    }

    return NULL;
}

int main() {
    pthread_t philosophers[N];
    PhilosopherArgs args[N];

    // Ініціалізуємо м'ютекси
    for (int i = 0; i < N; i++) {
        if (pthread_mutex_init(&forks[i], NULL) != 0) {
            fprintf(stderr, "Помилка ініціалізації м'ютекса для виделки %d\n", i);
            return 1;
        }
    }

    // Створюємо потоки
    for (int i = 0; i < N; i++) {
        args[i].id = i;
        if (pthread_create(&philosophers[i], NULL, philosopher_routine, &args[i]) != 0) {
            fprintf(stderr, "Помилка створення потоку для філософа %d\n", i);
            return 1;
        }
    }
    sleep(30);
    printf("Завершення роботи. Скасування потоків...\n");

    // Завершення потоків
    for (int i = 0; i < N; i++) {
        pthread_cancel(philosophers[i]);
        pthread_join(philosophers[i], NULL);
    }

    // Знищення м'ютексів
    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    return 0;
}