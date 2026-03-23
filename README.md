# Bricks3ds
A simple Arkanoid-like game developed in C++ for Nintendo 3DS consoles.

📌 Overview

Classic brick breaker game developed for the **Nintendo 3DS**.  
Runs on a modded 3DS console or compatible emulators.

The game was built using DevKitPro and several homebrew libraries.

## Technologies
- **Language**: C++
- **Toolchain**: DevKitPro
- **Core libraries**:
  - libctru (low-level hardware access)
  - citro2d (high-level 2D rendering)
  - citro3d (GPU-accelerated rendering support)

## Gameplay

The player controls a paddle moving left and right to bounce a ball and break bricks.

Some bricks require multiple hits to be destroyed.
If the ball falls below the paddle, the player loses a life.
When all lives are lost, the game restarts from level 1.

Because the two screens of the Nintendo 3ds have different sizes, scaling is applied when the ball moves between them (horizontal ratio ≈ 0.8).

## Level System

- Levels are loaded from external **text files**
- Each number in the grid file represents:
  - brick type / color / health points, or
  - empty space (usually 0)
- The level grid is automatically centered on screen
- Next level loads automatically when current one is cleared

🚀 Running the Project

You can run the game:

On a modded Nintendo 3DS
On a 3DS emulator

### Launch on Azahar Emulator
- Open Azahar
- Open game folder or drop *.3dsx* file on Azahar window
