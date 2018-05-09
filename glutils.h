#ifndef GL_UTILS
#define GL_UTILS

#include <GL/glew.h>

inline GLsizei sizeOfGLType(GLenum type)
{
    switch(type)
    {
        case GL_UNSIGNED_BYTE:
            return sizeof(GLbyte);
        case GL_BYTE:
            return sizeof(GLbyte);
        case GL_UNSIGNED_SHORT:
            return sizeof(GLushort);
        case GL_SHORT:
            return sizeof(GLshort);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);            
        case GL_INT:
            return sizeof(GLint);
        case GL_FLOAT:
            return sizeof(GLfloat);            
        case GL_UNSIGNED_BYTE_3_3_2:
        case GL_UNSIGNED_BYTE_2_3_3_REV:
            return 1;
        case GL_UNSIGNED_SHORT_5_6_5:
        case GL_UNSIGNED_SHORT_5_6_5_REV: 
        case GL_UNSIGNED_SHORT_4_4_4_4:
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        case GL_UNSIGNED_SHORT_5_5_5_1:
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            return 2;
        case GL_UNSIGNED_INT_8_8_8_8:
        case GL_UNSIGNED_INT_8_8_8_8_REV:
        case GL_UNSIGNED_INT_10_10_10_2:
        case GL_UNSIGNED_INT_2_10_10_10_REV:    
            return 4;
    }
}

inline GLsizei sizeOfGLFormat(GLenum format)
{
    switch(format)
    {
        case GL_RED: 
        case GL_RED_INTEGER:    
        case GL_STENCIL_INDEX:
        case GL_DEPTH_COMPONENT:
        case GL_DEPTH_STENCIL:  return 1;
        case GL_RG:
        case GL_RG_INTEGER:     return 2;
        case GL_RGB:
        case GL_BGR:
        case GL_RGB_INTEGER:
        case GL_BGR_INTEGER:    return 3;
        case GL_RGBA:
        case GL_BGRA: 
        case GL_RGBA_INTEGER:
        case GL_BGRA_INTEGER:   return 4;
    }
}

inline GLsizei sizeOfGLInternalFormat(GLenum internalType)
{
    switch(internalType)
    {   
        case GL_R8:
        case GL_R8I:    
        case GL_R8UI:   return 1;
        case GL_R16:
        case GL_R16I:   
        case GL_R16UI:  return 2;
        case GL_R32I:   
        case GL_R32UI:  return 4;
        case GL_RG16I:
        case GL_RG16UI: return 4;
        case GL_RG32I:
        case GL_RG32UI: return 8;
        case GL_RGB:
        case GL_RGB8:   return 3;
        case GL_RGB16I:
        case GL_RGB16UI:return 6;
        case GL_RGB32I:
        case GL_RGB32UI:return 12;            
        case GL_RGBA:
        case GL_RGBA8:  return 4;       
        case GL_R16F:   return 2;
        case GL_R32F:   return 4;
        case GL_RG16F:  return 4;
        case GL_RG32F:  return 8;
        case GL_RGB16F: return 6;
        case GL_RGB32F: return 12;    
    }
}

inline std::string toGLSLImageFormat(int32_t format)
{
    switch(format)
    {
        case GL_RGBA32F         : return "rgba32f";
        case GL_RGBA16F         : return "rgba16f";
        case GL_RG32F           : return "rg32f";
        case GL_RG16F           : return "rg16f";
        case GL_R11F_G11F_B10F  : return "rg11f_g11f_b10f";        
        case GL_R32F            : return "r32f";
        case GL_R16F            : return "r16f";
        case GL_RGBA32UI        : return "rgba32ui";
        case GL_RGBA16UI        : return "rgba16ui";
        case GL_RGB10_A2UI      : return "rgb10_a2ui";
        case GL_RGBA8UI         : return "rgba8ui";
        case GL_RG32UI          : return "rg32ui";
        case GL_RG16UI          : return "rg16ui";
        case GL_RG8UI           : return "rg8ui";
        case GL_R32UI           : return "r32ui";
        case GL_R16UI           : return "r16ui";
        case GL_R8UI            : return "r8ui";
        case GL_RGBA32I         : return "rgba32i";
        case GL_RGBA16I         : return "rgba16i";
        case GL_RGBA8I          : return "rgba8i";
        case GL_RG32I           : return "rg32i";
        case GL_RG16I           : return "rg16i";
        case GL_RG8I            : return "rg8i";
        case GL_R32I            : return "r32i";
        case GL_R16I            : return "r16i";
        case GL_R8I             : return "r8i";
        case GL_RGBA16          : return "rgba16";
        case GL_RGB10_A2        : return "rgb10_a2";
        case GL_RGBA8           : return "rgba8";
        case GL_RG16            : return "rg16";
        case GL_RG8             : return "rg8";
        case GL_R16             : return "r16";
        case GL_R8              : return "r8";
        case GL_RGBA16_SNORM    : return "rgba16_snorm";
        case GL_RGBA8_SNORM     : return "rgba8_snorm";
        case GL_RG16_SNORM      : return "rg16_snorm";
        case GL_RG8_SNORM       : return "rg8_snorm";
        case GL_R16_SNORM       : return "r16_snorm";
        case GL_R8_SNORM        : return "r8_snorm";
    }
}

inline std::string prefixImageFormat(int32_t format)
{
    switch(format)
    {
        case GL_RGBA32F         : 
        case GL_RGBA16F         : 
        case GL_RG32F           : 
        case GL_RG16F           : 
        case GL_R11F_G11F_B10F  : 
        case GL_R32F            : 
        case GL_R16F            : 
        case GL_RGBA16          : 
        case GL_RGB10_A2        : 
        case GL_RGBA8           : 
        case GL_RG16            : 
        case GL_RG8             : 
        case GL_R16             : 
        case GL_R8              : 
        case GL_RGBA16_SNORM    : 
        case GL_RGBA8_SNORM     : 
        case GL_RG16_SNORM      : 
        case GL_RG8_SNORM       : 
        case GL_R16_SNORM       : 
        case GL_R8_SNORM        : return "";
        
        case GL_RGBA32UI        : 
        case GL_RGBA16UI        : 
        case GL_RGB10_A2UI      : 
        case GL_RGBA8UI         : 
        case GL_RG32UI          : 
        case GL_RG16UI          : 
        case GL_RG8UI           : 
        case GL_R32UI           : 
        case GL_R16UI           : 
        case GL_R8UI            : return "u";
        
        case GL_RGBA32I         : 
        case GL_RGBA16I         : 
        case GL_RGBA8I          : 
        case GL_RG32I           : 
        case GL_RG16I           : 
        case GL_RG8I            : 
        case GL_R32I            : 
        case GL_R16I            : 
        case GL_R8I             : return "i";        
    }    
}

#endif
