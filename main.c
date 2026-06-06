#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include <conio.h>

#define ROWS 20
#define COLS 40
#define MAX_OBJECTS 100
#define MENU_HEIGHT 12
#define MENU_WIDTH 50

#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)

HANDLE hConsole;

void setConsoleColor(WORD color)
{
    SetConsoleTextAttribute(hConsole, color);
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

void clearScreen(void)
{
    COORD coord = {0, 0};
    DWORD count;
    GetConsoleScreenBufferInfo(hConsole, &(CONSOLE_SCREEN_BUFFER_INFO){.dwSize = {80, 25}});
    FillConsoleOutputCharacter(hConsole, ' ', 80 * 25, coord, &count);
    gotoxy(0, 0);
}

void hideCursor(void)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void showCursor(void)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

typedef enum {
    SHAPE_RECTANGLE = 1,
    SHAPE_CIRCLE,
    SHAPE_LINE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int id;
    ShapeType type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;
} Shape;

static char canvas[ROWS][COLS];
static Shape objects[MAX_OBJECTS];
static int objectCount = 0;
static int nextId = 1;

void clearCanvas(void)
{
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            canvas[y][x] = '_';
        }
    }
}

void displayCanvas(void)
{
    clearScreen();
    int start_row = 1;
    int start_col = 2;

    setConsoleColor(FOREGROUND_CYAN | FOREGROUND_INTENSITY);
    gotoxy(start_col, start_row);
    printf("Canvas (20x40):");
    setConsoleColor(FOREGROUND_WHITE);

    setConsoleColor(FOREGROUND_WHITE | FOREGROUND_INTENSITY);
    gotoxy(start_col, start_row + 1);
    printf("+-");
    for (int i = 0; i < COLS; ++i) {
        printf("-");
    }
    printf("-+");
    setConsoleColor(FOREGROUND_WHITE);

    for (int y = 0; y < ROWS; ++y) {
        gotoxy(start_col, start_row + 2 + y);
        printf("|");
        for (int x = 0; x < COLS; ++x) {
            printf("%c", canvas[y][x]);
        }
        printf("|");
    }

    setConsoleColor(FOREGROUND_WHITE | FOREGROUND_INTENSITY);
    gotoxy(start_col, start_row + ROWS + 2);
    printf("+-");
    for (int i = 0; i < COLS; ++i) {
        printf("-");
    }
    printf("-+");
    setConsoleColor(FOREGROUND_WHITE);

    gotoxy(start_col, start_row + ROWS + 3);
    printf("Press any key to continue...");
    setConsoleColor(FOREGROUND_WHITE);
    getch();
}

void drawPixel(int x, int y)
{
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canvas[y][x] = '*';
    }
}

void drawLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        drawPixel(x1, y1);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawRectangle(const Shape *shape)
{
    int left = shape->x1 < shape->x2 ? shape->x1 : shape->x2;
    int right = shape->x1 < shape->x2 ? shape->x2 : shape->x1;
    int top = shape->y1 < shape->y2 ? shape->y1 : shape->y2;
    int bottom = shape->y1 < shape->y2 ? shape->y2 : shape->y1;

    drawLine(left, top, right, top);
    drawLine(left, bottom, right, bottom);
    drawLine(left, top, left, bottom);
    drawLine(right, top, right, bottom);
}

