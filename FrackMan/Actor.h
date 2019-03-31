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
	} /*filled*/
	virtual ~Actor() {} /*filled*/
	virtual void doSomething() = 0; /*filled*/
	StudentWorld* getWorld() const { return m_world; }
	virtual void setDead(); // filled
	virtual bool isAlive() const { return m_val > 0; }
	// Can move?
	virtual bool canMove() const { return false; }
	// can be picked up by FrackMan or Protester?
	virtual bool canPickedbyP() const { return false; }
	virtual bool canPickedbyFM() const { return false; }
	// Can this actor dig through dirt?
	virtual bool canDigThroughDirt() const { return false; }

	// Can this actor pick items up?
	virtual bool hasHP() const { return false; }

	// Does this actor hunt the FrackMan?
	virtual bool isProtester() const { return false; }

	// is boulder?

	virtual bool isBoulder() const { return false; }

	// Can this actor need to be picked up to finish the level?

	virtual bool isOil() const { return false; }

	//can other actors pass through this actor?
	virtual bool canActorPassThroughMe() const = 0; //fill

	// is it possible to place it in map?
	bool isPlacable(const int& x, const int& y) const;
	virtual bool getAnnoyed(unsigned int amt) { return false; }; //filled

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
	{} /*filled*/
	virtual ~Dirt() {} /*filled*/
	virtual bool canActorPassThroughMe() const { return false; } //filled
	virtual void doSomething() { return; }	/*filled*/
	virtual void turnOrMove(const Direction& dir, const int& x, const int& y) { return; }

private:
};


////////////////////////////////////
//// ACTOR TO VALUE_HOLDER  //////
//////////////////////////////////

class ValueHolder : public Actor
{
public:
	ValueHolder(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth); /*filled*/
	   // can be picked up by FrackMan?
	virtual bool canPickedbyFM() const { return true; }
	virtual bool canPickedbyP() const { return false; }
	virtual bool canActorPassThroughMe() const { return true; }
	virtual ~ValueHolder() {}; /*filled*/
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
	} /*filled*/
	  // can be picked up by FrackMan?
	virtual bool canPickedbyFM() const {return true;}
	virtual ~SonarKit() {}; /*filled*/
	virtual void doSomething(); // timer // filled
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

	} /*filled*/
	  // can be picked up by FrackMan?
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
	virtual ~GoldNugget() {}; /*filled*/
	virtual void doSomething(); // timer // filled
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
	} /*filled*/
	  // can be picked up by FrackMan?
	virtual bool canPickedbyFM() const { return true; }
	virtual ~WaterPool() {}; /*filled*/
	virtual void doSomething(); // timer // filled
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
	} /*filled*/
	  // can be picked up by FrackMan?
	virtual bool canPickedbyFM() const { return true; }
	virtual ~Barrel() {}; /*filled*/
	virtual bool isOil() { return true; }
	virtual void doSomething(); // timer // filled
};

////////////////////////////////////
//// ACTOR TO MOVING_ACTOR  //////
//////////////////////////////////

class MovingActor : public Actor
{
public:
	MovingActor(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth)
		:Actor(sw, startX, startY, imageID, dir, size, depth)
	{} /*filled*/
	virtual ~MovingActor() {}; /*filled*/
	// Can move?
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
	} /*filled*/
	virtual void doSomething();	//fill
	virtual bool canActorPassThroughMe() const { return false; }	//filled
	virtual void specialMove(const int& x, const int& y); //filled
	int getDistanceTraveled() const { return m_journey; }// filled
	void continueTravel() { m_journey++; } // filled
	virtual ~Squirt() {} /*filled*/
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
	} /*filled*/
	virtual ~Boulder() {} /*filled*/
	virtual bool canActorPassThroughMe() const { return false; } //filled
	virtual void doSomething();	//filled
	virtual void specialMove(const int& digX, const int& digY) { return; }
	bool isStable() const { return m_state == 0; }
	virtual bool isBoulder() const { return true; }
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
	{} /*filled*/
	virtual ~HPholder() {}; /*filled*/
	int healthPoint() const { return m_hp; }
	virtual bool isAlive()const { return m_hp > 0; }
	virtual void setDead();
	//pick up gold nugget
	virtual void addGold() = 0; 
	//annoy this actor
	virtual bool getAnnoyed(unsigned int amt) { m_hp = m_hp - amt; return true; }; //filled
	virtual void specialMove(const int& digX, const int& digY) = 0;
	virtual bool hasHP() const { return true; }

