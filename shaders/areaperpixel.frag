#version 430

in vec2 GCoordST;
in int gl_PrimitiveID;
flat in float AreaPerPixel;

layout(std140, binding = 10) uniform AppData
{
    uvec2 depthIndices;
    uvec2 seamMaskIndices;
    uvec2 brushIndices;
    uvec2 paintIndices;
    uvec2 brushMaskIndices;
    uvec2 areaIndices;
    uvec2 lockIndices;
};

layout(location = 0) out float FragmentArea;

// layout(r32f, binding = 0) uniform coherent image2DArray data;
// layout(r32ui, binding = 1) uniform coherent uimage2DArray lock;

void main()
{
//     ivec2 coord = ivec2((GCoordST) * 2048);
//     
// //     imageAtomicAdd(lock, ivec3(coord, lockIndices.y), 2);
//     
//     bool have_written = false;
//     do 
//     {
//         bool can_write = (imageAtomicCompSwap(lock, ivec3(coord, 0), 0u, 1u) == 0u);
//         
//         if (can_write) 
//         {
//             float read = imageLoad(data, ivec3(coord, areaIndices.y)).x;    
//             imageStore(data, ivec3(coord, areaIndices.y), vec4(read + AreaPerPixel));
//             memoryBarrier();
//     
//             imageAtomicExchange(lock, ivec3(coord, 0), 0u);
//             have_written = true;
//         }
//     } while (!have_written);
// 
// //     bool done = false;
// //     uint locked = 0;
// //     while(!done)
// //     {
// //         locked = imageAtomicExchange(lock, ivec3(coord, lockIndices.y), 1u);
// //         
// //         if (locked == 0)
// //         {
// //             float read = imageLoad(data, ivec3(coord, areaIndices.y)).x;    
// //             imageStore(data, ivec3(coord, areaIndices.y), vec4(read + 0.5));        
// //             memoryBarrier();
// //     
// //             imageAtomicExchange(lock, ivec3(coord, lockIndices.y), 0u);        
// //             done = true;
// //         }
// //     }
//     
//     gl_FragDepth = 50.015;
//     
//     FragmentArea = vec4(1.0);
    FragmentArea = AreaPerPixel;
}
