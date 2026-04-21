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

	// Llamado por display() ANTES de glClear para establecer el color de fondo.
	// Cada escena puede sobrescribirlo. Por defecto: azul claro.
	virtual void setClearColor() const { glClearColor(0.6f, 0.7f, 0.8f, 1.0f); }

	void load();
	void unload();

protected:
	void destroy();
	void setGL();
	void resetGL();

	std::vector<Abs_Entity*> gObjects;
};

class Scene1 : public Scene { public: void init() override; };
class Scene2 : public Scene { public: void init() override; };
class Scene3 : public Scene { public: void init() override; };

class Scene4 : public Scene {
public:
	void init() override;
	void render(Camera const& cam) const override;
	void update() override;
	mutable bool mCaptureNext = false;
	void setCapture() { mCaptureNext = true; }
};

class Scene5 : public Scene { public: void init() override; };
class Scene6 : public Scene { public: void init() override; };
class Scene7 : public Scene { public: void init() override; };

class Scene8 : public Scene {
public:
	void init() override;
	void render(Camera const& cam) const override;
	void update() override;

	// Fondo negro solo para esta escena
	void setClearColor() const override { glClearColor(0.0f, 0.0f, 0.0f, 1.0f); }

	void rotate(); // tecla F: droide gira sobre sí mismo (nodo ficticio en Y)
	void orbit();  // tecla G: droide avanza por la superficie (nodo ficticio en X)

private:
	CompoundEntity* mNodoFicticio = nullptr; // acceso directo al nodo ficticio
};

#endif //_H_Scene_H_
