#ifndef UTILS_H
#define UTILS_H

#include "pch.h"

namespace glaceon {

/**
 * Generates a vector of unique indices for each vertex in the given vertex data.
 *
 * @param vertexData The vector of glm::vec3 vertices.
 * @param uniqueVertex The vector to store the unique vertices.
 *
 * @return A vector of size_t indices corresponding to the unique vertices in the vertexData vector.
 *
 * @throws None
 */
std::vector<size_t> GetIndexFromVertexData(const std::vector<glm::vec3>& vertexData, std::vector<glm::vec3>& uniqueVertex);

}// namespace glaceon

#endif//UTILS_H
