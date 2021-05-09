#include "scenebasic_uniform.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "helper/glutils.h"
#include <ctime>
#include "helper/noisetex.h"
#include <stdlib.h>
#include <time.h>

using std::cerr;
using std::endl;
using glm::vec3;
using glm::mat4;
using std::string;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(13.0f, 10.0f, 200, 2), angle(0.0f), drawBuf(1), time(0), deltaT(0), nParticles(4000), particleLifetime(6.0f), emitterPos(1, 0, 0), emitterDir(-1, 2, 0)
{
    mesh = ObjMesh::load("../Project_Template/media/Fountain.obj", true);
    mesh2 = ObjMesh::load("../Project_Template/media/Ball.obj", true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    projection = mat4(1.0f);
    GLfloat verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    unsigned int handle[2];
    glGenBuffers(2, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    prog.setUniform("NoiseTex", 0);

    GLuint noiseTex = NoiseTex::generate2DTex(6.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    model = mat4(1.0f);

    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("../Project_Template/media/texture/Disco.jpg");

    glActiveTexture(GL_TEXTURE1);
    ParticleUtils::createRandomTex1D(nParticles * 3);

    initBuffers();

    // Shader Spin
    prog.use();
    prog.setUniform("RandomTex", 1);
    prog.setUniform("ParticleTex", 0);
    prog.setUniform("ParticleLifetime", particleLifetime);
    prog.setUniform("Accel", vec3(0.0f, -0.5, 0.0f));
    prog.setUniform("ParticleSize", 0.05f);
    prog.setUniform("EmitterPos", emitterPos);
    prog.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));
    prog.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
    angle = glm::root_half_pi<float>();

    // Shader 2
    flatProg.use();
    flatProg.setUniform("NoiseTex", 0);
    flatProg.setUniform("RandomTex", 2);
    flatProg.setUniform("ParticleTex", 0);
    flatProg.setUniform("ParticleLifetime", particleLifetime);
    flatProg.setUniform("Accel", vec3(0.0f, -0.5, 0.0f));
    flatProg.setUniform("ParticleSize", 0.05f);
    flatProg.setUniform("EmitterPos", emitterPos);
    flatProg.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));
    flatProg.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));

    //Moss Shader
    MossProg.use();
    MossProg.setUniform("lights[0].Position", vec3(1.0f, 1.0f, 1.0f));
    MossProg.setUniform("lights[1].La", vec3(1.0f, 1.0f, 1.0f));
    MossProg.setUniform("lights[2].L", vec3(1.0f, 1.0f, 1.0f));
    // MOSS
    float x, z;
    for (int i = 0; i < 3; i++)
    {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    }
    // MOSS
}

void SceneBasic_Uniform::initBuffers()
{
    glGenBuffers(2, posBuf);
    glGenBuffers(2, velBuf);
    glGenBuffers(2, age);

    int size = nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat>tempData(nParticles);
    float rate = particleLifetime / nParticles;
    for (int i = 0; i < nParticles; i++) {
        tempData[i] = rate * (i - nParticles);
    }
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tempData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(2, particleArray);

    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    glGenTransformFeedbacks(2, feedback);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");

        GLuint progHandle = prog.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);

        prog.link();
        prog.use();

        flatProg.compileShader("shader/Noise_Shader.frag");
        flatProg.compileShader("shader/Noise_Shader.vert");
        flatProg.link();

        MossProg.compileShader("shader/Moss.frag");
        MossProg.compileShader("shader/Moss.vert");
        MossProg.link();
        MossProg.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    deltaT = t - time;
    time = t;
    angle = std::fmod(angle + 0.01f, glm::two_pi<float>());
}

void SceneBasic_Uniform::render()
{
    model = mat4(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    flatProg.use();
    setMatrices(flatProg);

    prog.use();
    prog.setUniform("Time", time);
    prog.setUniform("DeltaT", deltaT);

    prog.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(particleArray[1 - drawBuf]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    prog.setUniform("Pass", 2);
    view = glm::lookAt(vec3(4.0f * cos(angle), 1.5f, 4.0f * sin(angle)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
    prog.setUniform("Material.kd", 0.2f, 0.5f, 0.9f);
    prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    prog.setUniform("Material.Ka", 0.2f, 0.5f, 0.9f);
    prog.setUniform("Material.Shininess", 100.0f);

    flatProg.setUniform("Material.kd", 0.2f, 0.5f, 0.9f);
    flatProg.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    flatProg.setUniform("Material.Ka", 0.2f, 0.5f, 0.9f);
    flatProg.setUniform("Material.Shininess", 100.0f);

    MossProg.setUniform("Material.kd", 0.2f, 0.5f, 0.9f);
    MossProg.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    MossProg.setUniform("Material.Ka", 0.2f, 0.5f, 0.9f);
    MossProg.setUniform("Material.Shininess", 100.0f);

    // Mesh 1
    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("../Project_Template/media/texture/Disco.jpg");
    prog.link();
    prog.use();
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices(prog);
    mesh->render();

    // Plane
    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("../Project_Template/media/texture/Laser.jpg");
    flatProg.link();
    flatProg.use();
    model = mat4(1.0f);
    model = glm::translate(model, vec3(1.0f, -1.0f, 1.0f));
    setMatrices(flatProg);
    plane.render();

    // Mesh 2
    GLuint texture1 = Texture::loadTexture("../Project_Template/media/texture/Eye.jpg");
    GLuint texture2 = Texture::loadTexture("../Project_Template/media/texture/Energy.png");
    prog.link();
    prog.use();
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE1);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices(prog);
    mesh2->render();

    // Particles
    glActiveTexture(GL_TEXTURE0);
    Texture::loadTexture("../Project_Template/media/texture/star.png");
    prog.link();
    prog.use();
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    model - glm::rotate(model, glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices(prog);


    glDepthMask(GL_FALSE);
    glBindVertexArray(particleArray[drawBuf]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glActiveTexture(GL_TEXTURE0);

    // Sign
    Texture::loadTexture("../Project_Template/media/texture/Exit.jpg");
    prog.link();
    prog.use();
    model = mat4(1.0f);
    setMatrices(prog);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    drawBuf = 1 - drawBuf;
}

void SceneBasic_Uniform::drawScene()
{
    model = mat4(1.0f);
    setMatrices(prog);
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& p)
{
    mat4 mv = view * model;
    p.setUniform("MV", mv);
    p.setUniform("Proj", projection);
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    flatProg.setUniform("ModelViewMatrix", mv);
    flatProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    flatProg.setUniform("MVP", projection * mv);
    MossProg.setUniform("ModelViewMatrix", mv);
    MossProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    MossProg.setUniform("MVP", (mat4(projection) * mv));
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}
