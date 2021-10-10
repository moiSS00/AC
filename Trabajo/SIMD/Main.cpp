/*
 * Main.cpp
 *
 *  Created on: 13 sept. 2018
 *      Author: arias
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <immintrin.h>
#include <malloc.h>

#define ELEMENTSPERPACKET (sizeof(__m256)/sizeof(float))




using namespace cimg_library;

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



	float *pRcomp;
	float *pGcomp;
	float *pBcomp ; // Pointers to the R, G and B components

	float *pRcomp1;
	float *pGcomp1 ;
	float *pBcomp1 ; // Pointers to the R, G and B components


	float *pRnew ;
	float *pGnew ;
	float *pBnew ;
	float *pdstImage; // Pointer to the new image pixels
	unsigned int width, height; // Width and height of the image (both images are equal in size)
	int nComp; // Number of image components

	struct timespec tStart, tEnd;
	double dTimeS;


	float imageMixProportion = 0.5;
	float valorMaximoPixel = 255.0f;
	/***************************************************
	 *
	 * Variables initialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */

	//srcImage.display(); // If needed, show the source image
	width = srcImage.width(); // Getting information from the source image
	height  = srcImage.height();
	nComp = srcImage.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)



	//unsigned int pixelesSobrantes = width*height%ELEMENTSPERPACKET;


	// Pointers to the RGB arrays of the source image
	pRcomp = srcImage.data(); // pRcomp points to the R component
	pGcomp = pRcomp + height * width; // pGcomp points to the G component
	pBcomp = pGcomp + height * width; // pBcomp points to B component

	
	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (float *) malloc (width * height * nComp * sizeof(float));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}








	//srcImage.display();

	// Pointers to the RGB arrays of the source image
	pRcomp1 = srcImage2.data(); // pRcomp points to the R component
	pGcomp1 = pRcomp1 + height * width; // pGcomp points to the G component
	pBcomp1 = pGcomp1 + height * width; // pBcomp points to B component



	// Pointers to the RGB arrays of the destination image
	pRnew = pdstImage;	
	pGnew= pRnew+ height * width;
	pBnew= pGnew + height  * width;

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

	/*P(r,g,b) = c * A(r,g,b) + (1-c) * B(r,g,b)
	Con c = porcentaje de mezcla
	y A(r,g,b), B(r,g,b) y P(r,g,b) son los pixeles de la imágen A, B y resultado respectivamente*/

	__m256 *minir =(__m256*)_mm_malloc(sizeof(__m256),sizeof(__m256));
	__m256 *minir1 = (__m256*)_mm_malloc(sizeof(__m256),sizeof(__m256));
	__m256 *miniresultTemp = (__m256*)_mm_malloc(sizeof(__m256),sizeof(__m256));
	float *miniout = (float*) _mm_malloc(sizeof(float)*ELEMENTSPERPACKET,sizeof(__m256));
	__m256 *miniresultATemp = (__m256*)_mm_malloc(sizeof(__m256),sizeof(__m256));

	
	for(int j = 0; j<20 ; j++){
		for (unsigned int i=0; i< width * height; i+=ELEMENTSPERPACKET){
				
			//multipicacion primera componente primera imagen															
			*minir1 = _mm256_set1_ps(imageMixProportion);
			*minir = _mm256_loadu_ps((float*)(pRcomp+i));
			*miniresultTemp = _mm256_mul_ps(*minir, *minir1);

			//multiplicacion primera componente segunda imagen			
			*minir1 = _mm256_set1_ps(1-imageMixProportion);								
			*minir = _mm256_loadu_ps((float*)(pRcomp1+i));
			*miniresultATemp = _mm256_mul_ps(*minir, *minir1);

			//suma de primera componente ambas imagenes
			*minir = _mm256_add_ps(*miniresultTemp,*miniresultATemp);


			//escritura de resultado  acotado
			*minir1=_mm256_set1_ps(valorMaximoPixel);
			*miniresultTemp= _mm256_min_ps(*minir,*minir1);
			_mm256_storeu_ps(pRnew+i,*miniresultTemp);

			


			//multipicacion segunda componente primera imagen
			*minir1 = _mm256_set1_ps(imageMixProportion);
			*minir = _mm256_loadu_ps((float*)(pGcomp+i));
			*miniresultTemp = _mm256_mul_ps(*minir, *minir1);

			//multiplicacion segunda componente segunda imagen				
			*minir1 = _mm256_set1_ps(1-imageMixProportion);								
			*minir = _mm256_loadu_ps((float*)(pGcomp1+i));
			*miniresultATemp = _mm256_mul_ps(*minir, *minir1);

			//suma de segunda componente ambas imagenes
			*minir=_mm256_set1_ps(-1);
			*minir = _mm256_add_ps(*miniresultTemp,*miniresultATemp);

			//escritura de resultado

			//escritura de resultado  acotado
			*minir1=_mm256_set1_ps(valorMaximoPixel);
			*miniresultTemp= _mm256_min_ps(*minir,*minir1);
			_mm256_storeu_ps(pGnew+i,*miniresultTemp);


			

			//multipicacion tercera componente primera imagen				
			*minir1 = _mm256_set1_ps(imageMixProportion);
			*minir = _mm256_loadu_ps((float*)(pBcomp+i));
			*miniresultTemp = _mm256_mul_ps(*minir, *minir1);

			//multiplicacion tercera componente segunda imagen				
			*minir1 = _mm256_set1_ps(1-imageMixProportion);								
			*minir = _mm256_loadu_ps((float*)(pBcomp1+i));
			*miniresultATemp = _mm256_mul_ps(*minir, *minir1);

			//suma de segunda componente ambas imagenes
			*minir=_mm256_set1_ps(-1);
			*minir = _mm256_add_ps(*miniresultTemp,*miniresultATemp);

			//escritura de resultado  acotado
			*minir1=_mm256_set1_ps(valorMaximoPixel);
			*miniresultTemp= _mm256_min_ps(*minir,*minir1);
			_mm256_storeu_ps(pBnew+i,*miniresultTemp);
		
		}
	}


	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 *
	 * COMPLETE
	 *
	 */


	

	

	

	_mm_free(minir);
	_mm_free(miniout);
	_mm_free(minir1);
	_mm_free(miniresultTemp);
	_mm_free(miniresultATemp);

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
	free(pdstImage);
	return(0);

}