#include "RevolutionObjects.h"
#include "IndexMesh.h"

Torus::Torus(GLdouble R, GLdouble r, GLuint nPoints, GLuint nSamples) {
    std::vector<glm::vec2> profile;
    double angle = 2.0 * std::numbers::pi / nPoints;

    //generamos el circulo deplazado en R
    for (GLuint i = 0; i <= nPoints; ++i) {
        profile.push_back(glm::vec2(r* cos(angle*i) +R, r* sin(angle*i)));
    }

    mMesh = IndexMesh::generateByRevolution(profile, nSamples);
}