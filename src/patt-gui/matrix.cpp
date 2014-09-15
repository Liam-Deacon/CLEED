#include "matrix.hpp"

Matrix2x2::Matrix2x2()
{
    m.resize(2);
    m[0].resize(2);
    m[1].resize(2);
    m[0][0] = 1.;
    m[0][1] = 0.;
    m[1][0] = 0.;
    m[1][1] = 1.;
}


Matrix2x2::~Matrix2x2()
{

}
