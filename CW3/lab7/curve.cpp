#include <GL/gl3w.h>
#include "casteljau.h"
#include "curve.h"

Curve* g_Curve = NULL;

Curve::Curve()
{
	num_curve_verts = 0;
	num_ctrl_verts = 0;

}
Curve::~Curve()
{
	glDeleteVertexArrays(BUFFER_TYPE_COUNT, m_VAOs);
	glDeleteBuffers(BUFFER_TYPE_COUNT, m_VBOs);
}
void	Curve::create()
{
	std::vector<point> ctrl_points;
	float y = 1.0f;
	float offset = 20.0f;
	ctrl_points.push_back(point(0.0f, y, offset));
	ctrl_points.push_back(point(offset*1.5f, y, offset));
	ctrl_points.push_back(point(offset*1.5f, y, 0.0f));
	ctrl_points.push_back(point(offset, y, -offset));
	ctrl_points.push_back(point(0.0f, y, -offset*1.5f));
	ctrl_points.push_back(point(-offset*1.5f, y, -offset*1.5f));
	ctrl_points.push_back(point(-offset, y, 0));
	ctrl_points.push_back(point(-offset, y, offset));
	ctrl_points.push_back(point(0.0f, y, offset));

	m_CtrlPoints = std::list<point>(ctrl_points.begin(), ctrl_points.end());

	int num_evaluations = 128;
	std::vector<point> curve = EvaluateBezierCurve(ctrl_points, num_evaluations);

	int num_curve_floats = 0;
	float* curve_vertices = MakeFloatsFromVector(curve, num_curve_verts, num_curve_floats, 0.f, 0.f, 0.f);

	int num_ctrl_floats = 0;
	float* ctrl_vertices = MakeFloatsFromVector(ctrl_points, num_ctrl_verts, num_ctrl_floats, 0.f, 0.f, 0.f);


	glCreateBuffers(BUFFER_TYPE_COUNT, m_VBOs);
	glGenVertexArrays(BUFFER_TYPE_COUNT, m_VAOs);

	createBuffer(BUFFER_TYPE_LINE, curve_vertices, sizeof(float) * num_curve_floats);
	createBuffer(BUFFER_TYPE_POINT, ctrl_vertices, sizeof(float) * num_ctrl_floats);

	glPointSize(8);

}

void	Curve::createBuffer(BUFFER_TYPE type, float* data, int dataLen)
{
	glBindVertexArray(m_VAOs[type]);
	glNamedBufferStorage(m_VBOs[type], dataLen, data, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[type]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (6 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (6 * sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}
void	Curve::render()
{
	glBindVertexArray(m_VAOs[BUFFER_TYPE_LINE]);
	glDrawArrays(GL_LINE_STRIP, 0, num_curve_verts);

	glBindVertexArray(m_VAOs[BUFFER_TYPE_POINT]);
	glDrawArrays(GL_POINTS, 0, num_ctrl_verts);

}
void	Curve::getPosition(float t,glm::vec3& pos)
{
	point p = evaluate(t, m_CtrlPoints);
	pos.x = p.x;
	pos.y = p.y;
	pos.z = p.z;
}
