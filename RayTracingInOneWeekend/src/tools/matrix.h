#pragma once

#include <array>

#include "vec3.h"

template<typename T>
class Matrix
{
public:
	Matrix(unsigned int cols, unsigned int rows)
		: m_cols(cols), m_rows(rows) {};


	T GetAt(unsigned int col, unsigned int row) 
	{
		if (0 <= col && col < m_cols 
			&& 0 <= row && row < m_rows)
		{
			return 0;
		}
	}

	unsigned int GetCols() const { return m_cols; };
	unsigned int GetRows() const { return m_rows; };

private:
	const unsigned int m_cols;
	const unsigned int m_rows;
};