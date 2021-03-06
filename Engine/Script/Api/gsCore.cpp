/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2013 Charlie C.

    Contributor(s): Jonathan.
    				Thomas Trocha(dertom)
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "gsCore.h"
#include "gsPhysics.h"
#include "gkCam2ViewportRay.h"
#include "gkMesh.h"
#include "External/Ogre/gkOgreMaterialLoader.h"


gsProperty::gsProperty(gkVariable* var) : m_prop(var), m_creator(false)
{
}


gsProperty::gsProperty() : m_prop(0), m_creator(false)
{
}


gsProperty::gsProperty(const gkString& name, bool value)
{
	m_creator = true;
	m_prop = new gkVariable(name, false);
	m_prop->setValue(value);
}


gsProperty::gsProperty(const gkString& name, double value)
{
	m_creator = true;
	m_prop = new gkVariable(name, false);
	m_prop->setValue((float)value);
}



gsProperty::gsProperty(const gkString& name, const gkString& value)
{
	m_creator = true;
	m_prop = new gkVariable(name, false);
	m_prop->setValue(value);
}



gsProperty::gsProperty(const gsProperty& oth)
{
	m_creator = true;
	m_prop = new gkVariable(oth.getName(), false);
	m_prop->setValue(oth.getValue());
}


gsProperty::~gsProperty()
{
	makeDebug(false);
	if (m_creator)
		delete m_prop;
}



const gkString& gsProperty::getName(void) const
{
	static gkString localStr = "";
	if (m_prop)
		return m_prop->getName();
	return localStr;
}



const gkString& gsProperty::getValue(void) const
{
	static gkString localStr;
	if (m_prop)
		localStr = m_prop->getValueString();
	return localStr;
}



void gsProperty::makeDebug(bool v)
{
	if (m_prop && m_prop->isDebug() != v && gkEngine::getSingletonPtr())
	{
		m_prop->setDebug(v);
		if (v)
			gkEngine::getSingleton().addDebugProperty(m_prop);
		else
			gkEngine::getSingleton().removeDebugProperty(m_prop);
	}
}



gsPropertyType gsProperty::getType(void)  const
{

	if (m_prop)
	{
		switch (m_prop->getType())
		{
		case gkVariable::VAR_BOOL:
			return PROP_BOOL;
		case gkVariable::VAR_INT:
		case gkVariable::VAR_REAL:
			return PROP_NUMBER;
			// TODO
		default:
			break;
		}
		return PROP_STRING;
	}
	return PROP_NULL;
}



bool gsProperty::toBool(void)  const
{
	if (m_prop)
		return m_prop->getValueBool();
	return false;
}



double gsProperty::toNumber(void)  const
{
	if (m_prop)
		return (double)m_prop->getValueReal();
	return -1;

}



gkString gsProperty::toString(void)  const
{
	if (m_prop)
		return m_prop->getValueString();
	return "";
}



void gsProperty::fromBool(bool v)
{
	if (m_prop)
		m_prop->setValue(v);
}



void gsProperty::fromNumber(double v)
{
	if (m_prop)
		m_prop->setValue((gkScalar)v);
}



void gsProperty::fromString(const gkString& v)
{
	if (m_prop)
		m_prop->setValue(v);
}


#define PROP_INSERT_T(name, var, cast)\
    m_properties.insert(name, new gsProperty(name, (cast)m_defs->var));

#define PROP_INSERT_N(name, var) PROP_INSERT_T(name, var, double)
#define PROP_INSERT_B(name, var) PROP_INSERT_T(name, var, bool)
#define PROP_INSERT_S(name, var) PROP_INSERT_T(name, var, gkString)


static void gkVar_fromVar(gkVariable& v, bool& r)
{
	r = v.getValueBool();
}

static void gkVar_fromVar(gkVariable& v, int& r)
{
	r = v.getValueInt();
}
static void gkVar_fromVar(gkVariable& v, gkScalar& r)
{
	r = v.getValueReal();
}
static void gkVar_fromVar(gkVariable& v, gkString& r)
{
	r = v.getValueString();
}


#define PROP_SET_T(name, var, cast)\
    if (v.getName() == name )\
        gkVar_fromVar(v, (cast)m_defs->var);

#define PROP_SET_TE(name, var, cast)\
    if (v.getName() == name )\
    {\
        int c = (int)m_defs->var;\
        gkVar_fromVar(v, c);\
        m_defs->var = (cast)c;\
    }

#define PROP_SET_I(name, var) PROP_SET_T(name, var, int&)
#define PROP_SET_N(name, var) PROP_SET_T(name, var, gkScalar&)
#define PROP_SET_B(name, var) PROP_SET_T(name, var, bool&)
#define PROP_SET_S(name, var) PROP_SET_T(name, var, gkString&)
#define PROP_SET_E(name, var, E) PROP_SET_TE(name, var, E)





gsUserDefs::gsUserDefs(gkUserDefs* defs)
{
	m_defs = defs;

	PROP_INSERT_N("renderSystem",       rendersystem);
	PROP_INSERT_N("sceneManager",       sceneManager);
	PROP_INSERT_B("verbose",            verbose);
	PROP_INSERT_N("winx",               winsize.x);
	PROP_INSERT_N("winy",               winsize.y);
	PROP_INSERT_S("winTitle",           wintitle);
	PROP_INSERT_B("fullScreen",         fullscreen);
	PROP_INSERT_B("blenderMat",         blendermat);
	PROP_INSERT_B("matblending",		matblending);
	PROP_INSERT_B("grabInput",          grabInput);
	PROP_INSERT_B("debugFps",           debugFps);
	PROP_INSERT_B("debugPhysics",       debugPhysics);
	PROP_INSERT_B("debugPhysicsAABB",   debugPhysicsAabb);
	PROP_INSERT_B("usebulletDBVT",      useBulletDbvt);
	PROP_INSERT_B("showDebugProps",     showDebugProps);
	PROP_INSERT_B("debugSounds",        debugSounds);
	PROP_INSERT_B("enableShadows",      enableshadows);
	PROP_INSERT_S("shadowTechnique",    shadowtechnique);
	PROP_INSERT_N("colourShadowR",      colourshadow.r);
	PROP_INSERT_N("colourShadowG",      colourshadow.g);
	PROP_INSERT_N("colourShadowB",      colourshadow.b);
	PROP_INSERT_N("farDistanceShadow",  fardistanceshadow);
}




gsUserDefs::~gsUserDefs()
{
	UTsize p = 0;
	for (; p < m_properties.size(); ++p)
		delete m_properties.at(p);

	m_properties.clear();
}



