/*
 * FirstBoss.cpp
 *
 *  Created on: Apr 29, 2019
 *      Author: kylebrain
 */

#include "FirstBoss.h"
#include "GameMgr.h"
#include "UnitAI.h"
#include "CircleCollider.h"
#include "Entity381.h"
#include "FirstBossLimbs.h"
#include "BossCollider.h"
#include "BossAI.h"
#include "CameraTether.h"
#include "UiMgr.h"

FirstBoss::FirstBoss(int id, Ogre::Vector3 pos, Engine * eng) :
        Boss(id, "sphere.mesh", Ogre::Vector3(1, 0.8, 1.3), // scale
        pos, "Googalog", 4000, // Health
                2000, 90, 1000, // OrientedPhysics values
                eng){
    ogreEntity->setMaterialName("Stone");
    //GetAspect<UnitAI>()->SetCommand(new Intercept(this, eng->gameMgr->MainPlayer));

    aspects.push_back(new BossCircleCollider(this, this, scale.x * 100));
    aspects.push_back(new FirstBossLimbs(this));
    aspects.push_back(new BossAI(this, engine->gameMgr->MainPlayer));

}
FirstBoss::~FirstBoss(){
}

void FirstBoss::OnDeath() {
    //TODO: do what happens when boss dies
    engine->gameMgr->MainPlayer->GetAspect<CameraTether>()->Height = 1300;
    position = Ogre::Vector3(1e6, 0, 0);
    GetAspect<BossAI>()->currentState = NoState;
    engine->gameMgr->LoadLevelTwo();
    engine->uiMgr->mTrayMgr->destroyWidget(engine->uiMgr->bossHealth);
    engine->uiMgr->bossHealth = NULL;
}
