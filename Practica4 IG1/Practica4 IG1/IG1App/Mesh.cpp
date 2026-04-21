#include "Mesh.h"

using namespace std;
using namespace glm;

constexpr GLuint NONE = numeric_limits<GLuint>::max();

Mesh::Mesh()
	: mVAO(NONE)
	, mVBO(NONE)
	, mCBO(NONE)
	, mTCO(NONE)
	, mNBO(NONE)
{
}

Mesh::~Mesh()
{
	unload();
}

void Mesh::draw() const
{
	glDrawArrays(mPrimitive, 0, size());
}

void Mesh::load()
{
	assert(mVBO == NONE);

	if (vVertices.size() > 0) {
		glGenBuffers(1, &mVBO);
		glGenVertexArrays(1, &mVAO);

		glBindVertexArray(mVAO);

		// Atributo 0: posiciones
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER,
			vVertices.size() * sizeof(vec3), vVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
		glEnableVertexAttribArray(0);

		// Atributo 1: colores (solo si existen)
		if (vColors.size() > 0) {
			glGenBuffers(1, &mCBO);
			glBindBuffer(GL_ARRAY_BUFFER, mCBO);
			glBufferData(GL_ARRAY_BUFFER,
				vColors.size() * sizeof(vec4), vColors.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), nullptr);
			glEnableVertexAttribArray(1);
		}

		// Atributo 2: coordenadas de textura (solo si existen)
		// CORRECCIÓN CRÍTICA: antes se subía siempre aunque vTexCoords estuviera vacío,
		// lo que activaba el atributo 2 con un buffer vacío y corrompía el estado del VAO,
		// impidiendo que las normales (atributo 3) llegaran bien al shader → luz plana.
		if (vTexCoords.size() > 0) {
			glGenBuffers(1, &mTCO);
			glBindBuffer(GL_ARRAY_BUFFER, mTCO);
			glBufferData(GL_ARRAY_BUFFER,
				vTexCoords.size() * sizeof(vec2), vTexCoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), nullptr);
			glEnableVertexAttribArray(2);
		}

		// Atributo 3: normales (solo si existen)
		if (vNormals.size() > 0) {
			glGenBuffers(1, &mNBO);
			glBindBuffer(GL_ARRAY_BUFFER, mNBO);
			glBufferData(GL_ARRAY_BUFFER,
				vNormals.size() * sizeof(vec3), vNormals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
			glEnableVertexAttribArray(3);
		}

		glBindVertexArray(0); // desligar el VAO al terminar
	}
}

void Mesh::unload()
{
	if (mVAO != NONE) {
		glDeleteVertexArrays(1, &mVAO); mVAO = NONE;
		glDeleteBuffers(1, &mVBO);      mVBO = NONE;
		if (mCBO != NONE) { glDeleteBuffers(1, &mCBO); mCBO = NONE; }
		if (mTCO != NONE) { glDeleteBuffers(1, &mTCO); mTCO = NONE; }
		if (mNBO != NONE) { glDeleteBuffers(1, &mNBO); mNBO = NONE; }
	}
}

void Mesh::render() const
{
	assert(mVAO != NONE);
	glBindVertexArray(mVAO);
	draw();
}

Mesh* Mesh::createRGBAxes(GLdouble l)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_LINES;
	mesh->mNumVertices = 6;
	mesh->vVertices.reserve(6);
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0); mesh->vVertices.emplace_back(l, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0); mesh->vVertices.emplace_back(0.0, l, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0); mesh->vVertices.emplace_back(0.0, 0.0, l);
	mesh->vColors.reserve(6);
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0); mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0); mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0); mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);
	return mesh;
}

