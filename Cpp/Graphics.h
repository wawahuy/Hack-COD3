#pragma once
#include "glm/glm.hpp"

struct Ray {
	glm::fvec3 position;
	glm::fvec3 normal;
};

bool word2Screen(const glm::fvec3& world, glm::fvec3& out, const glm::fmat4& matrix);

glm::fmat4 createCamera(const glm::fvec3& eye, const glm::fvec3& forward, const glm::fvec3& left, const glm::fvec3& up);

glm::fvec3 closestPoint(const Ray& ray, const glm::fvec3& p);