private:
	 int m_hp;
};

///////   FRACK_MAN OF HP_HOLDERS    //////

class FrackMan : public HPholder
{
public:
	FrackMan(StudentWorld *sw, int startX = 30, int startY = 60, int imageID = IID_PLAYER, Direction dir = right, double size = 1, unsigned int depth = 0, int hp = 10)
		:HPholder(sw, startX, startY, imageID = IID_PLAYER, dir, size, depth, hp),
		m_water(5), m_charge(1), m_gold(0)
	{} /*filled*/
	virtual ~FrackMan() {} /*filled*/
	virtual void doSomething();	//filled
	virtual void specialMove(const int& digX, const int& digY); //dig
	virtual bool getAnnoyed(unsigned int amt); //filled
	virtual void addGold() { m_gold++; };	//fillP
	// player can't be passed anyways
	virtual bool canActorPassThroughMe() const { return false; }
	void addSonar() { m_charge++; };		// fillP
	void addWater() { m_water = m_water + 5; };		//fillP
	unsigned int getGold()const { return m_gold; }	//filled
	unsigned int getSonar() const { return m_charge;  } 	//filled
	unsigned int getWater() const { return m_water; } // filled
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
	Protester(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth, int hp); /*filled*/
	virtual ~Protester() {}; /*filled*/
	//pick up gold nugget
	virtual void addGold() = 0;
	virtual bool getAnnoyed(unsigned int amt) = 0; // filled
	//annoy this actor
	virtual void specialMove(const int& digX, const int& digY) = 0;
	virtual bool isProtester() const { return true; }
	// Set state to having gien up protest
	void setMustLeaveOilField() { m_state = 0;  } //filled
	int getState() const { return m_state; } // filled
	// Set number of ticks until next move
	int tick()const { return rest_ticks; } // filled
	void tickTock() { rest_ticks--; } // filled
	void setTicksToNextMove()
	{rest_ticks = tickPeriod; non_rest_ticks++; }// filled
	bool hasRecentlyShouted()
	{
		if (hasShouted && non_rest_ticks % 15 == 0)
		{
			hasShouted = false;
			return true;
		}
		else return false;
	}
	void setShouted() { hasShouted = true; }
private:
	int rest_ticks;
	int tickPeriod;
	int non_rest_ticks;
	int m_state; // 0 leaving state
				 // 1 normal state;
	bool hasShouted;
};

///////   REGULAR_PROTESTER OF PROTESTER   //////

class RegularP : public Protester
{
public:
	RegularP(StudentWorld *sw, int startX, int startY, int imageID = IID_PROTESTER, Direction dir = left, double size = 1, unsigned int depth = 0, int hp = 5)
		:Protester(sw, startX, startY, imageID, dir, size, depth, hp)
	{
		numSquaresToMoveInCurrentDirection = randInt(8, 60);
	} /*filled*/
	virtual ~RegularP() {}; /*filled*/
	virtual void doSomething();	//fill
	virtual bool getAnnoyed(unsigned int amt); //fill
	//pick up gold nugget
	virtual void addGold() = 0;
	//annoy this actor
	virtual void specialMove(const int& digX, const int& digY);
private:
	int numSquaresToMoveInCurrentDirection;
	

};

///////   HARDCORE_PROTESTER OF PROTESTER   //////

class HardcoreP : public Protester
{
public:
	HardcoreP(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth, int hp)
		:Protester(sw, startX, startY, imageID, dir, size, depth, hp)
	{} /*filled*/
	virtual ~HardcoreP() {}; /*filled*/
	virtual void doSomething();	//fill
	virtual bool getAnnoyed(unsigned int amt); //fill
	//pick up gold nugget
	virtual void addGold() = 0;
	//annoy this actor
	virtual void specialMove(const int& digX, const int& digY);
};

#endif // ACTOR_H_