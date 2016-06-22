/*
 * MatrixTools.h
 *
 *  Created on: May 31, 2016
 *      Author: Michael Wegner (michael.wegner@student.kit.edu)
 */

#ifndef NETWORKIT_CPP_ALGEBRAIC_MATRIXTOOLS_H_
#define NETWORKIT_CPP_ALGEBRAIC_MATRIXTOOLS_H_

#include "../graph/Graph.h"
#include <atomic>

namespace MatrixTools {

/**
 * Checks if @a matrix is symmetric.
 * @param matrix
 * @return True if @a matrix is symmetric, otherwise false.
 */
template<class MATRIX>
bool isSymmetric(const MATRIX& matrix) {
	bool output = true;
	matrix.forNonZeroElementsInRowOrder([&] (NetworKit::index i, NetworKit::index j, NetworKit::edgeweight w) {
		if (abs(matrix(j, i)-w) > NetworKit::EPSILON) {
			return false;
		}
	});
	return output;
}

/**
 * Checks if @a matrix is symmetric diagonally dominant (SDD).
 * @param matrix
 * @return True if @a matrix is SDD, false otherwise.
 */
template<class MATRIX>
bool isSDD(const MATRIX& matrix) {
	if (!isSymmetric(matrix)) {
		return false;
	}

	/* Criterion: a_ii >= \sum_{j != i} a_ij */
	std::vector<double> row_sum(matrix.numberOfRows());
	matrix.parallelForNonZeroElementsInRowOrder([&] (NetworKit::node i, NetworKit::node j, double value) {
		if (i == j) {
			row_sum[i] += value;
		} else {
			row_sum[i] -= abs(value);
		}
	});

	return std::all_of(row_sum.begin(), row_sum.end(), [] (double val) {return val > -NetworKit::EPSILON;});
}

/**
 * Checks if @a matrix is a Laplacian matrix.
 * @param matrix
 * @return True if @a matrix is a Laplacian matrix, false otherwise.
 */
template<typename MATRIX>
bool isLaplacian(const MATRIX& matrix) {
	if (!isSymmetric(matrix)) {
		return false;
	}

	/* Criterion: \forall_i \sum_j A_ij = 0  */
	std::vector<double> row_sum(matrix.numberOfRows());
	std::atomic<bool> right_sign(true);
	matrix.parallelForNonZeroElementsInRowOrder([&] (NetworKit::node i, NetworKit::node j, double value) {
		if (i != j && value > NetworKit::EPSILON) {
			right_sign = false;
		}
		row_sum[i] += value;
	});

	return right_sign && std::all_of(row_sum.begin(), row_sum.end(), [] (double val) {return abs(val) < NetworKit::EPSILON;});
}

/**
 * Computes a graph having the given @a laplacian.
 * @param laplacian
 * @return The graph having a Laplacian equal to @a laplacian.
 */
template<class MATRIX>
NetworKit::Graph laplacianToGraph(const MATRIX& laplacian) {
	assert(isLaplacian(laplacian));
	NetworKit::Graph G(std::max(laplacian.numberOfRows(), laplacian.numberOfColumns()), true, false);
	laplacian.forNonZeroElementsInRowOrder([&](NetworKit::node u, NetworKit::node v, NetworKit::edgeweight weight) {
		if (u != v) { // exclude diagonal
			if (u < v) {
				G.addEdge(u, v, -weight);
			}
		}
	});

	return G;
}

/**
 * Interprets the @a matrix as adjacency matrix of a graph. If @a matrix is non-symmetric, the graph will be directed.
 * @param matrix
 * @return The graph having an adjacency matrix equal to @a matrix.
 */
template<class MATRIX>
NetworKit::Graph matrixToGraph(const MATRIX& matrix) {
	bool directed = !isSymmetric(matrix);
	NetworKit::Graph G(std::max(matrix.numberOfRows(), matrix.numberOfColumns()), true, directed);
	matrix.forNonZeroElementsInRowOrder([&](NetworKit::node u, NetworKit::node v, NetworKit::edgeweight weight) {
		if (directed || u <= v) {
			G.addEdge(u, v, weight);
		}
	});

	return G;
}

}



#endif /* NETWORKIT_CPP_ALGEBRAIC_MATRIXTOOLS_H_ */