#include "Scene.h"
#include "RGBObjects.h"
#include "objects2D.h"
#include "objects3D.h"
#include "RevolutionObjects.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// ─── Scene base ───────────────────────────────────────────────────────────────

void Scene::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
}

Scene::~Scene()
{
	destroy();
	resetGL();
}

void Scene::destroy()
{
	for (Abs_Entity* el : gObjects) delete el;
	gObjects.clear();
}

void Scene::load()
{
	for (Abs_Entity* obj : gObjects) obj->load();
}

void Scene::unload()
{
	for (Abs_Entity* obj : gObjects) obj->unload();
}

void Scene::setGL()
{
	glClearColor(0.6f, 0.7f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void Scene::resetGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
}

void Scene::render(Camera const& cam) const
{
	cam.upload();
	for (Abs_Entity* el : gObjects)
		el->render(cam.viewMat());
}

void Scene::update()
{
	for (Abs_Entity* obj : gObjects)
		obj->update();
}

// ─── Scene1 ───────────────────────────────────────────────────────────────────

void Scene1::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	RegularPolygon* hexagono = new RegularPolygon(6, 200.0);
	hexagono->setColor(glm::vec4(255.0f, 0.0f, 255.0f, 1.0f));
	gObjects.push_back(hexagono);

	RegularPolygon* circulo = new RegularPolygon(50, 200.0);
	circulo->setColor(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f));
	gObjects.push_back(circulo);
}

// ─── Scene2 ───────────────────────────────────────────────────────────────────

void Scene2::init()
{
	setGL();
	const float r = 200.0f;
	gObjects.push_back(new RGBAxes(400.0));

	RGBTriangle* triangulo = new RGBTriangle(70.0, r);
	triangulo->setModelMat(translate(mat4(1.0f), vec3(r, 0.0f, 0.0f)));
	gObjects.push_back(triangulo);

	gObjects.push_back(new RGBRectangle(280.0, 280.0));

	RegularPolygon* circulo = new RegularPolygon(50, r);
	circulo->setColor(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f));
	gObjects.push_back(circulo);
}

// ─── Scene3 ───────────────────────────────────────────────────────────────────

void Scene3::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new RGBCube(200.0));
}

// ─── Scene4 ───────────────────────────────────────────────────────────────────

void Scene4::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new Ground(400.0, 400.0));

	BoxOutline* caja = new BoxOutline(100.0);
	caja->setModelMat(translate(mat4(1.0f), vec3(-100.0f, 50.0f, -100.0f)));
	gObjects.push_back(caja);

	Star3D* estrella = new Star3D(40.0, 8, 40.0);
	estrella->setModelMat(translate(mat4(1.0f), vec3(-100.0f, 50.0f, -100.0f)));
	gObjects.push_back(estrella);

	GlassParapet* cristalera = new GlassParapet(400.0);
	cristalera->setModelMat(scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f)));
	gObjects.push_back(cristalera);

	Photo* foto = new Photo(100.0, 75.0);
	mat4 fotoMat = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
	fotoMat = fotoMat * rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	foto->setModelMat(fotoMat);
	gObjects.push_back(foto);
}

void Scene4::render(Camera const& cam) const
{
	cam.upload();
	for (size_t i = 0; i + 1 < gObjects.size(); ++i)
		gObjects[i]->render(cam.viewMat());
	if (mCaptureNext) {
		glFlush();
		gObjects.back()->update();
		mCaptureNext = false;
	}
	gObjects.back()->render(cam.viewMat());
}

void Scene4::update()
{
	for (size_t i = 0; i + 1 < gObjects.size(); i++)
		gObjects[i]->update();
}

// ─── Scene5: Toro ─────────────────────────────────────────────────────────────

void Scene5::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new Torus(100.0, 50.0, 20, 100));
}

// ─── Scene6: IndexedBox ───────────────────────────────────────────────────────

void Scene6::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new IndexedBox(200.0));
}

// ─── Scene7: Droide ───────────────────────────────────────────────────────────

void Scene7::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));
	gObjects.push_back(new Droid(80.0));
}

// ─── Scene8: Planeta Dathomir ─────────────────────────────────────────────────

void Scene8::init()
{
	glEnable(GL_DEPTH_TEST);

	gObjects.push_back(new RGBAxes(400.0));

	const float rPlaneta = 200.0f;
	const float rDroide = 30.0f;

	Sphere* planeta = new Sphere(rPlaneta, 40, 40);
	planeta->setColor(glm::vec4(171.0f / 255.0f, 33.0f / 255.0f, 72.0f / 255.0f, 1.0f));
	gObjects.push_back(planeta);

	mNodoFicticio = new CompoundEntity();

	Droid* droide = new Droid(rDroide);
	droide->setModelMat(translate(mat4(1.0f),
		vec3(0.0f, rPlaneta + rDroide, 0.0f)));
	mNodoFicticio->addEntity(droide);

	gObjects.push_back(mNodoFicticio);
}

void Scene8::render(Camera const& cam) const
{
	cam.upload();
	for (Abs_Entity* el : gObjects)
		el->render(cam.viewMat());
}

void Scene8::update()
{
	orbit();
}

void Scene8::rotate()
{
	glm::mat4 m = mNodoFicticio->modelMat();
	m = glm::rotate(m, glm::radians(5.0f), glm::vec3(0, 1, 0));
	mNodoFicticio->setModelMat(m);
}

void Scene8::orbit()
{
	glm::mat4 m = mNodoFicticio->modelMat();
	m = glm::rotate(m, glm::radians(2.0f), glm::vec3(1, 0, 0));
	mNodoFicticio->setModelMat(m);
}