#ifndef CONST_H
#define CONST_H

namespace GLBuffer
{    
    const int Vertex = 0;
    const int Normal = 1;
    const int UV = 2;
    const int TAIndex = 3;
    const int Color = 4;
    const int Index = 5;
    const int Index2 = 6;
    const int Count = 7;    
    const int SubMesh = 7;        
}

namespace GLUniBuffer
{
    const int App = 0;
    const int Frame = 1;
    const int Material = 2;
    const int Model = 3;
    const int Transient = 4;
    const int Compute = 5;
    const int Count = 6;
}

namespace GLData
{
    const int Attribute = 0;
    const int Index = 1;
    const int Instance = 2;
    const int Command = 3;
}

enum Alignment
{
    Left = 1,
    Right = 2,
    Top = 4,
    Bottom = 8,
    CenteredVertical = 16,
    CenteredHorizontal = 32
};

#endif