void gsUserDefs::setValueEvent(gkVariable& v)
{
	if (m_properties.find(gkHashedString(v.getName())) != UT_NPOS)
	{
		PROP_SET_E("renderSystem",       rendersystem, OgreRenderSystem);
		PROP_SET_I("sceneManager",       sceneManager);
		PROP_SET_B("verbose",            verbose);
		PROP_SET_N("winx",               winsize.x);
		PROP_SET_N("winy",               winsize.y);
		PROP_SET_S("winTitle",           wintitle);
		PROP_SET_B("fullScreen",         fullscreen);
		PROP_SET_B("blenderMat",         blendermat);
		PROP_SET_B("grabInput",          grabInput);
		PROP_SET_B("debugFps",           debugFps);
		PROP_SET_B("debugPhysics",       debugPhysics);
		PROP_SET_B("debugPhysicsAABB",   debugPhysicsAabb);
		PROP_SET_B("usebulletDBVT",      useBulletDbvt);
		PROP_SET_B("showDebugProps",     showDebugProps);
		PROP_SET_B("debugSounds",        debugSounds);
		PROP_SET_B("enableShadows",      enableshadows);
		PROP_SET_S("shadowTechnique",    shadowtechnique);
		PROP_SET_N("colourShadowR",      colourshadow.r);
		PROP_SET_N("colourShadowG",      colourshadow.g);
		PROP_SET_N("colourShadowB",      colourshadow.b);
		PROP_SET_N("farDistanceShadow",  fardistanceshadow);
	}
}




const gsProperty& gsUserDefs::getProperty(const gkString& name)
{
	return __getitem__(name.c_str());
}




void gsUserDefs::addProperty(const gsProperty& prop)
{
	if (!prop.getName().empty())
	{
		if (m_properties.find(prop.getName()) == UT_NPOS)
			m_properties.insert(prop.getName(), new gsProperty(prop));
	}
}



bool gsUserDefs::hasProperty(const gkString& name)
{
	return m_properties.find(name) != UT_NPOS;
}



const gsProperty& gsUserDefs::__getitem__(const char* name)
{
	static gsProperty* prop = 0;
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		delete prop;
		prop = new gsProperty((const gsProperty&)(*(m_properties.at(pos))));
	}
	else
	{
		if (prop)
			delete prop;
		prop = new gsProperty();
	}
	return *prop;
}



void gsUserDefs::__setitem__(const char* name, bool  v)
{
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		(m_properties.at(pos))->fromBool(v);
		setValueEvent(m_properties.at(pos)->getClassRef());
	}
}



void gsUserDefs::__setitem__(const char* name, double v)
{
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		(m_properties.at(pos))->fromNumber(v);
		setValueEvent(m_properties.at(pos)->getClassRef());
	}
}



void gsUserDefs::__setitem__(const char* name, const gkString& v)
{
	UTsize pos = 0;
	if ((pos = m_properties.find(gkHashedString(name))) != UT_NPOS)
	{
		(m_properties.at(pos))->fromString(v);
		setValueEvent(m_properties.at(pos)->getClassRef());
	}
}



gsMouse::gsMouse()
	:    xpos(0), ypos(0),
	     xrel(0), yrel(0),
	     winx(0), winy(0),
	     wheel(0), moved(false)
{
}



gsMouse::~gsMouse()
{
}



void gsMouse::capture(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return;

	gkMouse* mse = gkWindowSystem::getSingleton().getMouse();
	xpos    = mse->position.x;
	ypos    = mse->position.y;
	xrel    = mse->relative.x;
	yrel    = mse->relative.y;
	winx    = mse->winsize.x;
	winy    = mse->winsize.y;
	wheel   = mse->wheelDelta;
	moved   = mse->moved;
}


bool gsMouse::isButtonDown(int btn)
{
	if (!gkWindowSystem::getSingletonPtr()) return false;
	return gkWindowSystem::getSingleton().getMouse()->isButtonDown((gsMouseButton)btn);
}



int getNumJoysticks(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return 0;
	return gkWindowSystem::getSingleton().getNumJoysticks();
}

gsJoystick::gsJoystick(int i)
	: m_joystick(1,3), m_index(i)
{
	GK_ASSERT(i >= 0);
	GK_ASSERT(i < getNumJoysticks());
}

gsJoystick::~gsJoystick()
{
}

void gsJoystick::capture(void)
{
	GK_ASSERT(m_index < getNumJoysticks());

	if(m_index >= 0 && m_index < getNumJoysticks())
		m_joystick = *gkWindowSystem::getSingleton().getJoystick(m_index);
}

int gsJoystick::getNumAxes()
{
	return m_joystick.getAxesNumber();
}

int gsJoystick::getAxis(int i)
{
	return m_joystick.getAxisValue(i);
}

int gsJoystick::getRelAxis(int i)
{
	return m_joystick.getRelAxisValue(i);
}

int gsJoystick::getNumButtons()
{
	return m_joystick.getButtonsNumber();
}

int gsJoystick::getButtonCount()
{
	return m_joystick.buttonCount;
}

bool gsJoystick::isButtonDown(int i)
{
	return m_joystick.isButtonDown(i);
}

bool gsJoystick::wasButtonPressed(int i)
{
	return m_joystick.wasButtonPressed(i);
}

gsVector3 gsJoystick::getAccel()
{
	const gkVector3& a = m_joystick.accel;
	return gsVector3(a.x, a.y, a.z);
}

int gsJoystick::getWinWidth(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return 0;
	return gkWindowSystem::getSingleton().getMainWindow()->getWidth();
}

int gsJoystick::getWinHeight(void)
{
	if (!gkWindowSystem::getSingletonPtr()) return 0;
	return gkWindowSystem::getSingleton().getMainWindow()->getHeight();
}



gsKeyboard::gsKeyboard()
{
}


gsKeyboard::~gsKeyboard()
{
}



bool gsKeyboard::isKeyDown(int sc)
{
	if (!gkWindowSystem::getSingletonPtr()) return false;
	return gkWindowSystem::getSingleton().getKeyboard()->isKeyDown((gkScanCode)sc);
}

bool gsKeyboard::isKeyUp(int sc)
{
	if (!gkWindowSystem::getSingletonPtr()) return false;
	return gkWindowSystem::getSingleton().getKeyboard()->isKeyUp((gkScanCode)sc);
}

void gsKeyboard::clearKey(int sc)
{
	if (!gkWindowSystem::getSingletonPtr()) return;
	return gkWindowSystem::getSingleton().getKeyboard()->clearKey((gkScanCode)sc);
}

gsEngine::gsEngine()
{
	m_defs = 0;
	m_running = false;
	if (m_ctxOwner = ((m_engine = gkEngine::getSingletonPtr()) == 0))
		m_engine = new gkEngine();
	else
	{
		m_engine = gkEngine::getSingletonPtr();
		m_running = m_engine->isRunning();
	}

	if (m_engine)
		m_engine->addListener(this);
}



