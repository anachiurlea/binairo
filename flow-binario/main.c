#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Cell_0.h"
#include "FontRasterized_0_1.h"

// Output: CellValue.txt (same folder as Cell.bin), line format:  d:'0', 93.0761%

//predefined constants:
#define DIGIT_SIZE 32
#define MIN_DIM 10
#define MAX_DIM 100

// exit codes (LabVIEW can read them)
#define ERR_USAGE    1
#define ERR_FILE     2
#define ERR_DIMS     3
#define ERR_PIXELS   4
#define ERR_OUTPUT   5




// read cell dimensions from binary file
int ReadCellSize(const char *filename, FILE **file, uint32_t *width, uint32_t *height){
    *file = fopen(filename, "rb");
    
    if (*file  == NULL){
        printf("Error: Couldn't open %s\n", filename);
        return ERR_FILE;
    }
    
    if (fread(width, sizeof(uint32_t), 1, *file) != 1){
        printf("Error: Couldn't read width\n");
        fclose(*file);
        *file = NULL;
        return ERR_FILE;
    }
    
    if (fread(height, sizeof(uint32_t), 1, *file) != 1){
        printf("Error: Couldn't read height\n");
        fclose(*file);
        *file = NULL;
        return ERR_FILE;
    }

    
    // bounds validation
    if (*width < MIN_DIM || *height > MAX_DIM || *height < MIN_DIM || *width > MAX_DIM || *width < DIGIT_SIZE || *height < DIGIT_SIZE){
        printf("Error: Invalid image dimensions (%u x %u)\n", *width, *height);
        fclose(*file);
        *file = NULL;
        return ERR_DIMS;
    }
    
    return 0;
}

// read pixel into array
int ReadPixel(FILE *file, unsigned char **cell, uint32_t width, uint32_t height){
    size_t pixelnb = (size_t)width * height;
    
    *cell =  (unsigned char *)malloc(pixelnb);
    if (*cell == NULL)
    {
        printf("Error: Memory allocation failed\n");
        return ERR_PIXELS;
    }
    
    size_t nbread = fread(*cell, sizeof(unsigned char), pixelnb, file);
    
    if (nbread != pixelnb)
    {
        printf("Error: Not enough pixels read (expected %zu, got %zu)\n", pixelnb, nbread);
        free(*cell);
        *cell = NULL;
        return ERR_PIXELS;
        
    }
    unsigned char extra;
    if (fread(&extra, 1, 1, file) == 1) {
        printf("Error: Too many pixels in file (expected %zu)\n", pixelnb);
        free(*cell); 
        *cell = NULL;
        return ERR_PIXELS;
    }

    // pixels must be 0 or 1
    for (size_t i = 0; i < pixelnb; i++) {
        if ((*cell)[i] > 1) {
            printf("Error: Invalid pixel value %u at index %zu\n", (unsigned)(*cell)[i], i);
            free(*cell); 
            *cell = NULL;
            return ERR_PIXELS;
        }
    }


    return 0;
    
}

// get bit from single-bit bitmap array
uint8_t GetDigitBitmapBit(int digit, int line, int col) {
    return (DigitBitmap[digit][line] >> col) & 1;   // column 0 = least significant bit
}


// get pixel value
unsigned char GetCellBit(unsigned char *cell, int width, int line, int col){
	return cell[(size_t)width * line + col];
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

// Best match in percentage of a 32x32 digit bitmap slid over the cell
double BestMatchPercent(const unsigned char* cell, int c_width, int c_height, int num) {
    int best = 0;
    for (int oy = 0; oy <= c_height - DIGIT_SIZE; oy++) {
        for (int ox = 0; ox <= c_width - DIGIT_SIZE; ox++) {
            int match = 0;
            for (int y = 0; y < DIGIT_SIZE; y++)
                for (int x = 0; x < DIGIT_SIZE; x++)
                    if (GetCellBit(cell, c_width, oy + y, ox + x) == GetDigitBitmapBit(num, y, x))
                        match++;
            if (match > best) best = match;
        }
    }
    return 100.0 * best / (DIGIT_SIZE * DIGIT_SIZE);
}

// Percentage of background pixels in the cell
double EmptyPercent(const unsigned char* cell, int c_width, int c_height) {
    size_t zeros = 0;
    size_t n = (size_t)c_width * c_height;
    for (size_t i = 0; i < n; i++)
        if (cell[i] == 0) zeros++;
    return 100.0 * zeros / n;
}

// Returns 1 or 0 for digit, or -1 if empty. *percent is the similarity
int IdentifyCell(const unsigned char* cell, int c_width, int c_height, double thr_empty, double thr0, double thr1, double *percent) {
    // checking if the cell is empty
    double empty = EmptyPercent(cell, c_width, c_height);
    if (empty >= thr_empty) {
        *percent = empty;
        return -1;
    }

    // checking bestmatchpercent for 0 and 1
    double prob_0 = BestMatchPercent(cell, c_width, c_height, 0);
    double prob_1 = BestMatchPercent(cell, c_width, c_height, 1);

    // meet treshhold
    bool is_0 = (prob_0 >= thr0);
    bool is_1 = (prob_1 >= thr1);

    // if they both meet we take the one with the higher probability compared to the threshold
    if (is_0 && is_1) {
        if ((prob_0 - thr0) > (prob_1 - thr1)) {
            *percent = prob_0;
            return 0;
        }
        *percent = prob_1;
        return 1;
    }
    else if (is_0) {
        *percent = prob_0;
        return 0;
    }
    else if (is_1) {
        *percent = prob_1;
        return 1;
    }

    // neither digit found: treat as empty
    *percent = empty;   
    return -1;
}

// build a "CellValue.txt" file
int WriteResult(int result, double percent) {
    FILE* out = fopen("CellValue.txt", "w");
    if (out == NULL) {
        printf("Error: Couldn't open CellValue.txt for writing\n");
        return ERR_OUTPUT;
    }
    //for an empty file
    if (result == -1) {
        fprintf(out, "d:' ', %.4f%%\n", percent);
    }
    // for 0 or 1
    else {
        fprintf(out, "d:'%d', %.4f%%\n", result, percent);
    }

    fclose(out);
    return 0;
}

int main(int argc, const char *argv[]) {
    // error handle not correct amount of data given
    if (argc != 5) {
        printf("Usage: %s \"path/to/Cell.bin\" thr_empty thr_0 thr_1\n", argv[0]);
        return ERR_USAGE;
    }
    double thr_empty = atoi(argv[2]);
    double thr0 = atoi(argv[3]);
    double thr1 = atoi(argv[4]);
    // error handle incorrect thresholds 
    if (thr_empty < 0 || thr_empty > 100 || thr0 < 0 || thr0 > 100 || thr1 < 0 || thr1 > 100) {
        printf("Error: thresholds must be between 0 and 100\n");
        return ERR_USAGE;
    }
    // reading file
    FILE* file = NULL;
    uint32_t width = 0, height = 0;
    unsigned char* cell = NULL;
    int err;

    //handling function errors while reading
    if ((err = ReadCellSize(argv[1], &file, &width, &height)) != 0) return err;
    if ((err = ReadPixel(file, &cell, width, height)) != 0) { 
        fclose(file); 
        return err; 
    }
    fclose(file);

    double percent = 0.0;
    int result = IdentifyCell(cell, (int)width, (int)height, thr_empty, thr0, thr1, &percent);
    free(cell);

    printf("d: '%s', %.4f%%\n", result == -1 ? "empty" : (result == 0 ? "0" : "1"), percent);

    if ((err = WriteResult(result, percent)) != 0) return err;
    return 0;
}
