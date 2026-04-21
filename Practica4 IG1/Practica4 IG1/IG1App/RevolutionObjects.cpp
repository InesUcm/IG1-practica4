#include "RevolutionObjects.h"
#include "IndexMesh.h"
#include <numbers>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Torus::Torus(GLdouble R, GLdouble r, GLuint nPoints, GLuint nSamples)
	: ColorMaterialEntity(glm::vec4(0, 1, 0, 1))
{
	std::vector<glm::vec2> profile;
	profile.reserve(nPoints + 1);
	double angle = 2.0 * std::numbers::pi / nPoints;
	for (GLuint i = 0; i <= nPoints; ++i)
		profile.emplace_back(
			(GLfloat)(r * cos(angle * i) + R),
			(GLfloat)(r * sin(angle * i)));
	mMesh = IndexMesh::generateByRevolution(profile, nSamples);
}

Sphere::Sphere(GLdouble radius, GLuint nParallels, GLuint nMeridians)
{
	std::vector<glm::vec2> profile;
	profile.reserve(nParallels + 1);
	for (GLuint i = 0; i <= nParallels; ++i) {
		float theta = (float)(-std::numbers::pi / 2.0 + std::numbers::pi * i / nParallels);
		profile.emplace_back((float)(radius * cos(theta)), (float)(radius * sin(theta)));
	}
	mMesh = IndexMesh::generateByRevolution(profile, nMeridians);
}

Disk::Disk(GLdouble R, GLdouble r, GLuint nRings, GLuint nSamples)
{
	std::vector<glm::vec2> profile;
	profile.reserve(nRings);
	for (GLuint i = 0; i < nRings; ++i) {
		float t = (nRings > 1) ? (float)i / (nRings - 1) : 0.0f;
		float rad = (float)(r + t * (R - r));
		profile.emplace_back(rad, 0.0f);
	}
	mMesh = IndexMesh::generateByRevolution(profile, nSamples);
}

Cone::Cone(GLdouble h, GLdouble r_inf, GLdouble r_sup, GLuint nRings, GLuint nSamples)
{
	std::vector<glm::vec2> profile;
	profile.reserve(nRings);
	for (GLuint i = 0; i < nRings; ++i) {
		float t = (nRings > 1) ? (float)i / (nRings - 1) : 0.0f;
		float rad = (float)(r_inf + t * (r_sup - r_inf));
		float y = (float)(t * h);
		profile.emplace_back(rad, y);
	}
	mMesh = IndexMesh::generateByRevolution(profile, nSamples);
}

SphereWithTexture::SphereWithTexture(GLdouble radius, GLuint nParallels, GLuint nMeridians,
	const std::string& texFile)
{
	mMesh = IndexMesh::generateSphere(radius, nParallels, nMeridians);
	mTexture = new Texture();
	mTexture->load(texFile);
}

// ─── Droid ────────────────────────────────────────────────────────────────────
//
//  Estructura según el enunciado (apt. 68) y correo de la profesora:
//
//  · Esfera texturizada (container.jpg), centrada en el origen.
//    Su polo norte está en Y = +radius.
//
//  · Cono truncado amarillo (cabeza):
//    - Base inferior r=radius, base superior r=radius/2, altura h=radius.
//    - generateByRevolution lo genera de Y=0 a Y=h en local.
//    - Lo trasladamos +radius en Y para que su base coincida con el polo norte.
//
//  · Disco amarillo (tapa la base inferior del cono):
//    - Igual que el cono, su plano local es Y=0.
//    - Misma traslación +radius en Y → queda pegado al polo norte de la esfera.
//    - Esto elimina la "franja oscura" que mencionaba la profesora.
//
//  · Dos cilindros verdes (brazos), a la altura del ecuador (Y=0):
//    - Se generan en eje Y local (de 0 a cLen).
//    - Brazo izquierdo: R(-90°,Z) → eje local Y apunta en -X global.
//      T(-radius, 0, 0) coloca el inicio en el borde izquierdo de la esfera.
//    - Brazo derecho: R(+90°,Z) → eje local Y apunta en +X global.
//      T(+radius, 0, 0) coloca el inicio en el borde derecho de la esfera.
//    - Orden: T * R (primero se aplica R, luego T — orden de multiplicación).

Droid::Droid(GLdouble radius)
{
	// ── 1. Esfera texturizada (cuerpo) ──────────────────────────────────────
	SphereWithTexture* body = new SphereWithTexture(
		radius, 20, 20, "..\\assets\\images\\container.jpg");
	addEntity(body);

	// ── 2. Cono truncado amarillo (cabeza) ──────────────────────────────────
	// Parámetros: h=radius, r_inf=radius, r_sup=radius/2, nRings=4, nSamples=20
	// Traslación +radius en Y para colocar la base en el polo norte de la esfera.
	Cone* hat = new Cone(radius, radius, radius / 2.0, 4, 20);
	hat->setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	hat->setModelMat(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)radius, 0.0f)));
	addEntity(hat);

	// ── 3. Disco amarillo (tapa la base del cono en Y=radius) ───────────────
	// Radio exterior = radius, radio interior = 0 (disco sólido), 2 anillos, 20 muestras.
	// Misma traslación +radius en Y que el cono para que coincidan.
	Disk* baseDisk = new Disk(radius, 0.0, 2, 20);
	baseDisk->setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	baseDisk->setModelMat(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)radius, 0.0f)));
	addEntity(baseDisk);

	// ── 4. Brazo izquierdo (cilindro verde, apunta en -X) ───────────────────
	// Longitud y radio proporcionales al tamaño del droide.
	GLdouble cLen = radius;
	GLdouble cRad = radius * 0.08;

	// R(-90°, Z): rota el eje Y local para que apunte en -X global.
	// T(-radius, 0, 0): desplaza al borde izquierdo de la esfera.
	// El cilindro crece desde X=-radius hasta X=-(radius+cLen).
	Cone* cyl1 = new Cone(cLen, cRad, cRad, 2, 12);
	cyl1->setColor(glm::vec4(0.0f, 204.0f / 255.0f, 0.0f, 1.0f));
	cyl1->setModelMat(
		glm::translate(glm::mat4(1.0f), glm::vec3(-(float)radius, 0.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	addEntity(cyl1);

	// ── 5. Brazo derecho (cilindro verde, apunta en +X) ─────────────────────
	// R(+90°, Z): rota el eje Y local para que apunte en +X global.
	// T(+radius, 0, 0): desplaza al borde derecho de la esfera.
	// El cilindro crece desde X=+radius hasta X=+(radius+cLen).
	Cone* cyl2 = new Cone(cLen, cRad, cRad, 2, 12);
	cyl2->setColor(glm::vec4(0.0f, 204.0f / 255.0f, 0.0f, 1.0f));
	cyl2->setModelMat(
		glm::translate(glm::mat4(1.0f), glm::vec3((float)radius, 0.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	addEntity(cyl2);
}