gsEngine::~gsEngine()
{
	if (m_defs)
		delete m_defs;


	for (UTsize i = 0; i < m_ticks.size(); ++i)
		delete m_ticks.at(i);

	if (m_engine)
		m_engine->removeListener(this);

	if (m_ctxOwner)
	{
		m_running = false;
		delete m_engine;
	}
}




void gsEngine::connect(int evt, gsFunction func)
{
	if (evt == EVT_TICK)
		m_ticks.push_back(new gkLuaEvent(func));
}




void gsEngine::connect(int evt, gsSelf self, gsFunction method)
{
	if (evt == EVT_TICK)
		m_ticks.push_back(new gkLuaEvent(self, method));
}



void gsEngine::tick(gkScalar delta)
{
	UTsize i;
	for (i = 0; i < m_ticks.size(); ++i)
	{
		gkLuaEvent* ob = m_ticks.at(i);

		ob->beginCall();
		ob->addArgument(delta);

		if (!ob->call())
		{
			m_ticks.erase(i);
			delete ob;

			if (i > 0)
			{
				i -= 1;
			}
			else
				break;
			continue;
		}
	}
}




void gsEngine::initialize(void)
{
	if (m_ctxOwner && m_engine && !m_engine->isInitialized())
		m_engine->initialize();
}




void gsEngine::requestExit(void)
{
	if (m_engine)
		m_engine->requestExit();
}




void gsEngine::saveTimestampedScreenShot(const gkString& filenamePrefix, const gkString& filenameSuffix)
{
	if (m_engine)
		m_engine->saveTimestampedScreenShot(filenamePrefix, filenameSuffix);
}




gsUserDefs& gsEngine::getUserDefs(void)
{
	if (m_defs == 0)
		m_defs = new gsUserDefs(&m_engine->getUserDefs());
	return *m_defs;
}

gkScene* gsEngine::loadBlendFile(const gkString& name)
{
	if (m_engine) // && m_ctxOwner) // && !m_running
	{
		if (!m_engine->isInitialized())
		{
			gkLogMessage("gsEngine: loadBlendFile on uninitialized engine.");
			return 0;
		}

		int options = gkBlendLoader::LO_ONLY_ACTIVE_SCENE | gkBlendLoader::LO_CREATE_UNIQUE_GROUP;
		gkBlendFile* gkb = gkBlendLoader::getSingleton().loadFile(gkUtils::getFile(name), options);
		if (!gkb)
		{
			gkLogMessage("gsEngine: File Loading failed!\n");
			return 0;
		}
		
		gkScene* scene = gkb->getMainScene();
		gkScene* activeScene = m_engine->getActiveScene();

		if (scene)
		{
			if (activeScene)
				gkSceneManager::getSingleton().copyObjects(scene, activeScene);
			else
				activeScene = scene;
		
			return activeScene;
		}
		else
			gkLogMessage("gsEngine: no usable scenes found in blend.");
	}

	return 0;
}

void gsEngine::unloadBlendFile(const gkString& name)
{
	if (m_engine) // && m_ctxOwner)
	{
		if (!m_engine->isInitialized())
			gkLogMessage("gsEngine: unloadBlendFile on uninitialized engine.");

		gkBlendLoader::getSingleton().unloadFile(name);
	}
}

void gsEngine::unloadAllBlendFiles()
{
	if (m_engine) // && m_ctxOwner)
	{
		if (!m_engine->isInitialized())
			gkLogMessage("gsEngine: unloadAllBlendFiles on uninitialized engine.");

		gkBlendLoader::getSingleton().unloadAll();
	}
}



gkScene* gsEngine::getActiveScene(void)
{
	if (m_engine && m_engine->isInitialized())
		return m_engine->getActiveScene();
	return 0;
}

gkScene* gsEngine::getScene(const gkString& sceneName)
{
	return static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(sceneName));
}




gkScene* gsEngine::addOverlayScene(gsScene* scene)
{
	if (scene)
		return addOverlayScene(scene->getOwner());
	return 0;
}


gkScene* gsEngine::addOverlayScene(const gkString& sceneName)
{
	gkScene* scene = 0;

	scene = static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(sceneName));
	addOverlayScene(scene);
	return scene;
}

gkScene* gsEngine::addOverlayScene(gkScene* scene)
{
	if (scene && !scene->isInstanced())
	{
		gkWindow* win;
		win = m_engine->getActiveScene()->getDisplayWindow();
		int zorder = win->getViewport(win->getViewportCount()-1)->getZOrder();
		scene->destroyInstance(true);
		scene->setDisplayWindow(win, zorder+1);
		scene->createInstance(true);
	}
	return scene;
}

gkScene* gsEngine::addBackgroundScene(const gkString& sceneName)
{
	gkScene* scene = 0;
	scene = (gkScene*)gkSceneManager::getSingleton().getByName(sceneName);
	addBackgroundScene(scene);
	return scene;
}

gkScene* gsEngine::addBackgroundScene(gsScene* scene)
{
	if (scene)
		return addBackgroundScene(scene->getOwner());
	return 0;
}

gkScene*  gsEngine::addBackgroundScene(gkScene* scene)
{
	if (scene && !scene->isInstanced())
	{
		gkWindow* win;
		win = m_engine->getActiveScene()->getDisplayWindow();
		int zorder = win->getViewport(0)->getZOrder();
		scene->setDisplayWindow(win, zorder-1);
		scene->createInstance(true);
	}
	return scene;
}


void gsEngine::removeScene(const gkString& m_sceneName)
{
	gkScene* scene = 0;
	scene = (gkScene*)gkSceneManager::getSingleton().getByName(m_sceneName);
	removeScene(scene);
}

void  gsEngine::removeScene(gkScene* scene)
{
	if (scene)
		scene->destroyInstance(true);
}

void  gsEngine::removeScene(gsScene* scene)
{
	if (scene)
		removeScene(scene->getOwner());
}

void gsEngine::run(void)
{
	if (!m_running && m_engine)
	{
		m_running = true;
		m_engine->run();
	}
}



gsHUD::gsHUD() : m_object(0)
{
}

gsHUD::gsHUD(gkHUD* ob) : m_object(ob)
{
}

void gsHUD::show(bool v)
{
	if (m_object)
		m_object->show(v);
}

gkHUDElement* gsHUD::getChild(const gkString& child)
{
	if (m_object)
	{
		gkHUDElement* ob = m_object->getChild(child);
		return ob;			
	}

	return 0;
}

gsHUDElement::gsHUDElement() : m_object(0)
{
}

gsHUDElement::gsHUDElement(gkHUDElement* ob) : m_object(ob)
{
}

void gsHUDElement::show(bool v)
{
	if (m_object)
		m_object->show(v);
}

gkString gsHUDElement::getValue()
{	
	return m_object ? m_object->getValue() : "";
}

void gsHUDElement::setValue(const gkString& value)
{
	if (m_object)
		m_object->setValue(value);
}

