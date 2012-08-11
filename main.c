/*
默认情况下，并行区内线程数=系统中核的个数。如果你需要修改，方法之一是设置环境变量。
1.set OMP_NUM_THREADS = 4
2.或者临时更改 omp_set_num_threads(4);
in default situation , thread num in parallel section = core num in
system,of course you can mannully refine it 
1.set the env Variable  by "set OMP_NUM_THREADS = N "
2.in your code call this function " omp_set_num_threads(4); "
*/
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define LEN     5

void test(int n);
void opemMP_num();
void openMP_hello();
void openMP_parallel();
void openMP_parallel_for();
void openMP_private(int N);
void openMP_critical(int N);
//void opemMP_critical_construct()
void openMP_reduction(int N);
void compute_pi_in_serial();
void compute_pi_in_parallel();
void opemMP_schedule_static();
void opemMP_schedule_dynamic();

int a[LEN]={1,3,5,7,9};
int b[LEN]={2,4,6,8,10};
int c[LEN];

int main(int argc,char* argv[])
{
    //opemMP_num();
    //openMP_hello();
    //openMP_parallel();
    //openMP_private(5);
    //openMP_critical(5);
    //compute_pi_in_serial();
    //compute_pi_in_parallel();
    //opemMP_schedule_static();
    opemMP_schedule_dynamic();
    return 1;
}

void test(int n)
{
    int i;
    for (i=0; i < 10000; ++i)
        ;
    printf("%d,",n);
}

void opemMP_num()
{
    int i;
    omp_set_num_threads(5);
    #pragma omp parallel for
        for( i = 0; i < 10; ++i)
            test(i);
    printf("\n");
}

void openMP_hello()
{
    omp_set_num_threads(4);
    int nthreads,tid;
    #pragma omp parallel private(nthreads,tid)
    {
        //obtain and print thread id
        tid = omp_get_thread_num();
        printf("Hello world from OMP thread %d.\n",tid);
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("number of thread :%d.\n",nthreads);
        }
    }
}

void openMP_parallel()
{
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        int i;
        printf("hello world.\n");
        for (i = 0; i < 6; ++i)
            printf("ID:%d ",i);
    }
}

///OpenMP属于共享内存的编程模型。在我们的多线程代码中，大部分数据都是可以共享的。共享内存给我们程序中数据的共享带来了极大的便利。因此在默认情况下，OpenMP将全局变量、静态变量设置为共享属性。
void openMP_private(int N)
{
    int i,x,y,pid;
    omp_set_num_threads(2);


    printf("private test 1...\n");

    #pragma omp parallel
    {
        int pid = omp_get_thread_num();
        #pragma omp for private(x,y)
        //此时i，pid是share，但是此处pid为局部变量，屏蔽了外部的pid
        for ( i = 0 ; i < N; ++i)
        {
            printf("computing in thread%d...now i=%d\n",pid,i);
            x = a[i];y = b[i];
            c[i] = x + y;
            printf(" finished in thread%d...now i=%d\n",pid,i);
        }
        printf("thread%d finished computing.\n");
        if (pid == 0)
        {
            int omp_thread_num = omp_get_num_threads();
            printf("now the threads num is %d.\n",omp_thread_num);
        }
    }
     printf("\nprivate test 2...\n");

    #pragma omp parallel
    {
        pid = omp_get_thread_num();
        //pid 定义在 并行去 之外，所以也就是共享变量，但是很明显的是与我们的意图不符。pid在这改表
        //线程的id，所以
        #pragma omp for private(i,x,y,pid)
        for ( int i = 0 ; i < N; ++i)
        {
            printf("computing in thread%d...now i=%d\n",pid,i);
            x = a[i];y = b[i];
            c[i] = x + y;
            printf(" finished in thread%d...now i=%d\n",pid,i);
        }
        printf("thread%d finished computing.\n");
        if (pid == 0)
        {
            int omp_thread_num = omp_get_num_threads();
            printf("now the threads num is %d.\n",omp_thread_num);
        }
    }
    printf("finished private test.\n");
}

void openMP_critical(int N)
{

    int sum = 0;
    #pragma omp parallel for shared(sum)
    for(int i = 0; i<N; ++i)
        #pragma omp critical 
        //if you comment these words forward ,build and run
        //you will see different results
        sum += a[i] + b[i];
    printf("sum=%d\n",sum);
}

void openMP_reduction(int N)
{
    int sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; ++i)
    {
        /* code */
        sum += a[i]+b[i];
    }
    printf("sum=%d\n", sum);
}

void compute_pi_in_serial()
{
    static long num_steps = 10000;
    double step,pi;
    double x,sum = 0;

    step = 1.0/(double)num_steps;
    for (int i = 0; i < num_steps; ++i)
    {
        x = (i+0.5)*step;
        sum += 4.0/(1.0+x*x);
    }
    pi = step *sum;
    printf("Pi=%f\n",pi );
}

void compute_pi_in_parallel()
{
    static long num_steps = 10000;
    double step,pi;
    double x,sum=0;

    step = 1.0/(double)num_steps;
     #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < num_steps; ++i)
    {
        /* code */
        x = (i+0.5)*step;
        sum += 4.0/(1.0+x*x);
    }
    pi = step *sum;
    printf("Pi=%f\n",pi );
}

/*
OpenMP中工作量的划分与调度 
*/

//schedule(static[,chunk])
//1.make blocks of iterations' size "chunk" to threads
//2.round robin distribution
void opemMP_schedule_static()
{
    int tid,count = 20;
    #pragma omp parallel for schedule(static,1) private(tid)
    for (int i = 0; i <= count; i += 1)
    {
        /* code */
        tid = omp_get_thread_num();
        printf("tid%di%d, ",tid,i );
    }
    printf("end\n");
    #pragma omp parallel for schedule(static,1) private(tid)
    for (int i = 0; i <= count; i += 4)
    {
        /* code */
        tid = omp_get_thread_num();
        printf("%d&%d, ",tid,i );
    }
    printf("end..\n");
    #pragma omp parallel for schedule(static,2) private(tid)
    for (int i = 0; i <= count; i += 1)
    {
        /* code */
        tid = omp_get_thread_num();
        printf("%d&%d, ",tid,i );
    }
    printf("end..\n");
    #pragma omp parallel for schedule(static,2) private(tid)
    for (int i = 0; i <= count; i += 4)
    {
        /* code */
        tid = omp_get_thread_num();
        printf("%d&%d, ",tid,i );
    }
    printf("end..\n");
    #pragma omp parallel for schedule(static,8) private(tid)
    for (int i = 3; i <= count; i += 2)
    {
        /* code */
        tid = omp_get_thread_num();
        printf("%d&%d, ",tid,i );
    }
    printf("end..\n");
}

//schedule (dynamic[,chunk])
//1.thread grab "chunk" iterations
//2.when done with iterations,thread request next set
void opemMP_schedule_dynamic()
{
    int tid,count = 10;
    for (int i = 0; i < 5; ++i)
    {
        #pragma omp parallel for schedule(dynamic,1) private(tid)
        for (int i = 0; i <= count; i += 1)
        {
            /* code */
            tid = omp_get_thread_num();
            printf("%d&%d, ",tid,i );
        }
        printf("end..\n");
    }    
}


//schedule(guide[,chunk])
//1.dynamic schedule starting with large blocks
//2.size of the blocks shrink;no smaller than chunk
//这个方式是动态方式的改进。在这个方式里，分块的x是不固定的，一开
//始块的大小(x)比较大，随着剩余工作量的减小，块的大小也随之变小。 
//...no more test