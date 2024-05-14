#ifndef UTILS_H
#define UTILS_H

#include "pch.h"

namespace glaceon {

/**
 * @brief Compares two glm::vec3 vectors for equality.
 *
 * This function compares two glm::vec3 vectors for equality by checking if their x, y, and z components are equal.
 *
 * @param a The first glm::vec3 vector to compare.
 * @param b The second glm::vec3 vector to compare.
 * @return True if the vectors are equal, false otherwise.
 */
bool CompareGlmVec3(const glm::vec3& a, const glm::vec3& b);

/**
 * Generates a vector of unique indices for each vertex in the given vertex data.
 *
 * @param vertexData The vector of glm::vec3 vertices.
 * @param uniqueVertex The vector to store the unique vertices.
 *
 * @return A vector of uint32_t indices corresponding to the unique vertices in the vertexData vector.
 *
 * @throws None
 */
std::vector<uint32_t> GetIndexFromVertexData(const std::vector<glm::vec3>& vertexData, std::vector<glm::vec3>& uniqueVertex);

/**
 * @brief Aligns a memory address to a specified alignment.
 *
 * This function aligns a memory address to the specified alignment value. It uses bitwise operations
 * to ensure the address is aligned correctly.
 *
 * @param address The memory address to be aligned.
 * @param alignment The desired alignment value, must be a power of two.
 * @return The aligned memory address.
 *
 * @note This function performs bitwise operations and is safe for aligning memory addresses.
 * However, it's important to note that pointer arithmetic on a void* is undefined behavior in C++,
 * so the input should be a properly typed pointer before calling this function.
 */

void* AlignAddress(void* address, uint32_t alignment);

/**
 * @brief Calculates the adjustment needed to align a memory address to a specified alignment.
 *
 * This function calculates the adjustment required to align a memory address to the specified alignment value.
 * The alignment value must be a power of two for proper alignment calculations.
 *
 * @param address The memory address to be aligned.
 * @param alignment The desired alignment value, must be a power of two.
 * @return The adjustment needed to align the memory address.
 *
 * @note If the memory address is already aligned to the specified alignment, the adjustment returned is 0.
 */
uint8_t AlignSize(const void* address, uint8_t alignment);

/**
 * @brief Aligns a memory address with a specified alignment, considering a header size.
 *
 * This function calculates the adjustment needed to align a memory address to the specified alignment
 * while also accommodating a header size.
 * 
 * @param address The memory address to be aligned.
 * @param alignment The desired alignment value, must be a power of two.
 * @param headerSize The size of the header to be considered for alignment adjustments.
 * @return The adjusted alignment size for the memory address with the header size taken into account.
 */
uint8_t AlignSizeWithHeader(const void* address, uint8_t alignment, size_t headerSize);

}// namespace glaceon

#endif//UTILS_H
