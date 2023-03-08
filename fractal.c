/*
   fractal.c - Mandelbrot fractal generation
   Starting code for CSE 30341 Project 3 - Spring 2023
   */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include "bitmap.h"
#include "fractal.h"

/* TODO:
 * Rerun and compare fractals using something bigger than 20x20
 */


//Describes a task for a thread start stop on the row and column
typedef struct startstop{
	int startx;
	int starty;
	int stopx;
	int stopy;

} startstop;

//Global Data
startstop *tasks = NULL;
int numtasks = 0;
struct FractalSettings  theSettings;
struct bitmap * pBitmap ;
pthread_mutex_t lock;

/* Generates the list of tasks the Width and Height of the task Group is specified the settings are also passed in. The width and height are a recommendation but are increased to absorb the extra pixels that cannot fit into the last block*/
void gentasks(int startstopwidth, int startstopheight, struct FractalSettings *settings) {
	//We are dividing the pixel blocks into smaller groups with a subset of the columns and rows. Delta x is the number of column pixels in a task and delta y is the number of row pixels in a task
	int deltax = startstopwidth;
	int deltay = startstopheight;
	if (startstopwidth <= 0 || startstopheight <= 0) {
		fprintf(stderr, "invalid startstopwidth\n");
		exit(1);
	}

	//Numwidth is the number of groupings the columns have been broken into and numheight is the number of groupings the rows have been broken into. The remainders are the number of extra pixels that have not been placed in a group. These pixels will be placed in the first groups (one pixel per group) until all the remainders have been placed in a group
	int numwidth = 0;
	int numheight = 0;
	int remainderx = 0;
	int remaindery = 0;

	//If the deltax is larger than the number of pixels in a width just set it to the width. There will only be one column grouping
	if (deltax > settings -> nPixelWidth) {
		deltax = settings -> nPixelWidth;
		numwidth = 1;
		remainderx = 0;
	} else {
		//Set the number of columns to the width divided by the number of pixels in each group. And set the remainder to the number of extra pixels in a column that have been placed in a group
		numwidth = settings -> nPixelWidth / startstopwidth;
		remainderx = settings -> nPixelWidth % startstopwidth;
		if (numwidth <= 0) {
			fprintf(stderr, "invalid heightwidth\n");
			exit(1);
		}
		//distribute these remainders evenly into each group and set remainder to the new extra pixel
		deltax += remainderx/numwidth;
		remainderx = remainderx%numwidth;
	}
	if (deltay > settings -> nPixelHeight) {
		deltay = settings -> nPixelHeight;
		numheight = 1;
		remaindery = 0;
	} else {
		numheight = settings -> nPixelHeight / startstopheight;
		remaindery = settings -> nPixelHeight % startstopheight;
		if (numheight <= 0) {
			fprintf(stderr, "invalid heightwidth\n");
			exit(1);
		}
		deltay += remaindery/numheight;
		remaindery = remaindery%numheight;

	}
	//Create the tak array to include all of the row column groups. Iterate over all of the pixels creating the groups 
	tasks = malloc(numwidth * numheight * sizeof(startstop));
	if (tasks == NULL) {
		perror("Malloc");
		exit(1);
	}
	int currstartx = 0;
	int currstarty = 0;
	int i = 0;
	for (currstarty = 0; currstarty < settings -> nPixelHeight;) {
		int currstopy = currstarty + deltay;
		if (remaindery > 0) {
			currstopy++;
			remaindery--;
		}
		for (currstartx = 0; currstartx < settings -> nPixelWidth; ) {
			int currstopx = currstartx + deltax;
			if (remainderx > 0) {
				currstopx++;
				remainderx--;
			}
			//We have identified a group by currstartstopx by currstartstopy. Add this to the task array and increment i which counts the number of groups
			tasks[i].startx = currstartx;	
			tasks[i].stopx = currstopx;	
			tasks[i].starty = currstarty;	
			tasks[i].stopy = currstopy;	
			currstartx = currstopx;
			printf("i = %d, num = %d\n", i, (numwidth*numheight));
			i++;
		}
		currstarty = currstopy;

	}
	numtasks = i;
}

