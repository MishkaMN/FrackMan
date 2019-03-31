#include "Actor.h"
#include "StudentWorld.h"
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
int randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}
Protester::Protester(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth, int hp)
	:HPholder(sw, startX, startY, imageID, dir, size, depth, hp), m_state(1), non_rest_ticks(0), hasShouted(false)
{
	if (3 - (getWorld()->getLevel() / 4) > 0)
		rest_ticks = 3 - (getWorld()->getLevel() / 4);
	else rest_ticks = 0;
	tickPeriod = rest_ticks;
}
ValueHolder::ValueHolder(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth)
	:Actor(sw, startX, startY, imageID, dir, size, depth)
{
	if (getWorld()->isDirtAt(getX(), getY()))
		setVisible(false);
	else setVisible(true);
}
bool Actor::isPlacable(const int& x, const int& y) const
{
	if (x < 0 || x > 60 || y < 0 || y > 60)
		return false;
	else return true;
}
void FrackMan::doSomething()
{
	if (!isAlive())
		return;
	specialMove(getX(), getY());
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			turnOrMove(left, getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			turnOrMove(right, getX(), getY());
			break;
		case KEY_PRESS_UP:
			turnOrMove(up, getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			turnOrMove(down, getX(), getY());
			break;
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
			if (getWater() > 0)
			{
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				m_water--;
				getWorld()->addSquirt(getX(),getY(), getDirection());
			}
			break;
		case 'Z': case 'z':
			if (getSonar() > 0)
			{
				getWorld()->playSound(SOUND_SONAR);
				m_charge--;
				getWorld()->revealAllNearby(getX(), getY());
			}
			break;
		case KEY_PRESS_TAB:
			if (getGold() > 0)
			{
				m_gold--;
				getWorld()->addGold(getX(), getY(), 100);
			}
		default: break;
		}
	}
	
}
void Barrel::doSomething()	
{
	if (!isAlive())
		return;
	else if (!isVisible() && getWorld()->isNearbyFrackMan(this, 4))
	{
		setVisible(true);
		return;
	}
	else if (getWorld()->isNearbyFrackMan(this, 3))
	{
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
	}
	return;

}
void MovingActor::turnOrMove(const Direction& dir, const int& x, const int& y)
{
	int desX, desY;
	switch (dir)
	{
	case left: desX = x - 1; desY = y; break;
	case right: desX = x + 1; desY = y; break;
	case up: desX = x; desY = y + 1; break;
	case down: desX = x; desY = y - 1; break;
	default: break;
	}
	if (getDirection() != dir)
		setDirection(dir);
	else if (getDirection() == dir && getWorld()->canActorMoveTo(this, desX, desY))
		moveTo(desX, desY);			//move

}
void FrackMan::specialMove(const int& digX, const int& digY)
{
	if (getWorld()->isDirtAt(digX, digY))
		getWorld()->getDug(digX, digY);
	return;
}
void Squirt::doSomething()
{
	specialMove(getX(),getY());
	turnOrMove(getDirection(), getX(), getY());
}
void Squirt::specialMove(const int& x, const int& y)
{
	int desX, desY;
	switch (getDirection())
	{
	case left: desX = x - 4; desY = y; break;
	case right: desX = x + 4; desY = y; break;
	case up: desX = x; desY = y + 4; break;
	case down: desX = x; desY = y - 4; break;
	default: break;
	}
	if (getWorld()->annoyAllNearbyProtesters(this, 2) > 0)
	{
		setDead();
		return; // fill
	}
	else if (!getWorld()->canActorMoveTo(this, desX, desY))
		setDead();
	else if (getDistanceTraveled() >= 4)
		setDead();
	else continueTravel();
}
void Actor::setDead() 
{
	m_val = 0;
}
void HPholder::setDead() { m_hp = 0; Actor::setDead(); }
void GoldNugget::doSomething()
{
	setBool();
	if (!isAlive())
		return;
	if (!isVisible() && getWorld()->isNearbyFrackMan(this, 4))
	{
		cout << "aa" << endl;
		setVisible(true);
		return;
	}
	else if (getWorld()->isNearbyFrackMan(this, 3) && canPickedbyFM())
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->giveFMGold();
		return;
	}
	/*else if (getWorld()->isNearbyAnyP(this, 3) && canPickedbyP())
	{
		setDead();
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		getWorld()->increaseScore(25);
		getWorld()->giveFMGold();
	}
	*/
	m_timer--;
	if (m_timer == 0)
		setDead();
}
bool FrackMan::getAnnoyed(unsigned int amt)
{
	bool completelyAnnoyed = HPholder::getAnnoyed(amt);
	if (healthPoint() <= 0)
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		return completelyAnnoyed;	// died
	}
	return false; // didn't die
}
void Boulder::doSomething()
{
	if (!isAlive())
		return;
	if (isStable() && !getWorld()->checkBoulderGround(this))	// stable but doesnt have ground
		m_state = 1; // enter waiting state
	if (m_state == 1 && m_timer <= 29)
		m_timer++;
	if (m_state == 1 && m_timer > 29)
	{
		getWorld()->playSound(SOUND_FALLING_ROCK);
		m_state = 2;
	}
	if (m_state == 2)
	{
		if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
			turnOrMove(down, getX(), getY());
		else setDead();
	}
	return;
}
void SonarKit::doSomething()
{
	if (!isAlive())
		return;
	if (m_timer >= 0)
		setDead();
	if (getWorld()->isNearbyFrackMan(this, 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveFMSonar();
		getWorld()->increaseScore(75);
	}
}
void WaterPool::doSomething()
{
	if (!isAlive())
		return;
	if (m_timer >= 0)
		setDead();
	if (getWorld()->isNearbyFrackMan(this, 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveFMWater();
		getWorld()->increaseScore(100);
	}

}
void RegularP::specialMove(const int& digX, const int& digY)
{
	if (numSquaresToMoveInCurrentDirection == 0)
	{
		numSquaresToMoveInCurrentDirection = randInt(8, 60);
		return;
	}
	numSquaresToMoveInCurrentDirection--;
}
void RegularP::doSomething()
{
	if (!isAlive())
		return;
	else if (tick() > 0)
	{
		tickTock();
		return;
	}
	else if (tick() == 0)
		setTicksToNextMove();

	// time to move

	if (getState() == 0)
	{
		if (getX() == 60 && getY() == 60)
			setDead();
		else //move();
		return;
	}
	else if (getWorld()->isNearbyFrackMan(this, 4) && getWorld()->facingTowardFrackMan(this) && !hasRecentlyShouted())
	{
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		getWorld()->annoyFrackMan();
		setShouted();
		return;
	}
	else if (getWorld()->lineOfSightToFrackMan(this) != none && !getWorld()->isNearbyFrackMan(this, 4))
	{
		setDirection(getWorld()->lineOfSightToFrackMan(this));
		turnOrMove(getDirection(), getX(), getY());
		numSquaresToMoveInCurrentDirection = 0;
		return;
	}
	else if (getWorld()->lineOfSightToFrackMan(this) == none)
	{
		specialMove(getX(), getY());
		// end
		return;
	}


}
// fill
