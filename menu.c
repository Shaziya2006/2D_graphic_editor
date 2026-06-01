#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void showMenu() {
    printf("=== 2D Graphic Editor ===\n");
    printf("1. Draw Line\n");
    printf("2. Draw Rectangle\n");
    printf("3. Draw Circle\n");
    printf("4. Fill Shape\n");
    printf("5. Clear Screen\n");
    printf("6. Exit\n");
    printf("========================\n");
}

int getChoice() {
    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);
    return choice;
}