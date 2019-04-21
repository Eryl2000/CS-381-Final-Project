/*
 * WeaponHolder.cpp
 *
 *  Created on: Apr 19, 2019
 *      Author: kylebrain
 */

#include "WeaponHolder.h"

WeaponHolder::WeaponHolder(Entity381 * entity) :
        Aspect(entity),
        heldWeapon(0),
        timeSinceLastShot(0) {

}
WeaponHolder::~WeaponHolder(){
}

void WeaponHolder::Tick(float dt){
    timeSinceLastShot += dt;
}

void WeaponHolder::UseWeapon() {
    if(heldWeapon != NULL && timeSinceLastShot >= heldWeapon->UseRate) {
        timeSinceLastShot = 0;
        heldWeapon->Use();
    }
}

void WeaponHolder::SetWeapon(Weapon * weapon){
    heldWeapon = weapon;
    //Ogre::Vector3 resultPosition = entity381->ogreSceneNode->convertWorldToLocalPosition(heldWeapon->ogreSceneNode->getPosition());
    //Ogre::Quaternion resultOrientation = entity381->ogreSceneNode->convertWorldToLocalOrientation(heldWeapon->ogreSceneNode->getOrientation());

    timeSinceLastShot = heldWeapon->UseRate;
    heldWeapon->ogreSceneNode->getParentSceneNode()->removeChild(
            heldWeapon->ogreSceneNode);
    entity381->ogreSceneNode->addChild(heldWeapon->ogreSceneNode);
    heldWeapon->ogreSceneNode->setOrientation(Ogre::Quaternion());
    heldWeapon->position = Ogre::Vector3(48, 95, -15);
    heldWeapon->ogreSceneNode->yaw(Ogre::Degree(-20));

    std::cout << heldWeapon->ogreSceneNode->getOrientation() << std::endl;

    heldWeapon->ogreSceneNode->setInheritScale(false);
}

