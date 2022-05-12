
#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

/////////////////////////////////Base class: Actor/////////////////////////////////////////////
Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, int speed,
    int horispeed, bool collision, StudentWorld* world, bool alive)
    : GraphObject(imageID, startX, startY, dir, size, depth), v_speed(speed),
    this_world(world),m_alive(alive), h_speed(horispeed),Iscolworthyactor(collision)
{}

int Actor::getspeed() const{
     return v_speed;
}

int Actor::get_hspeed() const{
     return h_speed;
}

bool Actor::isalive() const{
     return m_alive;
}

StudentWorld* Actor::get_world() const{
     return this_world;
}

void Actor::set_vspeed(int vspeed){
    v_speed = vspeed;
}

void Actor::set_hspeed(int hspeed){
     h_speed = hspeed;
}

void Actor::set_alive(bool alive){
     m_alive = alive;
}


bool Actor::is_white_line() const{
    return false;
}

void Actor::decreaseHitPoint(){
    return;
}

//algorithm to detect overlapping
bool Actor::isoverlap() const
{
    int delta_x = abs(getX() - get_world()->getRacer()->getX());
    int delta_y = abs(getY() - get_world()->getRacer()->getY());
    int rad1 = getRadius();
    int rad2 = get_world()->getRacer()->getRadius();
    if ((delta_x < (rad1 + rad2) * 0.25) && (delta_y < (rad1 + rad2) * 0.6))
        return true;
    else
        return false;
}


bool Actor::isSprayWorthy(){
    return false;
}

bool Actor::isCollisionWorthy() const{
    return Iscolworthyactor;
}

//////////////////////////////////BorderLine /////////////////////////////////////////
BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* world, bool white)
    : Actor(imageID, startX, startY, 0, 2, 2, -4,0,false,world,true), is_white(white)
{}

void BorderLine::doSomething()
{
    int GRspeed = get_world()->getRacer()->getspeed();
    moveTo(getX(), getY() + getspeed() - GRspeed);
    if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }
}

bool BorderLine::is_white_line() const
{
    return is_white;
}

//////////////////////////////////////GhostRacer/////////////////////////////////////
GhostRacer::GhostRacer(StudentWorld* world)
    :Actor(IID_GHOST_RACER,128,32,90,4,0,0,0,true,world,true),spraycounts(10),m_health(100)
{}

int GhostRacer::get_health() const {
    return m_health;
}

int GhostRacer::get_sprays() const {
    return spraycounts;
}

void GhostRacer::set_health(int health){
    m_health = health;
}

void GhostRacer::set_sprays(int spray){
    spraycounts = spray;
}

//movement algorithm
void GhostRacer::move()
{
    double max_shift_per_tick = 4.0;
    double direction = getDirection();
    static const double PI = 4 * atan(1.0);
    double delta_x = cos(direction*1.0 / 360 * 2 * PI) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::spin(){
    int dir = randInt(1, 2); //choosing clockwise or counter clockwise
    if (dir == 2)
        dir = 1;
    else
        dir = -1;
    int spin_angle = randInt(5, 20);
    setDirection(getDirection()+ dir*spin_angle);
    
    if ((getX() <= ROAD_CENTER - ROAD_WIDTH / 2) && getDirection() > 90){
        //check if it's swerving off the road from the left side
        set_health(get_health() - 10);
        setDirection(82);
        get_world()->playSound(SOUND_VEHICLE_CRASH);
    }

    if ((getX() >= ROAD_CENTER + ROAD_WIDTH / 2) && getDirection() < 90){
        //check if it's swerving off the road from the right side
        set_health(get_health() - 10);
        setDirection(98);
        get_world()->playSound(SOUND_VEHICLE_CRASH);
    }

}

void GhostRacer::doSomething(){
    if (get_health() <= 0)
        set_alive(false);

    if (!isalive())
        return;

    if ((getX() <= ROAD_CENTER - ROAD_WIDTH / 2) && getDirection() > 90){
        //check if it's swerving off the road from the left side
        set_health(get_health() - 10);
        setDirection(82);
        get_world()->playSound(SOUND_VEHICLE_CRASH);
    }

    if ((getX() >= ROAD_CENTER + ROAD_WIDTH / 2) && getDirection() < 90){
        //check if it's swerving off the road from the right side
        set_health(get_health() - 10);
        setDirection(98);
        get_world()->playSound(SOUND_VEHICLE_CRASH);
    }

    //check if the player pressed a key
    int k;
    if ((get_world()->getKey(k))){
        switch (k){
        case KEY_PRESS_SPACE:
            if (get_sprays() <= 0)
                break;
            else{
                static const double PI = 4 * atan(1.0);
                get_world() -> addSpray(SPRITE_HEIGHT * cos(getDirection() * 1.0 / 360 * 2* PI) + getX(),
                                        SPRITE_HEIGHT * sin(getDirection() * 1.0 / 360 * 2* PI) + getY(),
                                        getDirection());
                get_world()->playSound(SOUND_PLAYER_SPRAY);
                set_sprays(get_sprays() - 1);
                break;
            }
        case KEY_PRESS_LEFT:
            if (getDirection() < 114) {
                setDirection(getDirection() + 8);
            }
            break;
        case KEY_PRESS_RIGHT:
            if (getDirection() > 66) {
                setDirection(getDirection() - 8);
            }
            break;
        case KEY_PRESS_UP:
            if (getspeed() < 5) {
                set_vspeed(getspeed() + 1);
            }
            break;
        case KEY_PRESS_DOWN:
            if (getspeed() > -1) {
                set_vspeed(getspeed() - 1);
            }
            break;
        }
    }

    move(); //step5
}


//////////////////////////////Pedestrian/////////////////////////////////////
Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double size, int move)
    :Actor(imageID, x , VIEW_HEIGHT, 0, size, 0, -4, 0, true,sw, true)
    ,plandist(move), hitpt(2)
{}

