#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"

class Triangle {
    public:
        Vertex vertex1;
        Vertex vertex2;
        Vertex vertex3;

        float area;

        Triangle(Vertex v1, Vertex v2, Vertex v3);

    private:
        static int integerID;

        void calculateRGBID();

        float calculateArea();
};

#endif