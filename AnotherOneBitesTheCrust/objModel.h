/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 3
 *
 * objModel - Class that handles and stores an OBJ model
 *
 * Note: The provided files are based off MD2 models, which have
 * specific animations as listed. A solitary OBJ file is not
 * animated, but the series of files provided have been generated
 * for each frame of the animation, in order
 */

#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
//#include <QMatrix4x4>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace glm;


// Triangle
typedef struct
{
    GLuint index_xyz[3];    // Indexes to triangle's vertices
    GLuint index_uv[3];     // Indexes to triangles's coordinates
} tri;

// Animation
typedef struct
{
        int first_frame;    // First frame of the animation
        int last_frame;     // Number of frames
        int fps;            // Number of frames per second

} anim_t;

// Animation list
typedef enum {
        STAND,
        RUN,
        ATTACK,
        PAIN_A,
        PAIN_B,
        PAIN_C,
        JUMP,
        FLIP,
        SALUTE,
        FALLBACK,
        WAVE,
        POINT,
        CROUCH_STAND,
        CROUCH_WALK,
        CROUCH_ATTACK,
        CROUCH_PAIN,
        CROUCH_DEATH,
        DEATH_FALLBACK,
        DEATH_FALLFORWARD,
        DEATH_FALLBACKSLOW,
        BOOM,

        MAX_ANIMATIONS

} animType_t;

class ObjModel {
	public:
        ObjModel ();
        ObjModel (const char *filename, int parent);
        virtual ~ObjModel() {}

        bool LoadModel(const char *filename, int parent);

        int numVerts;
        int numFaces;

        // OpenGL handles for the model
        GLuint vao;
        GLuint vbo;
        GLuint texture;

        // Index to parent model
        int parent;

        // Tranformation matrices
        mat4 trans;
        mat4 rotation;
        mat4 scaling;

        vector<vec3> verts;
        vector<vec2> texs;
        vector<tri> tris;

        vector<vec3> vertNorms;
        vector<vec3> faceNorms;

        // Return composition of all transformations
        mat4 modelMatrix() {
            return trans * rotation * scaling;
        }

        static anim_t animlist[21]; // static array, listing the animations in the provided models
};

#endif // OBJMODEL_H