void gsHUDElement::setUvCoords(float u1, float v1, float u2, float v2)
{
	if (m_object)
		m_object->setUvCoords(u1, v1, u2, v2);
}

gkString gsHUDElement::getMaterialName()
{
	if (m_object)
		return m_object->getMaterialName();
	
	return "";
}

void gsHUDElement::setMaterialName(const gkString& material)
{
	if (m_object)
		m_object->setMaterialName(material);
}

float gsHUDElement::getMaterialAlpha()
{
	if (m_object)
		return m_object->getMaterialAlpha();

	return 1.f;
}

int gsHUDElement::getMaterialAlphaRejectValue()
{
	if (m_object)
		return m_object->getMaterialAlphaRejectValue();

	return 0;
}

void gsHUDElement::setMaterialAlphaRejectValue(int val, bool isGreater)
{
	if (m_object)
		m_object->setMaterialAlphaRejectValue(val, isGreater);
}

void gsHUDElement::setMaterialAlpha(float factor)
{
	if (m_object)
		m_object->setMaterialAlpha(factor);
}
	
gkString gsHUDElement::getParameter(const gkString& name)
{
	if (m_object)
		return m_object->getParameter(name);

	return "";
}

void gsHUDElement::setParameter(const gkString& name, const gkString& value)
{
	if (m_object)
		return m_object->setParameter(name, value);
}

gsVector3 gsHUDElement::getPosition()
{
	if (m_object){
		const gkVector2& vec2 = m_object->getPosition();
		mPosition.x = vec2.x;
		mPosition.y = vec2.y;
	}
	return mPosition;
}

void gsHUDElement::setPosition(gsVector3 vec)
{
	setPosition(vec.x,vec.y);
}

void gsHUDElement::setPosition(float x,float y)
{
	if (m_object)
		m_object->setPosition(x,y);
}

gsObject::gsObject() : m_object(0)
{
}



gsObject::gsObject(gkInstancedObject* ob) : m_object(ob)
{
}



void gsObject::createInstance(void)
{
	if (m_object)
	{
		gkEngine* eng = gkEngine::getSingletonPtr();
		m_object->createInstance(eng->isRunning());
	}
}



void gsObject::destroyInstance(void)
{
	if (m_object)
	{
		gkEngine* eng = gkEngine::getSingletonPtr();
		m_object->destroyInstance(eng->isRunning());
	}
}


void gsObject::reinstance(void)
{
	if (m_object)
	{
		gkEngine* eng = gkEngine::getSingletonPtr();
		m_object->reinstance(eng->isRunning());
	}
}


gkString gsObject::getName(void)
{
	if (m_object)
		return m_object->getName();
	return "";
}





gsScene::gsScene() : m_pickRay(0)
#ifdef OGREKIT_USE_PROCESSMANAGER
	, m_processManager(0)
#endif
{
}


gsScene::gsScene(gkInstancedObject* ob) : gsObject(ob),m_pickRay(0)
#ifdef OGREKIT_USE_PROCESSMANAGER
	,m_processManager(0)
#endif
{
}



gsScene::~gsScene()
{
	if (m_pickRay){
		delete m_pickRay;
	}
}

void gsScene::setLayer(int layer)
{
	if (m_object)
		return cast<gkScene>()->setLayer((UTuint32)layer);
}

int gsScene::getLayer()
{
	if (m_object)
		return cast<gkScene>()->getLayer();
	return 0;
}

bool gsScene::hasObject(const gkString& name)
{
	if (m_object)
		return cast<gkScene>()->hasObject(name);
	return false;
}

gkScene* gsScene::getOwner()
{
	if (m_object)
	{
		return cast<gkScene>();
	}
	return 0;
}

#ifdef OGREKIT_USE_PROCESSMANAGER
gkProcessManager* gsScene::getProcessManager()
{
	if (m_object)
	{
		if (!m_processManager)
			m_processManager = cast<gkScene>()->getProcessManager();
		return m_processManager;
	}
	return 0;
}
#endif

gkGameObject* gsScene::getObject(const gkString& name)
{
	if (m_object)
	{
		gkGameObject* gobj = cast<gkScene>()->getObject(name);
		if (gobj)
			return gobj;
	}
	return 0;
}


gkGameObject* gsScene::createEmpty(const gkString& name)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		if (!scene->hasObject(name))
		{
			gkGameObject* obj = scene->createObject(name);
			return obj;
		}
	}

	return 0;
}

gkGameObject* gsScene::createEntity(const gkString& name)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		if (!scene->hasObject(name))
		{
			gkEntity* ent = scene->createEntity(name);

		    gkMesh* mesh = scene->createMesh("mesh_"+name);
		    gkSubMesh* submesh = new gkSubMesh();
		    //submesh->setVertexColors(true);
		    mesh->addSubMesh(submesh);
		    ent->getEntityProperties().m_mesh = mesh;
			return ent;
		}
	}

	return 0;
}

gkLogicManager* gsScene::getLogicBrickManager()
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		return scene->getLogicBrickManager();
	}

	return 0;
}

gsRay* gsScene::getPickRay(float rayX,float rayY){
	if (m_object){

		if (rayX==-1 || rayY==-1)
		{
			gkMouse* mse = gkWindowSystem::getSingleton().getMouse();
			rayX = mse->position.x;
			rayY = mse->position.y;
		}

		gkCam2ViewportRay pickRay = gkCam2ViewportRay(rayX, rayY, 10000, cast<gkScene>());

		if (!m_pickRay){
			m_pickRay = new gsRay;
		}
		m_pickRay->setDirection(pickRay.getDirection());
		m_pickRay->setOrigin(pickRay.getOrigin());
		return m_pickRay;
	}
	return 0;
}

gkGameObject* gsScene::cloneObject(gsGameObject* obj, int lifeSpan, bool instantiate)
{
	if (m_object && obj)
	{
        gkScene*      scene   = cast<gkScene>();
		gkGameObject* gameObj = obj->cast<gkGameObject>();
		if (gameObj)
		{
			gkGameObject* newGameObj = scene->cloneObject(gameObj, lifeSpan, instantiate);
            if (newGameObj)
                return newGameObj;
		}
	}
    
	return 0;    
}


gkDynamicsWorld* gsScene::getDynamicsWorld(void)
{
	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		gkDynamicsWorld* world = scene->getDynamicsWorld();
		return world;
	}

	return 0;
}

gkCamera* gsScene::getMainCamera(void){
	if (m_object) {
		gkScene* scene = cast<gkScene>();
		return scene->getMainCamera();
	}
	return 0;
}

void gsScene::setUpdateFlags(unsigned int flags)
{
	if (m_object)
	{
		cast<gkScene>()->setUpdateFlags(flags);
	}
}

