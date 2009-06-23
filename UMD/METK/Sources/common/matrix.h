// matrix.h: Class for handle matrix of given size

# ifndef __MATRIX_HEADER__
# define __MATRIX_HEADER__

class MatrixRow
{
	double *r;
	int size;
public:
	MatrixRow(int s)
	{
		size = s;
		r = new double[s];
	}

	~MatrixRow ()
	{
		delete[] r;
	}

	double& operator[](int i)
	{
		if (i<0 || i>=size)
		{
			std::cout << "Row index is out of range" << std::endl;
			return r[0];
		}
		else
			return r[i];
	}
};


class Matrix
{
	MatrixRow **mat;	//Zeiger auf "Row"-Vektoren
	int lines, cols;	//Zeilen- und Spaltenanzahl

public:
	Matrix(int r, int c)
	{
		lines = r;
		cols = c;
		mat = new MatrixRow*[lines];
		for (int i=0; i<lines; i++)
		{
			mat[i] = new MatrixRow(cols);
		}
	}

	~Matrix()
	{
		for (int i=0; i<lines; i++)
		{
			delete mat[i];
		}
		delete[] mat;
	}

	int getLines() const { return lines; }

	int getCols() const { return cols; }

	MatrixRow& operator[] (int i)
	{
		if (i<0 || i>=lines)
		{
			std::cout << "Row index is out of range" << std::endl;
			return *mat[0];
		}
		else
			return *mat[i];
	}

};


#endif