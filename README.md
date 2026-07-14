# Renderer

Headless OpenGL compute shader renderer using EGL.

# Demo
Images have been denoised with a [online third‑party denoiser](https://online.visual-paradigm.com/ru/photo-effects-studio/photo-noise-reduction-tool/) to reduce render noise while preserving details

# 1. Rough plastic materials
| Original | Denoised |
|----------|----------|
|<img width="600" height="450" alt="output1" src="https://github.com/user-attachments/assets/58be4de3-1fd8-4fc8-ac90-bcca48eabd9d" /> | <img width="600" height="450" alt="output1_processed" src="https://github.com/user-attachments/assets/b95b3dd3-1e56-457d-86e0-660bbf5270b6" />|

# 2. Rough metal materials
| Original | Denoised |
|----------|----------|
| <img width="600" height="450" alt="output2" src="https://github.com/user-attachments/assets/b210985f-4497-4b94-a467-7be5ee02f1d9" /> | <img width="600" height="450" alt="output2_processed" src="https://github.com/user-attachments/assets/a79543be-b334-46a0-959d-fa76b4211f5e" /> |

# 3. Smooth metal box and smooth plastic walls
| Original | Denoised |
|----------|----------|
| <img width="600" height="450" alt="output4" src="https://github.com/user-attachments/assets/8ea7ad96-9a9b-407a-bdb7-3c08bd05e098" /> | <img width="600" height="450" alt="output4_processed" src="https://github.com/user-attachments/assets/63d22694-ef45-472a-b095-08a7b8e1840c" /> |

# 4. Smooth metal box and metal colorful walls
| Original | Denoised |
|----------|----------|
| <img width="600" height="450" alt="output5" src="https://github.com/user-attachments/assets/0c6be044-5b1e-462c-b2e1-5bb974f0c85c" /> | <img width="600" height="450" alt="output5_processed" src="https://github.com/user-attachments/assets/47e49f90-9796-42db-a4cb-56eb0e59337b" /> |

# 5. Transparent boxes
| Original | Denoised |
|----------|----------|
| <img width="600" height="450" alt="output6" src="https://github.com/user-attachments/assets/4419b958-fc71-4877-8c54-0bd44a67ccd6" /> | <img width="600" height="450" alt="output6_processed" src="https://github.com/user-attachments/assets/bf08c83b-aae9-4741-b5f4-0e4efa0b3efc" /> |

# 6. Side light source
| Original | Denoised |
|----------|----------|
| <img width="600" height="450" alt="output7" src="https://github.com/user-attachments/assets/c4854662-1d04-45fe-b13e-5686262f0c4d" /> | <img width="600" height="450" alt="output7_processed" src="https://github.com/user-attachments/assets/44a211da-4bea-42d8-a41b-0785e10ce8fe" /> |

# 7. Room
| Original | Denoised |
|----------|----------|
| <img width="600" height="450" alt="room" src="https://github.com/user-attachments/assets/1928fb91-e0c4-47a0-8cc2-0e53fd6b6741" /> | <img width="600" height="450" alt="room_processed" src="https://github.com/user-attachments/assets/d08b8f98-60e5-4148-84f0-277e7c3d8725" /> |


## Build

```bash
cd renderer
mkdir build && cd build
cmake ..
cmake --build .
./renderer