//Debug print of the tasks. Comment out call to thi
void printtasks() {
	for (int i = 0; i < numtasks; i++) {
		printf("startx = %d, stopx = %d, starty = %d, stopy = %d\n", tasks[i].startx, tasks[i].stopx, tasks[i].starty, tasks[i].stopy); 
	}

}


//report usage
void usage(int code) {
	printf(" -help         Display the help information\n");
	printf(" -xmin X       New value for x min\n");
	printf(" -xmax X       New value for x max\n");
	printf(" -ymin Y       New value for y min\n");
	printf(" -ymax Y       New value for y max\n");
	printf(" -maxiter N    New value for the maximum number of iterations (must be an integer)     \n");
	printf(" -width W      New width for the output image\n");
	printf(" -height H     New height for the output image\n");
	printf(" -output F     New name for the output file\n");
	printf(" -threads N    Number of threads to use for processing (default is 1) \n");
	printf(" -row          Run using a row-based approach  \n");
	printf(" -task         Run using a thread-based approach\n");
	exit(code);
}

//Used to convert strings to int and reports errors
int get_int(char *num) {
	char *end;
	errno = 0;
	int val = strtol(num, &end, 10);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		usage(1);
	}

	if (end == num) {
		fprintf(stderr, "No digits were found\n");
		usage(1);
	}
	return val;
}

//Used to convert strings to floats and reports errors
float get_float(char *num) {
	char *end;
	errno = 0;
	float val = strtof(num, &end);

	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		perror("strtol");
		usage(1);
	}

	if (end == num) {
		fprintf(stderr, "No digits were found\n");
		usage(1);
	}
	return val;
}

/*
   Compute the number of iterations at point x, y
   in the complex space, up to a maximum of maxiter.
   Return the number of iterations at that point.

   This example computes the Mandelbrot fractal:
   z = z^2 + alpha

   Where z is initially zero, and alpha is the location x + iy
   in the complex plane.  Note that we are using the "complex"
   numeric type in C, which has the special functions cabs()
   and cpow() to compute the absolute values and powers of
   complex values.
   */



static int compute_point( double x, double y, int max )
{
	double complex z = 0;
	double complex alpha = x + I*y;

	int iter = 0;

	while( cabs(z)<4 && iter < max ) {
		z = cpow(z,2) + alpha;
		iter++;
	}

	return iter;
}
/*
   Compute an entire image, writing each point to the given bitmap.
   Scale the image to the range (xmin-xmax,ymin-ymax).

HINT: Generally, you will want to leave this code alone and write your threaded code separately

*/

//Computes the portion of the image specified by a task iterates until there are no more tasks to compute
void compute_image_multithread ( )
	//struct FractalSettings * pSettings, struct bitmap * pBitmap)
{
	while (true) {
		pthread_mutex_lock(&lock);
		if (numtasks == 0) {
			pthread_mutex_unlock(&lock);
			return;
		} 
		numtasks--;
		startstop task = tasks[numtasks];
		pthread_mutex_unlock(&lock);
		int i,j;

		// For every pixel i,j, in the image...

		for(j=task.starty; j<task.stopy; j++) {
			for(i=task.startx; i<task.stopx; i++) {

				// Scale from pixels i,j to coordinates x,y
				double x = theSettings.fMinX + i*(theSettings.fMaxX - theSettings.fMinX) / theSettings.nPixelWidth;
				double y = theSettings.fMinY + j*(theSettings.fMaxY - theSettings.fMinY) / theSettings.nPixelHeight;

				// Compute the iterations at x,y
				int iter = compute_point(x,y,theSettings.nMaxIter);

				// Convert a iteration number to an RGB color.
				// (Change this bit to get more interesting colors.)
				int gray = 255 * iter / theSettings.nMaxIter;

				// Set the particular pixel to the specific value
				// Set the pixel in the bitmap.
				bitmap_set(pBitmap,i,j,gray);
			}
		}
	}
}

