#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS 20
#define COLS 40
#define MAX_OBJECTS 100

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
    printf("\n");
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            putchar(canvas[y][x]);
        }
        putchar('\n');
    }
    printf("\n");
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
    if (objectCount == 0) {
        printf("No objects added yet.\n");
        return;
    }

    printf("\nObjects in picture:\n");
    for (int i = 0; i < objectCount; ++i) {
        const Shape *shape = &objects[i];
        printf("ID %d: ", shape->id);
        switch (shape->type) {
            case SHAPE_RECTANGLE:
                printf("Rectangle at (%d,%d) to (%d,%d)\n", shape->x1, shape->y1, shape->x2, shape->y2);
                break;
            case SHAPE_CIRCLE:
                printf("Circle center (%d,%d) radius %d\n", shape->x1, shape->y1, shape->radius);
                break;
            case SHAPE_LINE:
                printf("Line from (%d,%d) to (%d,%d)\n", shape->x1, shape->y1, shape->x2, shape->y2);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle at (%d,%d), (%d,%d), (%d,%d)\n", shape->x1, shape->y1, shape->x2, shape->y2, shape->x3, shape->y3);
                break;
        }
    }
    printf("\n");
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
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        ;
    }
}

int promptInt(const char *message)
{
    int value;
    printf("%s", message);
    while (scanf("%d", &value) != 1) {
        resetInput();
        printf("Invalid integer. %s", message);
    }
    resetInput();
    return value;
}

int promptCoordinate(const char *message, int min, int max)
{
    int value;
    do {
        value = promptInt(message);
        if (value < min || value > max) {
            printf("Value must be between %d and %d.\n", min, max);
        }
    } while (value < min || value > max);
    return value;
}

Shape createRectangle(void)
{
    Shape shape;
    shape.type = SHAPE_RECTANGLE;
    shape.id = nextId++;
    printf("Enter rectangle coordinates. Values must remain inside the canvas.\n");
    shape.x1 = promptCoordinate("Left X (0-39): ", 0, COLS - 1);
    shape.y1 = promptCoordinate("Top Y (0-19): ", 0, ROWS - 1);
    shape.x2 = promptCoordinate("Right X (0-39): ", 0, COLS - 1);
    shape.y2 = promptCoordinate("Bottom Y (0-19): ", 0, ROWS - 1);
    return shape;
}

Shape createCircle(void)
{
    Shape shape;
    shape.type = SHAPE_CIRCLE;
    shape.id = nextId++;
    printf("Enter circle center and radius.\n");
    shape.x1 = promptCoordinate("Center X (0-39): ", 0, COLS - 1);
    shape.y1 = promptCoordinate("Center Y (0-19): ", 0, ROWS - 1);
    do {
        shape.radius = promptInt("Radius (1-20): ");
        if (shape.radius <= 0) {
            printf("Radius must be positive.\n");
        }
    } while (shape.radius <= 0);
    return shape;
}

Shape createLine(void)
{
    Shape shape;
    shape.type = SHAPE_LINE;
    shape.id = nextId++;
    printf("Enter line endpoints.\n");
    shape.x1 = promptCoordinate("Start X (0-39): ", 0, COLS - 1);
    shape.y1 = promptCoordinate("Start Y (0-19): ", 0, ROWS - 1);
    shape.x2 = promptCoordinate("End X (0-39): ", 0, COLS - 1);
    shape.y2 = promptCoordinate("End Y (0-19): ", 0, ROWS - 1);
    return shape;
}

Shape createTriangle(void)
{
    Shape shape;
    shape.type = SHAPE_TRIANGLE;
    shape.id = nextId++;
    printf("Enter triangle vertices.\n");
    shape.x1 = promptCoordinate("Vertex A X (0-39): ", 0, COLS - 1);
    shape.y1 = promptCoordinate("Vertex A Y (0-19): ", 0, ROWS - 1);
    shape.x2 = promptCoordinate("Vertex B X (0-39): ", 0, COLS - 1);
    shape.y2 = promptCoordinate("Vertex B Y (0-19): ", 0, ROWS - 1);
    shape.x3 = promptCoordinate("Vertex C X (0-39): ", 0, COLS - 1);
    shape.y3 = promptCoordinate("Vertex C Y (0-19): ", 0, ROWS - 1);
    return shape;
}

void addObject(void)
{
    if (objectCount >= MAX_OBJECTS) {
        printf("Object limit reached. Cannot add more shapes.\n");
        return;
    }

    printf("Choose a shape to add:\n");
    printf(" 1. Rectangle\n");
    printf(" 2. Circle\n");
    printf(" 3. Line\n");
    printf(" 4. Triangle\n");

    int choice = promptInt("Enter choice: ");
    Shape shape;
    switch (choice) {
        case SHAPE_RECTANGLE:
            shape = createRectangle();
            break;
        case SHAPE_CIRCLE:
            shape = createCircle();
            break;
        case SHAPE_LINE:
            shape = createLine();
            break;
        case SHAPE_TRIANGLE:
            shape = createTriangle();
            break;
        default:
            printf("Invalid shape choice.\n");
            return;
    }

    objects[objectCount++] = shape;
    renderObjects();
    printf("Added object ID %d.\n", shape.id);
}

void deleteObject(void)
{
    if (objectCount == 0) {
        printf("No objects to delete.\n");
        return;
    }

    int id = promptInt("Enter object ID to delete: ");
    int index = findObjectIndexById(id);
    if (index < 0) {
        printf("Object with ID %d not found.\n", id);
        return;
    }

    for (int i = index; i < objectCount - 1; ++i) {
        objects[i] = objects[i + 1];
    }
    --objectCount;
    renderObjects();
    printf("Deleted object ID %d.\n", id);
}

void modifyObject(void)
{
    if (objectCount == 0) {
        printf("No objects to modify.\n");
        return;
    }

    int id = promptInt("Enter object ID to modify: ");
    int index = findObjectIndexById(id);
    if (index < 0) {
        printf("Object with ID %d not found.\n", id);
        return;
    }

    Shape *shape = &objects[index];
    printf("Modifying object ID %d.\n", id);
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
    printf("Modified object ID %d.\n", id);
}

void printMenu(void)
{
    printf("2D Graphics Editor\n");
    printf("-------------------\n");
    printf("1. Add object\n");
    printf("2. Delete object\n");
    printf("3. Modify object\n");
    printf("4. Display picture\n");
    printf("5. List objects\n");
    printf("6. Clear all objects\n");
    printf("7. Exit\n");
}

int main(void)
{
    clearCanvas();
    renderObjects();

    while (1) {
        printMenu();
        int choice = promptInt("Enter menu selection: ");

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
                objectCount = 0;
                clearCanvas();
                renderObjects();
                printf("All objects removed.\n");
                break;
            case 7:
                printf("Exiting 2D Graphics Editor.\n");
                return 0;
            default:
                printf("Invalid menu choice. Please select 1-7.\n");
                break;
        }
    }
}
