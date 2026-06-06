# 2D Graphics Editor

A menu-driven 2D Graphics Editor implemented in C using a 2D character array as the canvas. Features a colorful Windows console interface with support for multiple shapes and comprehensive object management.

## Features

### Shape Drawing
- **Rectangle** – Draw rectangles with specified coordinates
- **Circle** – Draw circles with center point and radius
- **Line** – Draw lines between two endpoints
- **Triangle** – Draw triangles with three vertices

### Object Management
- **Add Objects** – Add new shapes to the canvas with automatic ID assignment
- **Delete Objects** – Remove objects by their unique ID
- **Modify Objects** – Update existing objects with new dimensions
- **List Objects** – View all objects with their coordinates and properties
- **Clear All** – Remove all objects from the canvas at once

### Display Features
- **Canvas Display** – 20x40 character grid with borders
- **Colored Output** – Windows Console API with color-coded menus and messages
- **Status Messages** – Color-coded feedback (green=success, red=error, yellow=info)
- **Object Tracking** – Each object has a unique ID for easy management

## Technical Specifications

### Canvas
- **Size:** 20 rows × 40 columns
- **Background:** Underscore character `_`
- **Drawing Character:** Asterisk `*`
- **Max Objects:** 100 simultaneously

### Implementation
- **Language:** C (C99)
- **Drawing Algorithms:** Bresenham's line algorithm, Midpoint circle algorithm
- **Platform:** Windows (uses Windows Console API)
- **UI:** Colored console menus with cursor positioning
- **Colors:** Cyan (titles), Green (headers/success), Yellow (info), Red (errors)

## Compilation

### Requirements
- GCC compiler (MinGW on Windows)
- Windows operating system (for Console API)

### Build
```bash
gcc main.c -o graphics_editor.exe
```

## Usage

### Running the Program
```bash
.\graphics_editor.exe
```

### Menu Options
1. **Add Object** – Select a shape type and enter coordinates
2. **Delete Object** – Remove an object by ID
3. **Modify Object** – Edit existing object parameters
4. **Display Picture** – View the rendered canvas
5. **List Objects** – Show all objects with their details
6. **Clear All Objects** – Remove all shapes from canvas
7. **Exit** – Close the program

### Input Examples

**Rectangle:**
- Left X: 5, Top Y: 3, Right X: 15, Bottom Y: 10

**Circle:**
- Center X: 20, Center Y: 10, Radius: 5

**Line:**
- Start X: 2, Start Y: 5, End X: 35, End Y: 15

**Triangle:**
- Vertex A: (5, 5), Vertex B: (25, 5), Vertex C: (15, 15)

## File Structure

- `main.c` – Complete implementation including:
  - Shape definitions and data structures
  - Canvas management functions
  - Drawing algorithms
  - Windows Console UI functions
  - Menu system and user input handling
  - Object management (add, delete, modify, list)

## Project Details

### Data Structures
- **Shape Structure:** Stores shape type, ID, coordinates, and radius
- **Canvas Array:** 2D char array for rendering
- **Object List:** Array of Shape structures with count tracking

### Key Functions
- `displayCanvas()` – Render the canvas with borders
- `drawLine()`, `drawCircle()`, `drawRectangle()`, `drawTriangle()` – Shape rendering
- `addObject()`, `deleteObject()`, `modifyObject()` – Object management
- `listObjects()` – Display all objects
- `printMenu()` – Show main menu
- Console helpers: `gotoxy()`, `setConsoleColor()`, `clearScreen()`

## Features Implemented

✅ Draw multiple shapes (Rectangle, Circle, Line, Triangle)  
✅ Add, delete, and modify objects  
✅ Unique object ID tracking  
✅ Display rendered canvas with borders  
✅ List all objects with coordinates  
✅ Input validation with range checking  
✅ Color-coded console interface  
✅ Clear all objects functionality  
✅ Formatted menus and status messages  
✅ Error handling and user feedback

## Technologies Used

- **C Programming Language** (C99 standard)
- **Windows Console API** (for colors and positioning)
- **Git & GitHub** (version control)
- **VS Code** (development environment)
- **GCC/MinGW** (compiler)

## Author

Shaziya Banu K

## License

Open source
