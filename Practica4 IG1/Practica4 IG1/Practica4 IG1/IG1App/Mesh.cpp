#include "Mesh.h"

using namespace std;
using namespace glm;

// Placeholder for the pending index of a GPU object
constexpr GLuint NONE = numeric_limits<GLuint>::max();

Mesh::Mesh()
 : mVAO(NONE)
 , mVBO(NONE)
 , mCBO(NONE)
 , mTCO(NONE) // ARREGLO: inicializar mTCO a NONE
{
}

Mesh::~Mesh()
{
	unload();
}

void
Mesh::draw() const
{
	glDrawArrays(
	  mPrimitive,
	  0,
	  size()); // primitive graphic, first index and number of elements to be rendered
}

void
Mesh::load()
{
	assert(mVBO == NONE); // not already loaded

	if (vVertices.size() > 0) { // transfer data
		glGenBuffers(1, &mVBO);
		glGenVertexArrays(1, &mVAO);

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, vVertices.size() * sizeof(vec3), vVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
		glEnableVertexAttribArray(0);

		if (vColors.size() > 0) {             // upload colors
			glGenBuffers(1, &mCBO);

			glBindBuffer(GL_ARRAY_BUFFER, mCBO);
			glBufferData(GL_ARRAY_BUFFER, vColors.size() * sizeof(vec4), vColors.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), nullptr);
			glEnableVertexAttribArray(1);
		}
			glGenBuffers(1, &mTCO);
			glBindBuffer(GL_ARRAY_BUFFER, mTCO);
			glBufferData(GL_ARRAY_BUFFER,
				vTexCoords.size() * sizeof(vec2),
				vTexCoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
				sizeof(vec2), nullptr);
			glEnableVertexAttribArray(2);

		
	}
}

void
Mesh::unload()
{
	if (mVAO != NONE) {
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVBO);
		mVAO = NONE;
		mVBO = NONE;

		if (mCBO != NONE) {
			glDeleteBuffers(1, &mCBO);
			mCBO = NONE;
		}

		// ARREGLO: liberar mTCO si fue creado
		if (mTCO != NONE) {
			glDeleteBuffers(1, &mTCO);
			mTCO = NONE;
		}
	}
}

void
Mesh::render() const
{
	assert(mVAO != NONE);

	glBindVertexArray(mVAO);
	draw();
}

Mesh*
Mesh::createRGBAxes(GLdouble l)
{
	Mesh* mesh = new Mesh();

	mesh->mPrimitive = GL_LINES;

	mesh->mNumVertices = 6;
	mesh->vVertices.reserve(mesh->mNumVertices);

	// X axis vertices
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(l, 0.0, 0.0);
	// Y axis vertices
	mesh->vVertices.emplace_back(0, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, l, 0.0);
	// Z axis vertices
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, l);

	mesh->vColors.reserve(mesh->mNumVertices);
	// X axis color: red  (Alpha = 1 : fully opaque)
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	// Y axis color: green
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	// Z axis color: blue
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	return mesh;
}

Mesh* Mesh::generateRegularPolygon(GLuint num, GLdouble r, GLuint primitiva)
{

	Mesh* mesh = new Mesh();

	mesh->mPrimitive = primitiva;

	mesh->mNumVertices = num;
	mesh->vVertices.reserve(mesh->mNumVertices);

	GLdouble angulo = 90.0;
	GLdouble x, y;

	for (GLuint i = 0; i < num; i++)
	{
		x = r * cos(radians(angulo));
		y = r * sin(radians(angulo));
		mesh->vVertices.emplace_back(x, y, 0.0);
		angulo += 360.0/num;
	}

	if (primitiva == GL_TRIANGLES) 
	{
		mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
		mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
		mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	}
	return mesh;
}

Mesh* Mesh::generateRectangle(GLdouble w, GLdouble h) 
{
	Mesh* mesh = new Mesh();

	mesh->mPrimitive = GL_TRIANGLE_STRIP;
	int num = 4;
	mesh->mNumVertices = num;
	mesh->vVertices.reserve(mesh->mNumVertices);

	GLdouble altura = h / 2.0;
	GLdouble ancho = w / 2.0;

	mesh->vVertices.emplace_back(-ancho, altura, 0.0);
	mesh->vVertices.emplace_back(-ancho, -altura, 0.0);
	mesh->vVertices.emplace_back(ancho, altura, 0.0);
	mesh->vVertices.emplace_back(ancho, -altura, 0.0);

	return mesh;
}

