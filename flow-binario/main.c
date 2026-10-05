#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "Cell_0.h"
#include "FontRasterized_0_1.h"

// read cell dimensions from binary file
int ReadCellSize(const char *filename, FILE **file, uint32_t* width, uint32_t* height){
    *file = fopen(filename, "rb");
    
    if (*file  == NULL){
        printf("Error: Couldn't open %s\n", filename);
        return 0;
    }
    
    if (fread(width, sizeof(uint32_t), 1, *file) != 1){
        printf("Error: Couldn't read width\n");
        fclose(*file);
        *file = NULL;
        return 0;
    }
    
    if (fread(height, sizeof(uint32_t), 1, *file) != 1){
        printf("Error: Couldn't read height\n");
        fclose(*file);
        *file = NULL;
        return 0;
    }
    
    // bounds validation
    if (*width < 10 || *height > 100 || *height < 10 || *width >100){
        printf("Error: Invalid image dimensions (%u x %u)\n", *width, *height);
        fclose(*file);
        *file = NULL;
        return 0;
    }
    
    return 1;
}

// read pixel into array
int ReadPixel(FILE *file, unsigned char **Cell, uint32_t width, uint32_t height){
    size_t pixelnb = (size_t)width * height;
    
    *Cell =  (unsigned char *)malloc(pixelnb * sizeof(unsigned char));
    if (*Cell == NULL)
    {
        printf("Error: Memory allocation failed\n");
        return 0;
    }
    
    size_t nbread = fread(*Cell, sizeof(unsigned char), pixelnb, file);
    
    if (nbread != pixelnb)
    {
        printf("Error: Not enough pixels read (expected %zu, got %zu)\n", pixelnb, nbread);
        free(*Cell);
        *Cell = NULL;
        return 0;
        
    }


    return 1;
    
}

// get bit from single-bit bitmap array
uint8_t GetDigitBitMapBit(const uint8_t *array, uint32_t x, uint32_t y) {
	size_t i_0 = x / 8 + 4 * y; // 4y gives us which row, since we are doing 4 times byte times y, and x/8 gives which byte within the row.
	size_t i_1 = x % 8; // x percent 8, gives remainder. X divided by 8 gives the byte, and the remainder gives the count (0,1,2,3) that tells us which bit we are on in the specific byte
	return (array[i_0] >> i_1) & 1;
}

// get pixel value
unsigned char GetCellBit(unsigned char* cell, int width, int line, int col){
	size_t i = (size_t)width * line + col;
	return cell[i];

}

// print cell content to the terminal
void PrintCell(unsigned char *cell, int height, int width) {
	for (int line = 0 ; line < height; line++) {
		for (int col = 0; col < width; col++) {
			if (GetCellBit(cell, width, line, col) == 1) {
				printf("*");
			}
			else {
				printf(".");
			}
		}
        printf("\n");
	}
}

// Compare Cell with Bitmap "0" and Bitmap "1" and return which number is identified at what percentage of pixels
int IdentifyCell(unsigned char* cell_array, int cell_width, int cell_height, unsigned char* bitmap_0, unsigned char* bitmap_1) {
    
    const int bitmap_width = 32;
    const int bitmap_height = 32;

    int max_matching_pixels_0 = 0; // when comparing with bitmap "0"
    int max_matching_pixels_1 = 0; // when comparing with bitmap "1"

    const double seuil = 0.90;

    // i is the offset in x ; j is the offset in y
    for (size_t i = 0; i <= (cell_width - bitmap_width); i++) {
        for (size_t j = 0; j <= (cell_height - bitmap_height); j++) {
            
            int current_matching_pixels_0 = 0;
            int current_matching_pixels_1 = 0;

            // for each offset count the number of matching pixels 
            // y is the line, x is the column
            for (size_t x = 0; x < bitmap_width; x++) {
                for (size_t y = 0; y < bitmap_height; y++) {

                    unsigned char cell_bit = GetCellBit(cell_array, cell_width, j + y, i + x);

                    unsigned char bitmap_bit_0 = GetDigitBitmapBit(bitmap_0, y, x);
                    unsigned char bitmap_bit_1 = GetDigitBitmapBit(bitmap_1, y, x);

                    if (cell_bit == bitmap_bit_0) {
                        current_matching_pixels_0++;
                    }

                    if (cell_bit == bitmap_bit_1) {
                        current_matching_pixels_1++;
                    }
                }
            }

            if (current_matching_pixels_0 > max_matching_pixels_0) {
                max_matching_pixels_0 = current_matching_pixels_0;
            }

            if (current_matching_pixels_1 > max_matching_pixels_1) {
                max_matching_pixels_1 = current_matching_pixels_1;
            }

        }
    }
 
    double best_ratio_0 = (double) max_matching_pixels_0 / (32*32); // best ratio of mathcing pixels when comparing with bitmap "0"

    double best_ratio_1 = (double) max_matching_pixels_1 / (32*32); // best ratio of mathcing pixels when comparing with bitmap "1"
    
    if (best_ratio_0 > seuil) {
        printf("chiffre '0' a été reconnu à %.2f%%\n", best_ratio_0 * 100.0);
        return 0;
    } else if (best_ratio_1 > seuil) {
        printf("chiffre '1' a été reconnu à %.2f%%\n", best_ratio_1 * 100.0);
        return 1;
    } else {
        printf("Error!");
        return -1;
    }
}

int main(void){
    FILE* file = NULL;
    uint32_t width = 0, height = 0;
    unsigned char* cell = NULL;
    //PrintCell(Cell_0, 75, 75);
    // first; read dimensions
    if (!ReadCellSize("Cell.bin", &file, &width, &height)) {
        return EXIT_FAILURE;
    }
    
    printf("Image dimensions: %u x %u\n", width, height);

    // second; read pixel buffer
    if (!ReadPixel(file, &cell, width, height)) {
        fclose(file);
        return EXIT_FAILURE;
    }

    // done with file
    fclose(file);

	printf("%d\n",IdentifyCell(cell, width, height, (unsigned char*)DigitBitmap[0], (unsigned char*)DigitBitmap[1]));
	
    // free allocated buffer
    free(cell);
    return EXIT_SUCCESS;
}
