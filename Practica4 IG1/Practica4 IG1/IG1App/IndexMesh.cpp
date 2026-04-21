#include "IndexMesh.h"
#include <numbers>

constexpr GLuint NONE = std::numeric_limits<GLuint>::max();

// ─── Apt. 55/60/67: malla por revolución (genera vértices, índices, normales y texCoords) ───
IndexMesh* IndexMesh::generateByRevolution(
	const std::vector<glm::vec2>& profile, GLuint nSamples, GLfloat angleMax)
{
	IndexMesh* mesh = new IndexMesh;
	mesh->mPrimitive = GL_TRIANGLES;
	int tamPerfil = (int)profile.size();

	mesh->vVertices.reserve((nSamples + 1) * tamPerfil);
	mesh->vTexCoords.reserve((nSamples + 1) * tamPerfil); // apt. 67: coordenadas de textura

	// Genera vértices rotando el perfil nSamples veces
	GLdouble theta1 = angleMax / nSamples;
	for (int i = 0; i <= (int)nSamples; ++i) {
		GLdouble c = cos(i * theta1), s = sin(i * theta1);
		for (int j = 0; j < tamPerfil; ++j) {
			auto p = profile[j];
			mesh->vVertices.emplace_back(p.x * c, p.y, -p.x * s);
			// s = i/nSamples (posición angular), t = 1 - j/(tamPerfil-1) (arriba→abajo)
			mesh->vTexCoords.emplace_back(
				(float)i / nSamples,
				1.0f - (float)j / (tamPerfil - 1.0f));
		}
	}

	// Genera índices: dos triángulos por cuadrilátero del mallado
	for (int i = 0; i < (int)nSamples; ++i)
		for (int j = 0; j < tamPerfil - 1; ++j) {
			if (profile[j].x != 0.0f)     // triángulo inferior (evita degenerar en el eje)
				for (auto [s, t] : { std::pair{i,j}, std::pair{i,j + 1}, std::pair{i + 1,j} })
					mesh->vIndexes.push_back(s * tamPerfil + t);
			if (profile[j + 1].x != 0.0f) // triángulo superior
				for (auto [s, t] : { std::pair{i,j + 1}, std::pair{i + 1,j + 1}, std::pair{i + 1,j} })
					mesh->vIndexes.push_back(s * tamPerfil + t);
		}

	mesh->mNumVertices = (GLuint)mesh->vVertices.size();
	mesh->buildNormalVectors(); // apt. 59/60: normales para iluminación
	return mesh;
}

// ─── Apt. 61: cubo indexado con 8 vértices y 36 índices (los del tema) ───
IndexMesh* IndexMesh::generateIndexedBox8(GLdouble l)
{
	IndexMesh* mesh = new IndexMesh;
	mesh->mPrimitive = GL_TRIANGLES;

	GLfloat h = (GLfloat)(l / 2.0);

	// 8 vértices del cubo centrado en el origen (mismos que los apuntes, tabla de la diapositiva)
	mesh->vVertices = {
		{ h,  h, -h}, // 0
		{ h, -h, -h}, // 1
		{ h,  h,  h}, // 2
		{ h, -h,  h}, // 3
		{-h,  h,  h}, // 4
		{-h, -h,  h}, // 5
		{-h,  h, -h}, // 6
		{-h, -h, -h}, // 7
	};

	// 36 índices = 12 triángulos x 3 vértices (exactamente los de las diapositivas)
	mesh->vIndexes = {
		0,1,2, 2,1,3,
		2,3,4, 4,3,5,
		4,5,6, 6,5,7,
		6,7,0, 0,7,1,
		4,6,2, 2,6,0,
		1,7,3, 3,7,5
	};

	mesh->mNumVertices = (GLuint)mesh->vVertices.size();
	mesh->buildNormalVectors();
	return mesh;
}

