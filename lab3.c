#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

//#define LOGGING

unsigned int block(char *arg)
{
    int i = 0;
    unsigned int size = atoi(arg);
    while (arg[i] >= '0' && arg[i] <= '9')
        i++;
    switch (arg[i])
    {
    case 'k':
    case 'K':
        size *= 1024;
        break;
    case 'M':
        size *= 1024 * 1024;
        break;
    }
    return size;
}
void test_RAM(unsigned int block_size, double *read_result, double *write_result)
{
#ifdef LOGGING
    int log = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
    char log_message[100];
#endif
    unsigned int buf_size=block_size/sizeof(int);
    int *block = malloc(buf_size*sizeof(int));
    int *buf = malloc(buf_size*sizeof(int));
    struct timespec start, finish;
    double sec = 0, nsec = 0;
    *write_result = 0;
    for (int i = 0; i < buf_size; i++)
    {
        buf[i] = rand();
    }
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < buf_size; i++)
    {
        block[i] = buf[i];
    }
    clock_gettime(CLOCK_REALTIME, &finish);
    sec = finish.tv_sec - start.tv_sec;
    nsec = finish.tv_nsec - start.tv_nsec;
    if (nsec < 0)
    {
        sec--;
        nsec += 1E9;
    }
    *write_result = sec + nsec / 1E9;
#ifdef LOGGING
    sprintf(log_message, "Write: %ld - start.sec, %ld - start.nsec, %ld - finish.sec, %ld - finish.nsec\n", start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%ld - dif.sec, %ld - dif.nsec\n", finish.tv_sec - start.tv_sec, finish.tv_nsec - start.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%f - dsec, %f - dnsec, %f - result\n", sec, nsec, *write_result);
    write(log, log_message, strlen(log_message));
#endif
    sec = nsec = 0;
    *read_result = 0;
    clock_gettime(CLOCK_REALTIME, &start);
    for (unsigned int i = 0; i < buf_size; i++)
    {
        block[i];
    }
    clock_gettime(CLOCK_REALTIME, &finish);
    sec = finish.tv_sec - start.tv_sec;
    nsec = finish.tv_nsec - start.tv_nsec;
    if (nsec < 0)
    {
        sec--;
        nsec += 1E9;
    }
    *read_result += sec + nsec / 1E9;
#ifdef LOGGING
    sprintf(log_message, "Read: %ld - start.sec, %ld - start.nsec, %ld - finish.sec, %ld - finish.nsec\n", start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%ld - dif.sec, %ld - dif.nsec\n", finish.tv_sec - start.tv_sec, finish.tv_nsec - start.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%f - dsec, %f - dnsec, %f - result\n", sec, nsec, *read_result);
    write(log, log_message, strlen(log_message));
#endif
    free(buf);
    free(block);
#ifdef LOGGING
    close(log);
