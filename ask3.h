#ifndef ASK3_H
#define ASK3_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <cmath>
#include <tbb/parallel_for.h>

struct timespec start;
double elapsed;
const double theta = 0.5;
int N;

class CoordinatesXY
{
public:
    long double x;
    long double y;
    CoordinatesXY(long double x, long double y) : x(x), y(y) {}
    CoordinatesXY() : x(0), y(0) {}

    bool operator==(const CoordinatesXY &other)
    {
        return (x == other.x) && (y == other.y);
    }
};
class Square;
class Planet;

class NetForce
{
public:
    long double fx;
    long double fy;
    NetForce(long double fx, long double fy) : fx(fx), fy(fy) {}
    NetForce() : fx(0), fy(0){};
    NetForce &operator+=(const NetForce &other)
    {
        fx += other.fx;
        fy += other.fy;
        return *this;
    }
};

class Planet
{
private:
    CoordinatesXY xy;
    long double velocityX;
    long double velocityY;
    long double mass;
    int exploded;
    std::string name;
    Square *square = nullptr; // square the leaf it belongs
public:
    // Constructor
    Planet(CoordinatesXY xy, long double velocityX, long double velocityY, long double mass, const std::string &name)
        : xy(xy), velocityX(velocityX), velocityY(velocityY), mass(mass), name(name), exploded(false) {}

    // Default constructor
    Planet() : xy(CoordinatesXY(0, 0)), velocityX(0), velocityY(0), mass(0), name(""), exploded(false) {}

    // operator overloads
    bool operator==(const Planet &other)
    {
        return xy == other.xy && velocityX == other.velocityX && velocityY == other.velocityY && name == other.name;
    }

    // Getter functions
    CoordinatesXY getXY() { return xy; }
    long double getX() { return xy.x; }
    long double getY() { return xy.y; }
    long double getVelocityX() { return velocityX; }
    long double getVelocityY() { return velocityY; }
    long double getMass() { return mass; }
    int hasExploed() { return exploded; }
    std::string getName() { return name; }
    Square *getSquare() { return square; }

    // Setter functions
    void setXY(CoordinatesXY xy) { this->xy = xy; }
    void setVelocityX(long double velocityX) { this->velocityX = velocityX; }
    void setVelocityY(long double velocityY) { this->velocityY = velocityY; }
    void setMass(long double mass) { this->mass = mass; }
    void setName(const std::string &name) { this->name = name; } // wonder if this can cause a bug
    void setSquare(Square *square) { this->square = square; }
    // Other
    void initiateExplostion()
    {
        // extreme case
        std::cout << "Planet with coordinates (" << xy.x << " , " << xy.y << ") has exploded." << std::endl;
        exploded = 1;
        return;
    }

    void gotoNextPosition()
    {
        const int dt = 1;
        long double vx = xy.x + dt * velocityX;
        long double vy = xy.y + dt * velocityY;
        xy = CoordinatesXY(xy.x + dt * vx, xy.y + dt * vy);
        return;
    }
};

class Square // universe
{
public:
    long double size; // R*2
    long long int id;
    CoordinatesXY xy;
    CoordinatesXY centerMass;
    long double mass;
    std::vector<Planet> *planets;
    std::vector<long long int> planetIndexes; // ideally pointer
    long long level;
    Square *nw, *ne, *sw, *se, *parent;