// ─── Apt. 64: cubo indexado con 24 vértices (4 por cara, normales correctas por cara) ───
IndexMesh* IndexMesh::generateIndexedBox(GLdouble l)
{
	IndexMesh* mesh = new IndexMesh;
	mesh->mPrimitive = GL_TRIANGLES;

	GLfloat h = (GLfloat)(l / 2.0);

	// Cada cara tiene 4 vértices propios (no compartidos) para que las normales sean correctas.
	// Las 6 caras en orden: +Z, -Z, +X, -X, +Y, -Y
	mesh->vVertices = {
		// cara +Z (frente)
		{-h,-h, h}, { h,-h, h}, { h, h, h}, {-h, h, h},
		// cara -Z (detrás)
		{ h,-h,-h}, {-h,-h,-h}, {-h, h,-h}, { h, h,-h},
		// cara +X (derecha)
		{ h,-h, h}, { h,-h,-h}, { h, h,-h}, { h, h, h},
		// cara -X (izquierda)
		{-h,-h,-h}, {-h,-h, h}, {-h, h, h}, {-h, h,-h},
		// cara +Y (arriba)
		{-h, h, h}, { h, h, h}, { h, h,-h}, {-h, h,-h},
		// cara -Y (abajo)
		{-h,-h,-h}, { h,-h,-h}, { h,-h, h}, {-h,-h, h},
	};

	// 36 índices: 2 triángulos por cara x 6 caras, base en 0,4,8,12,16,20
	mesh->vIndexes = {
		 0, 1, 2,  0, 2, 3,   // +Z
		 4, 5, 6,  4, 6, 7,   // -Z
		 8, 9,10,  8,10,11,   // +X
		12,13,14, 12,14,15,   // -X
		16,17,18, 16,18,19,   // +Y
		20,21,22, 20,22,23,   // -Y
	};

	mesh->mNumVertices = (GLuint)mesh->vVertices.size();
	mesh->buildNormalVectors(); // normales perfectas (una por cara)
	return mesh;
}

// ─── Apt. 67: esfera con generateByRevolution y texCoords ───
IndexMesh* IndexMesh::generateSphere(GLdouble radius, GLuint nParallels, GLuint nMeridians)
{
	// El perfil es un semicírculo en el plano XY, de abajo (polo sur) a arriba (polo norte)
	std::vector<glm::vec2> profile;
	profile.reserve(nParallels + 1);
	for (GLuint i = 0; i <= nParallels; ++i) {
		// theta va de -PI/2 (polo sur) a +PI/2 (polo norte)
		float theta = (float)(-std::numbers::pi / 2.0 + std::numbers::pi * i / nParallels);
		profile.emplace_back((float)(radius * cos(theta)), (float)(radius * sin(theta)));
	}
	return generateByRevolution(profile, nMeridians);
}

// ─── GPU: subir índices al EBO (tras el VAO del padre) ───
void IndexMesh::load()
{
	Mesh::load(); // crea VAO y sube vértices, colores, normales, texCoords
	glBindVertexArray(mVAO);
	glGenBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		vIndexes.size() * sizeof(GLuint),
		vIndexes.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void IndexMesh::unload()
{
	Mesh::unload();
	if (mIBO != NONE) {
		glDeleteBuffers(1, &mIBO);
		mIBO = NONE;
	}
}

void IndexMesh::draw() const
{
	glDrawElements(
		mPrimitive,
		(GLsizei)vIndexes.size(),
		GL_UNSIGNED_INT,
		nullptr);
}

// ─── Apt. 59: normales por método de Newell (acumular producto vectorial en cada vértice) ───
void IndexMesh::buildNormalVectors()
{
	vNormals.assign(vVertices.size(), glm::vec3(0.0f));

	for (size_t i = 0; i + 2 < vIndexes.size(); i += 3) {
		GLuint i0 = vIndexes[i], i1 = vIndexes[i + 1], i2 = vIndexes[i + 2];
		glm::vec3 v0 = vVertices[i0], v1 = vVertices[i1], v2 = vVertices[i2];
		glm::vec3 n = glm::cross(v1 - v0, v2 - v0); // normal del triángulo
		vNormals[i0] += n;
		vNormals[i1] += n;
		vNormals[i2] += n;
	}

	for (auto& n : vNormals)
		if (glm::length(n) > 0.0f)
			n = glm::normalize(n);
}