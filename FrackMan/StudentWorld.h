#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <vector>
#include <string>
#include "Actor.h"
#include <cmath>
#include <queue>
#include <algorithm>

class Node
{
	// Used for protester's BFS maze solving algorithm
public:
	Node() {}
	Node(int x, int y) { m_x = x; m_y = y; m_visited = false; }
	~Node() {}
	int getX() { return m_x; }
	int getY() { return m_y; }
	bool isVisited(){ return m_visited; }
private:
	int m_x, m_y;
	bool m_visited;
	// up, down, left, right
	int m_steps[4] = { 0,0,0,0 };
};
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class StudentWorld : public GameWorld
{
public:
	enum Direction { none, up, down, left, right };
	StudentWorld(std::string assetDir); 
	virtual ~StudentWorld(); 
	virtual int init(); 
	virtual int move(); 
	virtual void cleanUp(); 
	const std::vector <Actor*>& getActors()
	{
		return actors;
	}
	void addSquirt(int x, int y, GraphObject::Direction dir);
	void addGold(const int& x, const int& y, int timer); 
	void addRegularP();
	bool anyDirtAt(const int& x, const int& y) const;
	bool anyDirtWallAt(const GraphObject::Direction& dir, const int& x, const int& y) const;
	bool isProtesterAt(const int& x, const int& y); 
	void getDug(const int& x, const int& y); 
	void delDirtat(const int& x, const int& y, const int& size);
	const void deleteActor(Actor* ptr);
	bool canActorMoveTo(const Actor* ptr, const int& x, const int& y); 
	bool canMoveTo(const int& x, const int& y);
	bool doFramesOverlap(const Actor* a1, const Actor* a2) const;	
	bool doFramesOverlap(const int a1_x, const int a1_y, const int a2_x, const int a2_y) const;
	float disBetween(int x1, int y1, int x2, int y2);
	void populateCollectables(int num, char which);
	void revealAllNearby(const int& x, const int& y);
	bool isNearbyFrackMan(const Actor* a, const int& radius);
	Actor* anyNearbyP(const Actor* a, const int radius);
	void giveFMGold() { m_player->addGold(); G--; }
	void giveFMWater() { m_player->addWater(); }
	void giveFMSonar() { m_player->addSonar(); }
	bool checkBoulderGround(const Actor* a) const;
	int countOil();
	void decOil();
	void annoyFrackMan() { m_player->getAnnoyed(2); cout << "Health:\n"; cout << m_player->getHP(); }
	int annoyAllNearbyProtesters(int x, int y, int points);
	bool anyBoulderAt(const int& x, const int& y);
	// Is the Actor a facing toward the FrackMan?
	bool facingTowardFrackMan(Actor* a);
	// If the Actor a has a clear line of sight to the FrackMan, return the direction to the FrackMan, otherwise GraphObject::none.
	GraphObject::Direction lineOfSightToFrackMan(Actor* a);
	GraphObject::Direction getDir(Protester* actor, const int x, const int y);
	void updateMaze();
	void printDirt();
private:
	std::vector <Actor*> actors;
	FrackMan* m_player; 
	Dirt* m_dirts[64][64]; 
	int m_maze[64][64]; // Used for protester's exit path finding
	unsigned int L, G, B;
};

#endif // STUDENTWORLD_H_
