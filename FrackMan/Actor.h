#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <random>
#include <utility>

using namespace std;

int randInt(int min, int max);
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

////////////////////////////////////
/////// GRAPH_OBJECT TO ACTOR//////
//////////////////////////////////

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth)
		: GraphObject(imageID, startX, startY, dir, size, depth), m_world(sw), m_val(1)
	{
		setVisible(true);
	}
	
	virtual ~Actor() {} 
	
	virtual void doSomething() = 0; 
	
	StudentWorld* getWorld() const { return m_world; }
	
	virtual void setDead(); 
	
	virtual bool isAlive() const { return m_val > 0; }
	// Can move?
	virtual bool canMove() const { return false; }
	// can be picked up by FrackMan or Protester?
	virtual bool canPickedbyP() const { return false; }
	
	// can be picked up by frackman?
	virtual bool canPickedbyFM() const { return false; }
	
	// Can this actor dig through dirt?
	virtual bool canDigThroughDirt() const { return false; }

	// Can this actor pick items up?
	virtual bool hasHP() const { return false; }

	// Does this actor hunt the FrackMan?
	virtual bool isProtester() const { return false; }

	// is boulder?
	virtual bool isBoulder() const { return false; }

	// is an obstacle? dirt or boulder
	virtual bool isObstacle() const { return false; }

	// Can this actor need to be picked up to finish the level?
	virtual bool isOil() const { return false; }

	//	Can other actors pass through this actor? 
	virtual bool canActorPassThroughMe() const = 0; 

	// is it possible to place it in map?
	bool isPlacable(const int& x, const int& y) const;

	// Mainly for HP Holders
	virtual bool getAnnoyed(unsigned int amt) {return false; }; 

	// Used for Protesters and Frackman
	virtual void addGold() { return; };

	// Used for Moving objects and shortest path finding alg for protesters
	virtual void turnOrMove(const Direction& dir, const int& x, const int& y) { return; }

private:
	StudentWorld* m_world;

	int m_val; // all actors have value, for some, it is HP
};

///////     DIRT OF ACTOR    //////

class Dirt : public Actor
{
public: 
	Dirt(StudentWorld *sw, int startX, int startY, int imageID = IID_DIRT, Direction dir = right, double size = 0.25, unsigned int depth = 3)
		:Actor(sw, startX, startY, imageID, dir, size, depth)
	{} 
	virtual ~Dirt() {}
	virtual bool canActorPassThroughMe() const { return false; } 
	virtual void doSomething() { return; }	
	virtual bool isObstacle() { return true; }
	bool isDirt() { return true; }

private:
};


////////////////////////////////////
//// ACTOR TO VALUE_HOLDER  //////
//////////////////////////////////

class ValueHolder : public Actor
{
public:
	ValueHolder(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth);
	virtual bool canPickedbyFM() const { return true; }
	virtual bool canPickedbyP() const { return false; }
	virtual bool canActorPassThroughMe() const { return true; }
	virtual ~ValueHolder() {}; 
};

///////   SONAR_KIT OF VALUE_HOLDERS    //////

class SonarKit : public ValueHolder
{
public:
	SonarKit(StudentWorld *sw, int startX, int startY, int lvl, int imageID = IID_SONAR, Direction dir = right, double size = 1, unsigned int depth = 2)
		:ValueHolder(sw, startX, startY, imageID, dir, size, depth)
	{
		if (300 - 10 * lvl > 100)
			m_timer = 300 - 10 * lvl;
		else m_timer = 100;
	}
	virtual bool canPickedbyFM() const {return true;}
	virtual ~SonarKit() {};
	virtual void doSomething();
private:
	int m_timer;
};

///////   GOLD_NUGGET OF VALUE_HOLDERS    //////

class GoldNugget : public ValueHolder
{
public:
	GoldNugget(StudentWorld *sw, int startX, int startY, int timer, int imageID = IID_GOLD, Direction dir = right, double size = 1, unsigned int depth = 2)
		:ValueHolder(sw, startX, startY, imageID, dir, size, depth), m_timer(timer)
	{
		if (m_timer >= 0)
			pickableByFM = false;
		else pickableByFM = true;

	}
	virtual bool canPickedbyFM() const
	{
		return pickableByFM;
	}
	virtual bool canPickedbyP() const
	{
		return !pickableByFM;
	}
	void setBool() {
		if (m_timer >= 0)
			pickableByFM = false;
		else pickableByFM = true;
	}
	virtual ~GoldNugget() {}; 
	virtual void doSomething(); 
private:
	int m_timer;
	bool pickableByFM;
};

