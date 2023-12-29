#pragma once

#include <utility>
#include <vector>

namespace Graph {
bool isBipartite(std::vector<std::vector<int>> const& G,
                 std::pair<std::vector<int>, std::vector<int>>& bipartitionIndexes);

void createAdjFromBipartition(
    std::vector<std::vector<int>> const& G,
    std::pair<std::vector<int>, std::vector<int>> const& bipartitionIndexes,
    std::vector<std::vector<int>>& adjMatrix);

namespace Hungarian {

void solveHungarian(const std::vector<std::vector<int>>& original, std::vector<std::vector<int>>& M,
                    bool allow_negatives = true, bool maximize = true);

void getEdgesFromHungarian(std::vector<std::vector<int>> const& adjMatrix,
                           std::pair<std::vector<int>, std::vector<int>> const& bipartitionIndexes,
                           std::vector<std::vector<int>> const& hungarianMask,
                           std::vector<std::pair<int, int>>& edges);
} // namespace Hungarian

} // namespace Graph