gkScene* getActiveScene(void)
{
	gkEngine* eng = gkEngine::getSingletonPtr();
	if (eng && eng->isInitialized())
		return eng->getActiveScene();
	return 0;
}

gkScene* addScene(gsScene* scene,int zorder) {
	if (scene)
	{
		gkScene* tmpScene = scene->cast<gkScene>();
		if (!tmpScene->isInstanced())
		{
		gkWindow* win;
		win = gkEngine::getSingleton().getActiveScene()->getDisplayWindow();
		tmpScene->setDisplayWindow(win, zorder);
		tmpScene->createInstance(true);
		}
		return tmpScene;
	}
	return 0;
}

gkScene* getScene(const gkString& sceneName)
{
	return static_cast<gkScene*>(gkSceneManager::getSingleton().getByName(sceneName));
}

gkHUD* getHUD(const gkString& name)
{
	gkHUD* hud = gkHUDManager::getSingleton().getOrCreate(name);
	if (hud)
		return hud;
	return 0;
}

gsArray<gsGameObject, gkGameObject> &gsScene::getObjectList(void)
{
	m_objectCache.clear();


	if (m_object)
	{
		gkScene* scene = cast<gkScene>();
		gkGameObjectSet& objs = scene->getInstancedObjects();
		gkGameObjectSet::Iterator it = objs.iterator();

		while (it.hasMoreElements())
		{
			gkGameObject* obj = it.getNext();
			m_objectCache.push(obj);
		}
	}
	return m_objectCache;
}




gsGameObject::gsGameObject()
{
}



gsGameObject::gsGameObject(gkInstancedObject* ob) : gsObject(ob)
{
}




gsVector3 gsGameObject::getPosition(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getPosition());
	return gsVector3();
}



gsVector3 gsGameObject::getRotation(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getRotation().toVector3());
	return gsVector3();
}



gsQuaternion gsGameObject::getOrientation(void)
{
	if (m_object)
		return gsQuaternion(cast<gkGameObject>()->getOrientation());
	return gsQuaternion();
}



gsVector3 gsGameObject::getScale(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getScale());
	return gsVector3();
}



gsVector3 gsGameObject::getWorldPosition(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getWorldPosition());
	return gsVector3();
}



gsVector3 gsGameObject::getWorldRotation(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getWorldRotation().toVector3());
	return gsVector3();
}



gsQuaternion gsGameObject::getWorldOrientation(void)
{
	if (m_object)
		return gsQuaternion(cast<gkGameObject>()->getWorldOrientation());
	return gsQuaternion();
}



gsVector3 gsGameObject::getLinearVelocity(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getLinearVelocity());
	return gsVector3();
}




gsVector3 gsGameObject::getAngularVelocity(void)
{
	if (m_object)
		return gsVector3(cast<gkGameObject>()->getAngularVelocity());
	return gsVector3();
}




void gsGameObject::setLinearVelocity(const gsVector3& v,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setLinearVelocity(v,space);
}



void gsGameObject::setLinearVelocity(float x, float y, float z,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setLinearVelocity(gkVector3(x, y, z),space);
}



void gsGameObject::setAngularVelocity(const gsVector3& v,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setAngularVelocity(v,space);
}



void gsGameObject::setAngularVelocity(float x, float y, float z,gsTransformSpace space)
{
	if (m_object)
		cast<gkGameObject>()->setAngularVelocity(gkVector3(x, y, z),space);
}



void gsGameObject::setPosition(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->setPosition(v);
}



void gsGameObject::setPosition(float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->setPosition(gkVector3(x, y, z));
}

void gsGameObject::lookAt(const gsVector3& v)
{
	if(m_object)
	{
		gkVector3 front = gkVector3(v.x, v.y, v.z) - cast<gkGameObject>()->getWorldPosition();
		gkVector3 up    = gkVector3::UNIT_Z;
		gkVector3 right = front.crossProduct(up);
		up = right.crossProduct(front);
		front.normalise();
		up.normalise();   
		right.normalise();
		cast<gkGameObject>()->setOrientation(gkQuaternion(right, up, -front));
	}
}

void gsGameObject::lookAt(gsGameObject* lookAtObj)
{
	if (m_object)
	{
		if (!lookAtObj)
		{
			gkDebugScreen::printTo("Warning: You passed NIL as lookAt-Object. Ignoring LookAt(..)!");
			return;
		}

		lookAt(lookAtObj->getPosition());
	}
}

void gsGameObject::setRotation(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(gkEuler(v));
}



void gsGameObject::setRotation(float pitch, float yaw, float roll)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(gkEuler(pitch, yaw, roll));
}



void gsGameObject::setOrientation(const gsQuaternion& quat)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(quat);
}



void gsGameObject::setOrientation(float w, float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->setOrientation(gkQuaternion(w, x, y, z));
}

void gsGameObject::setScale(const gsVector3& v)
{
	if (m_object)
	{
		get()->setScale(gkVector3(v.x,v.y,v.z));
	}
}

void gsGameObject::setScale(float x,float y,float z)
{
	if (m_object)
	{
		get()->setScale(gkVector3(x,y,z));
	}
}

gsGameObjectTypes gsGameObject::getType(void)
{
	if (m_object)
		return (gsGameObjectTypes)cast<gkGameObject>()->getType();
	return OBT_UNKNOWN;
}




void gsGameObject::rotate(float dx, float dy, float dz)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(dx, dy, dz), TRANSFORM_PARENT);
}




void gsGameObject::rotate(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(v), TRANSFORM_PARENT);
}



void gsGameObject::rotate(const gsQuaternion& v)
{
	if (m_object)
		cast<gkGameObject>()->rotate(v, TRANSFORM_PARENT);
}



void gsGameObject::rotate(float dx, float dy, float dz, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(dx, dy, dz), ts);
}



void gsGameObject::rotate(const gsVector3& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->rotate(gkEuler(v), ts);
}



void gsGameObject::rotate(const gsQuaternion& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->rotate(v, ts);
}



void gsGameObject::translate(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->translate(v, TRANSFORM_PARENT);
}



void gsGameObject::translate(float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->translate(gkVector3(x, y, z), TRANSFORM_PARENT);
}


void gsGameObject::translate(const gsVector3& v, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->translate(v, ts);
}


void gsGameObject::translate(float x, float y, float z, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->translate(gkVector3(x, y, z), ts);
}


void gsGameObject::scale(const gsVector3& v)
{
	if (m_object)
		cast<gkGameObject>()->scale(v);
}



void gsGameObject::scale(float x, float y, float z)
{
	if (m_object)
		cast<gkGameObject>()->scale(gkVector3(x, y, z));
}



void gsGameObject::yaw(float deg)
{
	if (m_object)
		cast<gkGameObject>()->yaw(gkDegree(deg), TRANSFORM_PARENT);
}




void gsGameObject::yaw(float deg, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->yaw(gkRadian(deg), ts);
}



