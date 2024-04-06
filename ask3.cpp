#include "ask3.h"



CoordinatesXY calculate_centerMass(Planet p1, Planet p2){
    long double mass = p1.getMass() + p2.getMass();
    long double x = (p1.getX()*p1.getMass() + p2.getX()*p2.getMass()) / mass;
    long double y = (p1.getY()*p1.getMass() + p2.getY()*p2.getMass()) / mass;
    return CoordinatesXY(x,y);
}

long double calculate_gravity(Planet p1, Planet p2){
    long double gravity =1;

    return gravity;
}

long double netForce(Planet planet, BHTree tree){
    long double netforce =1;
    // start from root

    // if distance bettween

    return netforce;
}





























void barnes_hut(){
    
}



// ********************** MAIN ************************
int main(int argc, char* argv[]) {
    if(argc <2){
        std::cerr<<"Usage: ./a.out"<<argv[0]<< "<filename> <iterations_num>"<<std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if(!file){
        std::cerr << "Failed to open file "<<argv[1] << std::endl;
        return 1;
    }

    long double numPlanets;
    long double universeSize;
    long double i;

    long double x, y, velocityX, velocityY, mass;
    std::string name;

    file >> numPlanets >> universeSize;
    std::cout<< "Number of planets: " << numPlanets << std::endl;
    std::cout<< "Universe size: " << universeSize << std::endl;
    std::vector<Planet> planets;
    for(int i=0; i<numPlanets; i++){
        file >> x >> y>> velocityX >> velocityY >> mass >> name;
        Planet planet(x,y,velocityX,velocityY,mass,name);
        planets.push_back(planet);
    }
    for (auto& planet : planets) {
        std::cout << "Name: " << planet.getName() << ", "
                  << "Position: (" << planet.getX() << ", " << planet.getY() << "), "
                  << "Velocity: (" << planet.getVelocityX() << ", " << planet.getVelocityY() << "), "
                  << "Mass: " << planet.getMass() << std::endl;
    }

    
    return 0;
}