//This function is apropriate signature for a pthread. We are not passing any parameters and everything we set is a global.
void *run_multithread(void *args) {
	compute_image_multithread();
	return NULL;
}
/*
   Compute an entire image, writing each point to the given bitmap.
   Scale the image to the range (xmin-xmax,ymin-ymax).

HINT: Generally, you will want to leave this code alone and write your threaded code separately

*/


void compute_image_singlethread ( struct FractalSettings * pSettings, struct bitmap * pBitmap)
{
	int i,j;

	// For every pixel i,j, in the image...

	for(j=0; j<pSettings->nPixelHeight; j++) {
		for(i=0; i<pSettings->nPixelWidth; i++) {

			// Scale from pixels i,j to coordinates x,y
			double x = pSettings->fMinX + i*(pSettings->fMaxX - pSettings->fMinX) / pSettings->nPixelWidth;
			double y = pSettings->fMinY + j*(pSettings->fMaxY - pSettings->fMinY) / pSettings->nPixelHeight;

			// Compute the iterations at x,y
			int iter = compute_point(x,y,pSettings->nMaxIter);

			// Convert a iteration number to an RGB color.
			// (Change this bit to get more interesting colors.)
			int gray = 255 * iter / pSettings->nMaxIter;

			// Set the particular pixel to the specific value
			// Set the pixel in the bitmap.
			bitmap_set(pBitmap,i,j,gray);
		}
	}
}

/* Process all of the arguments as provided as an input and appropriately modify the
   settings for the project 
   @returns 1 if successful, 0 if unsuccessful (bad arguments) */
char processArguments (int argc, char * argv[], struct FractalSettings * pSettings)
{
	/* If we don't process anything, it must be successful, right? */
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-help") == 0) {
			usage(0);
		} else if (strcmp(argv[i], "-xmin") == 0) {
			i++;
			pSettings -> fMinX = get_float(argv[i]);

		} else if (strcmp(argv[i], "-xmax") == 0) {
			i++;
			pSettings -> fMaxX = get_float(argv[i]);
		} else if (strcmp(argv[i], "-ymin") == 0) {
			i++;
			pSettings -> fMinY = get_float(argv[i]);
		} else if (strcmp(argv[i], "-ymax") == 0) {
			i++;
			pSettings -> fMaxY = get_float(argv[i]);
		} else if (strcmp(argv[i], "-maxiter") == 0) {
			i++;
			pSettings -> nMaxIter = get_int(argv[i]);

		} else if (strcmp(argv[i], "-width") == 0) {
			i++;
			pSettings -> nPixelWidth = get_int(argv[i]);
		} else if (strcmp(argv[i], "-height") == 0) {
			i++;
			pSettings -> nPixelHeight = get_int(argv[i]);

		} else if (strcmp(argv[i], "-output") == 0) {
			i++;
			strncpy(pSettings -> szOutfile, argv[i], MAX_OUTFILE_NAME_LEN);
		} else if (strcmp(argv[i], "-threads") == 0) {
			i++;
			pSettings -> nThreads = get_int(argv[i]);
		} else if (strcmp(argv[i], "-row") == 0) {
			pSettings -> theMode  = MODE_THREAD_ROW;
		} else if (strcmp(argv[i], "-task") == 0) {
			pSettings -> theMode  = MODE_THREAD_TASK;
		} else {
			usage(1);
		}

	}
	return 1;
}

