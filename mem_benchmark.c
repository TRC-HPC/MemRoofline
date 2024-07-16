# include <stdio.h>
# include <unistd.h>
# include <math.h>
# include <float.h>
# include <limits.h>
# include <sys/time.h>

#ifndef COMPUTE_ITERS
#define COMPUTE_ITERS 0
#endif

#ifndef NTIMES
#define NTIMES 20
#endif

#ifndef STREAM_ARRAY_SIZE
#define STREAM_ARRAY_SIZE    100000000
#endif

#ifndef STREAM_TYPE
#define STREAM_TYPE double
#endif

static STREAM_TYPE      a[STREAM_ARRAY_SIZE],
                        b[STREAM_ARRAY_SIZE],
                        c[STREAM_ARRAY_SIZE];


void tuned_STREAM_Triad(STREAM_TYPE scalar)
{
        ssize_t j, k;
	double tmp = 2.0;
#pragma omp parallel for private (tmp, k)
        for (j=0; j<STREAM_ARRAY_SIZE; j++)
	{
            a[j] = b[j] + scalar * c[j];
	    for(k = 0; k < COMPUTE_ITERS; k++)
   	        tmp += scalar * a[j] - k;
	    a[j] *= tmp;
	}
}

double mysecond();

void main()
{
    ssize_t j = 0; 

    printf("Simple Mem Benchmark\n------------------\n");
    printf("Array Size = %ld MB, Arithmetic Intensity = %.3f\n\n", (long int) (STREAM_ARRAY_SIZE / 1e6), (double) (3*COMPUTE_ITERS + 3) / (sizeof(STREAM_TYPE) * 3));
#pragma omp parallel for
    for (j=0; j<STREAM_ARRAY_SIZE; j++) {
       	    a[j] = 1.0;
            b[j] = 2.0;
            c[j] = 4.0;
        }

#pragma omp parallel for
    for (j = 0; j < STREAM_ARRAY_SIZE; j++)
                a[j] = 2.0E0 * a[j];

	double prev_time, dtime;
	for(int k = 0; k < NTIMES; k++)
	{
		prev_time = mysecond();
		tuned_STREAM_Triad(k);
		dtime = mysecond() - prev_time;
		printf("Time per iteration = %lf usec, performance = %lf GFLOPs/s\n", dtime * 1e6, ((long long) (3*COMPUTE_ITERS + 3) * STREAM_ARRAY_SIZE) / 1e9 / dtime );
	}
}

double mysecond()
{
        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
