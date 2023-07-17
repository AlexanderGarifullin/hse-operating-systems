#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 256

// Идентификаторы для канала.
int pipefd[2];
// 0 - чтение.
// 1 - запись.

// Запись сообщения в неименованный канал. 
void sigusr1Handler(int sig) {
    const char* message = "Привет родитель, я потомок!";
    int bytes_written;

    printf("Записываем данные из потомка\n");
    // Запись данных в канал.
    bytes_written = write(pipefd[1], message, strlen(message) + 1);
    // Проверка записи данных.
    if (bytes_written == -1) {
        perror("Ошибка записи в канал");
        exit(5);
    }
    // Закрываем ненужный дескриптор. 
    close(pipefd[1]);
}


int main(){
    // Идентификатор процесса.
    pid_t pid;
    
    // Создание неименованного канала.
     if (pipe(pipefd) == -1) {
        perror("Ошибка создания канала");
        exit(2);
    }
    
    // Рождение процесса потомка. 
    pid = fork();
    // Проверка рождения.
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    // Дочерный процесс. 
    // Получает сигнал SIGUSR1. Передает родительскому процессу произвольную строку. 
    else if (pid == 0) {
        printf("Начался потомок\n");
        // Установка обработчика сигнала SIGUSR1
        signal(SIGUSR1, sigusr1Handler); 
        // Ожидаем сигнал. 
        pause();
        printf("Потомок закончился\n");
        // Закрываем ненужный дескриптор. 
        close(pipefd[0]);
        exit(0);
    }
    // Родитель процесс. 
    // Передает сигнал SIGUSR1. Ждет произвольную строку от дочернего процесса. Печатает полученную строку. 
    else {
        // Пауза для гарантированного создания дочернего процесса.
        sleep(1);
        
        // Закрываем ненужный дескрипторы. 
        close(pipefd[1]);
        
        printf("Начался родитель\n");
        
        // Передача сигнала SIGUSR1 дочернему процессу
        if (kill(pid, SIGUSR1) == -1) {
            perror("Ошибка отправки сигнала");
            exit(3);
        }
        
        // Пауза для гарантированной записи данных в канал потомком.
        sleep(1);
        
        char buffer[BUFFER_SIZE];
        int bytes_read;

        printf("Читаем данные в родителе\n");
        // Чтение данных из канала
        bytes_read = read(pipefd[0], buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            printf("Получено от потомка: %s\n", buffer);
        } else {
            perror("Ошибка чтения из канала");
            exit(4);
        }
        
        printf("Родитель закoнчился\n");
        // Закрываем ненужный дескрипторы. 
        close(pipefd[0]);
       
        exit(0);
    }
    return 0;
}