int Pedestrian::getMoveToPlanDist() const{
    return plandist;
}
void Pedestrian::setPlanDist(int dist){
    plandist = dist;
}

bool Pedestrian::isSprayWorthy(){
    return true;
}

int Pedestrian::getHitpt() const{
    return hitpt;
}

void Pedestrian::setHitpt(int pt){
    hitpt = pt;
}

//////////////////////////////Pedestrian: Human Pedestrian/////////////////////////////////////
HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, int dist)
    :Pedestrian(sw, IID_HUMAN_PED, x, 2.0, dist)
{}

void HumanPedestrian::decreaseHitPoint(){
    set_hspeed(-1*get_hspeed());
    if (getDirection() == 0)
        setDirection(180);
    else
        setDirection(0);
}

void HumanPedestrian::doSomething(){
    if (!isalive())
        return;
    
    if (isoverlap()){
        get_world()->getRacer()->set_alive(false);
        return;
    }
    int v_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int h_speed = get_hspeed();
    int x = getX() + h_speed;
    int y = getY() + v_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }
    setPlanDist(getMoveToPlanDist() - 1);
    if (getMoveToPlanDist() > 0)
        return;
    else{
        int h = randInt(-3, 3);
        while (h == 0)
            h = randInt(-3, 3);
        set_hspeed(h);
        int len = randInt(4, 32);
        setPlanDist(len);

        if (get_hspeed() < 0)
            setDirection(180);
        else
            setDirection(0);
    }

}

//////////////////////////////Pedestrian: Zombie Pedestrian/////////////////////////////////////
ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, int dist)
    :Pedestrian(sw, IID_ZOMBIE_PED, x, 3.0, dist)
    ,gruntCount(0)
{}

/*bool ZombiePedestrian::isSprayWorthy(){
    return true;
}*/

int ZombiePedestrian::getGruntCount() const{
    return gruntCount;
}

void ZombiePedestrian::setGruntCount(int num){
    gruntCount = num;
}

void ZombiePedestrian::decreaseHitPoint(){
    setHitpt(getHitpt()-1);
    if (getHitpt() <= 0){
        set_alive(false);
        get_world()->playSound(SOUND_PED_DIE);
        if (!isoverlap()){
            int i = randInt(1, 5);
            if (i == 5)
                get_world()->addHealingGoodie(getX(), getY());
        }
        get_world()->increaseScore(150);
    }
    else
        get_world()->playSound(SOUND_PED_HURT);
}

