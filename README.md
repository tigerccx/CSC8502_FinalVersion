# CSC8502_FinalVersion
## Introduction
This is the final version of the Computer Graphics coursework from CSC8502.
The original codebase is adapted with a lightweight Entity-Component framework.
Youtube demo video: https://www.youtube.com/watch?v=8f7jzPSOVvI
## Control
- Camera Control: [W,A,S,D] to move the camera, [Q,E] to lower/raise the camera, [Mouse Movement] to look around with the camera, hold [Shift] to accelerate camera movement. Camera auto navigation can be disabled with any of [W,A,S,D,Q,E].
- Light Control: [Comma, Period] to rotate the main light (directional light) in counter-clockwise/clockwise order, [L,K] to rotate the main light up/down. 
- Effect Control: [R] to enable volumetric lighting; [T] to enable blurring; [Y] to enable double vision; [U] to enable colour grading.
## Special Features
- PBR Rendering
- GPU Instancing
- Mirror
- Shadow Mapping for Directional, Spot and Point Light
- Volumetric Lighting
- Multiple Postprocessing Effects: Blurring, Double-Vision, Colour-Grading. 
- Order-dependent Transparent Rendering
- Refactored Codebase: To adapt to the framework, many of the original codes have been altered and various new classes are created. For example: Asset classes such as Texture has been encapsulated to support both 2D and Cube textures and to work together with RenderTargetTexture to allow off-screen rendering. Data classes such as Material have been modified to allow automatic per-frame data update. Functionalities have been rewritten as Components, which can be called and updated by the framework in each frame. A post-processing pipeline is added at the end of rendering to allow multiple consecutive effects. 
