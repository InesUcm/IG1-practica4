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

// ─── Droid (apt. 68) ──────────────────────────────────────────────────────────
//
// Exactamente según el enunciado:
//   - Esfera texturizada, centro en el origen
//   - Cono truncado amarillo: Cone(radius, radius/2, radius/2, nRings, radius)
//     → h=radius, r_inf=radius/2, r_sup=radius/2 → CILINDRO de radio radius/2 y altura radius
//     → "con origen en el centro de coordenadas" → sin traslación, base en Y=0
//     → La cima queda en Y=+radius, que es el polo norte de la esfera
//   - Correo profesora: añadir un Disk amarillo para tapar la cima (Y=+radius)
//     porque el cilindro de revolución queda abierto arriba y abajo
//     La base en Y=0 está dentro de la esfera y no se ve → solo tapamos la cima
//   - Dos cilindros verdes (0,204,0): apuntan en +Z (revolución en Y → rotamos -90° en X)
//     posicionados a los lados de la cabeza a la altura de Y=+radius (polo norte)

Droid::Droid(GLdouble radius)
{
	// ── 1. Cuerpo: esfera texturizada centrada en el origen ───────────────────
	SphereWithTexture* body = new SphereWithTexture(
		radius, 20, 20, "..\\assets\\images\\container.jpg");
	addEntity(body);

	// ── 2. Cabeza: cono truncado (cilindro) amarillo ──────────────────────────
	// Exactamente Cone(radius, radius/2, radius/2, nRings, nSamples):
	//   h = radius, r_inf = radius/2, r_sup = radius/2 → cilindro
	// Sin traslación: la base queda en Y=0 (interior de la esfera, no visible),
	// la cima queda en Y=+radius (polo norte, sobresale por encima de la esfera).
	GLdouble h = radius;
	GLdouble rCyl = radius / 2.0;
	Cone* hat = new Cone(h, rCyl, rCyl, 4, (GLuint)radius);
	hat->setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)); // amarillo
	// Sin setModelMat → identidad (origen en el centro de la esfera)
	addEntity(hat);

	// ── 3. Disco tapa: cierra la CIMA del cilindro en Y=+radius ──────────────
	// El correo de la profesora dice que la cabeza debe ser "completamente amarilla".
	// generateByRevolution genera el disco en Y=0 con normales en +Y.
	// Para cerrarlo en la cima (Y=+radius) basta con trasladarlo a Y=+radius;
	// la normal en +Y es la correcta (mira hacia fuera, hacia arriba).
	Disk* topDisk = new Disk(rCyl, 0.0, 2, (GLuint)radius);
	topDisk->setColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	topDisk->setModelMat(
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)radius, 0.0f)));
	addEntity(topDisk);

	// ── 4 & 5. Cilindros verdes (ojos/antenas) apuntando en +Z ───────────────
	// Posición: junto a la cima del cilindro dorado (Y ≈ +radius),
	// desplazados lateralmente en X para que queden uno a cada lado.
	// generateByRevolution genera en eje Y → rotar -90° en X para que apunten en +Z.
	// Orden de la matriz: T * R (primero rotamos, luego trasladamos al sitio final).
	GLdouble cLen = radius * 0.6;       // longitud del ojo
	GLdouble cRad = radius * 0.1;       // radio del ojo
	GLdouble offX = radius * 0.25;      // separación lateral entre los dos
	GLdouble offY = radius;             // misma altura que la cima del cilindro dorado

	for (int lado : {-1, 1}) {
		Cone* cyl = new Cone(cLen, cRad, cRad, 3, 12); // r_inf==r_sup → cilindro puro
		cyl->setColor(glm::vec4(0.0f, 204.0f / 255.0f, 0.0f, 1.0f)); // verde (0,204,0)

		// T * R: rotamos -90° en X (Y→Z), luego trasladamos al lado correspondiente
		cyl->setModelMat(
			glm::translate(glm::mat4(1.0f),
				glm::vec3((float)(lado * offX), (float)offY, 0.0f))
			* glm::rotate(glm::mat4(1.0f),
				glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
		addEntity(cyl);
	}
}