void ZombiePedestrian::doSomething(){
    if(!isalive())
        return;
    
    if (isoverlap()){
        get_world()->getRacer()->set_health( get_world()->getRacer()->get_health() - 5);
        set_alive(false);
        get_world()->increaseScore(150);
        get_world()->playSound(SOUND_PED_DIE);
        return;
    }

    int delta_X = abs(getX() - get_world()->getRacer()->getX());
    int Racer_x = get_world()->getRacer()->getX();
    int Racer_y = get_world()->getRacer()->getY();
    if ((delta_X <= 30) && (getY() > Racer_y)){
        setDirection(270);
        if (getX() < Racer_x)
            set_hspeed(1);
        if (getX() > Racer_x)
            set_hspeed(-1);

        setGruntCount(getGruntCount()-1);
        if (getGruntCount() <= 0){
            get_world()->playSound(SOUND_ZOMBIE_ATTACK);
            setGruntCount(20);
        }
    }

    int v_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int h_speed = get_hspeed();
    int x = getX() + h_speed;
    int y = getY() + v_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }

    if (getMoveToPlanDist() > 0){
        setPlanDist(getMoveToPlanDist()-1);
        return;
    }

    int new_hspeed = randInt(-3, 3);
    while (new_hspeed == 0)
        new_hspeed = randInt(-3, 3);
    set_hspeed(new_hspeed);
    setPlanDist(randInt(4, 32));
    if (get_hspeed() < 0)
        setDirection(180);
    else
        setDirection(0);
}

//////////////////////////////////////Oil Slick/////////////////////////////////////
OilSlick::OilSlick(StudentWorld* sw, double x, double y)
    :Actor(IID_OIL_SLICK, x, y, 0, randInt(2, 5), 1, -4, 0, false, sw, true)
{}

void OilSlick::doSomething(){
    int v_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int h_speed = get_hspeed();
    int x = getX() + h_speed;
    int y = getY() + v_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }
    
    if (isoverlap()){
        get_world()->playSound(SOUND_OIL_SLICK);
        get_world()->getRacer()->spin();
    }
}

//////////////////////////////////////Zombie Cab////////////////////////////////////
ZombieCab::ZombieCab(StudentWorld* sw, double x, double y, int relativeSpeed)
    :Actor(IID_ZOMBIE_CAB, x, y, 90, 4.0, 0, relativeSpeed, 0, true, sw, true)
    ,planlen(0), hitpt(3) ,isdamge(false)
{}

bool ZombieCab::isSprayWorthy(){
    return true;
}

void ZombieCab::decreaseHitPoint(){
    hitpt--;
    if (hitpt <= 0){
        set_alive(false);
        get_world() -> playSound(SOUND_VEHICLE_DIE);
        get_world() -> increaseScore(200);
        int i = randInt(1, 5);
        if (i == 5)
            get_world()->addOilSlick(getX(), getY());
    }
    else
        get_world()->playSound(SOUND_VEHICLE_HURT);
}

void ZombieCab::doSomething(){
    if(!isalive())
        return;
    
    if (isoverlap() && (!isdamge)){
        get_world()->getRacer()->set_health( get_world()->getRacer()->get_health() - 20);
        isdamge = true; //record that the cab has damaged Ghost Racer
        get_world()->playSound(SOUND_VEHICLE_CRASH);
        if (getX() < get_world()->getRacer()->getX()){
            set_hspeed(-5);
            setDirection(120 + randInt(0, 19));
        }
        else{
            set_hspeed(5);
            setDirection(60 - randInt(0, 19));
        }
    }
    
    int vert_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int horiz_speed = get_hspeed();
    int x = getX() + horiz_speed;
    int y = getY() + vert_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }

    int l = 0; //find out wich lane the cab is at
    if ((getX() > ROAD_CENTER - ROAD_WIDTH / 2) &&
        (getX() < ROAD_CENTER - ROAD_WIDTH / 6))
        l = 1;
    else if ((getX() > ROAD_CENTER - ROAD_WIDTH / 6) &&
             (getX() < ROAD_CENTER + ROAD_WIDTH / 6))
        l = 2;
    else
        l = 3;
   Actor* front = get_world()->FirstAppropriateCollisionFront(l, getY());
    if (front != nullptr //must check if it's nullptr first, otherwise there would be an error
         && getspeed() > get_world()->getRacer()->getspeed()
         && front->getY() - getY() < 96 ){
        set_vspeed(getspeed() - 0.5);
        return;
    }
    
    
    Actor* back = get_world()->FirstAppropriateCollisionBack(l, getY());

   if ( back != nullptr  //must check if it's nullptr first, otherwise there would be an error
        &&getspeed() <= get_world()->getRacer()->getspeed()
        && getY() - back->getY() < 96 ){
        set_vspeed(getspeed() + 0.5);
        return;
    }
    
    planlen--;
    if (planlen > 0)
        return;
    //pick new move plan
    planlen = randInt(4, 32);
    set_vspeed(getspeed() + randInt(-2, 2));
}

