#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include "helper/torus.h"
#include <glm/glm.hpp>

#include "helper/plane.h"
#include "helper/objmesh.h"

#include "helper/cube.h"
#include "helper/texture.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "helper/random.h"
#include "helper/grid.h"
#include "helper/particleutils.h"

class SceneBasic_Uniform : public Scene
{
private:
    //
    Plane plane;
    //
    std::unique_ptr<ObjMesh>mesh;
    std::unique_ptr<ObjMesh>mesh2;

    //
    GLSLProgram prog, flatProg, MossProg;
    //
    GLuint quad;
    glm::vec3 lightPos;
    //

    Random rand;
    glm::vec3 emitterPos, emitterDir;
    GLuint posBuf[2], velBuf[2], age[2];
    GLuint particleArray[2];
    GLuint feedback[2];
    GLuint drawBuf;
    Grid grid;
    int nParticles;
    float particleLifetime;
    float angle;
    float time, deltaT;
    void initBuffers();
    void setMatrices(GLSLProgram&);
    void compile();
    //
    void drawScene();
    //
public:
    SceneBasic_Uniform();
    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
