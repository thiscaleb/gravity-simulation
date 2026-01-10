## Math

This is my custom math libary, for implementing vector math

Why am I making my own? IDK it seemed fun at the time

Supports (or will support):

- 2D, 3D, and 4D Vector objects and functions
- 3x3 and 4x4 Matrixes and matrix operatons


### Matrix4

Matrix4 is defined as a float[4][4]. 
It will look something like this:

```
matrix4 model = {
    {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {n_pos.x, n_pos.y, n_pos.z, 1.0},
};
```

Normally, OpenGL expects a matrix to be written in column-major order, like this:
```
{
    1,0,0,x,
    0,1,0,y,
    0,0,1,z,
    0,0,0,1
}
```
So, in-memory, it looks like: 

```
1 0 0 0 0 1 0 0 0 0 1 0 x y z 1
```

This works out the exact same as the matrix4. Essentially, in the matrix4, think of each "row' as an OpenGL column.