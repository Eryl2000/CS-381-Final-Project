/*
 * NAME: Eryl Kenner
 * EMAIL: eryl.kenner@gmail.com
 */

#ifndef INC_ENTITY381_H_
#define INC_ENTITY381_H_

#include <string>
#include <vector>
#include "Ogre.h"
#include "Aspect.h"

class UnitAI;
class EntityMgr;

class Entity381{
public:
    Entity381(int id, std::string mesh, Ogre::Vector3 pos, EntityMgr *entMgr);
    virtual ~Entity381();
    void Tick(float dt);

    template<class AspectType>
    AspectType* GetAspect(){
        AspectType *ret = NULL;
        for(unsigned int i = 0; i < aspects.size(); ++i){
            ret = dynamic_cast<AspectType *>(aspects[i]);
            if(ret != NULL){
                return ret;
            }
        }
        return NULL;
    }

    int entityId;
    std::string meshName;
    Ogre::Vector3 position;
    Ogre::SceneNode *ogreSceneNode;
    Ogre::Entity *ogreEntity;
    std::vector<Aspect*> aspects;
    bool isSelected;

    EntityMgr *entityMgr;

private:
protected:
    virtual void InitAspects() = 0;
};

class SphereEntity381 : public Entity381{
public:
    SphereEntity381(int id, Ogre::Vector3 pos, EntityMgr *entMgr);
    ~SphereEntity381();
    virtual void InitAspects();
private:
};

class CubeEntity381 : public Entity381{
public:
    CubeEntity381(int id, Ogre::Vector3 pos, EntityMgr *entMgr);
    ~CubeEntity381();
    virtual void InitAspects();
private:
};

#endif /* INC_ENTITY381_H_ */
