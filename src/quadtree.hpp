#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <vector>

// my horrible quadtree implementation

template <typename T>
class Quadtree
{
private:
    Quadtree *childQuad;
    unsigned int *points;   // particle id
    unsigned char points_size;   // no of particles in a quad
    unsigned char capacity;    // max allowed points index + 1 (0 means 1 point)
    bool has_child;

    // quadtree bounds
    float bound_r;    // half width (assuming quad is always square)
    float bound_x;    // position of center
    float bound_y;

    // pointer to quad lines vector
    // for drawing 
    std::vector<float> *lines;

    // empty constructor
    // shouldnt be used outside
    Quadtree(){}
public:

    // constructor
    Quadtree(float bound_x, float bound_y, float bound_r, const unsigned char capacity, std::vector<float> *line):
        bound_x(bound_x), bound_y(bound_y), bound_r(bound_r), capacity(capacity), points_size(0), has_child(false), lines(line)
    {
        points = new unsigned int[capacity];
    }
    
    // destructor
    ~Quadtree()
    {
        // !! make sure to delete all memory allocated by new !!
        delete[] points;
        if(has_child)
            delete[] childQuad;
    }

    void subdivide()
    {
        // for draw stuff 
        // two mid cross lines (2d)
        //    | 
        // ---|---
        //    |
        lines->insert(lines->end(), {
                //     x            y      z
                bound_x-bound_r, bound_y, 0.0f,
                bound_x+bound_r, bound_y, 0.0f,
                bound_x, bound_y-bound_r, 0.0f,
                bound_x, bound_y+bound_r, 0.0f
            });

        // create 4 child quads with new bound size and positions
        // + = child origin, o = parent origin
        //   ---y--- 
        //  | + | + |
        //  |---o---x
        //  | + | + |
        //   ------- 
        float new_r = bound_r * 0.5;
        childQuad = new Quadtree[4];    // allocate space for 4 quadtree objects
        new (&childQuad[0]) Quadtree(bound_x+new_r, bound_y+new_r, new_r, capacity, lines);
        new (&childQuad[1]) Quadtree(bound_x-new_r, bound_y+new_r, new_r, capacity, lines);
        new (&childQuad[2]) Quadtree(bound_x-new_r, bound_y-new_r, new_r, capacity, lines);
        new (&childQuad[3]) Quadtree(bound_x+new_r, bound_y-new_r, new_r, capacity, lines);
        has_child = true;
    }

    void insertIntoChildren(float x, float y, unsigned int n)
    {
        if(childQuad[0].insert(x, y, n)) return;
        if(childQuad[1].insert(x, y, n)) return;
        if(childQuad[2].insert(x, y, n)) return;
        childQuad[3].insert(x, y, n);  // no need to check as this will be true if all above fails
    }

    bool insert(float x, float y, unsigned int n)
    {
        // dont insert if point is outside quad boundary
        if (x<bound_x-bound_r || x>=bound_x+bound_r ||
            y<bound_y-bound_r || y>=bound_y+bound_r)
            return false;

        // subdivide if quad is filled
        // note: there will be points inside this quad
        // we are not going to move those points into its
        // child quads when subdivided
        if (points_size < capacity)
        {
            // add the point into current quad
            points[points_size] = n;
            points_size++;
            return true;
        }
        else 
        {
            if (!has_child)
            {
                subdivide();

                // insert all points in parent to children
                //for (unsigned char i=0; i<points_size; i++)
                //    insertIntoChildren(x, y, n);
            }

            // try inserting the point into children
            insertIntoChildren(x, y, n);
            return true;
        }
    }

    // traverse through the tree based on bound condition 
    // do callback for all points points in quad
    void traverse(T *obj,bool (T::*boundCondition)(float x, float y, float r), void (T::*callback)(unsigned int id))
    {
        // check if region bound is outside quad bound
        // stop traversing when its true 
        if (!((obj->*boundCondition)(bound_x, bound_y, bound_r)))
            return;

        // region intersects with quad
        // do callback for all points in quad
        // this has to be done even if the quad has children because
        // points in quad are not inserted into children during subdivision
        for (unsigned char i=0; i<points_size; i++)
            (obj->*callback)(points[i]);

        // continue traversing through all child quads if children exists
        if (has_child)
        {
            childQuad[0].traverse(obj, boundCondition, callback);
            childQuad[1].traverse(obj, boundCondition, callback);
            childQuad[2].traverse(obj, boundCondition, callback);
            childQuad[3].traverse(obj, boundCondition, callback);
        }
    }
};

#endif
