#ifndef ASK3_H
#define ASK3_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include "tbb/tbb.h"



struct timespec start;
double elapsed;
const long double G=6.673e-11;
const  double theta=0.5;
int N;

class CoordinatesXY{
    public:
        long double x;
        long double y;
    CoordinatesXY(long double x, long double y):x(x),y(y){}
    CoordinatesXY():x(0),y(0){}

    bool operator==(const CoordinatesXY& other){
        return (x == other.x) && (y == other.y);
    }
};

class Planet {
private:
    CoordinatesXY xy;
    long double velocityX;
    long double velocityY;
    long double mass;
    int exploded;
    std::string name;

public:
    // Constructor
    Planet(long double x, long double y, long double velocityX, long double velocityY, long double mass, const std::string& name) 
        : x(x), y(y), velocityX(velocityX), velocityY(velocityY), mass(mass), name(name) {}

    // Default constructor
    Planet() : x(0), y(0), velocityX(0), velocityY(0), mass(0), name("") {}

    // Getter functions
    CoordinatesXY getXY() { return xy; }
    long long double getX() { return xy.x; }
    long long double getY() { return xy.y; }
    long double getVelocityX() { return velocityX; }
    long double getVelocityY() { return velocityY; }
    long double getMass() { return mass; }
    int hasExploed() { return exploded; }
    std::string getName() { return name; }

    // Setter functions
    void setXY(CoordinatesXY xy) {this.xy = xy;}
    void setVelocityX(long double velocityX) { this->velocityX = velocityX; }
    void setVelocityY(long double velocityY) { this->velocityY = velocityY; }
    void setMass(long double mass) { this->mass = mass; }
    void setName(const std::string& name) { this->name = name; } // wonder if this can cause a bug

    // Other
    void initiateExplostion(){
        // extreme case
        std::cout << "Planet with coordinates ("<< CoordinatesXY.x << " , " << CoordinatesXY.y <<") has exploded." << std::endl;
        exploded = 1;
        return;
    }
    


};

class BHNode{
    public:
        // Planet planet;
        std::vector<Planet> planets;
        // Note: can be empty
        BHNode *nw, *ne, *sw, *se; // Pointers to the four quadrants (other planets)

        BHNode(Planet planet):planet(planet),nw(NULL),ne(NULL),sw(NULL),se(NULL),exploded(0){}
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

    int BHTree_insert(Planet planet){
        BHNode node = new BHNode(planet);
        if(!root){
            root = node;
            return;
        }

        return 1;
    }
    int BHTree_length(){
        return 1;
    }
};


class Square // universe
{
public:
    long long int id;
    CoordinatesXY xy;
    std::vector<Planet>& planets;
    std::vector<long long int> planetIndexes; // ideally pointer
    Square *nw, *ne, *sw, *se;
    
    void buildTree(){
        if(planetIndexes <2)return;

        std::vector<long long int> nwPlanets, nePlanets, swPlanets, sePlanets;
        for (size_t i = 0; i < planetIndexes.size(); i++) {
            const Planet planet = (*planets)[i]; 

            if (planet.getXY() == xy) {
                planet.initiateExplostion();
                continue;
            }

            if (planet.getX() < xy.x && planet.getY() > xy.y) {
                nwPlanets.push_back(i); 
            } else if (planet.getX() > xy.x && planet.getY() > xy.y) {
                nePlanets.push_back(i); 
            } else if (planet.getX() < xy.x && planet.getY() < xy.y) {
                swPlanets.push_back(i); 
            } else {
                sePlanets.push_back(i); 
            }

            nw = new Square(planets,CoordinatesXY(-xy.x/2 , xy.y/2),nwPlanets);
            ne = new Square(planets,CoordinatesXY(xy.x/2 , xy.y/2),nePlanets);            
            sw = new Square(planets,CoordinatesXY(-xy.x/2 , -xy.y/2),swPlanets);
            se = new Square(planets,CoordinatesXY(xy.x/2 , -xy.y/2),sePlanets);
            
        }
    }
    
    Square( std::vector<Planet>& planets,CoordinatesXY xy):planets(planets),xy(xy){
        int i;
        for (i = 0; i < planets->size(); i++){
            planetIndexes.push_back(i);
        }
        buildTree();
    }
    Square( std::vector<Planet>& planets,CoordinatesXY xy,std::vector<long long int> planetIndexes):planets(planets),xy(xy),planetIndexes(planetIndexes){
        buildTree();
    }

};

class Universe{
    std::vector<Planet> planets;
    Square *root;

    Universe(std::vector<Planet> planets):planets(planets){
        root = new Square(&planets,CoordinatesXY(0,0));
    }
}

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