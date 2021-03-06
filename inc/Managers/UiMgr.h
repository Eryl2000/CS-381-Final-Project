/*
 * UiMgr.h
 *
 *  Created on: Apr 12, 2017
 *      Author: chad
 */

#ifndef INC_UIMGR_H_
#define INC_UIMGR_H_

#include <OgreWindowEventUtilities.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <Mgr.h>
#include <string>

#include <SdkTrays.h>

class UiMgr : public Mgr,
              public OIS::KeyListener,
              public OIS::MouseListener,
              public Ogre::WindowEventListener,
              public OgreBites::SdkTrayListener{
private:
    float waitTime;
    float currentTime = 0;
    std::string waveNum, ammoNum, timeElapsed, fps;
    int currentHealth;
    OgreBites::ProgressBar* playerHealth;

protected:
    virtual void windowResized(Ogre::RenderWindow *rw);
    virtual void windowClosed(Ogre::RenderWindow *rw);

    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    void buttonHit(OgreBites::Button *b);
    void itemSelected(OgreBites::SelectMenu *m);
    void EnableHud();

public:

    UiMgr(Engine *engine);
    ~UiMgr();
    virtual void Init();
    virtual void Tick(float dt);
    virtual void LoadLevel();
    virtual void stop();

    bool splashScreenDisable = false;
	void ClosingScreen();
	void Menu();
	bool gameStarted = false, screenClosed = false;
	bool startGame = false;
    void splashScreen(float dt);
    void UpdateLabels();


	OgreBites::InputContext mInputContext;
	OgreBites::SdkTrayManager* mTrayMgr;
	OgreBites::SdkTrayManager* closingScreenMgr;
	OgreBites::Label *you_died;
	OgreBites::Label *time_survived;
	OgreBites::Label *waves_survived;
	Ogre::OverlaySystem* mOverlaySystem;
	OgreBites::Label *waveLabel;
	OgreBites::Label *timeLabel;
	OgreBites::Label *weaponLabel;
	OgreBites::Label *ammoLabel;

	OgreBites::Button *StartButton;
	OgreBites::Button *EndButton;
	OgreBites::Label *fpsLabel;
	OgreBites::ProgressBar * bossHealth;

};

#endif /* INC_UIMGR_H_ */
