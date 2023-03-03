#ifndef __MATRIX_HPP__
#define __MATRIX_HPP__

#include <vector>

using uint = unsigned int;

class Matrix {
public:

//Constructors & Destroyer
    Matrix(uint R, uint C, double init = 0);
    Matrix(uint R, uint C, double min, double max);
    Matrix(const Matrix& o);
    Matrix();
    ~Matrix();

//Operators
    Matrix operator + (const Matrix& o) const;
    Matrix operator - (const Matrix& o) const;
    Matrix operator * (const Matrix& o) const;
    Matrix operator ^ (const Matrix& o) const;
    Matrix operator + (double o) const;
    Matrix operator - (double o) const;
    Matrix operator * (double o) const;
    void operator += (const Matrix& o);
    void operator -= (const Matrix& o);
    void operator *= (const Matrix& o);
    void operator ^= (const Matrix& o);
    void operator += (double o);
    void operator -= (double o);
    void operator *= (double o);
    void operator = (const Matrix& o);
    Matrix transpose() const;
    Matrix map(double (*f) (double));

//Getters
    uint getRows() const;
    uint getColumns() const;
    double get(uint r, uint c) const;
    std::vector<double> asArray() const;

//Setters
    void set(uint r, uint c, double v);

private:
    uint _rows;
    uint _columns;
    double** _matrix;
};

#endif