    CoordinatesXY calculate_centerMass(Square *sq1, Square *sq2)
    {
        std::cout<<"center mass\n";
        long double x = 0, y = 0;
        long double mass = sq1->mass + sq2->mass;
        // std::cout<<"VRHKE MAZA" << mass<< "\n";
        if (mass <= 0)
        {
            return sq1->centerMass;
        }
        x = (sq1->centerMass.x * sq1->mass + sq2->centerMass.x * sq2->mass) / mass;
        y = (sq1->centerMass.y * sq1->mass + sq2->centerMass.y * sq2->mass) / mass;
        std::cout<<"center mass CALCULATED\n";

        return CoordinatesXY(x, y);
    }
    bool planet_in_square(CoordinatesXY planetXY, CoordinatesXY squareXY, long double squareSize)
    {
        return  planetXY.x <= squareXY.x + squareSize / 2.0   &&
                planetXY.x >= squareXY.x - squareSize / 2.0   &&
                planetXY.y <= squareXY.y + squareSize / 2.0   &&
                planetXY.y >= squareXY.y - squareSize / 2.0
        ;
    }
    void buildTree()
    {
        mass = 0;
        centerMass = xy;
        std::cout << "Square: (" << xy.x << " , " << xy.y << ")\n";

        std::cout << "Total planet indexes: " << planetIndexes.size() << "\n";
        if (planetIndexes.size() < 2)
        {
            // assign the planet the square it belongs, aka the leaf
            if (planetIndexes.size() == 1)
            {
                (*planets)[planetIndexes[0]].setSquare(this);
                mass = (*planets)[planetIndexes[0]].getMass();
            }
            return;
        }
        std::vector<long long int> nwPlanets, nePlanets, swPlanets, sePlanets;
        CoordinatesXY nwXY, neXY, swXY, seXY;
        nwXY = CoordinatesXY(-size / 4.0 + xy.x,  size / 4.0 + xy.y);
        neXY = CoordinatesXY( size / 4.0 + xy.x,  size / 4.0 + xy.y);
        swXY = CoordinatesXY(-size / 4.0 + xy.x, -size / 4.0 + xy.y);
        seXY = CoordinatesXY( size / 4.0 + xy.x, -size / 4.0 + xy.y);
        for (long long int index : planetIndexes)
        {
            std::cout << "Planmet index (" << (*planets)[index].getX() << " , " << (*planets)[index].getY() << ") Square: (" << xy.x << " , " << xy.y << ")\n";
            
            if      (planet_in_square((*planets)[index].getXY(), nwXY, size / 2.0))
            {
                nwPlanets.push_back(index);
            }
            else if (planet_in_square((*planets)[index].getXY(), neXY, size / 2.0))
            {
                nePlanets.push_back(index);
            }
            else if (planet_in_square((*planets)[index].getXY(), swXY, size / 2.0))
            {
                swPlanets.push_back(index);
            }
            else
            {
                sePlanets.push_back(index);
            }
        }

        // std::cout<<"size: "<<size<<" side: " << squareSide <<std::endl;
        nw = new Square(planets, nwXY, size / 2.0, &nwPlanets, level + 1, this);
        centerMass = this->calculate_centerMass(this, nw); // 4 fores giati dinetai o typos mono gia 2 swmata sthn ekfwnhsh....
        mass += nw->mass;

        ne = new Square(planets, neXY, size / 2.0, &nePlanets, level + 1, this);
        centerMass = this->calculate_centerMass(this, ne);
        mass += ne->mass;

        sw = new Square(planets, swXY, size / 2.0, &swPlanets, level + 1, this);
        centerMass = this->calculate_centerMass(this, sw);
        mass += sw->mass;

        se = new Square(planets, seXY, size / 2.0, &sePlanets, level + 1, this);
        centerMass = this->calculate_centerMass(this, se);
        mass += se->mass;

        std::cout << "Center mass for Square: "
                  << ": (" << centerMass.x << " , " << centerMass.y << ") " << std::endl;
        return;
    }

    Square(std::vector<Planet> *planets, CoordinatesXY xy, long double size) : planets(planets), xy(xy), size(size), level(0), parent(NULL), centerMass(CoordinatesXY(0, 0)), mass(0)
    {
        int i;
        for (i = 0; i < planets->size(); i++)
        {
            planetIndexes.push_back(i);
        }
        std::cout << "Total planets: " << planetIndexes.size() << "\n";
        std::cout << "Size: " << this->size << "\n";
        buildTree();
    }
    Square(std::vector<Planet> *planets, CoordinatesXY xy, long double size, std::vector<long long int> *planetIndexes, long long int level, Square *parent)
        : planets(planets), xy(xy), size(size), level(level), parent(parent), centerMass(CoordinatesXY(0, 0)), mass(0)
    {
        for (int i = 0; i < planetIndexes->size(); i++)
        {
            // std::cout<<"mpainw\n";
            this->planetIndexes.push_back((*planetIndexes)[i]);
        }
        std::cout << "Square (" << xy.x << " , " << xy.y << ") with size: " << size << " and level: " << level << " Planets: " << this->planetIndexes.size() << "\n";
        buildTree();
    }
};

class Universe
{
    std::vector<Planet> planets;
    Square *root;
    long long int size;

    Universe(std::vector<Planet> planets, long long int size) : planets(planets), size(size)
    {
        root = new Square(&planets, CoordinatesXY(0, 0), size);
    }
};

NetForce netForce(Planet planet, Square *node);

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
long double calculate_gravity(long double m1, long double m2, long double distance);

void parseInput(char **input);

#endif