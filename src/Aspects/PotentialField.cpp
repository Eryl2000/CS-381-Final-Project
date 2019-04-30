/*
 * NAME: Eryl Kenner
 * EMAIL: eryl.kenner@gmail.com
 */

#include "PotentialField.h"
#include "OgreVector2.h"
#include "Entity381.h"
#include "OrientedPhysics3D.h"
#include <algorithm>

double PotentialField::c_Enemy_Enemy_repulsion = 1000000;
double PotentialField::e_Enemy_Enemy_repulsion = -3;
double PotentialField::c_Enemy_Enemy_attraction = 0;
double PotentialField::e_Enemy_Enemy_attraction = 1;

double PotentialField::c_Enemy_Target_repulsion = 0;
double PotentialField::e_Enemy_Target_repulsion = 1;
double PotentialField::c_Enemy_Target_attraction = 100/*10000*/;
double PotentialField::e_Enemy_Target_attraction = 0/*-1*/;

double PotentialField::c_Enemy_Obstacle_repulsion = 500000;
double PotentialField::e_Enemy_Obstacle_repulsion = -2;
double PotentialField::c_Enemy_Obstacle_attraction = 0;
double PotentialField::e_Enemy_Obstacle_attraction = 1;

std::vector<PotentialField *> PotentialField::potentialFields;

PotentialField::PotentialField(Entity381 *ent, PotentialFieldType t) :
        Aspect(ent),
        type(t),
        enabled(true){
    potentialFields.push_back(this);
}

PotentialField::~PotentialField(){
    potentialFields.erase(
            std::remove(potentialFields.begin(), potentialFields.end(), this),
            potentialFields.end());
}

void PotentialField::Tick(float dt){
    if(type != PotentialFieldType::Enemy){
        return;
    }
    Ogre::Vector2 vectorSum = Ogre::Vector2::ZERO;
    for(unsigned int i = 0; i < potentialFields.size(); ++i){
        if(potentialFields[i] == this || !potentialFields[i]->enabled){
            continue;
        }
        double c_rep = 0;
        double e_rep = 0;
        double c_attr = 0;
        double e_attr = 0;
        switch(potentialFields[i]->type) {
            case PotentialFieldType::Target:
                c_rep = PotentialField::c_Enemy_Target_repulsion;
                e_rep = PotentialField::e_Enemy_Target_repulsion;
                c_attr = PotentialField::c_Enemy_Target_attraction;
                e_attr = PotentialField::e_Enemy_Target_attraction;
                break;
            case PotentialFieldType::Obstacle:
                c_rep = PotentialField::c_Enemy_Obstacle_repulsion;
                e_rep = PotentialField::e_Enemy_Obstacle_repulsion;
                c_attr = PotentialField::c_Enemy_Obstacle_attraction;
                e_attr = PotentialField::e_Enemy_Obstacle_attraction;
                break;
            case PotentialFieldType::Enemy:
                c_rep = PotentialField::c_Enemy_Enemy_repulsion;
                e_rep = PotentialField::e_Enemy_Enemy_repulsion;
                c_attr = PotentialField::c_Enemy_Enemy_attraction;
                e_attr = PotentialField::e_Enemy_Enemy_attraction;
                break;
        }

        Ogre::Vector2 diff = Ogre::Vector2(
                entity381->position.x - potentialFields[i]->entity381->position.x,
                entity381->position.z - potentialFields[i]->entity381->position.z);
        diff.x /= potentialFields[i]->entity381->scale.x;
        diff.y /= potentialFields[i]->entity381->scale.z;
        const double d = diff.length();
        if(d >= 100.0 && potentialFields[i]->type == PotentialFieldType::Enemy){
            continue;
        }
        const double attr = c_attr * Ogre::Math::Pow(d, e_attr);
        const double rep = c_rep * Ogre::Math::Pow(d, e_rep);
        vectorSum += (rep - attr) * diff.normalisedCopy();
    }

    //std::cout << vectorSum << " ";
    OrientedPhysics3D *physics = entity381->GetAspect<OrientedPhysics3D>();
    if(physics != NULL){
        if(vectorSum != Ogre::Vector2::ZERO){
            physics->desiredHeading = Ogre::Degree(
                    Ogre::Math::ATan2(vectorSum.y, vectorSum.x)).valueDegrees();
            float angleDiff = physics->desiredHeading - physics->heading;
            if(angleDiff < 0){
                angleDiff += 360;
            }
            if(angleDiff > 180){
                angleDiff = 360 - angleDiff;
            }
            physics->desiredSpeed = physics->maxSpeed * (1.5 - angleDiff / 180);
        } else{
            physics->desiredSpeed = 0;
        }
    }
}

