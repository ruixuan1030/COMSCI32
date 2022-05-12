#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include"GraphObject.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getRacer();
    void recordSoulSaved();
    void addHealingGoodie(double x, double y);
    void addSpray(double x, double y, int dir);
    void addOilSlick(double x, double y);
    Actor* FirstAppropriateSpray(double x, double y, int rad);
    Actor* FirstAppropriateCollisionFront(int lan, double y);
    Actor* FirstAppropriateCollisionBack(int lan, double y);
    bool overlaps(double x1, double y1, double rad1, double x2, double y2, double rad2) const;

private:
    int soulSaved;
    GhostRacer* GR;
    std::list<Actor*> actors;
    std::list<Actor*>::iterator last_white_border();
    Actor* closest_collision(int lan, int delta_y);
    Actor* first_collision(int lane);
    int bonus;
};

#endif // STUDENTWORLD_H_
