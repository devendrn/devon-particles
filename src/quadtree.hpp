#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <vector>

// this is not an actual quadtree implementation
// super bad code 

// might be better to use just point id
// and reference the position from array
struct Point
{
    float x,y;
    unsigned int n;  // point reference index
};

// boudary - might be better if this was varibles within class?
struct Bound
{
    float x,y;  // position
    float r;    // half width
};

class Quadtree
{
private:
    Quadtree *childQuad1;
    Quadtree *childQuad2;
    Quadtree *childQuad3;
    Quadtree *childQuad4;
    Point *points;
    unsigned int points_size;
    const unsigned int capacity;
    bool has_child = false;
    Bound quad;

public:
    std::vector<float> *lines;

    Quadtree(Bound quad, int capacity, std::vector<float> *line): quad(quad), capacity(capacity)
    {
        points = new Point[capacity];
        points_size = 0;
        lines = line;
    };

    ~Quadtree()
    {
        delete[] points;
    }

    void subdivide()
    {
        // for draw stuff 
        // two mid cross lines (2d)
        lines->insert(lines->end(), {
                //   x          y     z
                quad.x-quad.r,quad.y,0.0f,
                quad.x+quad.r,quad.y,0.0f,
                quad.x,quad.y-quad.r,0.0f,
                quad.x,quad.y+quad.r,0.0f
            });

        float new_r = quad.r/2;
        childQuad1 = new Quadtree({quad.x + new_r,quad.y + new_r, new_r},capacity,lines);
        childQuad2 = new Quadtree({quad.x - new_r,quad.y + new_r, new_r},capacity,lines);
        childQuad3 = new Quadtree({quad.x - new_r,quad.y - new_r, new_r},capacity,lines);
        childQuad4 = new Quadtree({quad.x + new_r,quad.y - new_r, new_r},capacity,lines);
        has_child = true;
    }

    bool insert(float x, float y, unsigned int n)
    {
        // dont insert if point is outside quad boundary
        if (x<quad.x-quad.r || x>=quad.x+quad.r ||
            y<quad.y-quad.r || y>=quad.y+quad.r)
            return false;

        // subdivide if quad is filled
        // note: there will be points inside this quad
        // we are not going to move those points into its
        // child quads when subdivided
        if (points_size < capacity)
        {
            // add the point into current quad
            points[points_size] = {x,y,n};
            points_size++;
            return true;
        }
        else 
        {
            if (!has_child)
                subdivide();

            if(childQuad1->insert(x,y,n)) return true;
            if(childQuad2->insert(x,y,n)) return true;
            if(childQuad3->insert(x,y,n)) return true;
            childQuad4->insert(x,y,n);  // no need to check as this will be true if all above fails
            return true;
        }
    }
    
    // query all points in a given region
    void query(float x, float y, float r, std::vector<unsigned int> *found)
    {
        // check if region bound is outside quad bound
        // stop traversing when its true 
        if (x+r<quad.x-quad.r || x-r>quad.x+quad.r ||
            y+r<quad.y-quad.r || y-r>quad.y+quad.r)
            return;

        // region intersects with quad
        // check all points withing quad
        // push them to found if they are inside region boundary
        for (unsigned char i=0; i<points_size; i++)
        {
            if (x-r<points[i].x && points[i].x<x+r &&
                y-r<points[i].y && points[i].y<y+r)
                found->push_back(points[i].n);
        }

        // continue traversing through all child quads if children exists
        if (has_child)
        {
            childQuad1->query(x,y,r,found);
            childQuad2->query(x,y,r,found);
            childQuad3->query(x,y,r,found);
            childQuad4->query(x,y,r,found);
        }
    }

};

#endif
