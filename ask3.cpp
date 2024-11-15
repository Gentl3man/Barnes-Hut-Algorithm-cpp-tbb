#include "ask3.h"

CoordinatesXY calculate_centerMass(Planet p1, Planet p2)
{
    long double mass = p1.getMass() + p2.getMass();
    long double x = (p1.getX() * p1.getMass() + p2.getX() * p2.getMass()) / mass;
    long double y = (p1.getY() * p1.getMass() + p2.getY() * p2.getMass()) / mass;
    return CoordinatesXY(x, y);
}

long double calculate_gravity(long double m1, long double m2, long double distance)
{
    const long double G = 6.673e-11;
    return (G * m1 * m2) / (distance * distance);
}
long double calculate_distance(CoordinatesXY xy1, CoordinatesXY xy2)
{
    long double dx = xy2.x - xy1.x;
    long double dy = xy2.y - xy1.y;
    return sqrt(dx * dx + dy * dy);
}
bool node_contains_planet(Planet planet, Square *root)
{
    int i;
    // Could be a better way to do this with parents
    for (auto index : root->planetIndexes)
    {
        if (planet == root->planets->at(index))
            return 1;
    }
    return 0;
}

NetForce netForce(Planet planet, Square *node)
{
    if (!node || planet.hasExploed())
        return NetForce(0, 0); // node null, no children aka no gravity
    long double distance = calculate_distance(planet.getXY(), node->centerMass);
    long double gravity;
    NetForce ret = NetForce(0, 0);
    bool isSufficientlyFar = distance > planet.getSquare()->size / 2.0; 
    bool hasPlanet = node_contains_planet(planet, node);
    if (isSufficientlyFar && !hasPlanet)
    {
        // calculate the gravity of all bodies in the node
        // center of mass being the node's center of mass
        gravity = calculate_gravity(planet.getMass(), node->mass, distance);
        ret.fx = (gravity * (node->xy.x - planet.getX())) / distance;
        ret.fy = (gravity * (node->xy.y - planet.getY())) / distance;
    }
    else if (!isSufficientlyFar || hasPlanet)
    {
        // recursivly calculate gravity the children exert at the body
        if (node->ne)
            ret += netForce(planet, node->ne);
        if (node->nw)
            ret += netForce(planet, node->nw);
        if (node->se)
            ret += netForce(planet, node->se);
        if (node->sw)
            ret += netForce(planet, node->sw);
        // sunhstamenh twn  dynamewn einai to a8roisma twn dunamewn
    }
    return ret;
}

// ********************** MAIN ************************
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << "<filename> <iterations_num> <thread_num>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "Failed to open file " << argv[1] << std::endl;
        return 1;
    }

    long long iterations = std::stoll(argv[2]); // 'seconds' the simmulation will run
    int thread_num = std::stoll(argv[3]);
    long double numPlanets;
    long double universeSize;
    long double R; // aktina tou sympantos
    long double i;
    long double x, y, velocityX, velocityY, mass;
    std::string name;
    file >> numPlanets >> R;
    universeSize = (R * 2);
    // std::cout << "Number of planets: " << numPlanets << std::endl;
    // std::cout << "Universe size: " << universeSize << std::endl;
    std::vector<Planet> planets;
    for (int i = 0; i < numPlanets; i++)
    {
        file >> x >> y >> velocityX >> velocityY >> mass >> name;
        Planet planet(CoordinatesXY(x, y), velocityX, velocityY, mass, name);
        planets.push_back(planet);
    }
    // for (auto &planet : planets)
    // {
    //     std::cout << "Name: " << planet.getName() << ", "
    //               << "Position: (" << planet.getX() << ", " << planet.getY() << "), "
    //               << "Velocity: (" << planet.getVelocityX() << ", " << planet.getVelocityY() << "), "
    //               << "Mass: " << planet.getMass() << std::endl;
    // }

    tbb::task_arena arena(thread_num);
    elapsed = 0;
    long double iterationEllapsed = 0;

    for (i = 0; i < iterations; i++)
    {
        // std::cout << "Creating tree: " << i << std::endl;
        Square universe = Square(&planets, CoordinatesXY(0, 0), universeSize); // hide all the complexity
        clock_gettime(CLOCK_MONOTONIC, &start);
        arena.execute([&]
                      { tbb::parallel_for(
                            tbb::blocked_range<int>(0, planets.size()),
                            [&](const tbb::blocked_range<int> &range) -> void
                            {
                                int j;
                                NetForce force;
                                for (j = range.begin(); j != range.end(); j++)
                                {
                                    // std::cout << "j: " << j << "\n";
                                    force = netForce(planets[j], &universe);
                                    planets[j].setVelocityX(force.fx / planets[j].getMass());
                                    planets[j].setVelocityY(force.fy / planets[j].getMass());
                                    // std::cout << "Force: " << force.fy << " \n";
                                }
                            }); });
        arena.execute([&]
                      { tbb::parallel_for(
                            tbb::blocked_range<int>(0, planets.size()),
                            [&](const tbb::blocked_range<int> &range) -> void
                            {
                                int j;
                                for (j = range.begin(); j != range.end(); j++)
                                {
                                    // std::cout<<"moving planet: "<< j << " to different position \n";
                                    planets[j].gotoNextPosition();
                                }
                            }); });
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsed += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
        // for (auto &planet : planets)
        // {
        //     std::cout << "Name: " << planet.getName() << ", "
        //               << "Position: (" << planet.getX() << ", " << planet.getY() << "), "
        //               << "Velocity: (" << planet.getVelocityX() << ", " << planet.getVelocityY() << "), "
        //               << "Mass: " << planet.getMass() << std::endl;
        // }
    }

    std::ofstream output;
    output.open("output.txt");
    output << numPlanets << std::endl;
    output << universeSize << std::endl;
    for (auto &planet : planets)
    {
        output << planet.getX() << " " << planet.getY() << " " << planet.getVelocityX() << " " << planet.getVelocityY() << " " << planet.getMass() << " " << planet.getName() << std :: endl;
    }
    output.close();
    std::cout << "NetForce time calculation " << elapsed << "\n";
    return 0;
}
