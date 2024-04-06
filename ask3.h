#ifndef ASK3_H
#define ASK3_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "tbb/tbb.h"


struct timespec start;
double elapsed;
const long double G=6.673e-11;
const  double theta=0.5;
int N,threads_num;

class CoordinatesXY{
    public:
        long double x;
        long double y;
    CoordinatesXY(long double x, long double y):x(x),y(y){}
    CoordinatesXY():x(0),y(0){}
};

class Planet {
private:
    long double x;
    long double y;
    long double velocityX;
    long double velocityY;
    long double mass;
    std::string name;

public:
    // Constructor
    Planet(long double x, long double y, long double velocityX, long double velocityY, long double mass, const std::string& name) 
        : x(x), y(y), velocityX(velocityX), velocityY(velocityY), mass(mass), name(name) {}

    // Default constructor
    Planet() : x(0), y(0), velocityX(0), velocityY(0), mass(0), name("") {}

    // Getter functions
    long double getX() { return x; }
    long double getY() { return y; }
    long double getVelocityX() { return velocityX; }
    long double getVelocityY() { return velocityY; }
    long double getMass() { return mass; }
    std::string getName() { return name; }

    // Setter functions
    void setX(long double x) { this->x = x; }
    void setY(long double y) { this->y = y; }
    void setVelocityX(long double velocityX) { this->velocityX = velocityX; }
    void setVelocityY(long double velocityY) { this->velocityY = velocityY; }
    void setMass(long double mass) { this->mass = mass; }
    void setName(const std::string& name) { this->name = name; } // wonder if this can cause a bug
};

class BHNode{
    public:
        Planet planet;

        // Note: can be empty
        BHNode *nw, *ne, *sw, *se; // Pointers to the four quadrants (other planets)

        BHNode(Planet planet):planet(planet),nw(NULL),ne(NULL),sw(NULL),se(NULL){}
        ~BHNode() {
            delete nw;
            delete ne;
            delete sw;
            delete se;
        }
};

class BHTree{
    public:
    BHNode* root;

    BHTree(BHNode root):root(root){}
    ~BHTree(){
        delete root;
    }

    int BHTree_insert(){
        return 1;
    }
    int BHTree_length(){
        return 1;
    }
};


class Universe
{
public:
    long double size;
    BHTree* tree; // THE ROOT OF THE UNIVERSE !!!
};

long double netForce(Planet planet, BHTree tree){
        return 1;
}



/**
 * New position of the planet? Man...
*/
CoordinatesXY newPosition(Planet p, CoordinatesXY xy);


// must be recursive, divide into 4 pieces and each piece divide
// into 4, untill there are 0 or 1 bodies left
void barnes_hut();

/**
 * Calculate the center mass of 2 planets
*/
CoordinatesXY calculate_centerMass(Planet p1, Planet p2);

/**
 * Calculate the gravity
*/
long double calculate_gravity(Planet p1, Planet p2);

void parseInput(char **input);

#endif