//////////////////////////////////////Sprays////////////////////////////////////
Spray::Spray(StudentWorld* sw, double x, double y, int dir, int relativeSpeed, int relativeHoriSpeed)
    :Actor(IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1, relativeSpeed, relativeHoriSpeed, false, sw, true)
    ,maxdist(160)
{}
 
int Spray::getDist() const{
    return maxdist;
}

void Spray::setDist(int dist){
    maxdist = dist;
}

void Spray::doSomething(){
    if(!isalive())
        return;
    Actor* firstS = get_world()->FirstAppropriateSpray(getX(),getY(),getRadius());
    if (firstS != nullptr){
        firstS->decreaseHitPoint();
        set_alive(false);
        return;
    }
    else{
        if (SPRITE_HEIGHT<getDist())
            moveForward(SPRITE_HEIGHT);
        else
            moveForward(getDist());
        if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
            set_alive(false);
            return;
        }
        setDist(getDist() - SPRITE_HEIGHT);
        if (getDist() <= 0){
            set_alive(false);
            return;
        }
    }
}

//////////////////////////////////////Goodies////////////////////////////////////
Goodies::Goodies(StudentWorld* sw, double x, double y, int dir, int imageID, int size)
    :Actor(imageID, x, y, dir, size, 2, -4, 0, false, sw, true)
{}

//////////////////////////////////////Goodies: Healing////////////////////////////////////
HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
    :Goodies(sw, x, y, 0, IID_HEAL_GOODIE, 1.0)
{}

void HealingGoodie::doSomething(){
    int v_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int h_speed = get_hspeed();
    int x = getX() + h_speed;
    int y = getY() + v_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }
    
    if(isoverlap()){
        get_world()->playSound(SOUND_GOT_GOODIE);
        get_world()->increaseScore(250);
        get_world()->getRacer()->set_health(get_world()->getRacer()->get_health() + 10);
        if (get_world()->getRacer()->get_health() > 100)
            get_world()->getRacer()->set_health(100);
        set_alive(false);
    }
}


//////////////////////////////////////Goodies: HolyWater////////////////////////////////////
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
    :Goodies(sw, x, y, 90, IID_HOLY_WATER_GOODIE, 2.0)
{}
/*
bool HolyWaterGoodie::isSprayWorthy(){
    return false;
}

void HolyWaterGoodie::decreaseHitPoint(){
    set_alive(false);
}*/

void HolyWaterGoodie::doSomething(){
    int v_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int h_speed = get_hspeed();
    int x = getX() + h_speed;
    int y = getY() + v_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }
    
    if(isoverlap()){
        get_world()->playSound(SOUND_GOT_GOODIE);
        get_world()->increaseScore(50);
        get_world()->getRacer()->set_sprays(get_world()->getRacer()->get_sprays()+10);
        set_alive(false);
    }
}


//////////////////////////////////////Goodies: Soul////////////////////////////////////
SoulGoodie::SoulGoodie(StudentWorld* sw, double x)
    :Goodies(sw, x, VIEW_HEIGHT, 0, IID_SOUL_GOODIE, 4.0)
{}

void SoulGoodie::doSomething(){
    int v_speed = getspeed() - (get_world()->getRacer()->getspeed());
    int h_speed = get_hspeed();
    int x = getX() + h_speed;
    int y = getY() + v_speed;
    moveTo(x, y);
    if (getX() < 0 || getY() < 0|| getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT){
        set_alive(false);
        return;
    }
    
    if(isoverlap()){
        get_world()->playSound(SOUND_GOT_SOUL);
        get_world()->increaseScore(100);
        get_world()->recordSoulSaved();
        set_alive(false);
    }
    else
        setDirection(getDirection()-10);
}

