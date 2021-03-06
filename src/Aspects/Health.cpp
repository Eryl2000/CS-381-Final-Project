/*
 * Health.cpp
 *
 *  Created on: Apr 16, 2019
 *      Author: kylebrain
 */

#include "Health.h"
#include "SoundMgr.h"
#include "UiMgr.h"
#include "Player.h"
#include <iostream>

Health::Health(Entity381 * entity, int startingHealth, int dps) :
        Aspect(entity),
        CurrentHealth(startingHealth),
        StartingHealth(startingHealth),
        incomingDamagePerSecond(dps),
        timeSinceLastHit(0){
    if(incomingDamagePerSecond <= 0){
        timeSinceLastHit = -1;
    }
}

Health::~Health(){

}

bool Health::IsAlive(){
    return CurrentHealth > 0;
}

// returns true if attached entity is still alive
bool Health::TakeDamage(int damageAmount){
    if(timeSinceLastHit <= 0){
        if(incomingDamagePerSecond != 0){
            timeSinceLastHit = 1 / (float)incomingDamagePerSecond;
        }
        if(dynamic_cast<Player*>(entity381) != NULL && entity381->engine->uiMgr->screenClosed == false){
        entity381->engine->soundMgr->playSelectedSound(player_damage);
        	}
        CurrentHealth -= damageAmount;
        if(!IsAlive()){
            // health is never below zero
            CurrentHealth = 0;
            return false;
        } else{
            return true;
        }
    }
    return IsAlive();
}

void Health::Tick(float dt){
    timeSinceLastHit -= dt;
    if(timeSinceLastHit < 0){
        timeSinceLastHit = -1;
    }
}