Mesh* Mesh::generateRegularPolygon(GLuint num, GLdouble r, GLuint primitiva)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = primitiva;
	mesh->mNumVertices = num;
	mesh->vVertices.reserve(num);
	GLdouble angulo = 90.0;
	for (GLuint i = 0; i < num; i++) {
		mesh->vVertices.emplace_back(r * cos(radians(angulo)), r * sin(radians(angulo)), 0.0);
		angulo += 360.0 / num;
	}
	if (primitiva == GL_TRIANGLES) {
		mesh->vColors.push_back({ 1,0,0,1 });
		mesh->vColors.push_back({ 0,1,0,1 });
		mesh->vColors.push_back({ 0,0,1,1 });
	}
	return mesh;
}

Mesh* Mesh::generateRectangle(GLdouble w, GLdouble h)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_STRIP;
	mesh->mNumVertices = 4;
	mesh->vVertices.reserve(4);
	GLdouble hw = w / 2.0, hh = h / 2.0;
	mesh->vVertices.emplace_back(-hw, hh, 0.0);
	mesh->vVertices.emplace_back(-hw, -hh, 0.0);
	mesh->vVertices.emplace_back(hw, hh, 0.0);
	mesh->vVertices.emplace_back(hw, -hh, 0.0);
	return mesh;
}

Mesh* Mesh::generateRGBRectangle(GLdouble w, GLdouble h)
{
	Mesh* mesh = generateRectangle(w, h);
	mesh->vColors.reserve(4);
	mesh->vColors.push_back({ 1,0,0,1 });
	mesh->vColors.push_back({ 0,1,0,1 });
	mesh->vColors.push_back({ 0,1,0,1 });
	mesh->vColors.push_back({ 0,0,1,1 });
	return mesh;
}

Mesh* Mesh::generateRectangleTexCor(GLdouble w, GLdouble h, GLuint rw, GLuint rh)
{
	Mesh* m = generateRectangle(w, h);
	m->vTexCoords.reserve(4);
	m->vTexCoords.emplace_back(0, rh);
	m->vTexCoords.emplace_back(0, 0);
	m->vTexCoords.emplace_back(rw, rh);
	m->vTexCoords.emplace_back(rw, 0);
	return m;
}

Mesh* Mesh::generateCube(GLdouble length)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLES;
	mesh->mNumVertices = 36;
	mesh->vVertices.reserve(36);
	GLdouble l = length / 2.0;
	// +Z
	mesh->vVertices.emplace_back(-l, -l, l); mesh->vVertices.emplace_back(l, -l, l); mesh->vVertices.emplace_back(l, l, l);
	mesh->vVertices.emplace_back(-l, -l, l); mesh->vVertices.emplace_back(l, l, l); mesh->vVertices.emplace_back(-l, l, l);
	// -Z
	mesh->vVertices.emplace_back(l, -l, -l); mesh->vVertices.emplace_back(-l, -l, -l); mesh->vVertices.emplace_back(-l, l, -l);
	mesh->vVertices.emplace_back(l, -l, -l); mesh->vVertices.emplace_back(-l, l, -l); mesh->vVertices.emplace_back(l, l, -l);
	// +X
	mesh->vVertices.emplace_back(l, -l, l); mesh->vVertices.emplace_back(l, -l, -l); mesh->vVertices.emplace_back(l, l, -l);
	mesh->vVertices.emplace_back(l, -l, l); mesh->vVertices.emplace_back(l, l, -l); mesh->vVertices.emplace_back(l, l, l);
	// -X
	mesh->vVertices.emplace_back(-l, -l, -l); mesh->vVertices.emplace_back(-l, -l, l); mesh->vVertices.emplace_back(-l, l, l);
	mesh->vVertices.emplace_back(-l, -l, -l); mesh->vVertices.emplace_back(-l, l, l); mesh->vVertices.emplace_back(-l, l, -l);
	// +Y
	mesh->vVertices.emplace_back(-l, l, l); mesh->vVertices.emplace_back(l, l, l); mesh->vVertices.emplace_back(l, l, -l);
	mesh->vVertices.emplace_back(-l, l, l); mesh->vVertices.emplace_back(l, l, -l); mesh->vVertices.emplace_back(-l, l, -l);
	// -Y
	mesh->vVertices.emplace_back(-l, -l, -l); mesh->vVertices.emplace_back(l, -l, -l); mesh->vVertices.emplace_back(l, -l, l);
	mesh->vVertices.emplace_back(-l, -l, -l); mesh->vVertices.emplace_back(l, -l, l); mesh->vVertices.emplace_back(-l, -l, l);
	return mesh;
}

