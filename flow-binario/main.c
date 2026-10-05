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


    // free allocated buffer
    free(cell);
    return EXIT_SUCCESS;
}