void gsGameObject::pitch(float deg)
{
	if (m_object)
		cast<gkGameObject>()->pitch(gkRadian(deg), TRANSFORM_PARENT);
}



void gsGameObject::pitch(float deg, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->pitch(gkRadian(deg), ts);
}



void gsGameObject::roll(float deg)
{
	if (m_object)
		cast<gkGameObject>()->roll(gkRadian(deg), TRANSFORM_PARENT);
}



void gsGameObject::roll(float deg, gsTransformSpace ts)
{
	if (m_object)
		cast<gkGameObject>()->roll(gkRadian(deg), ts);
}



gsProperty gsGameObject::__getitem__(const gkString& prop)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (var)
			return gsProperty(var);
	}
	return gsProperty();
}



void gsGameObject::__setitem__(const gkString& prop, bool  v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (var)
			(*var).setValue(v);
	}
}



void gsGameObject::__setitem__(const gkString& prop, float v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (var)
			(*var).setValue(v);
	}
}



void gsGameObject::__setitem__(const gkString& prop, const char* v)
{
	if (m_object)
	{
		gkVariable* var = cast<gkGameObject>()->getVariable(prop);
		if (var)
			(*var).setValue(gkString(v));
	}
}



gkScene* gsGameObject::getScene(void)
{
	if (m_object)
		return cast<gkGameObject>()->getOwner();
	return 0;
}



int gsGameObject::getState(void)
{
	if (m_object)
		return cast<gkGameObject>()->getState();
	return -1;
}

void gsGameObject::changeState(int v)
{
	if (m_object)
		cast<gkGameObject>()->changeState(v);
}

bool gsGameObject::hasParent()
{
	return m_object && cast<gkGameObject>()->getParent() != 0;
}



void gsGameObject::setParent(gsGameObject* par)
{
	if (m_object && par)
	{
		gkGameObject* gobj = get();
		gkGameObject* pobj = par->get();

		if (gobj == pobj) return;

		if (!gobj->hasParent())
			gobj->setParent(pobj);
		else
		{
			if (gobj->getParent() != pobj)
			{
				gobj->getParent()->removeChild(gobj);
				gobj->setParent(pobj);
			}
		}
	}
}



void gsGameObject::setParentInPlace(gsGameObject* par)
{
	if (m_object && par)
	{
		gkGameObject* gobj = get();
		gkGameObject* pobj = par->get();

		if (gobj == pobj) return;

		if (!gobj->hasParent())
			gobj->setParentInPlace(pobj);
		else
		{
			if (gobj->getParent() != pobj)
			{
				gobj->clearParentInPlace();
				gobj->setParentInPlace(pobj);
			}
		}
	}
}



void gsGameObject::clearParent(void)
{
	if (m_object)
	{
		gkGameObject* gobj = get();

		if (gobj->hasParent())
			gobj->clearParent();
	}
}



void gsGameObject::clearParentInPlace(void)
{
	if (m_object)
	{
		gkGameObject* gobj = get();

		if (gobj->hasParent())
			gobj->clearParentInPlace();
	}
}



void gsGameObject::addChild(gsGameObject* chi)
{
	if (m_object && chi)
	{
		gkGameObject* gobj = get();
		gkGameObject* cobj = chi->get();
		if (gobj == cobj) return;

		if (!gobj->hasChild(cobj))
			gobj->addChild(cobj);
	}
}


void gsGameObject::removeChild(gsGameObject* chi)
{
	if (m_object && chi)
	{
		gkGameObject* gobj = get();
		gkGameObject* cobj = chi->get();
		if (gobj == cobj) return;


		if (gobj->hasChild(cobj))
			gobj->removeChild(cobj);
	}
}



gkGameObject* gsGameObject::getParent(void)
{
	return m_object && hasParent() ?
	       cast<gkGameObject>()->getParent() : 0;
}



void gsGameObject::enableContacts(bool v)
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
			ob->enableContactProcessing(v);
	}
}



bool gsGameObject::hasContacts()
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
			return !ob->getContacts().empty();
	}
	return false;
}



bool gsGameObject::hasContact(const gkString& object)
{
	if (m_object)
	{
		gkPhysicsController* ob = get()->getPhysicsController();
		if (ob)
			return ob->collidesWith(object);
	}
	return false;
}


void gsGameObject::playAnimation(const gkString& name, float blend, bool restart)
{
	if (m_object && m_object->isInstanced())
	{
		gkAnimationPlayer* player = get()->getAnimationPlayer(name);
		if (player == 0)
		{
			player = get()->addAnimation(name);
			if (!player)
			{
				gsDebugPrint(gkString("Couldn't find animation with name:"+name).c_str());
				return;
			}
		}
		if(restart){
			player->reset();
		}
		get()->playAnimation(name, blend, restart);
	}
}


gkGameObject* gsGameObject::getChildAt(int pos)
{
	if (m_object)
	{
		if (pos < (int)get()->getChildren().size())
			return get()->getChildren().at(pos);
		else
		{
			gkString error = gkString("Index out of bounds ")+gkToString(pos)+" in "+get()->getName();
			gsDebugPrint(error.c_str());
			return 0;
		}
	}
	return 0;
}

int gsGameObject::getChildCount(void)
{
	if (m_object)
	{
		return get()->getChildren().size();
	}
	return 0;
}

gkGameObject* gsGameObject::getChildByName(const gkString& name)
{
	if (m_object)
	{
		gkGameObjectArray::Iterator it = get()->getChildren().iterator();
		while (it.hasMoreElements())
		{
			gkGameObject* gobj = it.getNext();
			if (gobj->getName() == name)
				return gobj;
		}
	}
	return 0;
}

gkGameObjectInstance* gsGameObject::getGroupInstance()
{
	if (m_object)
	{
		gkGameObjectInstance* ginst = get()->getGroupInstance();
		return ginst;
	}
	return 0;
}

bool  gsGameObject::isGroupInstance()
{
	if (m_object)
	{
		return get()->isGroupInstance();
	}
	return false;
}

gsEntity::gsEntity()
{
}



gsEntity::gsEntity(gkInstancedObject* ob) : gsGameObject(ob)
{
}
gkMesh* gsEntity::getMesh() {
	gkEntity* ent = static_cast<gkEntity*>(get());
	return ent->getMesh();
}
bool gsEntity::hasCharacter(void)
{
	return get()->getAttachedCharacter() != 0;
}

gkCharacter* gsEntity::getCharacter(void)
{
	if (!hasCharacter()) return 0;

	return get()->getAttachedCharacter();
}

void gsEntity::setMaterialName(const gkString& name) {
	static_cast<gkEntity*>(m_object)->setMaterialName(name);
}

