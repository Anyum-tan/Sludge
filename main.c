#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

struct File{
    char* name;
    int size;
    char* data;
};

struct File* list_func(char **argv){
    FILE *rf = fopen(argv[2], "r");
    if (!rf){
        printf("Cannot open file!\n");
        struct File *file = (struct File *) malloc(sizeof(struct File) * 1);
        file[0].name = NULL;
        file[0].size = 0;
        file[0].data = NULL;
        return file;
    }

    struct File *archive = (struct File *) malloc(sizeof(struct File) * 100);
    int i;
    for (i = 0; i < 100; i++){
        archive[i].name = NULL;
        archive[i].size = 0;
        archive[i].data = NULL;
    }

    for(i = 0; !feof(rf); i++){
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        read = getline(&line, &len, rf);
        if (read > 0)
            line[read - 1] = '\0';
        if (strcmp(line, "+============================================+") != 0)
            break;
        if (len > 0)
            free(line);

        // Reading file name
        len = 0;
        read = getline(&line, &len, rf);
        line[read - 1] = '\0';
        archive[i].name = (char *) malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(archive[i].name, line);
        if (len > 0)
            free(line);

        // Reading file size
        len = 0;
        read = getline(&line, &len, rf);
        line[read - 1] = '\0';
        archive[i].size = atoi(line);
        if (len > 0)
            free(line);

        len = 0;
        read = getline(&line, &len, rf);
        line[read - 1] = '\0';
        if (len > 0)
            free(line);

        char temp[4294969];
        while (!feof(rf)){
            len = 0;
            read = getline(&line, &len, rf);
            if ((strcmp(line, "\n") != 0) || (line[read - 1] == '\n')){
                if (read > 0)
                    line[read - 1] = '\0';
            }
            if (strcmp(line, "+============================================+") == 0) {
                if (len > 0)
                    free(line);
                break;
            }
            else {
                if (read > 0)
                    line[read - 1] = '\n';
                if (strcmp(line, "\n") == 0){
                    strcat(temp, "\n");
                }
                else
                    strcat(temp, line);
            }
            if (len > 0)
                free(line);
        }

        archive[i].data = (char *) malloc(sizeof(char) * (strlen(temp) + 1));
        strcpy(archive[i].data, temp);
        if (strlen(temp) > 0)
            memset(temp, '\0', strlen(temp));
//        if (strlen(line) > 0)
//            memset(line, '\0', strlen(line));
    }
    fclose(rf);
    return archive;
}

char * read_from_file(char *filename){
    FILE *in_file = fopen(filename, "rb");
    if (!in_file){
        fprintf(stderr,"Error while opening a file!\n");
        return NULL;
    }

    fseek(in_file, 0L, SEEK_END);

    // calculating the size of the file
    long int file_size = ftell(in_file);
    fseek(in_file, 0L, SEEK_SET);
    char *allWords = (char *) malloc(sizeof(char) * (file_size + 1));
    fread(allWords, file_size, 1, in_file);
    allWords[file_size] = '\0';
    fclose(in_file);
    return allWords;
}

int archive_length(struct File *archive){
    int i, size = 0;
    for (i = 0; archive[i].name != NULL; i++)
        size++;
    return size;
}

void add_func(char **args, struct File *archive){
    struct File *file_names;
    if (!archive)
        file_names = list_func(args);
    else
        file_names = archive;
    int i;
    for (i = 3; args[i] != NULL; i++){
        short existing_file = 0;
        int j;
        for (j = 0; file_names[j].name != NULL; j++){
            if (strcmp(file_names[j].name, args[i]) == 0){
                existing_file = 1;
                char *allWords = read_from_file(args[i]);
                if (!allWords){
                    fprintf(stderr, "Error while opening file!\n");
                }
                else {
                    if (file_names[j].data)
                        free(file_names[j].data);
                    file_names[j].data = (char *) malloc(sizeof(char) * (strlen(allWords) + 2));
                    strcpy(file_names[j].data, allWords);
                    strcat(file_names[j].data, "\n");
                }
                free(allWords);
            }
        }
        if ((existing_file == 0) && !archive){
            char *allWords = read_from_file(args[i]);
            int len_archive = archive_length(file_names);
            file_names[len_archive].name = (char *) malloc(sizeof(char) * (strlen(args[i]) + 1));
            strcpy(file_names[len_archive].name, args[i]);
            file_names[len_archive].size = strlen(allWords);
            file_names[len_archive].data = (char *) malloc(sizeof(char) * (strlen(allWords) + 2));
            strcpy(file_names[len_archive].data, allWords);
            strcat(file_names[j].data, "\n");
            free(allWords);
        }
    }
    printf("Done adding data to archive\n");
    FILE *wf = fopen(args[2], "w");
    if (!wf){
        fprintf(stderr, "Cannot open file!\n");
        return;
    }

    int size = archive_length(file_names);
    for (i = 0; i < size; i++){
        char buf[100];
        strcpy(buf, "+============================================+\n");
        fputs(buf, wf);
        fputs(file_names[i].name, wf);
        fputc('\n', wf);

        char size_arr[10];
        sprintf(size_arr, "%ld", file_names[i].size);
        fputs(size_arr, wf);
        fputc('\n', wf);
        fputs(buf, wf);
        fputs(file_names[i].data, wf);
        fputs(buf, wf);
    }
    fclose(wf);
}

