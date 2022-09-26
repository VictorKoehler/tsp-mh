#ifndef _SIMPLE_MATRIX_H
#define _SIMPLE_MATRIX_H

#include <assert.h>
#include <vector>

template <typename T>
class Simple2DMatrix {
   private:
    int sX, sY;
    std::vector< std::vector<T> > mat;

   public:
    Simple2DMatrix(int rows, int cols) { resize(rows, cols); }
    Simple2DMatrix(int size) : Simple2DMatrix(size, size) {}
    // ~Array2D() {}

    inline int rows() const { return sX; };
    inline int cols() const { return sY; };
    inline int size() const { assert(rows() == cols()); return sX; };

    inline T operator()(int i, int j) const { assert(i < rows() && j < cols()); return mat[i][j]; };
    inline T& operator()(int i, int j) { assert(i < rows() && j < cols()); return mat[i][j]; };

	inline void resize(int r, int c) {
		sX = r;
		sY = c;
		mat.resize(r);
		for (auto& ri : mat) {
			ri.resize(c);
		}
	}

	inline void resize(int r, int c, T d) {
		sX = r;
		sY = c;
		mat.resize(r);
		for (auto& ri : mat) {
			ri.resize(c, d);
		}
	}
};

#endif
