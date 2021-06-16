#include "Graphics.h"
#pragma once
#include "glm/glm.hpp"

bool word2Screen(glm::fvec3 world, glm::fvec3& out, const glm::fmat4& matrix) {
    // pvm * vertext
    const glm::vec4 p1 = matrix * glm::vec4(world, 1.0f);
    if (p1.w < 0.1f) {
        return false;
    }

    // ndc
    out = glm::fvec3(p1) / p1.w;
    return true;
}

glm::fmat4 createCamera(glm::fvec3 eye, glm::fvec3 forward, glm::fvec3 left, glm::fvec3 up) {
    glm::fmat4 matrixOut = glm::fmat4(1.0f);
    float* matrix = (float*)&matrixOut;
    matrix[0] = left.x;
    matrix[4] = left.y;
    matrix[8] = left.z;
    matrix[1] = up.x;
    matrix[5] = up.y;
    matrix[9] = up.z;
    matrix[2] = forward.x;
    matrix[6] = forward.y;
    matrix[10] = forward.z;
    matrix[12] = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
    matrix[13] = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
    matrix[14] = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;
    return matrixOut;
}