#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{}

StudentWorld::~StudentWorld(){
    cleanUp();
}

GhostRacer* StudentWorld::getRacer(){
    return GR;
}

void StudentWorld::recordSoulSaved(){
    soulSaved++;
}

int StudentWorld::init()
{
    soulSaved = 0;
    bonus = 5000;
    GR = new GhostRacer(this); //create and add a new Ghost Racer
    //add N border line objects
    double LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    double RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    
    //add N yellow border line objects
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int j = 0; j < N; j++){
        double y = j * SPRITE_HEIGHT;
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, y, this, false));
    }
    for (int j = 0; j < N; j++){
        double y = j * SPRITE_HEIGHT;
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, y, this, false));
    }

    //add M yellow border line objects
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    for (int j = 0; j < M; j++){
        double y = j * 4 * SPRITE_HEIGHT;
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, y, this, true));
    }
    for (int j = 0; j < M; j++){
        double y = j * 4 * SPRITE_HEIGHT;
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, y, this, true));
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

    if (!(GR->isalive())){
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }           //if player died at this tick

    else
        GR->doSomething();

    for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        if ((*p)->isalive())
            (*p)->doSomething();   //tell all of the actors that are currently active to do something
        
        if (!(GR->isalive())){
            playSound(SOUND_PLAYER_DIE);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }           //if player died at this tick, after the actor's movement
    }
    
    for (list<Actor*>::iterator p = actors.begin(); p != actors.end();p++){
        if (!((*p)->isalive())){
            delete (*p);
            p = actors.erase(p);
            p--;
        }                       //if an actor dies, erase it
    }
    
    //add new actors
    //calculate the distance between the last border picture to the upper edge
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - (*last_white_border())->getY();
    //check if there is space to add yellow borderline
    if (delta_y >= SPRITE_HEIGHT){
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2, new_border_y, this, false));
        actors.push_back(new BorderLine(IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2, new_border_y, this, false));
    }
    //check if there is space to add white borderline
    if (delta_y >= 4 * SPRITE_HEIGHT){
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, new_border_y, this, true));
        actors.push_back(new BorderLine(IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, new_border_y, this, true));
    }
   
    //add new oil slick
    int ChanceOilSlick = max(150 - getLevel() * 10, 40);
    int ioil = randInt(0, ChanceOilSlick);
    if (ioil == 0){
        int oil_location = randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2);
        actors.push_back(new OilSlick(this, oil_location, VIEW_HEIGHT));
    }
    
    //add new zombie pedestrian
    int ChanceZombiePed = max(100 - getLevel() * 10, 20);
    int izombie = randInt(0, ChanceZombiePed - 1);
    if (izombie == 0){
        int z_location = randInt(0, VIEW_WIDTH);
        actors.push_back(new ZombiePedestrian(this, z_location, 0));
    }
    
    //add a new human pedestrian
    int ChanceHumanPed = max(200 - getLevel() * 10, 30);
    int ihuman = randInt(0, ChanceHumanPed - 1);
    if (ihuman == 0){
        int h_location = randInt(0, VIEW_WIDTH);
        actors.push_back(new HumanPedestrian(this, h_location, 0));
    }
    
    //add new holy water goodie
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int iholywater = randInt(0, ChanceOfHolyWater - 1);
    if (iholywater == 0){
        int water_location = randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2);
        actors.push_back(new HolyWaterGoodie(this, water_location, VIEW_HEIGHT));
    }
    
    //add new soul goodie
    int isoul = randInt(0, 100 - 1);
    if (isoul == 0){
        int soul_location = randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2);
        actors.push_back(new SoulGoodie(this, soul_location));
    }
  
    //add new zombie cab
    int ChanceVehicle = max(100 - getLevel() * 10, 20);
    int chance_rand = randInt(0, ChanceVehicle - 1);
    if (chance_rand == 0){
        int cur_lane[3];
        for (int i = 0; i < 3; i++){
            cur_lane[i] = randInt(1,3);
        }
        while (cur_lane[1] == cur_lane[0])
            cur_lane[1] = randInt(1, 3);
        while (cur_lane[2] == cur_lane[1] || cur_lane[2] == cur_lane[0])
            cur_lane[2] = randInt(1, 3);
        for (int i = 0; i < 3; i++){
            int Racerlane = 0;
            if (GR->getX() < ROAD_CENTER - ROAD_WIDTH / 6)
                Racerlane = 1;
            else if (GR->getX() < ROAD_CENTER + ROAD_WIDTH / 6)
                Racerlane = 2;
            else
                Racerlane = 3;
    
            if (cur_lane[i] != Racerlane) {   //pushed the cab at the center of each lane
                Actor* closest_bottom = closest_collision(cur_lane[i], 1);
                if (closest_bottom == nullptr || closest_bottom->getY() > VIEW_HEIGHT / 3){
                    int x;
                    if (cur_lane[i] == 1)
                        x = ROAD_CENTER - ROAD_WIDTH / 3;
                    else if (cur_lane[i] == 2)
                        x = ROAD_CENTER;
                    else
                        x = ROAD_CENTER + ROAD_WIDTH / 3;
                    actors.push_back(new ZombieCab(this, x, SPRITE_HEIGHT / 2, getRacer()->getspeed() + randInt(2, 4)));
                    break;
                }
            }
            Actor* closest_top = closest_collision(cur_lane[i], 2);
            if (closest_top == nullptr || closest_top->getY() < VIEW_HEIGHT * 2 / 3){
                int x = 0;
                if (cur_lane[i] == 1)
                    x = ROAD_CENTER - ROAD_WIDTH / 3;
                else if (cur_lane[i] == 2)
                    x = ROAD_CENTER;
                else
                    x = ROAD_CENTER + ROAD_WIDTH / 3;
                actors.push_back(new ZombieCab(this, x , VIEW_HEIGHT - SPRITE_HEIGHT / 2, getRacer()->getspeed() - randInt(2, 4)));
                break;
            }
        }
    }
    
    if(bonus > 0)
        bonus--;
    
    if (soulSaved >= 2 * getLevel() + 5){
        increaseScore(bonus);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }

    //update the status text
    ostringstream oss;
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << 2 * getLevel() + 5 - soulSaved << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << GR->get_health() << "  ";
    oss << "Sprays: " << getRacer()->get_sprays() << "  ";
    oss << "Bonus: " << bonus;
    string s = oss.str();
    setGameStatText(s);
    
    return GWSTATUS_CONTINUE_GAME;
}