#endif
}
void test_SSD(int fd, unsigned int block_size, double *read_result, double *write_result)
{
#ifdef LOGGING
    int log = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
#endif
    unsigned int buf_size=block_size/sizeof(int);
    int *block = malloc(buf_size*sizeof(int));
    struct timespec start, finish;
    double sec = 0, nsec = 0;
    for (int i = 0; i < buf_size; i++)
    {
        block[i] = rand();
    }
    clock_gettime(CLOCK_REALTIME, &start);
    write(fd, block, buf_size);
    fsync(fd);
    clock_gettime(CLOCK_REALTIME, &finish);
    sec = finish.tv_sec - start.tv_sec;
    nsec = finish.tv_nsec - start.tv_nsec;
    if (nsec < 0)
    {
        sec--;
        nsec += 1E9;
    }
    *write_result = sec + nsec / 1E9;
#ifdef LOGGING
    char log_message[100];
    sprintf(log_message, "Write: %ld - start.sec, %ld - start.nsec, %ld - finish.sec, %ld - finish.nsec\n", start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%ld - dif.sec, %ld - dif.nsec\n", finish.tv_sec - start.tv_sec, finish.tv_nsec - start.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%f - dsec, %f - dnsec, %f - result\n", sec, nsec, *write_result);
    write(log, log_message, strlen(log_message));
#endif
    sec = nsec = 0;
    memset(block, 0, buf_size);
    lseek(fd, 0, SEEK_SET);
    clock_gettime(CLOCK_REALTIME, &start);
    read(fd, block, buf_size);
    clock_gettime(CLOCK_REALTIME, &finish);
    sec = finish.tv_sec - start.tv_sec;
    nsec = finish.tv_nsec - start.tv_nsec;
    if (nsec < 0)
    {
        sec--;
        nsec += 1E9;
    }
    *read_result = sec + nsec / 1E9;
#ifdef LOGGING
    sprintf(log_message, "Read: %ld - start.sec, %ld - start.nsec, %ld - finish.sec, %ld - finish.nsec\n", start.tv_sec, start.tv_nsec, finish.tv_sec, finish.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%ld - dif.sec, %ld - dif.nsec\n", finish.tv_sec - start.tv_sec, finish.tv_nsec - start.tv_nsec);
    write(log, log_message, strlen(log_message));
    sprintf(log_message, "\t%f - dsec, %f - dnsec, %f - result\n", sec, nsec, *read_result);
    write(log, log_message, strlen(log_message));
    close(log);
#endif
    free(block);
}
void report(FILE *csv, char *memory_type, unsigned int block_size, int launch_num, double write_time, double average_write, double read_time, double average_read)
{
    fprintf(csv, "%s;", memory_type);
    fprintf(csv, "%d;", block_size);
    fprintf(csv, "int;");
    fprintf(csv, "%d;", block_size);
    fprintf(csv, "%d;", launch_num);
    fprintf(csv, "clock_gettime;");
    fprintf(csv, "%f;", write_time);
    fprintf(csv, "%f;", average_write);
    double wr_band = block_size / average_write / 1E6;
    fprintf(csv, "%f;", wr_band);
    double abs_err_wr = fabs(write_time - average_write);
    fprintf(csv, "%f;", abs_err_wr);
    fprintf(csv, "%f%%;", abs_err_wr / write_time * 100);
    fprintf(csv, "%f;", read_time);
    fprintf(csv, "%f;", average_read);
    double rd_band = block_size / average_read / 1E6;
    fprintf(csv, "%f;", rd_band);
    double abs_err_rd = fabs(read_time - average_read);
    fprintf(csv, "%f;", abs_err_rd);
    fprintf(csv, "%f%%\n", abs_err_rd / read_time * 100);
}
int main(int argc, char **argv)
{
    int opt;
    char memory_type[10];
    unsigned int block_size;
    unsigned int launch_count;
    while ((opt = getopt(argc, argv, "m:b:l:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            strcpy(memory_type, optarg);
            break;
        case 'b':
            block_size = block(optarg);
            break;
        case 'l':
            launch_count = atoi(optarg);
            break;
        default:
            perror("getopt");
            exit(EXIT_FAILURE);
        }
    }
    double *write_results = malloc(launch_count * sizeof(double));
    double *read_results = malloc(launch_count * sizeof(double));
    double average_read = 0, average_write = 0;
    for (int i = 0; i < launch_count; i++)
    {
        printf("Testing %s with %d bytes of memory: %d/%d\n", memory_type, block_size, i + 1, launch_count);
        if (strcmp(memory_type, "RAM") == 0)
            test_RAM(block_size, &read_results[i], &write_results[i]);
        else if (strcmp(memory_type, "SSD") == 0)
        {
            int fd = open("test_file.dat", O_RDWR | O_CREAT, S_IRWXU);
            test_SSD(fd, block_size, &read_results[i], &write_results[i]);
            unlink("test_file.dat");
        }
        else if (strcmp(memory_type, "flash") == 0)
        {
            int fd = open("/media/sofya/TRANSCEND/test_file.dat", O_RDWR | O_CREAT, S_IRWXU);
            test_SSD(fd, block_size, &read_results[i], &write_results[i]);
            unlink("/media/sofya/TRANSCEND/test_file.dat");
        }
        printf("\033c");
        average_read += read_results[i];
        average_write += write_results[i];
    }
    average_write /= launch_count;
    average_read /= launch_count;
    FILE *csv = fopen("lab3.csv", "a+");
    for (int i = 0; i < launch_count; i++)
    {
        report(csv, memory_type, block_size, i + 1, write_results[i], average_write, read_results[i], average_read);
    }
    return 0;
}