#pragma once
#include "glm/vec3.hpp"

void buildQuadsBox(glm::fvec3 (&outVertices)[16], glm::fvec3 size) {
	glm::fvec3 hs = size * 0.5f;
	outVertices[0] = glm::fvec3(-1.0f * hs.x,  1.0f * hs.y, 1.0f * hs.z);
	outVertices[1] = glm::fvec3( 1.0f * hs.x,  1.0f * hs.y, 1.0f * hs.z);
	outVertices[2] = glm::fvec3( 1.0f * hs.x, -1.0f * hs.y, 1.0f * hs.z);
	outVertices[3] = glm::fvec3(-1.0f * hs.x, -1.0f * hs.y, 1.0f * hs.z);

	outVertices[4] = glm::fvec3(-1.0f * hs.x, 1.0f * hs.y, -1.0f * hs.z);
	outVertices[5] = glm::fvec3(1.0f * hs.x, 1.0f * hs.y, -1.0f * hs.z);
	outVertices[6] = glm::fvec3(1.0f * hs.x, -1.0f * hs.y, -1.0f * hs.z);
	outVertices[7] = glm::fvec3(-1.0f * hs.x, -1.0f * hs.y, -1.0f * hs.z);

	outVertices[8] = glm::fvec3(-1.0f * hs.x, 1.0f * hs.y, 1.0f * hs.z);
	outVertices[9] = glm::fvec3(-1.0f * hs.x, -1.0f * hs.y, 1.0f * hs.z);
	outVertices[10] = glm::fvec3(-1.0f * hs.x, -1.0f * hs.y, -1.0f * hs.z);
	outVertices[11] = glm::fvec3(-1.0f * hs.x, 1.0f * hs.y, -1.0f * hs.z);

	outVertices[12] = glm::fvec3(1.0f * hs.x, 1.0f * hs.y, 1.0f * hs.z);
	outVertices[13] = glm::fvec3(1.0f * hs.x, -1.0f * hs.y, 1.0f * hs.z);
	outVertices[14] = glm::fvec3(1.0f * hs.x, -1.0f * hs.y, -1.0f * hs.z);
	outVertices[15] = glm::fvec3(1.0f * hs.x, 1.0f * hs.y, -1.0f * hs.z);
}