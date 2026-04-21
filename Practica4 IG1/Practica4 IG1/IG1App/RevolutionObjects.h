#pragma once
#include "Entity.h"
#include <string>

// ─── Apt. 60: Torus con iluminación ───
class Torus : public ColorMaterialEntity
{
public:
	explicit Torus(GLdouble R, GLdouble r, GLuint nPoints = 40, GLuint nSamples = 40);
};

// ─── Apt. 65: Esfera por revolución ───
class Sphere : public ColorMaterialEntity
{
public:
	explicit Sphere(GLdouble radius, GLuint nParallels = 20, GLuint nMeridians = 20);
};

// ─── Apt. 65: Disco / corona circular ───
class Disk : public ColorMaterialEntity
{
public:
	explicit Disk(GLdouble R, GLdouble r, GLuint nRings, GLuint nSamples);
};

// ─── Apt. 65: Cono / cilindro / tronco de cono ───
class Cone : public ColorMaterialEntity
{
public:
	explicit Cone(GLdouble h, GLdouble r, GLdouble R, GLuint nRings, GLuint nSamples);
};

// ─── Apt. 67: Esfera con textura ───
class SphereWithTexture : public EntityWithTexture
{
public:
	explicit SphereWithTexture(GLdouble radius, GLuint nParallels, GLuint nMeridians,
		const std::string& texFile);
};

// ─── Apt. 68: Droide (entidad compuesta) ───
// Correo profesora: cabeza = cono truncado + disco (completamente amarillo)
class Droid : public CompoundEntity
{
public:
	explicit Droid(GLdouble radius);
};