int check_file_in_archive(struct File *f, char *filename){
    int size = archive_length(f);
    int i;
    for (i = 0; i < size; i++){
        if (strcmp(f[i].name, filename) == 0)
            return 1;
    }
    return 0;
}

int get_index_of_file(struct File *f, char *filename){
    int size = archive_length(f);
    int i;
    for (i = 0; i < size; i++){
        if (strcmp(f[i].name, filename) == 0)
            return i;
    }
    return -1;
}

void delete_from_file_archive(struct File *f, int index){
    int i, size = archive_length(f);
    if (index >= 0 && index < size){
        for (i = index; i < size - 1; i++){
            if (f[i].name)
                free(f[i].name);
            f[i].name = (char *) malloc(sizeof(char) * (strlen(f[i + 1].name) + 1));
            strcpy(f[i].name, f[i + 1].name);

            f[i].size = f[i + 1].size;

            if (f[i].data)
                free(f[i].data);
            f[i].data = (char *) malloc(sizeof(char) * (strlen(f[i + 1].data) + 1));
            strcpy(f[i].data, f[i + 1].data);
        }

        if (f[i].name)
            free(f[i].name);
        f[i].name = NULL;

        f[i].size = 0;

        if (f[i].data)
            free(f[i].data);
        f[i].data = NULL;
    }
    if ((size == 1) && (index == size + 1)){
        if (f[0].name)
            free(f[0].name);
        f[0].name = NULL;

        f[0].size = 0;

        if (f[0].data)
            free(f[0].data);
        f[0].data = NULL;
    }
    if ((index < 0) || (index > size))
        fprintf(stderr, "Invalid index provided!\n");
}


void extract_func(int argc, char **args){
    struct File *archive = list_func(args);
    int size = archive_length(archive);
    int indices_to_delete[size];
    int i, j = 0;

    // Initializing indices to delete array
    for (i = 0; i < size; i++)
        indices_to_delete[i] = -1;

    // find file in sludge name
    if (argc > 3){
        for (i = 3; i < argc; i++){
            if (check_file_in_archive(archive, args[i]) == 1){
                FILE *fp = fopen(args[i], "w");
                if (!fp){
                    fprintf(stderr, "Couldn't open file\n");
                    return;
                }
                indices_to_delete[j++] = get_index_of_file(archive, args[i]);
                fputs(archive[indices_to_delete[j - 1]].data, fp);
                fclose(fp);
            }
            else {
                fprintf(stderr, "One or more provided files doesn't exist in archive!\n");
            }
        }
    }
    else {
        for (i = 0; i < size; i++){
            FILE *fp = fopen(archive[i].name, "w");
            if (!fp){
                fprintf(stderr, "Couldn't open file\n");
                return;
            }
            indices_to_delete[j++] = i;
            fputs(archive[i].data, fp);
            fclose(fp);
        }
    }

    for (i = 0; i < j; i++){
        delete_from_file_archive(archive, indices_to_delete[i]);
    }

    if (j > 0)
        add_func(args, archive);
}

int main(int argc, char **argv){
    int i;
    struct File *archive;

    if (strcmp(argv[1], "-l") == 0){
        printf("You chose list command\n");
        archive = list_func(argv);
        for (i = 0; archive[i].name != NULL; i++){
            printf("Name: %s\n", archive[i].name);
            printf("Size: %d\n", archive[i].size);
//            printf("Data: %s\n\n", archive[i].data);
        }

        for (i = 0; archive[i].name != NULL; i++){
            free(archive[i].name);
            free(archive[i].data);
        }
        free(archive);
    }
    else if (strcmp(argv[1], "-a") == 0){
        printf("You chose add command\n");
        add_func(argv, NULL);
    }
    else if (strcmp(argv[1], "-e") == 0) {
        printf("You chose extract command\n");
        extract_func(argc, argv);
    }
}