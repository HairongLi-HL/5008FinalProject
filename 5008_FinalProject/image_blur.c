
#include <stdio.h>
#include <time.h>


int main()
{
	clock_t start, stop;

	start = clock();						// Note the start time for profiling purposes.

    //Drawback here!
    // FILE *fIn = fopen("/Users/yuki/Desktop/CS5008FinalProject/test.PNG","r");			// Input File name
	// FILE *fOut = fopen("/Users/yuki/Desktop/CS5008FinalProject/test_res.PNG","w+");		    	// Output File name

	FILE *fIn = fopen("lena_color.bmp","r");			// Input File name
	FILE *fOut = fopen("lena_blur_color.bmp","w+");		    	// Output File name

	int i,j,y, x;
	unsigned char byte[54];

	if(fIn==NULL)							// check if the input file has not been opened succesfully.
	{											
		printf("File does not exist.\n");
	}

	for(i=0;i<54;i++)						// read the 54 byte header from fIn
	{									
		byte[i] = getc(fIn);								
	}

	fwrite(byte,sizeof(unsigned char),54,fOut);			// write the header back

	// extract image height, width and bitDepth from imageHeader 
	int height = *(int*)&byte[18];
	int width = *(int*)&byte[22];
	int bitDepth = *(int*)&byte[28];

	printf("width: %d\n",width);
	printf("height: %d\n",height );

	int size = height*width;					// calculate the image size

	unsigned char buffer[size][3];					// store the input image data
	unsigned char out[size][3];					// store the output image data

	for(i=0;i<size;i++)						// read image data character by character, each pixel will have a tuple of 3 values
	{
		buffer[i][2]=getc(fIn);					// blue
		buffer[i][1]=getc(fIn);					// green
		buffer[i][0]=getc(fIn);					// red
	}
	

    /**
     * Key here!!!!
     * 
     * we will get the new pixel that will replace the old one
     * we calculated the weighted average of the actual pixel 
	 * 
	 * https://www.youtube.com/watch?v=4Eh0y3LHTNU
	 *
    */

	float v=1.0 / 9.0;						// initialize the blurrring kernel: the res has to be nultiplied by 1/9
	float kernel[3][3]={{v,v,v},
						{v,v,v},
						{v,v,v}};



	/**
	 * 
	 * pixel
	 * 
	 * input: buffer[i][j]
	 * input: value
	 *   (0,0)
	 *     0,1,2
	 * 0 [[1,2,3],
	 * 1 [4, 5,6],
	 * 2 [7,8,9]]
	 * 
	 * blur 
	 * Kernel: weight
	 * {1,1,1},
	 * {1,1,1},
	 * {1,1,1}};
	*/



    /**
     * for each non-edge pixel, we just do the sum of itself and pixels around it 
     * and we divide by 9. 
     * 
     * we want to avoid edge pixels because they don't have enough neighbors,
     * so we avoid the first and last row/col
    */
	for(x=1;x<height-1;x++)  //rotate: for(i=0;i<height;i++) 
	{					
		for(y=1;y<width-1;y++)
		{
			float sum0= 0.0; //becasue we're working with RGB
			float sum1= 0.0;
			float sum2= 0.0;
			for(i=-1;i<=1;++i)
			{
				for(j=-1;j<=1;++j)
				{	
					// matrix multiplication with kernel with every color plane
					sum0=sum0+(float)kernel[i+1][j+1]*buffer[(x+i)*width+(y+j)][0];
					sum1=sum1+(float)kernel[i+1][j+1]*buffer[(x+i)*width+(y+j)][1];
					sum2=sum2+(float)kernel[i+1][j+1]*buffer[(x+i)*width+(y+j)][2];
				}
			}
			//ouyput
			out[(x)*width+(y)][0]=sum0;
			out[(x)*width+(y)][1]=sum1;
			out[(x)*width+(y)][2]=sum2;
		}
	}

    /**
     * After performing this operation on each non-edge pixl, 
     * we get the image blurred by using box blur
    */
	for(i=0;i<size;i++)						//write image data back to the file
	{
		putc(out[i][2],fOut);
		putc(out[i][1],fOut);
		putc(out[i][0],fOut);
	}
		
	fclose(fOut);
	fclose(fIn);

	stop = clock();
	printf("\nCLOCKS_PER_SEC = %ld\n",stop-start); //1000000
	printf("%lf ms\n",((double)(stop-start) * 1000.0)/CLOCKS_PER_SEC );
	return 0;

}