void drawCircle(const Shape *shape)
{
    int cx = shape->x1;
    int cy = shape->y1;
    int r = shape->radius;
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y) {
        drawPixel(cx + x, cy + y);
        drawPixel(cx + y, cy + x);
        drawPixel(cx - y, cy + x);
        drawPixel(cx - x, cy + y);
        drawPixel(cx - x, cy - y);
        drawPixel(cx - y, cy - x);
        drawPixel(cx + y, cy - x);
        drawPixel(cx + x, cy - y);

        y += 1;
        if (err <= 0) {
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void drawTriangle(const Shape *shape)
{
    drawLine(shape->x1, shape->y1, shape->x2, shape->y2);
    drawLine(shape->x2, shape->y2, shape->x3, shape->y3);
    drawLine(shape->x3, shape->y3, shape->x1, shape->y1);
}

void renderObjects(void)
{
    clearCanvas();
    for (int i = 0; i < objectCount; ++i) {
        const Shape *shape = &objects[i];
        switch (shape->type) {
            case SHAPE_RECTANGLE:
                drawRectangle(shape);
                break;
            case SHAPE_CIRCLE:
                drawCircle(shape);
                break;
            case SHAPE_LINE:
                drawLine(shape->x1, shape->y1, shape->x2, shape->y2);
                break;
            case SHAPE_TRIANGLE:
                drawTriangle(shape);
                break;
        }
    }
}

void listObjects(void)
{
    clearScreen();
    gotoxy(2, 1);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("OBJECTS IN PICTURE");
    setConsoleColor(FOREGROUND_WHITE);

    if (objectCount == 0) {
        gotoxy(2, 3);
        printf("No objects added yet.");
        gotoxy(2, 5);
        printf("Press any key to continue...");
        getch();
        return;
    }

    int row = 3;
    for (int i = 0; i < objectCount; ++i) {
        const Shape *shape = &objects[i];
        gotoxy(2, row++);
        switch (shape->type) {
            case SHAPE_RECTANGLE:
                printf("ID %d: Rectangle at (%d,%d) to (%d,%d)", shape->id, shape->x1, shape->y1, shape->x2, shape->y2);
                break;
            case SHAPE_CIRCLE:
                printf("ID %d: Circle center (%d,%d) radius %d", shape->id, shape->x1, shape->y1, shape->radius);
                break;
            case SHAPE_LINE:
                printf("ID %d: Line from (%d,%d) to (%d,%d)", shape->id, shape->x1, shape->y1, shape->x2, shape->y2);
                break;
            case SHAPE_TRIANGLE:
                printf("ID %d: Triangle at (%d,%d),(%d,%d),(%d,%d)", shape->id, shape->x1, shape->y1, shape->x2, shape->y2, shape->x3, shape->y3);
                break;
        }
    }

    gotoxy(2, row + 2);
    printf("Press any key to continue...");
    getch();
}

int findObjectIndexById(int id)
{
    for (int i = 0; i < objectCount; ++i) {
        if (objects[i].id == id) {
            return i;
        }
    }
    return -1;
}

void resetInput(void)
{
    while (_kbhit()) {
        _getch();
    }
}

int promptInt(const char *message, int row, int col)
{
    char input_str[20];
    int value = 0;
    int valid = 0;

    while (!valid) {
        gotoxy(col, row);
        setConsoleColor(FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
        printf("%s", message);
        setConsoleColor(FOREGROUND_WHITE);
        fflush(stdout);

        showCursor();
        scanf("%d", &value);
        hideCursor();

        if (value >= 0) {
            valid = 1;
        } else {
            gotoxy(col, row + 1);
            setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("Invalid input. Try again.");
            setConsoleColor(FOREGROUND_WHITE);
            getch();
            gotoxy(col, row + 1);
            printf("                          ");
        }
    }

    return value;
}

int promptCoordinate(const char *message, int min, int max, int row, int col)
{
    int value;
    int valid = 0;

    while (!valid) {
        value = promptInt(message, row, col);
        if (value < min || value > max) {
            gotoxy(col, row + 1);
            setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("Value must be between %d and %d.", min, max);
            setConsoleColor(FOREGROUND_WHITE);
            getch();
            gotoxy(col, row + 1);
            printf("                                    ");
        } else {
            valid = 1;
        }
    }

    return value;
}

Shape createRectangle(void)
{
    clearScreen();
    gotoxy(2, 1);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("CREATE RECTANGLE");
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 3);
    printf("Enter rectangle coordinates. Values must remain inside the canvas.");

    Shape shape;
    shape.type = SHAPE_RECTANGLE;
    shape.id = nextId++;
    shape.x1 = promptCoordinate("Left X (0-39): ", 0, COLS - 1, 5, 2);
    shape.y1 = promptCoordinate("Top Y (0-19): ", 0, ROWS - 1, 7, 2);
    shape.x2 = promptCoordinate("Right X (0-39): ", 0, COLS - 1, 9, 2);
    shape.y2 = promptCoordinate("Bottom Y (0-19): ", 0, ROWS - 1, 11, 2);
    return shape;
}

Shape createCircle(void)
{
    clearScreen();
    gotoxy(2, 1);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("CREATE CIRCLE");
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 3);
    printf("Enter circle center and radius.");

    Shape shape;
    shape.type = SHAPE_CIRCLE;
    shape.id = nextId++;
    shape.x1 = promptCoordinate("Center X (0-39): ", 0, COLS - 1, 5, 2);
    shape.y1 = promptCoordinate("Center Y (0-19): ", 0, ROWS - 1, 7, 2);
    shape.radius = promptInt("Radius (1-20): ", 9, 2);
    return shape;
}

Shape createLine(void)
{
    clearScreen();
    gotoxy(2, 1);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("CREATE LINE");
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 3);
    printf("Enter line endpoints.");

    Shape shape;
    shape.type = SHAPE_LINE;
    shape.id = nextId++;
    shape.x1 = promptCoordinate("Start X (0-39): ", 0, COLS - 1, 5, 2);
    shape.y1 = promptCoordinate("Start Y (0-19): ", 0, ROWS - 1, 7, 2);
    shape.x2 = promptCoordinate("End X (0-39): ", 0, COLS - 1, 9, 2);
    shape.y2 = promptCoordinate("End Y (0-19): ", 0, ROWS - 1, 11, 2);
    return shape;
}

