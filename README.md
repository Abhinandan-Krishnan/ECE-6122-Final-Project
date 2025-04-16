# 3D Animation Engine

![C++](https://img.shields.io/badge/language-C++-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.3+-red.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A multithreaded OpenGL application that renders and animates 3D objects with kinematic-based collisions.

## Features

- **3D Model Loading** - Imports OBJ files using ASSIMP library
- **Multithreaded Animation** - Each object's movement calculated in a separate thread
- **Interactive Control** - Press 'G' to start/stop animations
- **Confined Movement** - Objects remain within the visible scene
- **Random Movement** - Each object moves at different speeds and rotates around unique axes
- **Good Vibes** - Changes light intensity randomly like it's in a Disco

## How it works

The application uses multithreading to calculate each object's movement independently. When the user presses 'G', objects begin moving with randomized velocities and rotation axes. The collision checker detects collisions between objects and calculates appropriate bounce responses based on conservation of momentum principles.
Objects are constrained to stay within a defined area near the center of the scene using boundary collision detection. Each object runs in its own thread to ensure smooth animation regardless of the number of objects in the scene.

## Requirements

- C++17 or higher
- OpenGL 4.3+
- GLFW3
- GLEW
- ASSIMP
- GLM
