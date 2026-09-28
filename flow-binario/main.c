#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>


int ReadCellSize(const char *filename, FILE **file, uint32_t* width, uint32_t* height){
    *file = fopen(filename, "rb");
    
    if (*file  == NULL){
        printf("Couldn't open %s\n", filename);
        return 0;
    }
    
    if (fread(width, sizeof(uint32_t), 1, *file) != 1){
        printf("Couldn't read width\n");
        fclose(*file);
        return 0;
    }
    
    if (fread(height, sizeof(uint32_t), 1, file) != 1){
        printf("Couldn't read height\n");
        fclose(*file);
        return 0;
    }
    
    if (*width < 10 || *height > 100 || *height < 10 || *width >100){
        printf("Invalid image dimensions\n");
        fclose(*file);
        return 0;
    }
    
    return 1;
}

int ReadPixel(FILE *file, unsigned char **Cell, uint32_t width, uint32_t height){
    size_t pixelnb;
    size_t nbread;
    
    pixelnb = (size_t)width *height;
    *Cell =  malloc(pixelnb * sizeof(unsigned char));
    
    if (*Cell == NULL)
    {
        printf("Memory allocation failed \n");
        return 0;
    }
    
    nbread = fread(*Cell, sizeof(unsigned char), pixelnb, file);
    
    if (nbread != pixelnb)
    {
        printf("Not enough pixels \n");
        free(*Cell);
        *Cell = NULL;
        return 0;
        
    }
    return 1;
    
}
uint8_t GetDigitBitMapBit(uint8_t* array, uint32_t x, uint32_t y) {
	size_t i_0 = x / 8 + 4 * y;// 4y gives us which row, since we are doing 4 times byte times y, and x/8 gives which byte within the row.
	size_t i_1 = x % 8; // x percent 8, gives remainder. X divided by 8 gives the byte, and the remainder gives the count (0,1,2,3) that tells us which bit we are on in the specific byte
	return (array[i_0] >> i_1) & 1;
}
unsigned char GetCellBit(unsigned char* cell, int width, int line, int col){
	size_t i = width * line + col;
	return cell[i];

}
PrintCell(unsigned char *cell, int height, int width) {
	for (int line = 0 ; line < height; line++) {
		for (int col = 0; col < width; col++) {
			if (GetCellBit(cell, width, line, col) == 1) {
				printf("*");
			}
			else {
				printf(".");
			}
		}printf("\n");
	}
}


int main(){
    printf("Hello World! /n");
    retrun 0;
}