Shape createTriangle(void)
{
    clearScreen();
    gotoxy(2, 1);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("CREATE TRIANGLE");
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 3);
    printf("Enter triangle vertices.");

    Shape shape;
    shape.type = SHAPE_TRIANGLE;
    shape.id = nextId++;
    shape.x1 = promptCoordinate("Vertex A X (0-39): ", 0, COLS - 1, 5, 2);
    shape.y1 = promptCoordinate("Vertex A Y (0-19): ", 0, ROWS - 1, 7, 2);
    shape.x2 = promptCoordinate("Vertex B X (0-39): ", 0, COLS - 1, 9, 2);
    shape.y2 = promptCoordinate("Vertex B Y (0-19): ", 0, ROWS - 1, 11, 2);
    shape.x3 = promptCoordinate("Vertex C X (0-39): ", 0, COLS - 1, 13, 2);
    shape.y3 = promptCoordinate("Vertex C Y (0-19): ", 0, ROWS - 1, 15, 2);
    return shape;
}

void addObject(void)
{
    if (objectCount >= MAX_OBJECTS) {
        clearScreen();
        gotoxy(2, 2);
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("Object limit reached. Cannot add more shapes.");
        setConsoleColor(FOREGROUND_WHITE);
        gotoxy(2, 4);
        printf("Press any key to continue...");
        getch();
        return;
    }

    clearScreen();
    gotoxy(2, 1);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("ADD OBJECT");
    setConsoleColor(FOREGROUND_WHITE);

    gotoxy(2, 3);
    printf("Choose a shape to add:");
    gotoxy(5, 5);
    printf("1. Rectangle");
    gotoxy(5, 6);
    printf("2. Circle");
    gotoxy(5, 7);
    printf("3. Line");
    gotoxy(5, 8);
    printf("4. Triangle");

    int choice = promptInt("Enter choice: ", 10, 2);
    Shape shape;
    switch (choice) {
        case 1:
            shape = createRectangle();
            break;
        case 2:
            shape = createCircle();
            break;
        case 3:
            shape = createLine();
            break;
        case 4:
            shape = createTriangle();
            break;
        default:
            clearScreen();
            gotoxy(2, 2);
            setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
            printf("Invalid shape choice.");
            setConsoleColor(FOREGROUND_WHITE);
            gotoxy(2, 4);
            printf("Press any key to continue...");
            getch();
            return;
    }

    objects[objectCount++] = shape;
    renderObjects();

    clearScreen();
    gotoxy(2, 2);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Added object ID %d successfully!", shape.id);
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 4);
    printf("Press any key to continue...");
    getch();
}

void deleteObject(void)
{
    if (objectCount == 0) {
        clearScreen();
        gotoxy(2, 2);
        setConsoleColor(FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
        printf("No objects to delete.");
        setConsoleColor(FOREGROUND_WHITE);
        gotoxy(2, 4);
        printf("Press any key to continue...");
        getch();
        return;
    }

    listObjects();
    int id = promptInt("Enter object ID to delete: ", 15, 2);
    int index = findObjectIndexById(id);
    if (index < 0) {
        clearScreen();
        gotoxy(2, 2);
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("Object with ID %d not found.", id);
        setConsoleColor(FOREGROUND_WHITE);
        gotoxy(2, 4);
        printf("Press any key to continue...");
        getch();
        return;
    }

    for (int i = index; i < objectCount - 1; ++i) {
        objects[i] = objects[i + 1];
    }
    --objectCount;
    renderObjects();

    clearScreen();
    gotoxy(2, 2);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Deleted object ID %d successfully!", id);
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 4);
    printf("Press any key to continue...");
    getch();
}

