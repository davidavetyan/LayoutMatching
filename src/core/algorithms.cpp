#include "algorithms.h"

#include <queue>
#include <vector>

namespace Graph {

bool isBipartite(std::vector<std::vector<int>> const& G,
                 std::pair<std::vector<int>, std::vector<int>>& bipartitionIndexes)
{
    // Create a color array to store colors
    // assigned to all veritces. Vertex
    // number is used as index in this array.
    // The value '-1' of colorArr[i]
    // is used to indicate that no color
    // is assigned to vertex 'i'. The value 1
    // is used to indicate first color
    // is assigned and value 0 indicates
    // second color is assigned.
    int V = G.size();
    std::vector<int> colorArr(V);
    for (int i = 0; i < V; ++i)
        colorArr[i] = -1;

    // Assign first color to source
    colorArr[0] = 1;

    // Create a queue (FIFO) of vertex
    // numbers and enqueue source vertex
    // for BFS traversal
    std::queue<int> q;
    q.push(0);

    // Run while there are vertices
    // in queue (Similar to BFS)
    while (!q.empty())
    {
        // Dequeue a vertex from queue ( Refer http://goo.gl/35oz8 )
        int u = q.front();
        q.pop();

        // Return false if there is a self-loop
        if (G[u][u] == 1)
            return false;

        // Find all non-colored adjacent vertices
        for (int v = 0; v < V; ++v)
        {
            // An edge from u to v exists and
            // destination v is not colored
            if (G[u][v] && colorArr[v] == -1)
            {
                // Assign alternate color to this adjacent v of u
                colorArr[v] = 1 - colorArr[u];
                q.push(v);
            }

            // An edge from u to v exists and destination
            // v is colored with same color as u
            else if (G[u][v] && colorArr[v] == colorArr[u])
                return false;
        }
    }

    // If we reach here, then all adjacent
    // vertices can be colored with alternate color

    bipartitionIndexes.first.clear();
    for (int i = 0; i < V; i++)
    {
        if (colorArr[i])
            bipartitionIndexes.first.push_back(i);
        else
            bipartitionIndexes.second.push_back(i);
    };
    return true;
}

void createAdjFromBipartition(
    std::vector<std::vector<int>> const& G,
    std::pair<std::vector<int>, std::vector<int>> const& bipartitionIndexes,
    std::vector<std::vector<int>>& adjMatrix)
{
    int V = G.size();

    int nRows = bipartitionIndexes.first.size();
    int nCols = bipartitionIndexes.second.size();

    adjMatrix.resize(nRows);
    for (std::vector<int>& vecRow : adjMatrix)
        vecRow.resize(nCols);

    int nVertex1;
    int nVertex2;
    for (int i = 0; i < bipartitionIndexes.first.size(); i++)
    {
        nVertex1 = bipartitionIndexes.first[i];
        for (int j = 0; j < bipartitionIndexes.second.size(); j++)
        {
            nVertex2 = bipartitionIndexes.second[j];
            adjMatrix[i][j] = G[nVertex1][nVertex2];
        }
    }
}

namespace Hungarian {

/* Handle negative elements if present. If allowed = true, add abs(minval) to
     * every element to create one zero. Else throw an exception */
void handle_negatives(std::vector<std::vector<int>>& matrix, bool allowed = true)
{
    int minval = std::numeric_limits<int>::max();

    for (auto& elem : matrix)
        for (auto& num : elem)
            minval = std::min(minval, num);

    if (minval < 0)
    {
        if (!allowed)
        { //throw
            throw std::exception("Only non-negative values allowed");
        }
        else
        { // add abs(minval) to every element to create one zero
            minval = abs(minval);

            for (auto& elem : matrix)
                for (auto& num : elem)
                    num += minval;
        }
    }
}

/* Ensure that the matrix is square by the addition of dummy rows/columns if necessary */
void pad_matrix(std::vector<std::vector<int>>& matrix)
{
    std::size_t i_size = matrix.size();
    std::size_t j_size = matrix[0].size();

    if (i_size > j_size)
    {
        for (auto& vec : matrix)
            vec.resize(i_size, 0 /*std::numeric_limits<int>::max()*/);
    }
    else if (i_size < j_size)
    {
        while (matrix.size() < j_size)
            matrix.push_back(std::vector<int>(j_size, std::numeric_limits<int>::max()));
    }
}

/* For each row of the matrix, find the smallest element and subtract it from every
     * element in its row.
     * For each col of the matrix, find the smallest element and subtract it from every
     * element in its col. Go to Step 2. */
void step1(std::vector<std::vector<int>>& matrix, int& step)
{
    // process rows
    for (auto& row : matrix)
    {
        auto smallest = *std::min_element(begin(row), end(row));
        if (smallest > 0)
            for (auto& n : row)
                n -= smallest;
    }

    // process cols
    int sz = matrix.size(); // square matrix is granted
    for (int j = 0; j < sz; ++j)
    {
        int minval = std::numeric_limits<int>::max();
        for (int i = 0; i < sz; ++i)
        {
            minval = std::min(minval, matrix[i][j]);
        }

        if (minval > 0)
        {
            for (int i = 0; i < sz; ++i)
            {
                matrix[i][j] -= minval;
            }
        }
    }

    step = 2;
}

/* helper to clear the temporary vectors */
inline void clear_covers(std::vector<int>& cover)
{
    for (auto& n : cover)
        n = 0;
}

/* Find a zero (Z) in the resulting matrix.  If there is no starred zero in its row or
     * column, star Z. Repeat for each element in the matrix. Go to Step 3.  In this step,
     * we introduce the mask matrix M, which in the same dimensions as the cost matrix and
     * is used to star and prime zeros of the cost matrix.  If M(i,j)=1 then C(i,j) is a
     * starred zero,  If M(i,j)=2 then C(i,j) is a primed zero.  We also define two vectors
     * RowCover and ColCover that are used to "cover" the rows and columns of the cost matrix.
     * In the nested loop (over indices i and j) we check to see if C(i,j) is a zero value
     * and if its column or row is not already covered.  If not then we star this zero
     * (i.e. set M(i,j)=1) and cover its row and column (i.e. set R_cov(i)=1 and C_cov(j)=1).
     * Before we go on to Step 3, we uncover all rows and columns so that we can use the
     * cover vectors to help us count the number of starred zeros. */
void step2(const std::vector<std::vector<int>>& matrix, std::vector<std::vector<int>>& M,
           std::vector<int>& RowCover, std::vector<int>& ColCover, int& step)
{
    int sz = matrix.size();

    for (int r = 0; r < sz; ++r)
        for (int c = 0; c < sz; ++c)
            if (matrix[r][c] == 0)
                if (RowCover[r] == 0 && ColCover[c] == 0)
                {
                    M[r][c] = 1;
                    RowCover[r] = 1;
                    ColCover[c] = 1;
                }

    clear_covers(RowCover); // reset vectors for posterior using
    clear_covers(ColCover);

    step = 3;
}


/* Cover each column containing a starred zero.  If K columns are covered, the starred
     * zeros describe a complete set of unique assignments.  In this case, Go to DONE,
     * otherwise, Go to Step 4. Once we have searched the entire cost matrix, we count the
     * number of independent zeros found.  If we have found (and starred) K independent zeros
     * then we are done.  If not we procede to Step 4.*/
void step3(const std::vector<std::vector<int>>& M, std::vector<int>& ColCover, int& step)
{
    int sz = M.size();
    int colcount = 0;

    for (int r = 0; r < sz; ++r)
        for (int c = 0; c < sz; ++c)
            if (M[r][c] == 1)
                ColCover[c] = 1;

    for (auto& n : ColCover)
        if (n == 1)
            colcount++;

    if (colcount >= sz)
    {
        step = 7; // solution found
    }
    else
    {
        step = 4;
    }
}

// Following functions to support step 4
void find_a_zero(int& row, int& col, const std::vector<std::vector<int>>& matrix,
                 const std::vector<int>& RowCover, const std::vector<int>& ColCover)
{
    int r = 0;
    int c = 0;
    int sz = matrix.size();
    bool done = false;
    row = -1;
    col = -1;

    while (!done)
    {
        c = 0;
        while (true)
        {
            if (matrix[r][c] == 0 && RowCover[r] == 0 && ColCover[c] == 0)
            {
                row = r;
                col = c;
                done = true;
            }
            c += 1;
            if (c >= sz || done)
                break;
        }
        r += 1;
        if (r >= sz)
            done = true;
    }
}

bool star_in_row(int row, const std::vector<std::vector<int>>& M)
{
    bool tmp = false;
    for (unsigned c = 0; c < M.size(); c++)
        if (M[row][c] == 1)
            tmp = true;

    return tmp;
}


void find_star_in_row(int row, int& col, const std::vector<std::vector<int>>& M)
{
    col = -1;
    for (unsigned c = 0; c < M.size(); c++)
        if (M[row][c] == 1)
            col = c;
}


/* Find a noncovered zero and prime it.  If there is no starred zero in the row containing
     * this primed zero, Go to Step 5.  Otherwise, cover this row and uncover the column
     * containing the starred zero. Continue in this manner until there are no uncovered zeros
     * left. Save the smallest uncovered value and Go to Step 6. */
void step4(const std::vector<std::vector<int>>& matrix, std::vector<std::vector<int>>& M,
           std::vector<int>& RowCover, std::vector<int>& ColCover, int& path_row_0, int& path_col_0,
           int& step)
{
    int row = -1;
    int col = -1;
    bool done = false;

    while (!done)
    {
        find_a_zero(row, col, matrix, RowCover, ColCover);

        if (row == -1)
        {
            done = true;
            step = 6;
        }
        else
        {
            M[row][col] = 2;
            if (star_in_row(row, M))
            {
                find_star_in_row(row, col, M);
                RowCover[row] = 1;
                ColCover[col] = 0;
            }
            else
            {
                done = true;
                step = 5;
                path_row_0 = row;
                path_col_0 = col;
            }
        }
    }
}

// Following functions to support step 5
void find_star_in_col(int c, int& r, const std::vector<std::vector<int>>& M)
{
    r = -1;
    for (unsigned i = 0; i < M.size(); i++)
        if (M[i][c] == 1)
            r = i;
}

void find_prime_in_row(int r, int& c, const std::vector<std::vector<int>>& M)
{
    for (unsigned j = 0; j < M.size(); j++)
        if (M[r][j] == 2)
            c = j;
}

void augment_path(std::vector<std::vector<int>>& path, int path_count,
                  std::vector<std::vector<int>>& M)
{
    for (int p = 0; p < path_count; p++)
        if (M[path[p][0]][path[p][1]] == 1)
            M[path[p][0]][path[p][1]] = 0;
        else
            M[path[p][0]][path[p][1]] = 1;
}

void erase_primes(std::vector<std::vector<int>>& M)
{
    for (auto& row : M)
        for (auto& val : row)
            if (val == 2)
                val = 0;
}


/* Construct a series of alternating primed and starred zeros as follows.
     * Let Z0 represent the uncovered primed zero found in Step 4.  Let Z1 denote the
     * starred zero in the column of Z0 (if any). Let Z2 denote the primed zero in the
     * row of Z1 (there will always be one).  Continue until the series terminates at a
     * primed zero that has no starred zero in its column.  Unstar each starred zero of
     * the series, star each primed zero of the series, erase all primes and uncover every
     * line in the matrix.  Return to Step 3.  You may notice that Step 5 seems vaguely
     * familiar.  It is a verbal description of the augmenting path algorithm (for solving
     * the maximal matching problem). */
void step5(std::vector<std::vector<int>>& path, int path_row_0, int path_col_0,
           std::vector<std::vector<int>>& M, std::vector<int>& RowCover, std::vector<int>& ColCover,
           int& step)
{
    int r = -1;
    int c = -1;
    int path_count = 1;

    path[path_count - 1][0] = path_row_0;
    path[path_count - 1][1] = path_col_0;

    bool done = false;
    while (!done)
    {
        find_star_in_col(path[path_count - 1][1], r, M);
        if (r > -1)
        {
            path_count += 1;
            path[path_count - 1][0] = r;
            path[path_count - 1][1] = path[path_count - 2][1];
        }
        else
        {
            done = true;
        }

        if (!done)
        {
            find_prime_in_row(path[path_count - 1][0], c, M);
            path_count += 1;
            path[path_count - 1][0] = path[path_count - 2][0];
            path[path_count - 1][1] = c;
        }
    }

    augment_path(path, path_count, M);
    clear_covers(RowCover);
    clear_covers(ColCover);
    erase_primes(M);

    step = 3;
}

// methods to support step 6
template<typename T>
void find_smallest(T& minval, const std::vector<std::vector<T>>& matrix,
                   const std::vector<int>& RowCover, const std::vector<int>& ColCover)
{
    for (unsigned r = 0; r < matrix.size(); r++)
        for (unsigned c = 0; c < matrix.size(); c++)
            if (RowCover[r] == 0 && ColCover[c] == 0)
                if (minval > matrix[r][c])
                    minval = matrix[r][c];
}

/* Add the value found in Step 4 to every element of each covered row, and subtract it
     * from every element of each uncovered column.  Return to Step 4 without altering any
     * stars, primes, or covered lines. Notice that this step uses the smallest uncovered
     * value in the cost matrix to modify the matrix.  Even though this step refers to the
     * value being found in Step 4 it is more convenient to wait until you reach Step 6
     * before searching for this value.  It may seem that since the values in the cost
     * matrix are being altered, we would lose sight of the original problem.
     * However, we are only changing certain values that have already been tested and
     * found not to be elements of the minimal assignment.  Also we are only changing the
     * values by an amount equal to the smallest value in the cost matrix, so we will not
     * jump over the optimal (i.e. minimal assignment) with this change. */
template<typename T>
void step6(std::vector<std::vector<T>>& matrix, const std::vector<int>& RowCover,
           const std::vector<int>& ColCover, int& step)
{
    T minval = std::numeric_limits<T>::max();
    find_smallest(minval, matrix, RowCover, ColCover);

    int sz = matrix.size();
    for (int r = 0; r < sz; r++)
        for (int c = 0; c < sz; c++)
        {
            if (RowCover[r] == 1)
                matrix[r][c] += minval;
            if (ColCover[c] == 0)
                matrix[r][c] -= minval;
        }

    step = 4;
}

/* Calculates the optimal cost from mask matrix */
int output_solution(const std::vector<std::vector<int>>& original,
                    const std::vector<std::vector<int>>& M)
{
    int res = 0;

    for (unsigned j = 0; j < original.begin()->size(); ++j)
        for (unsigned i = 0; i < original.size(); ++i)
            if (M[i][j])
            {
                auto it1 = original.begin();
                std::advance(it1, i);
                auto it2 = it1->begin();
                std::advance(it2, j);
                res += *it2;
                continue;
            }

    return res;
}


/* Main function of the algorithm */
void solveHungarian(const std::vector<std::vector<int>>& original, std::vector<std::vector<int>>& M,
                    bool allow_negatives, bool maximize)
{
    /* Initialize data structures */

    // Work on a vector copy to preserve original matrix
    // Didn't passed by value cause needed to access both
    std::vector<std::vector<int>> matrix(original.size(),
                                         std::vector<int>(original.begin()->size()));

    auto it = original.begin();
    for (auto& vec : matrix)
    {
        std::copy(it->begin(), it->end(), vec.begin());
        if (maximize)
        {
            allow_negatives = true;
            for (auto& i : vec)
                i *= (-1);
        }
        it = std::next(it);
    }

    // handle negative values -> pass true if allowed or false otherwise
    // if it is an unsigned type just skip this step
    if (!std::is_unsigned<int>::value)
    {
        handle_negatives(matrix, allow_negatives);
    }


    // make square matrix
    pad_matrix(matrix);
    std::size_t sz = matrix.size();

    /* The masked matrix M.  If M(i,j)=1 then C(i,j) is a starred zero,
         * If M(i,j)=2 then C(i,j) is a primed zero. */
    M.assign(sz, std::vector<int>(sz, 0));

    /* We also define two vectors RowCover and ColCover that are used to "cover"
         *the rows and columns of the cost matrix C*/
    std::vector<int> RowCover(sz, 0);
    std::vector<int> ColCover(sz, 0);

    int path_row_0, path_col_0; //temporary to hold the smallest uncovered value

    // Array for the augmenting path algorithm
    std::vector<std::vector<int>> path(sz + 1, std::vector<int>(2, 0));

    /* Now Work The Steps */
    bool done = false;
    int step = 1;
    while (!done)
    {
        switch (step)
        {
        case 1:
            step1(matrix, step);
            break;
        case 2:
            step2(matrix, M, RowCover, ColCover, step);
            break;
        case 3:
            step3(M, ColCover, step);
            break;
        case 4:
            step4(matrix, M, RowCover, ColCover, path_row_0, path_col_0, step);
            break;
        case 5:
            step5(path, path_row_0, path_col_0, M, RowCover, ColCover, step);
            break;
        case 6:
            step6(matrix, RowCover, ColCover, step);
            break;
        case 7:
            for (auto& vec : M)
            {
                vec.resize(original.begin()->size());
            }
            M.resize(original.size());
            done = true;
            break;
        default:
            done = true;
            break;
        }
    }

    // return output_solution(original, M);
}

void getEdgesFromHungarian(std::vector<std::vector<int>> const& adjMatrix,
                           std::pair<std::vector<int>, std::vector<int>> const& bipartitionIndexes,
                           std::vector<std::vector<int>> const& hungarianMask,
                           std::vector<std::pair<int, int>>& edges)
{
    edges.clear();

    for (unsigned j = 0; j < adjMatrix.begin()->size(); ++j)
    {
        for (unsigned i = 0; i < adjMatrix.size(); ++i)
        {
            if (hungarianMask[i][j])
            {
                auto it1 = adjMatrix.begin();
                std::advance(it1, i);
                auto it2 = it1->begin();
                std::advance(it2, j);

                edges.push_back({ bipartitionIndexes.first[i], bipartitionIndexes.second[j] });

                continue;
            }
        }
    }
}

} // namespace Hungarian

} // namespace Graph
