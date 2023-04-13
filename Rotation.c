#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// define BMP header structure
typedef struct {
    char type[2];
    int size;
    int reserved;
    int offset;
    int header_size;
    int width;
    int height;
    short planes;
    short bits;
    int compression;
    int image_size;
    int x_pixels_per_meter;
    int y_pixels_per_meter;
    int colors_used;
    int important_colors;
} BMPHeader;

// calculate the rotated pixel position
void rotate(int cx, int cy, int *x, int *y, double angle) {
    double radians = angle * M_PI / 180.0;
    double cos_theta = cos(radians);
    double sin_theta = sin(radians);
    int dx = *x - cx;
    int dy = *y - cy;
    *x = (int)(dx * cos_theta - dy * sin_theta + cx);
    *y = (int)(dx * sin_theta + dy * cos_theta + cy);
}

int main(int argc, char *argv[]) {
    // check command line arguments
    if (argc != 4) {
        printf("Usage: %s input output angle\n", argv[0]);
        return 1;
    }
    // open input file for reading
    FILE *in_file = fopen(argv[1], "rb");
    if (in_file == NULL) {
        printf("Error: cannot open input file.\n");
        return 1;
    }
    // open output file for writing
    FILE *out_file = fopen(argv[2], "wb");
    if (out_file == NULL) {
        printf("Error: cannot open output file.\n");
        fclose(in_file);
        return 1;
    }
    // read BMP header from input file
    BMPHeader header;
    fread(&header, sizeof(header), 1, in_file);
    // validate BMP header
    if (header.type[0] != 'B' || header.type[1] != 'M') {
        printf("Error: invalid BMP header.\n");
        fclose(in_file);
        fclose(out_file);
        return 1;
    }
    if (header.bits != 24) {
        printf("Error: unsupported pixel format.\n");
        fclose(in_file);
        fclose(out_file);
        return 1;
    }
    // read image data into a 3-dimensional array
    unsigned char ***data = (unsigned char ***)malloc(header.height * sizeof(unsigned char **));
    for (int i = 0; i < header.height; i++) {
        data[i] = (unsigned char **)malloc(header.width * sizeof(unsigned char *));
        for (int j = 0; j < header.width; j++) {
            data[i][j] = (unsigned char *)malloc(3 * sizeof(unsigned char));
            fread(data[i][j], sizeof(unsigned char), 3, in_file);
        }
    }
    // calculate rotation angle and center point
    double angle = atof(argv[3]);
    int cx = header.width / 2;
    int cy = header.height / 2;
    // write BMP header to output file
    fwrite(&header, sizeof(header), 1, out_file);
    // write rotated image data to output file
    for (int y = 0; y < header.height; y++) {
        for (int x = 0; x < header.width; x++) {
            int new_x = x;
            int new_y = y;
            rotate(cx, cy, &new_x, &new_y, angle
);
// interpolate pixel value from input image
int x1 = (int)floor(new_x);
int y1 = (int)floor(new_y);
int x2 = x1 + 1;
int y2 = y1 + 1;
if (x1 < 0 || x2 >= header.width || y1 < 0 || y2 >= header.height) {
// pixel is outside input image, set to black
fwrite("\x00\x00\x00", sizeof(unsigned char), 3, out_file);
} else {
// calculate weights for bilinear interpolation
double w1 = (double)(x2 - new_x) * (double)(y2 - new_y);
double w2 = (double)(new_x - x1) * (double)(y2 - new_y);
double w3 = (double)(x2 - new_x) * (double)(new_y - y1);
double w4 = (double)(new_x - x1) * (double)(new_y - y1);
// interpolate pixel color
unsigned char *p1 = data[y1][x1];
unsigned char *p2 = data[y1][x2];
unsigned char *p3 = data[y2][x1];
unsigned char *p4 = data[y2][x2];
unsigned char r = (unsigned char)(w1 * p1[2] + w2 * p2[2] + w3 * p3[2] + w4 * p4[2]);
unsigned char g = (unsigned char)(w1 * p1[1] + w2 * p2[1] + w3 * p3[1] + w4 * p4[1]);
unsigned char b = (unsigned char)(w1 * p1[0] + w2 * p2[0] + w3 * p3[0] + w4 * p4[0]);
// write pixel color to output file
unsigned char color[] = {b, g, r};
fwrite(color, sizeof(unsigned char), 3, out_file);
}
}
}
// free memory allocated for input image data
for (int i = 0; i < header.height; i++) {
for (int j = 0; j < header.width; j++) {
free(data[i][j]);
}
free(data[i]);
}
free(data);
// close input and output files
fclose(in_file);
fclose(out_file);
return 0;
}