void gsSkeleton::attachObjectToBone(const gkString& boneName, gsGameObject* gsobj,gsVector3 loc,gsVector3 orientation,gsVector3 scale)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		gkTransformState* trans = new gkTransformState(loc,gkEuler(orientation).toQuaternion(),scale);
		skel->attachObjectToBone(boneName,gsobj->cast<gkGameObject>(),trans);
	}
}

void gsSkeleton::attachObjectToBoneInPlace(const gkString& boneName, gsGameObject* gsobj)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		skel->attachObjectToBoneInPlace(boneName,gsobj->cast<gkGameObject>());
	}
}


void gsSkeleton::setBoneManual(const gkString& boneName,bool setManual)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);
		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			bone->setManuallyControlled(setManual);
		}
		else gsDebugPrint(gkString("Skeleton doesn't have a bone with name "+boneName).c_str());
	}
}

bool gsSkeleton::isBoneManual(const gkString& boneName)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);

		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			return bone->isManuallyControlled();
		}  // if
		else gsDebugPrint(gkString("Skeleton does have a bone with name "+boneName).c_str());
	}  // if
	return false;
}

void gsSkeleton::applyBoneChannelTransform(const gkString& boneName, gsVector3 loc, gsVector3 orientation,gsVector3 scale, gkScalar pWeight)
{
	if (m_object)
	{
		gkSkeleton* skel = static_cast<gkSkeleton*>(m_object);

		gkBone* bone = skel->getBone(boneName);
		if (bone)
		{
			gkTransformState lState = gkTransformState(loc,gkEuler(orientation).toQuaternion(),scale);
			bone->applyChannelTransform(lState, pWeight);
		}  // if
		else gsDebugPrint(gkString("Skeleton does have a bone with name "+boneName).c_str());
	}  // if
}  // void applyBoneChannelTransform

gsLight::gsLight()
{
}



gsLight::gsLight(gkInstancedObject* ob) : gsGameObject(ob)
{
}


gsCurve::gsCurve()
{
}

gsCurve::gsCurve(gkInstancedObject* ob) : gsGameObject(ob)
{
}

int gsCurve::getPointCount()
{
	if (m_object){
		return static_cast<gkCurve*>(m_object)->getPointCount();
	}
	return -1;
}

bool gsCurve::isCyclic()
{
	if (m_object){
		return static_cast<gkCurve*>(m_object)->isCyclic();
	}
	return false;
}

gsVector3 gsCurve::getPoint(int nr)
{
	if (m_object){
		return gsVector3(static_cast<gkCurve*>(m_object)->getPoint(nr));
	}
	return gsVector3(0,0,0);
}


void  gsCamera::setClipping(float start, float end)
{
	if (m_object)
		cast<gkCamera>()->setClip(gkMax(start, 0.0001f), end);
}



float gsCamera::getClipStart()
{
	if (m_object)
		return cast<gkCamera>()->getCameraProperties().m_clipstart;
	return 0;
}

gsCamera::gsCamera()
{
}


gsCamera::gsCamera(gkInstancedObject* ob) : gsGameObject(ob)
{
}


float gsCamera::getClipEnd()
{
	if (m_object)
		return cast<gkCamera>()->getCameraProperties().m_clipend;
	return 0;
}



void  gsCamera::setFov(float fov)
{
	if (m_object)
		cast<gkCamera>()->setFov(gkDegree(fov));
}



float gsCamera::getFov()
{
	if (m_object)
		return cast<gkCamera>()->getCameraProperties().m_fov;
	return 0;
}



void gsCamera::makeCurrent()
{
	if (m_object)
		cast<gkCamera>()->makeCurrent();
}


gsSkeleton::gsSkeleton()
{
}



gsSkeleton::gsSkeleton(gkInstancedObject* ob) : gsGameObject(ob)
{
}


gsParticles::gsParticles()
{
}



gsParticles::gsParticles(gkInstancedObject* ob) : gsGameObject(ob)
{
}



gsDebugger::gsDebugger(gsScene* sc)
	:   m_debugger(0)
{
	if (sc)
	{
		gkScene* scene = sc->cast<gkScene>();
		if (scene)
			m_debugger = scene->getDebugger();

	}
}




void gsDebugger::drawLine(const gsVector3& from, const gsVector3& to, const gsVector3& color)
{
	if (m_debugger)
		m_debugger->drawLine(from, to, color);
}




void gsDebugger::drawObjectAxis(gsGameObject* ptr, float size)
{
	if (m_debugger && ptr)
	{
		gkGameObject* ob = ptr->cast<gkGameObject>();
		if (ob)
		{
			const gkVector3&     axis   = ob->getWorldPosition();
			const gkQuaternion&  ori    = ob->getOrientation();

			gkVector3 x = (ori * gkVector3(size, 0, 0));
			gkVector3 y = (ori * gkVector3(0, size, 0));
			gkVector3 z = (ori * gkVector3(0, 0, size));


			m_debugger->drawLine(axis, axis + x, gkVector3(1, 0, 0));
			m_debugger->drawLine(axis, axis + y, gkVector3(0, 1, 0));
			m_debugger->drawLine(axis, axis + z, gkVector3(0, 0, 1));
		}
	}
}

void gsDebugger::drawCurve(gsCurve* curve, const gsVector3& color)
{
	if (m_debugger && curve)
		m_debugger->drawCurve(curve->cast<gkCurve>(), gkVector3(color));
}


void gsDebugger::clear(void)
{
	if (m_debugger)
		m_debugger->clear();
}




void gsDebugPrint(const char* str)
{
	gkDebugScreen::printTo(str);
}

void sendMessage(const char* from,const char* to,const char* subject,const char* body){
	gkMessageManager::getSingletonPtr()->sendMessage(from,to,subject,body);
}

bool gsSetCompositorChain(gsCompositorOp op, const gkString& compositorName)
{
#ifdef OGREKIT_USE_COMPOSITOR
	return gkCompositorManager::getSingleton().setCompositorChain((gkCompositorOp)op, compositorName);
#else
	return false;
#endif
}

void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, float value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,value);
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,value);
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}

void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, gsVector3* value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,gkVector3(*value));
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,gkVector3(*value));
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}

void setMaterialParam(const gkString& matName, int shaderType,const gkString& paramName, gsVector4* value)
{
	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (!matPtr.isNull())
	{
		Ogre::Material::TechniqueIterator iter(matPtr.get()->getTechniqueIterator());
		while (iter.hasMoreElements())
		{
			Ogre::Technique* technique = iter.getNext();
			Ogre::Technique::PassIterator passIter(technique->getPassIterator());
			while (passIter.hasMoreElements())
			{
				Ogre::Pass* pass = passIter.getNext();
				if (shaderType == ST_VERTEX)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
					if (!params.isNull()){
						params.get()->setNamedConstant(paramName,gkVector4(value->x,value->y,value->z,value->w));
					}
				}
				else if (shaderType == ST_FRAGMENT)
				{
					Ogre::GpuProgramParametersSharedPtr params = pass->getFragmentProgramParameters();
					if (!params.isNull() && params.get()->_findNamedConstantDefinition(paramName,false)){
						params.get()->setNamedConstant(paramName,gkVector4(value->x,value->y,value->z,value->w));
					}
				}



			}
		}
	} else {
		gkDebugScreen::printTo("Unknown Material:"+matName);
	}
}

