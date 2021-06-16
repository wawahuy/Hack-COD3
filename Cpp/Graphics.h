#pragma once
#include "glm/glm.hpp"

bool word2Screen(glm::fvec3 world, glm::fvec3& out, const glm::fmat4& matrix);

glm::fmat4 createCamera(glm::fvec3 eye, glm::fvec3 forward, glm::fvec3 left, glm::fvec3 up);