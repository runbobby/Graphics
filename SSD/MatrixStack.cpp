#include "MatrixStack.h"

///Note: This Matrix Stack most naturally applies to the order of composition associated with animation hierarchies,
///but was also useful for BWTJT and CJTWT
///One could imagine a stack that multiplies in the other direction, but it was not needed here
///Theoretically, this class enables certain independent animations for multiple SSD models, but
///this was not explored here.

MatrixStack::MatrixStack() : m_matrices(1, Matrix4f::identity())
{
	// Initialize the matrix stack with the identity matrix.
}

void MatrixStack::clear()
{
	m_matrices = std::vector<Matrix4f>(1, Matrix4f::identity());
	// Revert to just containing the identity matrix.
}

Matrix4f MatrixStack::top()
{
	// Return the top of the stack
	return m_matrices.back();
}

void MatrixStack::push( const Matrix4f& m )
{
	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m
	m_matrices.push_back(m_matrices.back()*m);

}

void MatrixStack::pop()
{
	m_matrices.pop_back();
	// Remove the top element from the stack
}