list<Actor*>::iterator StudentWorld::last_white_border(){
    list<Actor*>::iterator p = actors.end();
    for (p--;; p--){
        if ((*p)->is_white_line())
            return p;

        if (p == actors.begin())
            return p;
    }
}

void StudentWorld::cleanUp(){
    delete GR;
    for (list<Actor*>::iterator p = actors.begin();actors.size() != 0;){
        delete (*p);
        p = actors.erase(p);
    }
}

void StudentWorld::addHealingGoodie(double x, double y){
    actors.push_back(new HealingGoodie(this, x, y));
}

void StudentWorld::addSpray(double x, double y, int dir){
    actors.push_back(new Spray(this, x, y, dir, 0, 0));
}

void StudentWorld::addOilSlick(double x, double y){
    actors.push_back(new OilSlick(this, x, y));
}


bool StudentWorld::overlaps(double x1, double y1, double rad1, double x2, double y2, double rad2) const{
    int delta_x = abs(x1 - x2);
    int delta_y = abs(y1 - y2);
    if (delta_x < (rad1+rad2) * 0.25 && delta_y < (rad1+rad2) * 0.6)
        return true;
    else
        return false;
}

Actor* StudentWorld::FirstAppropriateSpray(double x, double y, int rad){
    for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        if ((*p)->isalive() && (*p)->isSprayWorthy() && overlaps(x,y,rad,(*p)->getX(),(*p)->getY(),(*p)->getRadius()))
            return (*p);
    }
    return nullptr;
}

