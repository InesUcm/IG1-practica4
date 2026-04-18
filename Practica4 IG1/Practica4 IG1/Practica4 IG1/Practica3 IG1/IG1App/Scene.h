#ifndef _H_Scene_H_
#define _H_Scene_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Entity.h"

#include <vector>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene();

	Scene(const Scene& s) = delete;
	Scene& operator=(const Scene& s) = delete;

	virtual void init();
	virtual void update();

	virtual void render(Camera const& cam) const;

	void load();
	void unload();

protected:
	void destroy();
	void setGL();
	void resetGL();

	std::vector<Abs_Entity*> gObjects;
};

class Scene1 : public Scene {
public:
	void init() override;
};

class Scene2 : public Scene {
public:
	void init() override;
};

class Scene3 : public Scene {
public:
	void init() override;
};

class Scene4 : public Scene {
public:
	void init() override;
	void render(Camera const& cam) const override;
	//sobrescribimos update para que no se acyualice la foto en cada frame
	void update() override;
	//booleano para saber si la foto ya ha sido capturada 
	//false = ya ha sido capturada, no hay que capturarla ora vez
	//true = la escena se ha movido y hay que capturar la foto otra vez
	mutable bool mCaptureNext = false;
	//metodo para indicar que hay que capturar la foto otra vez
	void setCapture() { mCaptureNext = true; }
};

#endif //_H_Scene_H_