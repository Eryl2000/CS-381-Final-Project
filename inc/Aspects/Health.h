/*
 * Health.h
 *
 *  Created on: Apr 16, 2019
 *      Author: kylebrain
 */

#ifndef INC_ASPECTS_HEALTH_H_
#define INC_ASPECTS_HEALTH_H_

#include "Aspect.h"

class Health : public Aspect {
public:
	int CurrentHealth;

	Health(Entity381 * entity, int startingHealth = 100);
	~Health();

	bool IsAlive();
	bool TakeDamage(int damageAmount);

	virtual void Tick(float dt);
};


#endif /* INC_ASPECTS_HEALTH_H_ */