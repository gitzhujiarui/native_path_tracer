# Native Path Tracer

## Overview
A native path tracer with Monte Carlo integration, correct ray-object intersection, BVH acceleration,
and sobol sequence super sampling. This path tracer is able to handle DIFFUSE, SPECULAR, and GLASS
surface materials. I also use RussianRoulette with a minimum depth of 5 to ensure global illumination.

## Usage
After you have cloned or downloaded the code, `cd` into _build_, then use the following command to compile
and run the code:
```
cmake ..
make
./RayTracing
```

## Sample Output
Here're the outputs generated by my path tracer with 128 samples per pixel:

![alt text](https://github.com/gitzhujiarui/native_path_tracer/blob/main/output_image/cornell_box.png?raw=true)

![alt text](https://github.com/gitzhujiarui/native_path_tracer/blob/main/output_image/cornell_box_specular.png?raw=true)