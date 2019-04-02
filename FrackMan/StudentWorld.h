#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <vector>
#include <string>
#include "Actor.h"


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class StudentWorld : public GameWorld
{
public:
	enum Direction { none, up, down, left, right };
	StudentWorld(std::string assetDir); // fill
	virtual ~StudentWorld(); // fill
	virtual int init(); // fill
	virtual int move(); // fill
	virtual void cleanUp(); // fill
	void addSquirt(int x, int y, GraphObject::Direction dir); //filled
	void addGold(const int& x, const int& y, int timer); // filled warning
	bool isDirtAt(const int& x, const int& y) const; /*filled*/
	bool isProtesterAt(const int& x, const int& y); //fill
	void getDug(const int& x, const int& y); /*filled*/
	void delDirtat(const int& x, const int& y, const int& size);
	void deleteActor(Actor* ptr); // filled
	bool canActorMoveTo(Actor* ptr, const int& x, const int& y); // filled
	bool doesFrameOverlap(Actor* a1, Actor* a2) const;	//filled
	float disBetween(int x1, int y1, int x2, int y2);
	void populateCollectables(int num, char which);
	void revealAllNearby(const int& x, const int& y);
	bool isNearbyFrackMan(Actor* a, const int& radius);
	void giveFMGold() { m_player->addGold(); G--; }
	void giveFMWater() { m_player->addWater(); }
	void giveFMSonar() { m_player->addSonar(); }
	bool checkBoulderGround(Actor* a) const;
	bool isSafe(Actor* a); // filled
	int countOil();
	void annoyFrackMan() { m_player->getAnnoyed(2); }
	int annoyAllNearbyProtesters(Actor* annoyer, int points);
	bool StudentWorld::isBoulderAt(const int& x, const int& y);
	// Is the Actor a facing toward the FrackMan?
	bool facingTowardFrackMan(Actor* a);
	// If the Actor a has a clear line of sight to the FrackMan, return
	// the direction to the FrackMan, otherwise GraphObject::none.
	GraphObject::Direction lineOfSightToFrackMan(Actor* a);
	//bool isNearbyAnyP(this, 3)
private:
	std::vector <Actor*> actors;
	FrackMan* m_player; /*filled*/
	Dirt* m_dirts[64][60]; /*filled*/
	unsigned int L, G, B;
};

#endif // STUDENTWORLD_H_