///////   WATER_POOL OF VALUE_HOLDERS    //////
class WaterPool : public ValueHolder
{
public:
	WaterPool(StudentWorld *sw, int startX, int startY, int lvl, int imageID = IID_WATER_POOL, Direction dir = right, double size = 1, unsigned int depth = 2)
		:ValueHolder(sw, startX, startY, imageID, dir, size, depth)
	{
		if (300 - 10 * lvl > 100)
			m_timer = 300 - 10 * lvl;
		else m_timer = 100;
	}
	virtual bool canPickedbyFM() const { return true; }
	virtual ~WaterPool() {}; 
	virtual void doSomething();
private:
	int m_timer;
};

///////   BARREL_OIL OF VALUE_HOLDERS    //////

class Barrel : public ValueHolder
{
public:
	Barrel(StudentWorld *sw, int startX, int startY, int lvl, int imageID = IID_BARREL, Direction dir = right, double size = 1, unsigned int depth = 2)
		:ValueHolder(sw, startX, startY, imageID, dir, size, depth)
	{
		setVisible(false);
	}
	virtual bool canPickedbyFM() const { return true; }
	virtual ~Barrel() {};
	virtual bool isOil() { return true; }
	virtual void doSomething(); 
};

////////////////////////////////////
//// ACTOR TO MOVING_ACTOR  //////
//////////////////////////////////

class MovingActor : public Actor
{
public:
	MovingActor(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth)
		:Actor(sw, startX, startY, imageID, dir, size, depth)
	{}
	virtual ~MovingActor() {}; 
	virtual bool canMove() const { return true; }
	void turnOrMove(const Direction& dir, const int& x, const int& y);
	virtual void specialMove(const int& digX, const int& digY) = 0;
};

///////   SQUIRT OF MOVING_ACTORS   //////

class Squirt : public MovingActor
{
public:
	Squirt(StudentWorld *sw, int startX, int startY, Direction dir, int imageID = IID_WATER_SPURT, double size = 1, unsigned int depth = 1)
		:MovingActor(sw, startX, startY, imageID, dir, size, depth)
	{
		m_journey = 0;
	}
	virtual void doSomething();	
	virtual bool canActorPassThroughMe() const { return false; }	
	virtual void specialMove(const int& x, const int& y); 
	int getDistanceTraveled() const { return m_journey; }
	void continueTravel() { m_journey++; }
	virtual ~Squirt() {}
private:
	int m_journey;
};

///////     BOULDER OF MOVING_ACTOR    //////

class Boulder : public MovingActor
{
public:
	Boulder(StudentWorld *sw, int startX, int startY, int imageID = IID_BOULDER, Direction dir = down, double size = 1, unsigned int depth = 1)
		:MovingActor(sw, startX, startY, imageID, dir, size, depth), m_state(0), m_timer(0)
	{
		setVisible(true);
	}
	virtual ~Boulder() {}
	virtual bool canActorPassThroughMe() const { return false; } 
	virtual void doSomething();
	virtual void specialMove(const int& digX, const int& digY) { return; }
	bool isStable() const 
	{ 
		return m_state != 2; //stable when 0 or 1 
	}
	virtual bool isBoulder() const { return true; }
	virtual bool isObstacle() { return true; }
private:
	int m_state; // 0 for stable
				 // 1 for waiting
				 // 2 for falling
	int m_timer;
};

///////////////////////////////////////////
/////// MOVING_ACTOR TO HP_HOLDER  //////
/////////////////////////////////////////

class HPholder : public MovingActor
{
public:
	HPholder(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth, int hp)
		:MovingActor(sw, startX, startY, imageID, dir, size, depth), m_hp(hp)
	{}
	virtual ~HPholder() {};
	int getHP() const { return m_hp; }
	virtual bool isAlive()const { return is_alive; }
	virtual void setDead();
	virtual void addGold() = 0; 
	virtual bool getAnnoyed(unsigned int amt) { m_hp = m_hp - amt; return true; }; 
	virtual void specialMove(const int& digX, const int& digY) = 0;
	virtual bool hasHP() const { return true; }

private:
	 int m_hp;
	 bool is_alive;
};

