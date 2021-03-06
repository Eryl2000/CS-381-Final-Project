/*
 * NAME: Eryl Kenner
 * EMAIL: eryl.kenner@gmail.com
 */

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreVector3.h>
#include <OrientedPhysics.h>
#include <SdkTrays.h>

#include "Engine.h"
#include "InputMgr.h"
#include "GfxMgr.h"
#include "EntityMgr.h"
#include "UiMgr.h"
#include "GameMgr.h"
#include "AiMgr.h"

#include "Enemy.h"
#include "Player.h"

#include "WeaponHolder.h"
#include "CameraTether.h"
#include "PotentialField.h"
#include "UnitAI.h"

using Ogre::Vector3;

class Command;
class Intercept;

InputMgr::InputMgr(Engine *eng) :
        Mgr(eng),
        mInputMgr(0),
        mMouse(0),
        mKeyboard(0),
        leftMouseHeld(0){

}

InputMgr::~InputMgr(){
    Ogre::WindowEventUtilities::removeWindowEventListener(engine->gfxMgr->mWindow, this);
    windowClosed(engine->gfxMgr->mWindow);
}

void InputMgr::Init(){
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

    OIS::ParamList pl;
    size_t windowHandle = 0;
    std::ostringstream windowHandleStr;

    engine->gfxMgr->mWindow->getCustomAttribute("WINDOW", &windowHandle);
    windowHandleStr << windowHandle;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHandleStr.str()));

    pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true")));
    pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));

    mInputMgr = OIS::InputManager::createInputSystem(pl);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard,
            true));
    mKeyboard->setEventCallback(this);
    mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));
    mMouse->setEventCallback(this);

    pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));

    windowResized(engine->gfxMgr->mWindow);
    Ogre::WindowEventUtilities::addWindowEventListener(engine->gfxMgr->mWindow, this);
}

void InputMgr::Tick(float dt){
    if(engine->gfxMgr->mWindow->isClosed()){
        engine->keepRunning = false;
        return;
    }
    mKeyboard->capture();
    mMouse->capture();
    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE)){
        engine->keepRunning = false;
        return;
    }

    mouseLocation = GetClickedPosition(lastMousePos);

    if(leftMouseHeld){
        engine->gameMgr->MainPlayer->GetAspect<WeaponHolder>()->UseWeapon();
    }

    UpdatePlayer(dt);
}

void InputMgr::UpdatePlayer(float dt){
    static bool ctrlR_DownLastFrame = false;
    bool ctrlR = mKeyboard->isKeyDown(OIS::KC_LCONTROL)
            && mKeyboard->isKeyDown(OIS::KC_R);
    Player *player = engine->gameMgr->MainPlayer;

    Vector3 movement = Vector3::ZERO;
    if(mKeyboard->isKeyDown(OIS::KC_A)){
        movement.x -= 1;
    }
    if(mKeyboard->isKeyDown(OIS::KC_D)){
        movement.x += 1;
    }
    if(mKeyboard->isKeyDown(OIS::KC_W)){
        movement.z -= 1;
    }
    if(mKeyboard->isKeyDown(OIS::KC_S)){
        movement.z += 1;
    }

    // movement == zero, will tell the player to go back to idle animation
    player->Move(movement, dt);

    if(mKeyboard->isKeyDown(OIS::KC_UP)){
        player->GetAspect<CameraTether>()->Height += 400 * dt;
    }
    if(mKeyboard->isKeyDown(OIS::KC_DOWN)){
        player->GetAspect<CameraTether>()->Height -= 400 * dt;
    }
    if(ctrlR && !ctrlR_DownLastFrame){
        player->GetAspect<PotentialField>()->enabled ^= true;
    }
    ctrlR_DownLastFrame = ctrlR;

}

void InputMgr::LoadLevel(){

}

void InputMgr::Stop(){
    if(mInputMgr != NULL){
        mInputMgr->destroyInputObject(mMouse);
        mInputMgr->destroyInputObject(mKeyboard);

        OIS::InputManager::destroyInputSystem(mInputMgr);
        mInputMgr = 0;
    }
}

