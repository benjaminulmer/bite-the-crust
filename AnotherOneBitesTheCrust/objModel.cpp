// CPSC 453: Introduction to Computer Graphics
//
// Implementation file for a class to handle obj file data
//
// Created by Troy Alderson
// Department of Computer Science
// University of Calgary
//
// Used with permission for CPSC 453 HW3

#include "objModel.h"
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>
#include <limits>

using namespace std;
using namespace glm;

// Initialize the 21 model animations, for the provided models
anim_t ObjModel::animlist[ 21 ] =
{
        // First, last, fps

        {   0,  39,  9 },       // STAND
        {  40,  45, 10 },       // RUN
        {  46,  53, 10 },       // ATTACK
        {  54,  57,  7 },       // PAIN_A
        {  58,  61,  7 },       // PAIN_B
        {  62,  65,  7 },       // PAIN_C
        {  66,  71,  7 },       // JUMP
        {  72,  83,  7 },       // FLIP
        {  84,  94,  7 },       // SALUTE
        {  95, 111, 10 },       // FALLBACK
        { 112, 122,  7 },       // WAVE
        { 123, 134,  6 },       // POINT
        { 135, 153, 10 },       // CROUCH_STAND
        { 154, 159,  7 },       // CROUCH_WALK
        { 160, 168, 10 },       // CROUCH_ATTACK
        { 169, 172,  7 },       // CROUCH_PAIN
        { 173, 177,  5 },       // CROUCH_DEATH
        { 178, 183,  7 },       // DEATH_FALLBACK
        { 184, 189,  7 },       // DEATH_FALLFORWARD
        { 190, 197,  7 },       // DEATH_FALLBACKSLOW
        { 198, 198,  5 },       // BOOM
};

// Default constructor
ObjModel::ObjModel () {

}

// File-loading constructor
ObjModel::ObjModel (const char *filename, int parent) {
    this->parent = parent;
    trans = mat4(1.0f);
    scaling = mat4(1.0f);
    rotation = mat4(1.0f);
    LoadModel(filename, parent);
}

// Loads the model, populating the data
bool ObjModel::LoadModel(const char *filename, int parent) {

    // Clear prior data
    numVerts = numFaces = 0;
    verts.clear(); texs.clear(); tris.clear();

    // From http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

    printf("Loading OBJ file %s...\n", filename);

    // Try to open the file
    FILE * file = fopen(filename, "r");
    if( file == NULL ){
        printf("Impossible to open the file! Do you have the correct filename?\n");
        //getchar();
        return false;
    }
    // Set max and min values to -infinity and +infinity respectively
    float xMax = -std::numeric_limits<float>::infinity();
    float yMax = -std::numeric_limits<float>::infinity();
    float zMax = -std::numeric_limits<float>::infinity();
    float xMin = std::numeric_limits<float>::infinity();
    float yMin = std::numeric_limits<float>::infinity();
    float zMin = std::numeric_limits<float>::infinity();

    // Load values from the file
    while( true ){
        char lineHeader[128];
        // Read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // Parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            // x, y, z: vertex coordinates
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            verts.push_back(vertex);
            numVerts++;

            // Change max and min values if vertex read in is the new max or min
            if (vertex.x > xMax) xMax = vertex.x;
            if (vertex.x < xMin) xMin = vertex.x;
            if (vertex.y > yMax) yMax = vertex.y;
            if (vertex.y < yMin) yMin = vertex.y;
            if (vertex.z > zMax) zMax = vertex.z;
            if (vertex.z < zMin) zMin = vertex.z;

        } else if ( strcmp( lineHeader, "vt" ) == 0 ){
            // u, v: texture coordinates
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            texs.push_back(uv);
        } else if ( strcmp( lineHeader, "vn" ) == 0 ){
            // Normal coordinates (ignored)
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            // normals.push_back(normal);

        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            // Face indices
            tri t;
            int matches = fscanf(file, "%d/%d %d/%d %d/%d\n", &(t.index_xyz[0]), &(t.index_uv[0]), &(t.index_xyz[1]), &(t.index_uv[1]), &(t.index_xyz[2]), &(t.index_uv[2]) );
            if (matches != 6){
                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                return false;
            }
            tris.push_back(t);
            numFaces++;

        } else {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }

    }
    // Scale the model if it does not have a parent
    if (parent == -1) {
        float maxAxis = std::max(xMax - xMin, (std::max(yMax - yMin, zMax - zMin)));
		
		

		scaling = scale(scaling, vec3(1/maxAxis)); 
    }
    return true;
}