//This function supports both row and multithreaded options. The task array has to already be created. This function launches all of the threads which will take turns emptying the task list. This function also creates and destroys the bitmap
void run_threads() {
	/* Create a bitmap of the appropriate size */
	pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

	/* Fill the bitmap with dark blue */
	bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));
	//Create the lock
	if (pthread_mutex_init(&lock, NULL) != 0) {
		perror("pthread_mutex_init");
		exit(1);
	}
	/* Compute the image */
	//Creates an array of threads and then create a thread for every element in the array
	pthread_t *threads = malloc(theSettings.nThreads*sizeof(pthread_t));
	for (int i = 0; i < theSettings.nThreads; i++) {
		int ret = pthread_create(&threads[i], NULL, run_multithread, NULL);
		if (ret != 0) {
			perror("pthread_create");
			exit(1);
		}
	}
	//Join all of the threads
	for (int i = 0; i < theSettings.nThreads; i++) {
		int ret = pthread_join(threads[i], NULL);
		if (ret != 0) {
			perror("pthread_create");
		}
	}
	//Destroy the lock
	pthread_mutex_destroy(&lock);
	free(threads);
	// Save the image in the stated file.
	if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
		fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
		exit(1);
	}            
}

int main( int argc, char *argv[] )
{

	// The initial boundaries of the fractal image in x,y space.
	theSettings.fMinX = DEFAULT_MIN_X;
	theSettings.fMaxX = DEFAULT_MAX_X;
	theSettings.fMinY = DEFAULT_MIN_Y;
	theSettings.fMaxY = DEFAULT_MAX_Y;
	theSettings.nMaxIter = DEFAULT_MAX_ITER;

	theSettings.nPixelWidth = DEFAULT_PIXEL_WIDTH;
	theSettings.nPixelHeight = DEFAULT_PIXEL_HEIGHT;

	theSettings.nThreads = DEFAULT_THREADS;
	theSettings.theMode  = MODE_THREAD_SINGLE;

	strncpy(theSettings.szOutfile, DEFAULT_OUTPUT_FILE, MAX_OUTFILE_NAME_LEN);

	/* TODO: Adapt your code to use arguments where the arguments can be used to override 
	   the default values 

	   -help         Display the help information
	   -xmin X       New value for x min
	   -xmax X       New value for x max
	   -ymin Y       New value for y min
	   -ymax Y       New value for y max
	   -maxiter N    New value for the maximum number of iterations (must be an integer)     
	   -width W      New width for the output image
	   -height H     New height for the output image
	   -output F     New name for the output file
	   -threads N    Number of threads to use for processing (default is 1) 
	   -row          Run using a row-based approach        
	   -task         Run using a thread-based approach

	   Support for setting the number of threads is optional

	   You may also appropriately apply reasonable minimum / maximum values (e.g. minimum image width, etc.)
	   */


	/* Are there any locks to set up? */


	if(processArguments(argc, argv, &theSettings))
	{
		/* Dispatch here based on what mode we might be in */
		if(theSettings.theMode == MODE_THREAD_SINGLE)
		{
			/* Create a bitmap of the appropriate size */
			pBitmap = bitmap_create(theSettings.nPixelWidth, theSettings.nPixelHeight);

			/* Fill the bitmap with dark blue */
			bitmap_reset(pBitmap,MAKE_RGBA(0,0,255,0));

			/* Compute the image */
			compute_image_singlethread(&theSettings, pBitmap);

			// Save the image in the stated file.
			if(!bitmap_save(pBitmap,theSettings.szOutfile)) {
				fprintf(stderr,"fractal: couldn't write to %s: %s\n",theSettings.szOutfile,strerror(errno));
				return 1;
			}            
		}
		else if(theSettings.theMode == MODE_THREAD_ROW)
		{
			gentasks(theSettings.nPixelWidth, 1, &theSettings);
			run_threads();
			free(tasks);
		}
		/* A row-based approach will not require any concurrency protection */

		/* Could you send an argument and write a different version of compute_image that works off of a
		   certain parameter setting for the rows to iterate upon? */
		else if(theSettings.theMode == MODE_THREAD_TASK)
		{
			gentasks(20,20, &theSettings);
			run_threads();
			free(tasks);
		}
		else 
		{
			/* Uh oh - how did we get here? */
			fprintf(stderr, "Invalid mode\n");
		}
	}
	else
	{
		/* Probably a great place to dump the help */

		/* Probably a good place to bail out */
		exit(-1);
	}

	/* TODO: Do any cleanup as required */
	bitmap_delete(pBitmap);
	return 0;
}
