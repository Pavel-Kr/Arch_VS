#include <iostream>
#include <cmath>
#include <locale.h>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
using namespace std;

double* Test() {
    double* results = new double[3];
    struct timespec start,finish;
    double sec,nsec;
    clock_gettime(CLOCK_REALTIME,&start);
    sin(rand());
    clock_gettime(CLOCK_REALTIME,&finish);
    sec=finish.tv_sec-start.tv_sec;
    nsec=finish.tv_nsec-start.tv_nsec;
    results[0] = sec*1000+nsec/1000000.0;
    clock_gettime(CLOCK_REALTIME,&start);
    cos(rand());
    clock_gettime(CLOCK_REALTIME,&finish);
    sec=finish.tv_sec-start.tv_sec;
    nsec=finish.tv_nsec-start.tv_nsec;
    results[1] = sec*1000+nsec/1000000.0;
    clock_gettime(CLOCK_REALTIME,&start);
    log(rand());
    clock_gettime(CLOCK_REALTIME,&finish);
    sec=finish.tv_sec-start.tv_sec;
    nsec=finish.tv_nsec-start.tv_nsec;
    results[2] = sec*1000+nsec/1000000.0;
    return results;
}

double calculate_perf(double average){
    return 1/average;
}

char* get_CPU_name(){
    FILE* file=fopen("/proc/cpuinfo","r");
    if(file==NULL){
        perror("get_cpu_name");
        exit(EXIT_FAILURE);
    }
    char* line=NULL;
    size_t size=0;
    char* cpu_name;
    while(getline(&line,&size,file)!=-1){
        cpu_name=strstr(line,"model name");
        if(cpu_name!=NULL) break;
    }
    while(cpu_name[0]!=':') cpu_name++;
    cpu_name++;
    int i=0;
    while(cpu_name[i]!='\n') i++;
    cpu_name[i]=0;
    return cpu_name;
}

void report(char* cpu_name, const char* task, double time, int l_num, double average, FILE* file_res) {
    fprintf(file_res, "%s;", cpu_name);
    fprintf(file_res, "%s;", task);
    fprintf(file_res, "int;");
    fprintf(file_res, "None;");
    fprintf(file_res, ";");//Instruction count
    fprintf(file_res, "clock_gettime();");
    fprintf(file_res, "%f msec;", time);
    fprintf(file_res, "%d;", l_num);
    fprintf(file_res, "%f msec;", average);
    double abs_err = abs(time - average);
    fprintf(file_res, "%f msec;", abs_err);//AbsError
    fprintf(file_res, "%f%%;", abs_err / time);//RelError
    fprintf(file_res, "%f;",calculate_perf(average));//TaskPerf
    fprintf(file_res, "\n");
}

int main()
{
    srand(time(NULL));
    setlocale(LC_ALL, "rus");
    cout << "Введите количество тестов: ";
    int tests;
    cin >> tests;
    tests = max(tests, 10);
    double** test_results = new double* [tests];
    for (int i = 0; i < tests; i++) {
        test_results[i] = Test();
        //cout << "Test " << i + 1 << " sin= " << test_results[i][0] << "sec, cos= " << test_results[i][1] << "sec, log= " << test_results[i][2] << "sec" << endl;
    }
    double average_time[3] = { 0,0,0 };
    double dispersion[3] = { 0,0,0 };
    double deviation[3] = { 0,0,0 };
    for (int i = 0; i < tests; i++) {
        average_time[0] += test_results[i][0] / tests;
        average_time[1] += test_results[i][1] / tests;
        average_time[2] += test_results[i][2] / tests;
    }
    for (int i = 0; i < tests; i++) {
        dispersion[0] += pow((test_results[i][0] - average_time[0]), 2) / tests;
        dispersion[1] += pow((test_results[i][1] - average_time[1]), 2) / tests;
        dispersion[2] += pow((test_results[i][2] - average_time[2]), 2) / tests;
    }
    for (int i = 0; i < 3; i++) {
        deviation[i] = sqrt(dispersion[i]);
    }
    cout << "Sin: Среднее время выполнения = " << average_time[0] << "мс, Дисперсия = " << dispersion[0] << ", Стандартное отклонение = " << deviation[0] <<"мс" << endl;
    cout << "Cos: Среднее время выполнения = " << average_time[1] << "мс, Дисперсия = " << dispersion[1] << ", Стандартное отклонение = " << deviation[1] <<"мс" << endl;
    cout << "Log: Среднее время выполнения = " << average_time[2] << "мс, Дисперсия = " << dispersion[2] << ", Стандартное отклонение = " << deviation[2] <<"мс" << endl;
    FILE* file_res = fopen("lab2.csv", "w");
    fprintf(file_res, "PModel;Task;OpType;Opt;InsCount;Timer;Time;LNum;AvTime;AbsErr;RelErr;TaskPerf\n");
    char* cpu_name=get_CPU_name();
    printf("%s\n",cpu_name);
    for (int i = 0; i < tests; i++) {
        report(cpu_name, "sin", test_results[i][0], i + 1, average_time[0], file_res);
        report(cpu_name, "cos", test_results[i][1], i + 1, average_time[1], file_res);
        report(cpu_name, "log", test_results[i][2], i + 1, average_time[2], file_res);
    }
}
