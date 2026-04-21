#pragma once
#include "Mesh.h"
#include <numbers>

class IndexMesh : public Mesh
{
public:
	void load() override;
	void unload() override;
	void draw() const override;

	// Apt. 59: normales por método de Newell sobre los triángulos indexados
	void buildNormalVectors();

	// Apt. 55: malla de revolución indexada
	static IndexMesh* generateByRevolution(
		const std::vector<glm::vec2>& profile,
		GLuint nSamples,
		GLfloat angleMax = 2 * std::numbers::pi_v<float>);

	// Apt. 61: cubo indexado con 8 vértices compartidos (normales por Newell)
	static IndexMesh* generateIndexedBox8(GLdouble l);

	// Apt. 64: cubo indexado con 24 vértices (4 por cara, sin compartir)
	static IndexMesh* generateIndexedBox(GLdouble l);

	// Apt. 67: esfera con coordenadas de textura
	static IndexMesh* generateSphere(GLdouble radius, GLuint nParallels, GLuint nMeridians);

private:
	std::vector<GLuint> vIndexes; // tabla de índices
	GLuint mIBO = std::numeric_limits<GLuint>::max(); // index buffer object
};