void InputMgr::windowResized(Ogre::RenderWindow* rw){
    int left, top;
    unsigned int width, height, depth;

    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void InputMgr::windowClosed(Ogre::RenderWindow* rw){
    if(rw == engine->gfxMgr->mWindow){
        if(mInputMgr){
            mInputMgr->destroyInputObject(mMouse);
            mInputMgr->destroyInputObject(mKeyboard);

            OIS::InputManager::destroyInputSystem(mInputMgr);
            mInputMgr = 0;
        }
    }
}

std::pair<bool, Ogre::Vector3> InputMgr::GetClickedPosition(Ogre::Vector2 mousePosition){
    Ogre::Ray mouseRay = engine->gfxMgr->mCamera->getCameraToViewportRay(mousePosition.x,
            mousePosition.y);
    std::pair<bool, Ogre::Real> point = mouseRay.intersects(engine->gameMgr->mPlane);

    if(point.first){
        Ogre::Vector3 intersect = mouseRay.getPoint(point.second);
        return std::pair<bool, Ogre::Vector3>(true, intersect);
    } else{
        return std::pair<bool, Ogre::Vector3>(false, Ogre::Vector3::ZERO);
    }
}

std::pair<bool, Ogre::Vector3> InputMgr::GetClickedPosition(const OIS::MouseEvent& me){
    Ogre::Viewport* vp = engine->gfxMgr->mViewport;
    Ogre::Vector2 mousePosition = Ogre::Vector2(
            me.state.X.abs / Ogre::Real(vp->getActualWidth()),
            me.state.Y.abs / Ogre::Real(vp->getActualHeight()));
    lastMousePos = mousePosition;
    return GetClickedPosition(mousePosition);
}

bool InputMgr::mouseMoved(const OIS::MouseEvent& me){
    if(engine->uiMgr->mTrayMgr->injectMouseMove(me))
        return true;

    mouseLocation = GetClickedPosition(me);
    if(mouseLocation.first){
        engine->gameMgr->MainPlayer->LookAt(mouseLocation.second);
    }

    return true;
}

bool InputMgr::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id){
    if(engine->uiMgr->mTrayMgr->injectMouseUp(me, id))
        return true;

    mouseLocation = GetClickedPosition(me);
    if(id == OIS::MouseButtonID::MB_Left){
        leftMouseHeld = false;
    }

    return true;
}

bool InputMgr::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id){
    if(engine->uiMgr->mTrayMgr->injectMouseDown(me, id))
        return true;

    mouseLocation = GetClickedPosition(me);
    if(id == OIS::MouseButtonID::MB_Left){
        leftMouseHeld = true;
    }

    if(id == OIS::MouseButtonID::MB_Right){
        engine->gameMgr->MainPlayer->GetAspect<WeaponHolder>()->ThrowWeapon();
    }

    /*
     std::pair<bool, Ogre::Vector3> intersection = GetClickedPosition(me);
     if(intersection.first){
     Ogre::Vector3 cameraPos = engine->gfxMgr->mCameraNode->getPosition();
     Ogre::Vector3 intersect = intersection.second;

     float shortestDistance = -1;
     Entity381 *best = NULL;
     for(unsigned int i = 0; i < (engine->entityMgr->entities).size(); ++i){
     Entity381 *cur = engine->entityMgr->entities[i];
     Ogre::Vector3 pos = cur->position;
     float distance = (pos - intersect).crossProduct(pos - cameraPos).length()
     / (intersect - cameraPos).length();
     if((distance < shortestDistance || shortestDistance == -1)
     && distance <= selectionDistanceThreshold){
     shortestDistance = distance;
     best = cur;
     }
     }

     if(id == OIS::MouseButtonID::MB_Left){
     if(shortestDistance >= 0){
     engine->entityMgr->entities[engine->entityMgr->currentEntity]->isSelected =
     false;
     engine->entityMgr->currentEntity = best->entityId;
     engine->entityMgr->entities[engine->entityMgr->currentEntity]->isSelected =
     true;
     }
     } else if(id == OIS::MouseButtonID::MB_Right){
     Entity381 *cur = engine->entityMgr->entities[engine->entityMgr->currentEntity];
     UnitAI *unitAI = cur->GetAspect<UnitAI>();
     if(unitAI != NULL){
     if(shortestDistance >= 0){
     unitAI->SetCommand(new Intercept(cur, best));
     } else{
     if(mKeyboard->isKeyDown(OIS::KC_LSHIFT)){
     unitAI->AddCommand(new MoveTo(cur, intersect));
     } else{
     unitAI->SetCommand(new MoveTo(cur, intersect));
     }
     }
     }
     }
     //engine->entityMgr->CreateEntityOfTypeAtPosition(EntityType::Sphere, intersect);
     }

     */
    return true;
}

bool InputMgr::keyPressed(const OIS::KeyEvent& ke){

    switch(ke.key) {
        case OIS::KC_SPACE:
            //engine->gameMgr->MainPlayer->GetAspect<WeaponHolder>()->ReloadWeapon();
            std::cout << engine->gameMgr->MainPlayer->position << std::endl;
            break;
        case OIS::KC_1:
            engine->gameMgr->LoadLevelOne();
            break;
        case OIS::KC_2:
            engine->gameMgr->LoadLevelTwo();
            break;
        case OIS::KC_P:
            engine->entityMgr->RemoveAllEnemies();
            break;
        default:
            break;
    }
    return true;
}

bool InputMgr::keyReleased(const OIS::KeyEvent& ke){
    return true;
}

