#pragma once
#include "point.h"
#include <list>
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

class Curve
{
public:
	Curve();
	~Curve();
	void	create();
	void	render();
	void	getPosition(float t, glm::vec3& pos);
private:
	enum BUFFER_TYPE {
		BUFFER_TYPE_LINE,
		BUFFER_TYPE_POINT,
		BUFFER_TYPE_COUNT
	};
	void	createBuffer(BUFFER_TYPE type, float* data, int dataLen);
	unsigned int m_VBOs[BUFFER_TYPE_COUNT];
	unsigned int m_VAOs[BUFFER_TYPE_COUNT];
	int num_curve_verts;
	int num_ctrl_verts;
	std::list<point> m_CtrlPoints;

};

extern	Curve* g_Curve;