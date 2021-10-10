/*
 * Main.cpp
 *
 *  Created on: 13 sept. 2018
 *      Author: <Student>
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

using namespace cimg_library;
const int N =8; //Numero de hilos. 

float *pRcomp, *pGcomp, *pBcomp; // Pointers to the R, G and B components
	float *pRcomp1, *pGcomp1, *pBcomp1; // Pointers to the R, G and B components fo image 2
	float *pRnew, *pGnew, *pBnew;
	int width, height; // Width and height of the image (both images are equal in size)
	float *pdstImage; // Pointer to the new image pixels
	int nComp; // Number of image components
	float imageMixProportion = 0.5;
	int a[N];


void* ThreadProc(void* arg)
{
	int j = *((int*)arg);
	int npixels = (height * width)/N; //numero de pixels que operara cada hilo
	int nInicio = npixels * j;

	for (int i = nInicio ; i< npixels*(j+1) ; i++){
		*(pRnew + i) =  *(pRcomp + i) * imageMixProportion + *(pRcomp1+i) * (1-imageMixProportion);
		if (*(pRnew+i)>255) *(pRnew+i) = 255;
		*(pGnew + i) =   *(pGcomp + i) * imageMixProportion + *(pGcomp1+i) * (1-imageMixProportion);
		if (*(pGnew+i)>255) *(pGnew+i) = 255;
		*(pBnew + i) =    *(pBcomp + i) * imageMixProportion + *(pBcomp1+i) * (1-imageMixProportion);
		if (*(pBnew+i)>255) *(pBnew+i) = 255;
	}


	//printf("Thread %d finished.\n", i);
	return NULL;
}

/**********************************
 * TODO
 * 	- Change the data type returned by CImg.srcImage to adjust the
 * 	requirements of your workgroup
 * 	- Change the data type of the components pointers to adjust the
 * 	requirements of your workgroup
 */

int main() {
	CImg<float> srcImage("historico.bmp"); // Open file and object initialization
	CImg<float> srcImage2("hojas.bmp");

	struct timespec tStart, tEnd;
	double dTimeS;

	pthread_t thread[N]; // array de hilos
	
	

	/***************************************************
	 *
	 * Variables initialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */

	srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	nComp = srcImage.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)


	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (float *) malloc (width * height * nComp * sizeof(float));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}
		pRcomp = srcImage.data(); // pRcomp points to the R component
		pGcomp = pRcomp + height * width; // pGcomp points to the G component
		pBcomp = pGcomp + height * width; // pBcomp points to B component

		// Pointers to the RGB arrays of the source image
		pRcomp1 = srcImage2.data(); // pRcomp points to the R component
		pGcomp1 = pRcomp1 + height * width; // pGcomp points to the G component
		pBcomp1 = pGcomp1 + height * width; // pBcomp points to B component

		// Pointers to the RGB arrays of the destination image
		pRnew = pdstImage;
		pGnew= pRnew + height * width;
		pBnew= pGnew + height * width;	


	/*********************************************
	 * Algorithm start
	 *
	 * Measure initial time
	 *
	 *	COMPLETE
	 *
	 */
	
	if (clock_gettime(CLOCK_REALTIME,&tStart)==-1)
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}



	/************************************************
	 * Algorithm.
	 * In this example, the algorithm is a components exchange
	 *
	 * TO BE REPLACED BY YOUR ALGORITHM
	 */

	for(int x = 0; x < 20; x++)
	{
		// Thread creationS
		for (int i = 0; i < N; i++)
		{
			a[i] = i;
			if (pthread_create(&thread[i], NULL, ThreadProc, &a[i])!= 0)
			{
				fprintf(stderr, "ERROR: Creating thread %d\n", i);
				return EXIT_FAILURE;
			}
		}

		// Wait till the completion of all threads
		//printf("Main thread waiting...\n");
		for (int i = 0; i < N; i++)
		{
			pthread_join(thread[i], NULL);
		}
		//printf("Main thread finished.\n");
	}





	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 *
	 * COMPLETE
	 *
	 */

	if (clock_gettime(CLOCK_REALTIME,&tEnd)==-1)
	{
		printf("ERROR: clock_gettime: %d.\n", errno);
		exit(EXIT_FAILURE);
	}

	dTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dTimeS += (tEnd.tv_nsec - tStart.tv_nsec) /1e+9;
	printf("Total time (s): %f s.\n", dTimeS);

		
	// Create a new image object with the calculated pixels
	// In case of normal color image use nComp=3,
	// In case of B&W image use nComp=1.
	CImg<float> dstImage(pdstImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina2.bmp"); 

	// Display the destination image
	dstImage.display(); // If needed, show the result image
	return(0);

}
