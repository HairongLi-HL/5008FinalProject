#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_COLOR 255
#define BRIGHTNESS_FACTOR 80

void blur_image(char *input_file, char *output_file);
void increase_brightness(char *input_file, char *output_file);
void rotate_image(char *input_file, char *output_file);
void color_image(char *input_file, char *output_file);

int main() {
    int choice;
    char input_file[255], output_file[255];

    printf("Enter the input file name: ");
    scanf("%s", input_file);

    printf("Enter the output file name: ");
    scanf("%s", output_file);


    printf("Choose an operation:\n");
    printf("1. Rotate image\n");
    printf("2. Increase brightness\n");
    printf("3. Blur image\n");
	printf("4. Color effect image\n");
	printf("5. Exit the program\n");
    printf("Enter your choice (1, 2, 3, 4 to process image, or 5 to exit): ");
    scanf("%d", &choice);

	while (choice != 5){
		switch (choice) {
			case 1:
				rotate_image(input_file, output_file);
				break;
			case 2:
				increase_brightness(input_file, output_file);
				break;
			case 3:
				blur_image(input_file, output_file);
				break;
			case 4:
				color_image(input_file, output_file);
				break;
			default:
				printf("Invalid choice. Exiting...\n");
				exit(1);
		}
		printf("\nEnter the output file name: ");
    	scanf("%s", output_file);

		printf("Choose an operation:\n");
		printf("1. Rotate image\n");
		printf("2. Increase brightness\n");
		printf("3. Blur image\n");
		printf("4. Color effect image");
		printf("5. Exit the program\n");
		printf("Enter your choice (1, 2, 3, 4 to process image, or 5 to exit): ");
		scanf("%d", &choice);
	}

    return 0;
}

void rotate_image(char *input_file, char *output_file) {
    FILE *fIn = fopen(input_file, "r");
    FILE *fOut = fopen(output_file, "w+");

    int i, j;
    unsigned char byte[54];

    if (fIn == NULL) {
        printf("File does not exist.\n");
    }

    for (i = 0; i < 54; i++) {
        byte[i] = getc(fIn);
    }

    fwrite(byte, sizeof(unsigned char), 54, fOut);

    int height = *(int *)&byte[18];
    int width = *(int *)&byte[22];
    int bitDepth = *(int *)&byte[28];

    int size = height * width;

    unsigned char buffer[width][height][3];
    unsigned char out_buffer[width][height][3];

    for (i = 0; i < size; i++) {
        buffer[i % width][i / width][0] = getc(fIn); // blue
        buffer[i % width][i / width][1] = getc(fIn); // green
        buffer[i % width][i / width][2] = getc(fIn); // red
    }

    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            out_buffer[j][height - 1 - i][0] = buffer[i][j][0]; // blue
            out_buffer[j][height - 1 - i][1] = buffer[i][j][1]; // green
            out_buffer[j][height - 1 - i][2] = buffer[i][j][2]; // red
		}	
	}

	for (i = 0; i < size; i++) {
		putc(out_buffer[i % width][i / width][0], fOut); // blue
		putc(out_buffer[i % width][i / width][1], fOut); // green
		putc(out_buffer[i % width][i / width][2], fOut); // red
	}

	fclose(fIn);
	fclose(fOut);
}

void increase_brightness(char *input_file, char *output_file) {
    FILE *fIn = fopen(input_file,"r");					
    FILE *fOut = fopen(output_file,"w+");	    	//Output File name

	int i;
	unsigned char byte[54];

	if (fIn == NULL) {
		printf("File does not exist.\n");
		exit(1);
	}

	for (i = 0; i < 54; i++) {
		byte[i] = getc(fIn);
	}

	fwrite(byte, sizeof(unsigned char), 54, fOut);

	int height = *(int *)&byte[18];
	int width = *(int *)&byte[22];
	int bitDepth = *(int *)&byte[28];

	int size = height * width;

	unsigned char buffer[size][3];
	int temp;

	for (i = 0; i < size; i++) {
		buffer[i][0] = getc(fIn); // blue
		buffer[i][1] = getc(fIn); // green
		buffer[i][2] = getc(fIn); // red
	}

	for (i = 0; i < size; i++) {
		for (int j = 0; j < 3; j++) {
			temp = buffer[i][j] + BRIGHTNESS_FACTOR;
			buffer[i][j] = (temp > MAX_COLOR) ? MAX_COLOR : temp;
		}
	}

	for (i = 0; i < size; i++) {
		putc(buffer[i][0], fOut); // blue
		putc(buffer[i][1], fOut); // green
		putc(buffer[i][2], fOut); // red
	}

	fclose(fIn);
	fclose(fOut);

}

void color_image(char *input_file, char *output_file) {
    clock_t start, stop;

	start = clock();						// Note the start time for profiling purposes.

	FILE *fIn = fopen(input_file,"r");					
    FILE *fOut = fopen(output_file,"w+");		    	// Output File name

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

	int size = height*width;					// calculate the image size

	unsigned char buffer[size][3];					// store the input image data
	unsigned char out[size][3];					// store the output image data

	for(i=0;i<size;i++)						// read image data character by character, each pixel will have a tuple of 3 values
	{
		buffer[i][2]=getc(fIn);					// blue
		buffer[i][1]=getc(fIn);					// green
		buffer[i][0]=getc(fIn);					// red
	}
	
	float v=3.0 / 9.0;						// initialize the blurrring kernel: the res has to be nultiplied by 1/9
	float kernel[3][3]={{v,v,v},
						{v,v,v},
						{v,v,v}};

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
	// printf("\nCLOCKS_PER_SEC = %ld\n",stop-start); //1000000
	// printf("%lf ms\n",((double)(stop-start) * 1000.0)/CLOCKS_PER_SEC );
}


void blur_image(char *input_file, char *output_file) {
    clock_t start, stop;

	start = clock();						// Note the start time for profiling purposes.

	FILE *fIn = fopen(input_file,"r");					
    FILE *fOut = fopen(output_file,"w+");		    	// Output File name

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

	int size = height*width;					// calculate the image size

	unsigned char buffer[size][3];					// store the input image data
	unsigned char out[size][3];					// store the output image data

	for(i=0;i<size;i++)						// read image data character by character, each pixel will have a tuple of 3 values
	{
		buffer[i][2]=getc(fIn);					// blue
		buffer[i][1]=getc(fIn);					// green
		buffer[i][0]=getc(fIn);					// red
	}
	
	float v=1.0 / 9.0;						// initialize the blurrring kernel: the res has to be nultiplied by 1/9
	float kernel[3][3]={{v,v,v},
						{v,v,v},
						{v,v,v}};

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
	// printf("\nCLOCKS_PER_SEC = %ld\n",stop-start); //1000000
	// printf("%lf ms\n",((double)(stop-start) * 1000.0)/CLOCKS_PER_SEC );
}