void modifyObject(void)
{
    if (objectCount == 0) {
        clearScreen();
        gotoxy(2, 2);
        setConsoleColor(FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
        printf("No objects to modify.");
        setConsoleColor(FOREGROUND_WHITE);
        gotoxy(2, 4);
        printf("Press any key to continue...");
        getch();
        return;
    }

    listObjects();
    int id = promptInt("Enter object ID to modify: ", 15, 2);
    int index = findObjectIndexById(id);
    if (index < 0) {
        clearScreen();
        gotoxy(2, 2);
        setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
        printf("Object with ID %d not found.", id);
        setConsoleColor(FOREGROUND_WHITE);
        gotoxy(2, 4);
        printf("Press any key to continue...");
        getch();
        return;
    }

    Shape *shape = &objects[index];
    switch (shape->type) {
        case SHAPE_RECTANGLE:
            *shape = createRectangle();
            shape->id = id;
            break;
        case SHAPE_CIRCLE:
            *shape = createCircle();
            shape->id = id;
            break;
        case SHAPE_LINE:
            *shape = createLine();
            shape->id = id;
            break;
        case SHAPE_TRIANGLE:
            *shape = createTriangle();
            shape->id = id;
            break;
    }

    renderObjects();

    clearScreen();
    gotoxy(2, 2);
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    printf("Modified object ID %d successfully!", id);
    setConsoleColor(FOREGROUND_WHITE);
    gotoxy(2, 4);
    printf("Press any key to continue...");
    getch();
}

void printMenu(void)
{
    clearScreen();

    gotoxy(10, 1);
    setConsoleColor(FOREGROUND_CYAN | FOREGROUND_INTENSITY);
    printf("2D GRAPHICS EDITOR");
    setConsoleColor(FOREGROUND_WHITE);

    gotoxy(5, 3);
    printf("=====================");
    gotoxy(5, 5);
    printf("1. Add object");
    gotoxy(5, 6);
    printf("2. Delete object");
    gotoxy(5, 7);
    printf("3. Modify object");
    gotoxy(5, 8);
    printf("4. Display picture");
    gotoxy(5, 9);
    printf("5. List objects");
    gotoxy(5, 10);
    printf("6. Clear all objects");
    gotoxy(5, 11);
    printf("7. Exit");
    gotoxy(5, 12);
    printf("=====================");

    gotoxy(5, 14);
    setConsoleColor(FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
    printf("Enter your choice: ");
    setConsoleColor(FOREGROUND_WHITE);
}

int main(void)
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    clearScreen();
    hideCursor();

    clearCanvas();
    renderObjects();

    while (1) {
        printMenu();
        int choice = promptInt("", 14, 24);

        switch (choice) {
            case 1:
                addObject();
                break;
            case 2:
                deleteObject();
                break;
            case 3:
                modifyObject();
                break;
            case 4:
                displayCanvas();
                break;
            case 5:
                listObjects();
                break;
            case 6:
                clearScreen();
                gotoxy(2, 2);
                setConsoleColor(FOREGROUND_YELLOW | FOREGROUND_INTENSITY);
                printf("Clearing all objects...");
                setConsoleColor(FOREGROUND_WHITE);
                fflush(stdout);
                Sleep(1000);
                objectCount = 0;
                clearCanvas();
                renderObjects();
                clearScreen();
                gotoxy(2, 2);
                setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("All objects removed.");
                setConsoleColor(FOREGROUND_WHITE);
                gotoxy(2, 4);
                printf("Press any key to continue...");
                getch();
                break;
            case 7:
                clearScreen();
                gotoxy(10, 10);
                setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("Exiting 2D Graphics Editor");
                setConsoleColor(FOREGROUND_WHITE);
                gotoxy(15, 12);
                printf("Goodbye!");
                fflush(stdout);
                Sleep(1500);
                clearScreen();
                setConsoleColor(FOREGROUND_WHITE);
                return 0;
            default:
                clearScreen();
                gotoxy(2, 2);
                setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
                printf("Invalid menu choice. Please select 1-7.");
                setConsoleColor(FOREGROUND_WHITE);
                gotoxy(2, 4);
                printf("Press any key to continue...");
                getch();
                break;
        }
    }

    return 0;
}
