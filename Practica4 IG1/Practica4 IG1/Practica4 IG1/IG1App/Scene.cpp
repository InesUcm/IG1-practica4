#include "Scene.h"
#include "RGBObjects.h"
#include "objects2D.h"
#include "objects3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

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

// Libera la memoria de todas las entidades
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
	glClearColor(0.6f, 0.7f, 0.8f, 1.0f); // fondo azul claro, totalmente opaco
	glEnable(GL_DEPTH_TEST);               // activar test de profundidad (z-buffer)
}

void Scene::resetGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH_TEST);
}

void Scene::render(Camera const& cam) const
{
	cam.upload(); // sube viewport, view matrix y projection matrix
	for (Abs_Entity* el : gObjects)
		el->render(cam.viewMat());
}

void Scene::update()
{
	for (Abs_Entity* obj : gObjects)
		obj->update();
}


void Scene1::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	// Hexágono magenta
	RegularPolygon* hexagono = new RegularPolygon(6, 200.0);
	hexagono->setColor(glm::vec4(255.0f, 0.0f, 255.0f, 1.0f));
	gObjects.push_back(hexagono);

	// Círculo amarillo
	RegularPolygon* circulo = new RegularPolygon(50, 200.0);
	circulo->setColor(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f));
	gObjects.push_back(circulo);
}


void Scene2::init()
{
	setGL();
	const float r = 200.0f; // radio

	gObjects.push_back(new RGBAxes(400.0));

	// Triángulo desplazado a la derecha
	RGBTriangle* triangulo = new RGBTriangle(70.0, r);
	triangulo->setModelMat(translate(mat4(1.0f), vec3(r, 0.0f, 0.0f)));
	gObjects.push_back(triangulo);

	// Rectángulo
	gObjects.push_back(new RGBRectangle(280.0, 280.0));

	// Círculo amarillo
	RegularPolygon* circulo = new RegularPolygon(50, r);
	circulo->setColor(glm::vec4(255.0f, 255.0f, 0.0f, 1.0f));
	gObjects.push_back(circulo);
}


void Scene3::init()
{
	setGL();
	gObjects.push_back(new RGBAxes(400.0));

	// Cubo
	gObjects.push_back(new RGBCube(200.0));
}


void Scene4::init()
{
	setGL();

	gObjects.push_back(new RGBAxes(400.0));

	// Suelo
	gObjects.push_back(new Ground(400.0, 400.0));

	// Caja con textura exterior e interior
	BoxOutline* caja = new BoxOutline(100.0);
	caja->setModelMat(translate(mat4(1.0f), vec3(-100.0f, 50.0f, -100.0f)));
	gObjects.push_back(caja);

	// Estrella 3D
	Star3D* estrella = new Star3D(40.0, 8, 40.0);
	estrella->setModelMat(translate(mat4(1.0f), vec3(-100.0f, 50.0f, -100.0f)));
	gObjects.push_back(estrella);

	// Cristalera
	GlassParapet* cristalera = new GlassParapet(400.0);
	cristalera->setModelMat(scale(mat4(1.0f), vec3(1.0f, 0.5f, 1.0f)));
	gObjects.push_back(cristalera);

	// Foto sobre el suelo
	Photo* foto = new Photo(100.0, 75.0);
	mat4 fotoMat = translate(mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
	fotoMat = fotoMat * rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	foto->setModelMat(fotoMat);
	gObjects.push_back(foto);
}

void Scene4::render(Camera const& cam) const
{
	cam.upload();

	// Renderizar todo excepto el último objeto (la foto).
	for (size_t i = 0; i + 1 < gObjects.size(); ++i)
		gObjects[i]->render(cam.viewMat());

	//Si hay que capturar la escena otra vez
	if (mCaptureNext) {
		// Capturar el buffer con lo que se acaba de renderizar
		glFlush();
		gObjects.back()->update();

		mCaptureNext = false;
	}
	// Ahora renderizar la foto con la captura
	gObjects.back()->render(cam.viewMat());
}

void
Scene4::update() {
	// Solo actualizamos los objetos que NO sean la foto, para avitar carga de la GPU por la foto
	for (size_t i = 0; i + 1 < gObjects.size(); i++) {
		gObjects[i]->update();
	}
}
