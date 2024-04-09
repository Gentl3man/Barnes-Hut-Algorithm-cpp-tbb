#ifndef ASK3_H
#define ASK3_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include "tbb/tbb.h"
#include <math.h>




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
class Square;

class Planet {
private:
    CoordinatesXY xy;
    long double velocityX;
    long double velocityY;
    long double mass;
    int exploded;
    std::string name;
    Square& square; // square the leaf it belongs
public:
    // Constructor
    Planet(long double x, long double y, long double velocityX, long double velocityY, long double mass, const std::string& name) 
        : x(x), y(y), velocityX(velocityX), velocityY(velocityY), mass(mass), name(name) {}

    // Default constructor
    Planet() : x(0), y(0), velocityX(0), velocityY(0), mass(0), name("") {}

    // Getter functions
    CoordinatesXY getXY() { return xy; }
    long double getX() { return xy.x; }
    long double getY() { return xy.y; }
    long double getVelocityX() { return velocityX; }
    long double getVelocityY() { return velocityY; }
    long double getMass() { return mass; }
    int hasExploed() { return exploded; }
    std::string getName() { return name; }
    Square& getSquare() { return square;}

    // Setter functions
    void setXY(CoordinatesXY xy) { this->xy = xy; }
    void setVelocityX(long double velocityX) { this->velocityX = velocityX; }
    void setVelocityY(long double velocityY) { this->velocityY = velocityY; }
    void setMass(long double mass) { this->mass = mass; }
    void setName(const std::string& name) { this->name = name; } // wonder if this can cause a bug
    // void setSquare(Square *square) { this->square = square; }
    // Other
    void initiateExplostion(){
        // extreme case
        std::cout << "Planet with coordinates ("<< CoordinatesXY.x << " , " << CoordinatesXY.y <<") has exploded." << std::endl;
        exploded = 1;
        return;
    }
    


};

class Square // universe
{
public:
    long long int id;
    CoordinatesXY xy;
    long double  size; //TODO add it
    CoordinatesXY centerMass;
    long double mass;
    std::vector<Planet>& planets;
    std::vector<long long int> planetIndexes; // ideally pointer
    long long level;
    Square *nw, *ne, *sw, *se,*parent;

    CoordinatesXY calculate_centerMass(Square *sq1, Square *sq2){
        long double x,y;
        long double mass = sq1->mass + sq2->mass;
        x=(sq1->centerMass.x*sq1->mass + sq2->centerMass.x*sq2->mass)/mass;
        x=(sq1->centerMass.y*sq1->mass + sq2->centerMass.y*sq2->mass)/mass;
        return CoordinatesXY(x,y);
    }
    
    void buildTree(){
        this->mass=0;
        centerMass = xy;
        
        if(planetIndexes.size() <2){
            // assign the planet the square it belongs, aka the leaf
            if(planetIndexes.size() == 1){
                planets[planetIndexes[0]].setSquare(this);
                mass=(planets)[planetIndexes[0]];
            }
            return;
        }
        std::vector<long long int> nwPlanets, nePlanets, swPlanets, sePlanets;
        for (size_t i = 0; i < planetIndexes.size(); i++) {
            const Planet planet = planets[i]; 

            if (planet.getXY() == xy) {
                planet.initiateExplostion();
                continue;
            }
            if(planet.hasExploed())continue;

            if (planet.getX() < xy.x && planet.getY() > xy.y) {
                nwPlanets.push_back(i); 
            } else if (planet.getX() > xy.x && planet.getY() > xy.y) {
                nePlanets.push_back(i); 
            } else if (planet.getX() < xy.x && planet.getY() < xy.y) {
                swPlanets.push_back(i); 
            } else {
                sePlanets.push_back(i); 
            }

            nw = new Square(planets,CoordinatesXY(-xy.x/2 , xy.y/2) ,nwPlanets   ,level+1 ,this);
            mass +=nw->mass;
            centerMass = this.calculate_centerMass(this,nw); // 4 fores giati dinetai o typos mono gia 2 swmata sthn ekfwnhsh....

            ne = new Square(planets,CoordinatesXY(xy.x/2 , xy.y/2)  ,nePlanets   ,level+1 ,this);
            mass +=ne->mass;
            centerMass = this.calculate_centerMass(this,ne);

            sw = new Square(planets,CoordinatesXY(-xy.x/2 , -xy.y/2),swPlanets   ,level+1 ,this);
            mass +=sw->mass;
            centerMass = this.calculate_centerMass(this,sw);

            se = new Square(planets,CoordinatesXY(xy.x/2 , -xy.y/2) ,sePlanets   ,level+1 ,this);
            mass +=se->mass;
            centerMass = this.calculate_centerMass(this,se);

        }
    }
    
    Square( std::vector<Planet>& planets,CoordinatesXY xy):planets(planets),xy(xy),level(0),parent(NULL){
        int i;
        for (i = 0; i < planets->size(); i++){
            planetIndexes.push_back(i);
        }
        buildTree();
    }
    Square( std::vector<Planet>& planets,CoordinatesXY xy,std::vector<long long int> planetIndexes,long long int level,Square *parent)
        :planets(planets),xy(xy),planetIndexes(planetIndexes),level(level),parent(parent){
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
 * Calculate the gravity
*/
long double calculate_gravity(Planet p1, Planet p2);

void parseInput(char **input);

#endif