gkGameObjectInstance* createGroupInstance(gkString groupName,gsVector3 loc,gsVector3 rot,gsVector3 scale){
	gkEngine* eng = gkEngine::getSingletonPtr();
	if (eng && eng->isInitialized()){
		gkScene* scene = eng->getActiveScene();
		return createGroupInstance(scene,groupName,loc,rot,scale);
	}
	return 0;
}

gkGameObjectInstance* createGroupInstance(gsScene* gScene,gkString groupName,gsVector3 loc,gsVector3 rot,gsVector3 scale){
	if (gScene)
	{
		gkScene* scene = gScene->getOwner();
		if (scene)
			return createGroupInstance(scene,groupName,loc,rot,scale);
	}
	return 0;
}

gkGameObjectInstance* createGroupInstance(gkScene* scene,gkString groupName,gsVector3 loc,gsVector3 rot,gsVector3 scale){
	gkGroupManager* mgr = gkGroupManager::getSingletonPtr();
	if (mgr->exists(groupName))
	{
		gkGameObjectGroup* ggobj = (gkGameObjectGroup*)mgr->getByName(groupName);


		gkEngine* eng = gkEngine::getSingletonPtr();
		if (eng && eng->isInitialized()){
			if (!scene)
				scene = eng->getActiveScene();
			gkGameObjectInstance* inst = ggobj->createGroupInstance(scene, gkResourceName(gkUtils::getUniqueName("gi"+groupName), ""), 0,scene->getLayer());
			gkQuaternion quat;
			quat = gkEuler(gkVector3(rot.x,rot.y,rot.z)).toQuaternion();
			gkVector3 pos(loc.x,loc.y,loc.z);
			gkVector3 scl(scale.x,scale.y,scale.z);
			inst->getRoot()->getProperties().m_transform =  gkTransformState(pos, quat, scl);
			bool b = inst->isInstanced();
			if (!b){
				inst->createInstance(false);
			}

			return inst;
		}
	}
	return NULL;
}


gsGameObjectInstance::gsGameObjectInstance( gkGameObjectInstance* inst ) : m_gobj(inst)
{
}

gsGameObjectInstance::~gsGameObjectInstance() {
}

void gsGameObjectInstance::destroyInstance()
{
	if (m_gobj)
		m_gobj->destroyInstance(true);
}

void gsGameObjectInstance::createInstance()
{
	if (m_gobj)
		m_gobj->createInstance(true);
}
void gsGameObjectInstance::reinstance()
{
	if (m_gobj)
		m_gobj->reinstance(true);
}


int gsGameObjectInstance::getElementCount()
{
	return m_gobj?m_gobj->getObjects().size():0;
}

gkGameObject* gsGameObjectInstance::getElementAt(int pos)
{
	return m_gobj?m_gobj->getObjects().at(pos):NULL;
}

gkGameObject* gsGameObjectInstance::getElementByName(gkString name)
{
	return m_gobj?m_gobj->getObject(name):NULL;
}

gkGameObject* gsGameObjectInstance::getRoot(void)
{
	return m_gobj->getRoot();
}

gkString gsGameObjectInstance::getName()
{
	return m_gobj?m_gobj->getName():"";
}

gsMesh::gsMesh(gkMesh* mesh) : m_mesh(mesh)
{}

void gsMesh::addSubMesh(gsSubMesh* submesh) {
	m_mesh->addSubMesh(submesh->m_submesh);
	// if we add the submesh to the mesh, the mesh is responsible for disposing
	// this from now on:
	submesh->m_isMeshCreator=false;
}

gsSubMesh::gsSubMesh() : m_isMeshCreator(true)
{
	m_submesh = new gkSubMesh;
}

gsSubMesh::gsSubMesh(gkSubMesh* submesh) : m_submesh(submesh), m_isMeshCreator(false)
{}

gsSubMesh::~gsSubMesh()
{
	if (m_isMeshCreator)
		delete m_submesh;
}

const gkTriangle gsSubMesh::addTriangle(const gkVertex& v0,
            unsigned int i0,
            const gkVertex& v1,
            unsigned int i1,
            const gkVertex& v2,
            unsigned int i2) {
	return m_submesh->addTriangle(v0,i0,v1,i1,v2,i2,gkTriangle::TRI_COLLIDER);
}

void gsSubMesh::setMaterialName(const gkString& materialName)
{
	m_submesh->setMaterialName(materialName);
}

bool isMaterialInitialized(const gkString& matName)
{
	Ogre::ResourcePtr resPtr = Ogre::MaterialManager::getSingleton().getByName(matName);
	if (resPtr.isNull())
		return false;
	else
		return true;
}
void initMaterial(const gkString& matName)
{
	if (!isMaterialInitialized(matName)){
		// for now we have to find the material-props by searching all submeshes!!
		// TODO: create gamekit-material-manager

		gkMesh* mesh = 0;

		gkMeshManager::ResourceIterator iter(gkMeshManager::getSingleton().getResourceIterator());
		while (iter.hasMoreElements()){
			gkMesh* mesh = static_cast<gkMesh*>(iter.getNext().second);

			gkMesh::SubMeshIterator subIter(mesh->getSubMeshIterator());
			while (subIter.hasMoreElements()) {
				gkSubMesh* subM = static_cast<gkSubMesh*>(subIter.getNext());

				if (subM->getMaterialName() == matName) {
					// init the material
					// TODO: check if there might be problems due to the group
					gkMaterialLoader::loadSubMeshMaterial(subM, mesh->getGroupName());
				}
			}

		}
	}
}


void import(const gkString& scriptName)
{
	gkLuaScript* script = gkLuaManager::getSingleton().getByName<gkLuaScript>(gkResourceName(scriptName, ""));

	if (script)
		script->execute();
}

gkString getPlatform() {
#if GK_PLATFORM == GK_PLATFORM_WIN32
	   return "WIN32";
#elif GK_PLATFORM == GK_PLATFORM_APPLE
	   return  "APPLE";
#elif GK_PLATFORM == GK_PLATFORM_APPLE_IOS
	   return  "APPLE_IOS";
#elif GK_PLATFORM == GK_PLATFORM_LINUX
	   return  "LINUX";
#elif GK_PLATFORM == GK_PLATFORM_ANDROID
	return "ANDROID";
#else
	return "UNKNOWN";
#endif
}

bool isSoundAvailable() {
#ifdef OGREKIT_OPENAL_SOUND
	return true;
#else
	return false;
#endif
}