Actor* StudentWorld::first_collision(int lane){
    int left = 0, right = 0;
    if (lane == 1){
        left = ROAD_CENTER - ROAD_WIDTH / 2;
        right = ROAD_CENTER - ROAD_WIDTH / 6;
    }
    else if (lane == 2){
        left = ROAD_CENTER - ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 6;
    }
    else{
        left = ROAD_CENTER + ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 2;
    }

    for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        int X_coord = (*p)->getX();
        if (((*p)->isCollisionWorthy()) && X_coord > left && X_coord < right)
            return(*p);
    }
    return nullptr;
}

Actor* StudentWorld::closest_collision(int lan, int delta_y)
{
    int left = 0, right = 0;
    if (lan == 1){
        left = ROAD_CENTER - ROAD_WIDTH / 2;
        right = ROAD_CENTER - ROAD_WIDTH / 6;
    }
    else if (lan == 2){
        left = ROAD_CENTER - ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 6;
    }
    else{
        left = ROAD_CENTER + ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 2;
    }

    Actor* firstcolli = first_collision(lan);

    if (firstcolli == nullptr)
        return nullptr;

    if (delta_y == 1){
        for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
            int x = (*p)->getX();
            int y = (*p)->getY();
            if (x >= left && x <= right && (*p)->isCollisionWorthy() && (y < firstcolli->getY()))
                firstcolli = (*p);
        }
    }
    else{
        for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
            int x = (*p)->getX();
            int y = (*p)->getY();
            if (x >= left && x <= right && (*p)->isCollisionWorthy() && ( y> firstcolli->getY()))
                firstcolli = (*p);
        }
    }
    return firstcolli;
}

Actor* StudentWorld::FirstAppropriateCollisionFront(int lan, double y){
    
    int left = 0, right = 0;
    if (lan == 1){
        left = ROAD_CENTER - ROAD_WIDTH / 2;
        right = ROAD_CENTER - ROAD_WIDTH / 6;
    }
    else if (lan == 2){
        left = ROAD_CENTER - ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 6;
    }
    else{
        left = ROAD_CENTER + ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 2;
    }


    Actor* firstcolliFront = nullptr;

    int min = VIEW_HEIGHT;

    for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        int X_coord = (*p)->getX();
        int Y_coord = (*p)->getY();
        if (X_coord >= left && X_coord <= right && (*p)->isCollisionWorthy() && (Y_coord > y) && Y_coord <= min){
            firstcolliFront = (*p);
            min = (*p)->getY();
        }
    }
    
    return firstcolliFront;
}

Actor* StudentWorld::FirstAppropriateCollisionBack(int lan, double y){
    int left = 0, right = 0;
    if (lan == 1){
        left = ROAD_CENTER - ROAD_WIDTH / 2;
        right = ROAD_CENTER - ROAD_WIDTH / 6;
    }
    else if (lan == 2){
        left = ROAD_CENTER - ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 6;
    }
    else{
        left = ROAD_CENTER + ROAD_WIDTH / 6;
        right = ROAD_CENTER + ROAD_WIDTH / 2;
    }

    Actor* firstcolliBack = nullptr;

    int max = 0;
    for (list<Actor*>::iterator p = actors.begin(); p != actors.end(); p++){
        int X_coord = (*p)->getX();
        int Y_coord = (*p)->getY();
        if (p == actors.end())
            break;
        if (X_coord >= left && X_coord <= right && (*p)->isCollisionWorthy()
            && (Y_coord < y)
            && Y_coord >= max){
            firstcolliBack = (*p);
            max = (*p)->getY();
        }
    }
    return firstcolliBack;
}


