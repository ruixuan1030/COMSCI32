#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
class StudentWorld;


/////////////////////////////////Base class: Actor/////////////////////////////////////////////
class Actor : public GraphObject
{
    public:
        Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth,
              int speed, int horispeed, bool collision, StudentWorld* world, bool alive);
        virtual void doSomething() = 0;
        int getspeed() const;
        int get_hspeed() const;
        bool isalive() const; //check if the actor is alive(need to be removed)
        StudentWorld* get_world() const; //get this actor's world
        void set_vspeed(int vspeed);  //set vertical speed
        void set_hspeed(int hspeed);  //set horizontal speed 
        void set_alive(bool alive); //mark this actor alive/dead
        virtual bool is_white_line() const; //for border class
        bool isoverlap() const; //check if it's overlapped with GR
    
          // If this actor is affected by holy water projectiles, then inflict that
          // affect on it and return true; otherwise, return false.
        virtual bool isSprayWorthy();
        virtual void decreaseHitPoint();

          // Does this object affect zombie cab placement and speed?
        virtual bool isCollisionWorthy() const;
    private:
        int v_speed; //vertical speed
        bool m_alive;
        int h_speed; //horizontal speed
        bool Iscolworthyactor;
        StudentWorld* this_world;
};

//////////////////////////////////BorderLine /////////////////////////////////////////
class BorderLine : public Actor
{
    public:
        BorderLine(int imageID, double startX, double startY, StudentWorld* world, bool white);
        virtual void doSomething();
        virtual bool is_white_line() const;
    private:
        bool is_white;
};

//////////////////////////////////////GhostRacer/////////////////////////////////////
class GhostRacer : public Actor
{
    public:
        GhostRacer(StudentWorld* world);
        virtual void doSomething();
        int get_health() const;
        int get_sprays() const;
        void set_health(int health);
        void set_sprays(int spray);
        void spin();
    private:
        int spraycounts; //projectiles count
        int m_health;
        void move();
};

//////////////////////////////Pedestrian/////////////////////////////////////
class Pedestrian: public Actor
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double size, int move);
    int getHitpt() const;
    void setHitpt(int pt);
    virtual bool isSprayWorthy();
    virtual void doSomething() = 0;
    int getMoveToPlanDist() const;
    void setPlanDist(int dist);
private:
    int plandist;
    int hitpt;
};

///human pedestrian
class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, int dist);
    virtual void doSomething();
    virtual void decreaseHitPoint();
};

///zombie pedestrian
class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, int dist);
    virtual void doSomething();
    virtual void decreaseHitPoint();
  //  virtual bool isSprayWorthy();
    int getGruntCount() const;
    void setGruntCount(int num);
private:
    int gruntCount;
};

//////////////////////////////////////Oil Slick/////////////////////////////////////
class OilSlick : public Actor
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doSomething();
};

//////////////////////////////////////Zombie Cab////////////////////////////////////
class ZombieCab : public Actor
{
public:
    ZombieCab(StudentWorld* sw, double x, double y, int relativeSpeed);
    virtual void doSomething();
    virtual void decreaseHitPoint();
    virtual bool isSprayWorthy();
private:
    int planlen; //plan length
    int hitpt;  //hit points
    bool isdamge;
};

//////////////////////////////////////Holy Water Projectile////////////////////////////////////
class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir, int relativeSpeed, int relativeHoriSpeed);
    virtual void doSomething();
    int getDist() const;
    void setDist(int dist);
private:
    int maxdist; //max travel distance
};

//////////////////////////////////////Goodies////////////////////////////////////
class Goodies: public Actor
{
public:
    Goodies(StudentWorld* sw, double x, double y, int dir, int imageID, int size);
    virtual void doSomething() = 0;
    
};

//Healing Goodies
class HealingGoodie : public Goodies
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething();
};

//holy water goodies
class HolyWaterGoodie : public Goodies
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
   // virtual bool isSprayWorthy();
  //  virtual void decreaseHitPoint();
    virtual void doSomething();
};

//soul goodies
class SoulGoodie : public Goodies
{
public:
    SoulGoodie(StudentWorld* sw, double x);
    virtual void doSomething();
};


#endif // ACTOR_H_


