/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */

#include "LCD_GFX.h"
#include "ST7735.h"
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>

typedef struct {
    char name[20];   // Snack name
    float price;     // Snack price
    int stock;       // Stock available
} Snack;

Snack snacks[2] = {
    {"Chips", 1.50, 10},
    {"Soda", 2.00, 1}
};


/******************************************************************************
 * Local Functions
 ******************************************************************************/

/**************************************************************************//**
* @fn			void swap(uint8_t *a, uint8_t *b)
* @brief		Helper that swaps the values stored in 2 pointers
* @note
*****************************************************************************/

void swap(uint8_t *a, uint8_t *b) {
    uint8_t c = *a;
    *a = *b;
    *b = c;
}


/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((((31 * (red + 4)) / 255) << 11) | (((63 * (green + 2)) / 255) << 5) | ((31 * (blue + 4)) / 255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/

void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    LCD_setAddr(x, y, x, y);
    SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/

void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor) {
    uint16_t row = character - 0x20; //Determine row of ASCII table starting at space
    int i, j;
    if ((LCD_WIDTH - x > 7)&&(LCD_HEIGHT - y > 7)) {
        for (i = 0; i < 5; i++) {
            uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
            for (j = 0; j < 8; j++) {
                if (((pixels >> j) & 1) == 1) {
                    LCD_drawPixel(x + i, y + j, fColor);
                } else {
                    LCD_drawPixel(x + i, y + j, bColor);
                }
            }
        }
    }
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{
	int x = radius;
    int y = 0;
    int p = 1 - radius; // Decision parameter

    while (x >= y)
    {
        // Draw symmetric points
        LCD_drawPixel(x0 + x, y0 + y, color);
        LCD_drawPixel(x0 - x, y0 + y, color);
        LCD_drawPixel(x0 + x, y0 - y, color);
        LCD_drawPixel(x0 - x, y0 - y, color);
        LCD_drawPixel(x0 + y, y0 + x, color);
        LCD_drawPixel(x0 - y, y0 + x, color);
        LCD_drawPixel(x0 + y, y0 - x, color);
        LCD_drawPixel(x0 - y, y0 - x, color);

        y++;

        if (p <= 0)
            // move upward
            p += 2 * y + 1;
        else
        {
            x--;
            // move upward and left since we start from far right
            p += 2 * (y - x) + 1;
        }
    }
}

//works


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
	int dx = x1 - x0;
    int dy = y1 - y0;
    if (dx < 0) {
        dx = -1 * dx;
    }
    if (dy < 0) {
        dy = -1 * dy;
    }
    int sx = (x0 < x1) ? 1 : -1; // Step for x
    int sy = (y0 < y1) ? 1 : -1; // Step for y
  
    int err = dx - dy; // Initial decision parameter

    while (1) {
        LCD_drawPixel(x0, y0, c);
      
        if (x0 == x1 && y0 == y1) break; // Stop when endpoint is reached
      
        int e2 = 2 * err;
      
        if (e2 > -dy) { // Adjust x
            err -= dy;
            x0 += sx;
        }
      
        if (e2 < dx) { // Adjust y
            err += dx;
            y0 += sy;
        }
    }
  
}

// works now, wiki help :D



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	if (x1 < x0) {
        swap(&x0, &x1);
    }
  
    if (y1 < y0) {
        swap(&y0, &y1);
    }
  
    // Set area of interest
    LCD_setAddr(x0, y0, x1, y1);
  
    // Set the color for each pixel in the area of interest
    for (uint8_t i = x0; i <= x1; i++) {
        for (uint8_t j = y0; j <= x1; j++){
                SPI_ControllerTx_16bit(color);
        }
    }
}

// works
/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
	LCD_drawBlock(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
  
}

//works

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
  
    // LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
    int i = 0;
    while (str[i] != '\0') {
        LCD_drawChar(x, y, str[i], fg, bg);
        i++;
        x+=6;
    }
    
     
}


// Insufficient balance
// Product you have chosen:, Price: TODO: find some way to print out the registers too 


// Display snack information on the LCD
void displaySnackInfo(int snackIndex, int userBalance) {
    LCD_setScreen(0xFFFF);  // Clear the display

    // Display Snack Info
    LCD_drawString(40, 30, "Snack: ", 0x0000, 0xFFFF);
    LCD_drawString(40 + (6 * 6), 30, snacks[snackIndex].name, 0x0000, 0xFFFF);  // Display snack name
    
    char priceStr[20];
    sprintf(priceStr, "$%.2f", snacks[snackIndex].price);  // Convert price to string
    LCD_drawString(40, 50, "Price: ", 0x0000, 0xFFFF);
    LCD_drawString(40 + (6 * 6), 50, priceStr, 0x0000, 0xFFFF);  // Display price
    
    char stockStr[10];
    sprintf(stockStr, "%d", snacks[snackIndex].stock);  // Convert stock to string
    LCD_drawString(40, 70, "Stock: ", 0x0000, 0xFFFF);
    LCD_drawString(40 + (6 * 6), 70, stockStr, 0x0000, 0xFFFF);  // Display stock

    // Display registers (or any other data you want to display)
    char balanceStr[20];
    sprintf(balanceStr, "Balance: $%.2f", (double)userBalance);
    LCD_drawString(40, 90, balanceStr, 0x0000, 0xFFFF);  // Display balance
}

// Check if the user balance is sufficient to purchase the snack
bool isBalanceSufficient(int snackIndex, int userBalance) {
    return userBalance >= snacks[snackIndex].price;
}

// Dispense the selected snack
void dispenseSnack(int snackIndex) {
    LCD_setScreen(0xFFFF);
    LCD_drawString(30, 50, "Dispensing: ", 0x0000, 0xFFFF);
    LCD_drawString(30 + (6 * 12), 50, snacks[snackIndex].name, 0x0000, 0xFFFF);  // Display snack name  // Display snack being dispensed
    // Start dispensing logic here (motor control, etc.)
}

// Reset the display after dispensing the snack
void resetDisplayAfterDispense() {
    // Wait for 10 seconds (use appropriate delay function for your platform)
    LCD_setScreen(0xFFFF);
    LCD_drawString(60, 50, "Hello!", 0x0000, 0xFFFF);
    LCD_drawString(25, 70, "Please pick a snack", 0x0000, 0xFFFF);
}

// Update the stock of the selected snack
void updateStock(int snackIndex) {
    snacks[snackIndex].stock--;  // Decrease stock by 1
}

float getPrice(int snackIndex) {
    return snacks[snackIndex].price;
}

int getStock(int snackIndex) {
    return snacks[snackIndex].stock;
}

// Select a snack and handle the logic for dispensing
void selectSnack(int snackIndex, int userBalance) {
    displaySnackInfo(snackIndex, userBalance);  // Display the snack info
    
    if (isBalanceSufficient(snackIndex, userBalance)) {  // Check if balance is sufficient
        dispenseSnack(snackIndex);  // Dispense the snack
        updateStock(snackIndex);    // Update the stock after dispensing
        resetDisplayAfterDispense();  // Wait and reset display
    } else {
        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Insufficient balance.", 0x0000, 0xFFFF);
                
    }
}