Mesh* Mesh::generateRGBCubeTriangles(GLdouble length)
{
	Mesh* mesh = generateCube(length);
	mesh->vColors.reserve(36);
	for (int i = 0; i < 12; i++) mesh->vColors.push_back({ 1,0,0,1 });
	for (int i = 0; i < 12; i++) mesh->vColors.push_back({ 0,0,1,1 });
	for (int i = 0; i < 12; i++) mesh->vColors.push_back({ 0,1,0,1 });
	return mesh;
}

Mesh* Mesh::generateBoxOutline(GLdouble length)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_STRIP;
	mesh->mNumVertices = 10;
	mesh->vVertices.reserve(10);
	GLdouble l = length / 2.0;
	mesh->vVertices.push_back({ -l, l, l }); mesh->vVertices.push_back({ -l,-l, l });
	mesh->vVertices.push_back({ l, l, l }); mesh->vVertices.push_back({ l,-l, l });
	mesh->vVertices.push_back({ l, l,-l }); mesh->vVertices.push_back({ l,-l,-l });
	mesh->vVertices.push_back({ -l, l,-l }); mesh->vVertices.push_back({ -l,-l,-l });
	mesh->vVertices.push_back({ -l, l, l }); mesh->vVertices.push_back({ -l,-l, l });
	return mesh;
}

Mesh* Mesh::generateBoxOutlineTexCor(GLdouble length)
{
	Mesh* m = generateBoxOutline(length);
	m->vTexCoords.reserve(10);
	m->vTexCoords.push_back({ 0.0f,1.0f }); m->vTexCoords.push_back({ 0.0f,0.0f });
	m->vTexCoords.push_back({ 1.0f,1.0f }); m->vTexCoords.push_back({ 1.0f,0.0f });
	m->vTexCoords.push_back({ 2.0f,1.0f }); m->vTexCoords.push_back({ 2.0f,0.0f });
	m->vTexCoords.push_back({ 3.0f,1.0f }); m->vTexCoords.push_back({ 3.0f,0.0f });
	m->vTexCoords.push_back({ 4.0f,1.0f }); m->vTexCoords.push_back({ 4.0f,0.0f });
	return m;
}

Mesh* Mesh::generateStar3D(GLdouble re, GLuint np, GLdouble h)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_FAN;
	int total = 2 * np;
	mesh->mNumVertices = 2 + total;
	mesh->vVertices.reserve(mesh->mNumVertices);
	GLdouble ri = re / 2.0;
	GLdouble step = 360.0 / total;
	mesh->vVertices.push_back({ 0,0,0 });
	for (int i = 0; i <= total; i++) {
		GLdouble r = (i % 2 == 0) ? re : ri;
		GLdouble a = radians(90.0 + i * step);
		mesh->vVertices.push_back({ (float)(r * cos(a)), (float)(r * sin(a)), (float)h });
	}
	return mesh;
}

Mesh* Mesh::generateStar3DTexCor(GLdouble re, GLuint np, GLdouble h)
{
	Mesh* m = generateStar3D(re, np, h);
	int total = 2 * np;
	m->vTexCoords.reserve(m->mNumVertices);
	GLdouble step = 360.0 / total;
	m->vTexCoords.push_back({ 0.5f,0.5f });
	for (int i = 0; i <= total; i++) {
		GLdouble r = (i % 2 == 0) ? 0.5 : 0.25;
		GLdouble a = radians(90.0 + i * step);
		m->vTexCoords.push_back({ (float)(0.5 + r * cos(a)), (float)(0.5 + r * sin(a)) });
	}
	return m;
}