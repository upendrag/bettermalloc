#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

void print_cases()
{
    printf("Test cases are as follows..\n");
    printf("case 1: [8-256 bytes]; N small objects twice; exact pointer\n");
    printf("case 2: [8-256 bytes]; N small objects twice; range pointer\n");
    printf("case 3: [8-256 bytes]; alloc N objects, free N/2 repeat; exact pointer\n");
    printf("case 4: [8-256 bytes]; alloc N objects, free N/2 repeat; range pointer\n");
    printf("case 5: alternate small(64) & large(64k); N objects twice; exact pointer\n");
    printf("case 6: alternate small(64) & large(64k); N objects twice; range pointer\n");
    printf("case 7: alternate small(64) & large(64k); alloc N objects, free N/2 repeat; exact pointer\n");
    printf("case 8: alternate small(64) & large(64k); alloc N objects, free N/2 repeat; range pointer\n");
}

void run_test1(int exact)
{
    if (mem_init(100)) {
        perror("mem_init");
        exit(EXIT_FAILURE);
    }

    int i, j;
    srand(time(NULL));
    char *s[300], *t[300];
    struct timeval tv_b, tv_a;
    gettimeofday(&tv_b, NULL);
    for (i=0; i<2; i++) {
        for (j=0; j<300; j++) {
            int size = (rand() % 249) + 8;
            s[j] = (char *) mem_alloc(size);
            if (s[j] == NULL)
                break;
        }
        printf("Allocated %d objects\n", j);
        if (exact) {
            for (j=0; j<300; j++) {
                if (s[j] == NULL || mem_free(s[j]) == -1)
                    break;
            }
        }
        else {
            for (j=0; j<300; j++) {
                if (s[j] == NULL || mem_free(s[j] + 7) == -1)
                    break;
            }
        }
        printf("Freed %d objects\n", j);
    }
    gettimeofday(&tv_a, NULL);
    printf("mem_alloc: Time difference: %ld microseconds\n", 
            ((tv_a.tv_sec - tv_b.tv_sec)*1000000L) + tv_a.tv_usec - tv_b.tv_usec);
   
    int n = j;
     
    gettimeofday(&tv_b, NULL);
    for (i=0; i<2; i++) {
        for (j=0; j<n; j++) {
            int size = (rand() % 249) + 8;
            t[j] = (char *) malloc(size);
        }
        printf("Allocated %d objects\n", j);
        for (j=0; j<n; j++) {
            free(t[j]);
        }
        printf("Freed %d objects\n", j);
    }
    gettimeofday(&tv_a, NULL);
    printf("malloc: Time difference: %ld microseconds\n", 
            ((tv_a.tv_sec - tv_b.tv_sec)*1000000L) + tv_a.tv_usec - tv_b.tv_usec);
}

void run_test2()
{
    run_test1(0);
}

void run_test3()
{
}

void run_test4()
{
}

void run_test5()
{
}

void run_test6()
{
}

void run_test7()
{
}

void run_test8()
{
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: test.o <case>\n");
        print_cases();
        exit(EXIT_FAILURE);
    }

    if (*(argv[1] + 1) != 0 ||
        *argv[1] < '1' || *argv[1] > '8') {
        printf("Usage: test.o <case>\n");
        print_cases();
        exit(EXIT_FAILURE);
    }
    
    switch (*argv[1]) {
        case '1':
            run_test1(1);
            break;
        case '2':            
            run_test2();
            break;
        case '3':
            run_test3();
            break;
        case '4':
            run_test4();
            break;
        case '5':
            run_test5();
            break;
        case '6':
            run_test6();
            break;
        case '7':
            run_test7();
            break;
        case '8':
            run_test8();
            break;
        default:
            printf("Unknown case\n");
            exit(EXIT_FAILURE);
    }

    return 0;
}