Mesh* Mesh::generateRGBRectangle(GLdouble w, GLdouble h) 
{
	Mesh* mesh = generateRectangle(w, h);

	mesh->vColors.reserve(mesh->mNumVertices);

	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul

	return mesh;
}
Mesh* Mesh::generateRectangleTexCor(GLdouble w, GLdouble h, GLuint rw, GLuint rh) 
{
	Mesh* m = generateRectangle(w, h);

	m->vTexCoords.reserve(m->mNumVertices);

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
	// 6 caras × 2 triángulos × 3 vértices = 36
	mesh->mNumVertices = 36;
	mesh->vVertices.reserve(mesh->mNumVertices);

	GLdouble lado = length / 2.0;

	//Cara1
	mesh->vVertices.emplace_back(-lado, -lado, lado);//Triángulo1
	mesh->vVertices.emplace_back(lado, -lado, lado);
	mesh->vVertices.emplace_back(lado, lado, lado);

	mesh->vVertices.emplace_back(-lado, -lado, lado);//Triángulo2
	mesh->vVertices.emplace_back(lado, lado, lado);
	mesh->vVertices.emplace_back(-lado, lado, lado);

	//Cara2
	mesh->vVertices.emplace_back(lado, -lado, -lado);//Triángulo1
	mesh->vVertices.emplace_back(-lado, -lado, -lado);
	mesh->vVertices.emplace_back(-lado, lado, -lado);

	mesh->vVertices.emplace_back(lado, -lado, -lado);//Triángulo2
	mesh->vVertices.emplace_back(-lado, lado, -lado);
	mesh->vVertices.emplace_back(lado, lado, -lado);

	//Cara3
	mesh->vVertices.emplace_back(lado, -lado, lado);//Triángulo1
	mesh->vVertices.emplace_back(lado, -lado, -lado);
	mesh->vVertices.emplace_back(lado, lado, -lado);

	mesh->vVertices.emplace_back(lado, -lado, lado);//Triángulo2
	mesh->vVertices.emplace_back(lado, lado, -lado);
	mesh->vVertices.emplace_back(lado, lado, lado);

	//Cara4
	mesh->vVertices.emplace_back(-lado, -lado, -lado);//Triángulo1
	mesh->vVertices.emplace_back(-lado, -lado, lado);
	mesh->vVertices.emplace_back(-lado, lado, lado);

	mesh->vVertices.emplace_back(-lado, -lado, -lado);//Triángulo2
	mesh->vVertices.emplace_back(-lado, lado, lado);
	mesh->vVertices.emplace_back(-lado, lado, -lado);

	//Cara5
	mesh->vVertices.emplace_back(-lado, lado, lado);//Triángulo1
	mesh->vVertices.emplace_back(lado, lado, lado);
	mesh->vVertices.emplace_back(lado, lado, -lado);

	mesh->vVertices.emplace_back(-lado, lado, lado);//Triángulo2
	mesh->vVertices.emplace_back(lado, lado, -lado);
	mesh->vVertices.emplace_back(-lado, lado, -lado);

	//Cara6
	mesh->vVertices.emplace_back(-lado, -lado, -lado);//Triángulo1
	mesh->vVertices.emplace_back(lado, -lado, -lado);
	mesh->vVertices.emplace_back(lado, -lado, lado);

	mesh->vVertices.emplace_back(-lado, -lado, -lado);//Triángulo2
	mesh->vVertices.emplace_back(lado, -lado, lado);
	mesh->vVertices.emplace_back(-lado, -lado, lado);

	return mesh;
}

Mesh* Mesh::generateBoxOutline(GLdouble length)
{
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_STRIP;

	// número de vértices es 10: los 8 del cubo más 2 repetidos para cerrar el contorno
	mesh->mNumVertices = 10;
	mesh->vVertices.reserve(mesh->mNumVertices);

	GLdouble lado = length / 2.0;

	mesh->vVertices.push_back(glm::vec3(-lado, lado, lado));
	mesh->vVertices.push_back(glm::vec3(-lado, -lado, lado));

	mesh->vVertices.push_back(glm::vec3(lado, lado, lado));
	mesh->vVertices.push_back(glm::vec3(lado, -lado, lado));

	mesh->vVertices.push_back(glm::vec3(lado, lado, -lado));
	mesh->vVertices.push_back(glm::vec3(lado, -lado, -lado));

	mesh->vVertices.push_back(glm::vec3(-lado, lado, -lado));
	mesh->vVertices.push_back(glm::vec3(-lado, -lado, -lado)); 

	// dos vertices para cerrar el contorno
	mesh->vVertices.push_back(glm::vec3(-lado, lado, lado));
	mesh->vVertices.push_back(glm::vec3(-lado, -lado, lado));

	return mesh;
}

