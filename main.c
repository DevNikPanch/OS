#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

// Изменение текущей директории на указанную
int cd_fnc(char *args[]) {
    if (args[1] == NULL) {
        char path[256];
        getcwd(path, 256);
        printf("%s\n", path);
    } else {
        if (chdir(args[1])) { // попытка сменить директорию
            perror("cd");
        }
    }
    return 0;
}


// Выводит список файлов по указанной директории
int dir_fnc(char *args[]){

    DIR *dir;
    if (args[1] == NULL) { // вывод каталога файлов текущенго пути
        char path[256];
        getcwd(path, 256);
        dir = opendir(path);
    }
    else{ // передача введённого каталога
        dir = opendir(args[1]);
    }

    if (dir == NULL) { // проверка на существование пути
        perror("dir");
    }

    struct dirent *entry; // структура для вхождений, которая хранит данные о файле

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}

// Отображение перемененных сред
int environ_fnc(char *args[]) {
    extern char **environ;

    int i = 0;
    while(environ[i]) {
        printf("%s\n", environ[i++]);
    }

    return 0;
}

// Отображение указанного текста
int echo_fnc(char *args[]) {
    if (args[1] == NULL) {
        printf("Usage: echo <something>\n");
    } else {
        int i = 1;
        while (args[i] != NULL){
            printf("%s ", args[i]);
            i++;
        }
        printf("\n");
    }
    return 0;

}

// Приостановление выполнения оболочки
int pause_fnc(char *args[]) {
    printf("Press any key...");
    getchar();
    return 0;
}

// Очищение экрана
int clr_fnc(char *args[]) {
    printf("\033c");
    return 0;
}

// Ззавершение выполнения оболочки
int quit_fnc(char *args[]) {
    return 1;
}

// ----------------------------------------------------------------------------------------

// Вывод списка доступных команд
int help_fnc(char *args[]) {
    printf("Available commands:\n");
    printf("cd <directory> - change directory to <directory>. If argument <directory> isn't present, then output current one.\n");
    printf("dir <directory> - shows all files in <directory>\n");
    printf("environ - shows all environmental variables\n");
    printf("echo <text> - outputs <text>\n");
    printf("pause - makes a pause, until u press any key\n");
    printf("help - help-box\n");
    printf("clr - clears the screen\n");
    printf("quit - exits from shell\n");
    printf("\nUse 'help' to display this help message.\n");
    return 0;
}

// Запускает внешние программы
int cmd_run (char *args[]) {
    pid_t pid = fork(); // создание нового процесса

    switch (pid) {
        case -1: // при вызове fork возникла ошибка
            perror("fork");
            break;

        case 0: // код потомка
            execvp(args[0], args); // дублирование действия оболочки
            perror("execvp");
            exit(EXIT_FAILURE);

        default: // код родительского процесса
            wait(NULL); // ожидание завершения дочернего процесса
            break;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char * inp_ptr; // указатель на строку
    size_t len; // размер строки
    int flag = 0; // статус выполнения, если не 0, то программа завершается
    char *args[10];
    int i; // индекс для args

    char path[256];
    getcwd(path, 256);
    setenv("shell", strcat(path, "/myshell"), 1); // добавление (замена) нового значения в массив переменных среды

    while (!flag){
        // Переключение на ввод с файла, если предоставлен аргумент или же вывод ошибки если файла не существует
        if (argc == 2 && freopen(argv[1], "r", stdin) == NULL) {
            printf("Error: input file\n");
            exit(1);
        }

        getcwd(path, 256);
        printf("(Shell) %s $ ", path);
        inp_ptr = NULL;
        getline(&inp_ptr, &len, stdin);
        i = 0;
        args[i++] = strtok(inp_ptr, " \n");
        while ((args[i++] = strtok(NULL, " \n")));

        if (strcmp(inp_ptr, "\n") == 0){
            continue;
        }

        else if (args[0] != NULL && strcmp(args[0], "cd") == 0) {
            cd_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "dir") == 0) {
            dir_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "env") == 0) {
            environ_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "echo") == 0) {
            echo_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "pause") == 0) {
            pause_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "help") == 0) {
            help_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "quit") == 0) {
            flag = quit_fnc(args);
        }

        else if (args[0] != NULL && strcmp(args[0], "clr") == 0) {
            clr_fnc(args);
        }
        else{
            cmd_run(args);
        }

        len = 0;
        free(inp_ptr);
        memset(args, 0, 10 * sizeof(char *));
    }
    return 0;
}