///////   FRACK_MAN OF HP_HOLDERS    //////

class FrackMan : public HPholder
{
public:
	FrackMan(StudentWorld *sw, int startX = 30, int startY = 60, int imageID = IID_PLAYER, Direction dir = right, double size = 1, unsigned int depth = 0, int hp = 10)
		:HPholder(sw, startX, startY, imageID, dir, size, depth, hp),
		m_water(10), m_charge(1), m_gold(10)
	{}
	virtual ~FrackMan() {} 
	virtual void doSomething();	
	void setDead();
	virtual void specialMove(const int& digX, const int& digY);
	virtual bool getAnnoyed(unsigned int amt); 
	virtual void addGold() { m_gold++; };	
	// player can't be passed anyways
	virtual bool canActorPassThroughMe() const { return false; }
	void addSonar() { m_charge++; };		
	void addWater() { m_water = m_water + 5; };		
	unsigned int getGold()const { return m_gold; }	
	unsigned int getSonar() const { return m_charge;  } 	
	unsigned int getWater() const { return m_water; }
	// Can this actor dig through dirt?
	virtual bool canDigThroughDirt() const { return true; }

private:
	unsigned int m_water;
	unsigned int m_charge;
	unsigned int m_gold;
};


///////////////////////////////////////////
/////// HP_HOLDER TO PROTESTER  /////////
/////////////////////////////////////////

class Protester : public HPholder
{
public:
	Protester(StudentWorld *sw, int imageID, int startX = 56, int startY = 60, Direction dir = left, double size = 1.0, unsigned int depth = 0, int hp = 5);
	virtual ~Protester() {}; 
	virtual void addGold() = 0;
	void specialMove(const int& digX, const int& digY);
	bool isProtester() const { return true; }
	// Set state to having given up protest
	void setMustLeaveOilField(); 
	int getState() const { return m_state; } 
	// Set number of ticks until next move
	int tick()const { return rest_ticks; } 
	void tickTock() { rest_ticks--; }
	void setTicksToNextMove() {rest_ticks = tickPeriod; non_rest_ticks++; }
	bool hasRecentlyShouted()
	{
		if (hasShouted && (non_rest_ticks != last_shout_tick) && (non_rest_ticks - last_shout_tick) % 15 == 0)
		{
			hasShouted = false;
			return true;
		}
		else if (hasShouted)
		{
			return true;
		}
		else return false;
	}
	void setDead();
	void setShouted() { hasShouted = true; last_shout_tick = non_rest_ticks; }
	bool canActorPassThroughMe() const { return !getState(); }
	GraphObject::Direction getDirPerp();
	void doSomething();
	bool getAnnoyed(unsigned int amt);
	void leaveOilField();
private:
	int rest_ticks;
	int tickPeriod;
	int non_rest_ticks;
	int m_state; // 0 leaving state; 1 normal state;
	bool hasShouted;
	int last_shout_tick = 0;
	bool shouldMovePerp;
	int numSquaresToMoveInCurrentDirection;
	
};

///////   REGULAR_PROTESTER OF PROTESTER   //////

class RegularP : public Protester
{
public:
	RegularP(StudentWorld *sw, int startX, int startY, int imageID = IID_PROTESTER)
		:Protester(sw, imageID, startX, startY)
	{}
	RegularP(StudentWorld *sw, int imageID = IID_PROTESTER)
		:Protester(sw, imageID)
	{} 
	virtual ~RegularP() {}
	void addGold() { setMustLeaveOilField();  return; }
private:
};

///////   HARDCORE_PROTESTER OF PROTESTER   //////

class HardcoreP : public Protester
{
public:
	HardcoreP(StudentWorld *sw, int startX, int startY, int imageID = IID_HARD_CORE_PROTESTER)
		:Protester(sw, imageID, startX, startY)
	{} 
	virtual ~HardcoreP() {};
	//virtual void doSomething();
	//virtual bool getAnnoyed(unsigned int amt);
	//pick up gold nugget
	void addGold();
	//annoy this actor
	//virtual void specialMove(const int& digX, const int& digY);
	// did not implement hardcoreP because it is not much different from regularP
};

#endif // ACTOR_H_