Mesh* Mesh::generateBoxOutlineTexCor(GLdouble length) 
{
	Mesh* m = generateBoxOutline(length);
	m->vTexCoords.reserve(m->mNumVertices);

	m->vTexCoords.push_back(glm::vec2(0.0, 1.0)); 
	m->vTexCoords.push_back(glm::vec2(0.0, 0.0)); 
	m->vTexCoords.push_back(glm::vec2(1.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(1.0, 0.0)); 

	m->vTexCoords.push_back(glm::vec2(2.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(2.0, 0.0));

	m->vTexCoords.push_back(glm::vec2(3.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(3.0, 0.0));

	m->vTexCoords.push_back(glm::vec2(4.0, 1.0));
	m->vTexCoords.push_back(glm::vec2(4.0, 0.0));

	return m;
}

Mesh* Mesh::generateRGBCubeTriangles(GLdouble length) 
{
	Mesh* mesh = generateCube(length);

	mesh->vColors.reserve(mesh->mNumVertices);

	//Cada cara tiene 6 vertices(dos triangulos)
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo

	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo


	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo

	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo
	mesh->vColors.push_back({ 1, 0, 0, 1 }); // Rojo


	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul

	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul


	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul

	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul
	mesh->vColors.push_back({ 0, 0, 1, 1 }); // Azul


	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde

	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde


	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde

	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde
	mesh->vColors.push_back({ 0, 1, 0, 1 }); // Verde

	return mesh;
}

Mesh* Mesh::generateStar3D(GLdouble re, GLuint np, GLdouble h) {
	Mesh* mesh = new Mesh();
	mesh->mPrimitive = GL_TRIANGLE_FAN;
	
	//vertice inicial
	mesh->vVertices.push_back(glm::vec3(0, 0, 0));

	//mismo numero de vertices internos (en la circunferencia ri) y vertices externos de la estrella (en la circunferencia re)
	int totalVertices = 2 * np;
	//1 vertice inical (0,0, 0), np vertices internos de la estrella + np vertices externos de la estrella, 1 vertice para cerrar la estrella
	mesh->mNumVertices = 2+ totalVertices;
	mesh->vVertices.reserve(mesh->mNumVertices);

	//radio de la circunferencia interna
	GLdouble ri = re / 2;

	//calculamos cada cuantos grados hay un vertice de la estrella: 360/2np
	GLdouble angleStep = 360.0 / totalVertices;

	for (int i = 0; i <= totalVertices; i++) {
		//Alternamos entre numeros pares e impares para saber en que circunefrencia va el vertice
		GLdouble r;
		if (i % 2 == 0) {
			r = re; // Si el índice es par, usamos el radio exterior re
		}
		else {
			r = ri; // Si el índice es impar, usamos el radio interior ri
		}

		// Calculamos el ángulo en radianes de cada vertice
		GLdouble angle = glm::radians(90.0 + i * angleStep);

		//Calculamos la posición del vertice según el angulo
		GLdouble x = r * cos(angle);
		GLdouble y = r * sin(angle);

		mesh->vVertices.push_back(glm::vec3(x, y, h));
	}

	return mesh;
}
Mesh* Mesh::generateStar3DTexCor(GLdouble re, GLuint np, GLdouble h) 
{
	Mesh* m = generateStar3D(re, np, h);
	m->vTexCoords.reserve(m->mNumVertices);

	m->vTexCoords.push_back(glm::vec2(0.5, 0.5));

	int totalPoints = 2 * np;
	GLdouble angleStep = 360.0 / totalPoints;

	for (int i = 0; i <= totalPoints; i++) {
		// Para la textura, el radio debe estar normalizado entre 0 y 0.5k mj
		// (porque el radio total de la imagen es 0.5 desde el centro)
		GLdouble r;
		if (i % 2 == 0) r = 0.5;  // Puntas (borde de la imagen)
		else r = 0.25;            // Valles (mitad de camino al centro)

		GLdouble angle = glm::radians(90.0 + i * angleStep);

		// Mapeo circular: centro (0.5, 0.5) + dirección * radio
		GLfloat u = 0.5f + (GLfloat)(r * cos(angle));
		GLfloat v = 0.5f + (GLfloat)(r * sin(angle));

		m->vTexCoords.push_back(glm::vec2(u, v));
	}

	return m;
}
