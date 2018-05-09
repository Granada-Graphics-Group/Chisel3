#include "sphere.h"

Sphere::Sphere(std::string name, float radius, int resolution)
:
    Mesh(name),
    mRadius(radius),
    mResolution(resolution)
{
    //TODO: Finish the implementation
    
    // vectors to hold our data
    // vertice positions
    std::vector<glm::vec3> v;
    // texture map
    std::vector<glm::vec2> t;
    // normals
    std::vector<glm::vec3> n;
    // iniatiate the variable we are going to use
    float X1,Y1,X2,Y2,Z1,Z2;
    float inc1,inc2,inc3,inc4,inc5,radius1,radius2;

    for(int w = 0; w < resolution; w++) 
    {
        for(int h = (-resolution/2); h < (resolution/2); h++) 
        {


            inc1 = (w/(float)resolution) * 2 * glm::pi<double>();
            inc2 = ((w+1)/(float)resolution)* 2 * glm::pi<double>();
            inc3 = (h/(float)resolution) * glm::pi<double>();
            inc4 = ((h+1)/(float)resolution) * glm::pi<double>();

            X1 = sin(inc1);
            Y1 = cos(inc1);
            X2 = sin(inc2);
            Y2 = cos(inc2);

            // store the upper and lower radius, remember everything is going to be drawn as triangles
            radius1 = radius*cos(inc3);
            radius2 = radius*cos(inc4);


            Z1 = radius*sin(inc3);
            Z2 = radius*sin(inc4);

            // insert the triangle coordinates
            v.push_back(glm::vec3(radius1*X1,Z1,radius1*Y1));
            v.push_back(glm::vec3(radius1*X2,Z1,radius1*Y2));
            v.push_back(glm::vec3(radius2*X2,Z2,radius2*Y2));

            v.push_back(glm::vec3(radius1*X1,Z1,radius1*Y1));
            v.push_back(glm::vec3(radius2*X2,Z2,radius2*Y2));
            v.push_back(glm::vec3(radius2*X1,Z2,radius2*Y1));


            // insert the texture map, im using this to draw texture out from a texture atlas,
            // so you probably want to write your own algorithm for this!
//             t.push_back(glm::vec2(0.6666f, 0.5f)*glm::vec2(1,0.25f)+glm::vec2(0,0.25f*Type));
//             t.push_back(glm::vec2(0.666f, 1.0f)*glm::vec2(1,0.25f)+glm::vec2(0,0.25f*Type));
//             t.push_back(glm::vec2(1.0f, 1.0f)*glm::vec2(1,0.25f)+glm::vec2(0,0.25f*Type));
// 
//             t.push_back(glm::vec2(0.6666f, 0.5f)*glm::vec2(1,0.25f)+glm::vec2(0,0.25f*Type));
//             t.push_back(glm::vec2(1.00f, 1.0f)*glm::vec2(1,0.25f)+glm::vec2(0,0.25f*Type));
//             t.push_back(glm::vec2(1.00f, 0.5f)*glm::vec2(1,0.25f)+glm::vec2(0,0.25f*Type));

            // insert the normal data
            n.push_back(glm::vec3(X1,Z1,Y1)/ glm::length(glm::vec3(X1,Z1,Y1)));
            n.push_back(glm::vec3(X2,Z1,Y2)/ glm::length(glm::vec3(X2,Z1,Y2)));
            n.push_back(glm::vec3(X2,Z2,Y2)/ glm::length(glm::vec3(X2,Z2,Y2)));
            n.push_back(glm::vec3(X1,Z1,Y1)/ glm::length(glm::vec3(X1,Z1,Y1)));
            n.push_back(glm::vec3(X2,Z2,Y2)/ glm::length(glm::vec3(X2,Z2,Y2)));
            n.push_back(glm::vec3(X1,Z2,Y1)/ glm::length(glm::vec3(X1,Z2,Y1)));
        }
    }

/*
    // finally, create the buffers and bind the data to them
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    indexVBO(v, t, n, indices, indexed_vertices, indexed_uvs, indexed_normals);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

    // store the number of indices for later use
    size = indices.size();

    // clean up after us
    indexed_normals.clear();
    indexed_uvs.clear();
    indexed_vertices.clear();
    indices.clear();*/
    n.clear();
    v.clear();
    t.clear();    
}

