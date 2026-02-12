# Doogo - The Good Boy Adventure

**Doogo** is a 3D exploration game built with C and [raylib](https://www.raylib.com/). You play as a dog in an infinite procedural world, collecting bones and trying to stay healthy.

## Features

*   **3D Third-Person Gameplay**: Navigate a 3D world with mouse-look camera controls.
*   **Infinite World**: The world generates objects (trees, grass, clouds) around you as you move, creating an endless playground.
*   **Survival Mechanics**:
    *   **Health**: Decreases over time or via hazards (if implemented). Eat **Meat** to restore health.
    *   **Stamina**: Used for sprinting. Regenerates over time at the cost of a small amount of health.
*   **Scoring**: Collect **Bones** to increase your score.

## Controls

| Key | Action |
| :--- | :--- |
| **W, A, S, D** | Move Character |
| **Mouse** | Look / Rotate Camera |
| **Space** | Jump |
| **Left Shift** | Sprint (Consumes Stamina) |
| **ESC** | Pause Game / Back |
| **Enter / Click** | Select Menu Option |

## Building

This project requires a C compiler and the **raylib** library.

### Windows (MinGW/GCC)

```bash
gcc src/*.c -o doogo.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

### Linux

```bash
gcc src/*.c -o doogo -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.