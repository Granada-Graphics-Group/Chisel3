#version 450
#extension GL_ARB_fragment_shader_interlock : enable
#extension GL_ARB_bindless_texture : require

in vec2 GCoordST;
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

layout(pixel_interlock_ordered) in;
layout(location = 0) out float FragmentArea;

layout(r32f, binding = 0) uniform coherent image2DArray data;

layout(bindless_sampler) uniform sampler2DArray ArrayTexture[20];

void main()
{
    ivec3 layerSize = textureSize(ArrayTexture[areaIndices.x], 0);    
    ivec2 coord = ivec2((GCoordST) * layerSize.x);

    beginInvocationInterlockARB();
    {   
        float read = imageLoad(data, ivec3(coord, areaIndices.y)).x;    
        imageStore(data, ivec3(coord, areaIndices.y), vec4(read + AreaPerPixel));
    }
    endInvocationInterlockARB();

    FragmentArea = AreaPerPixel;
}
