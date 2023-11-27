
#include "node.h"
class Light;

class Jeep : public Node
{
public:
				Jeep();
virtual		~Jeep();
	void		create();
	bool		update(double dt);
	void		switchMove();
	void		turnLight();
private:
	Node* createWheel(Material* mtl, float x, float z, float wheelWidth, float angle);
	Node* createLight(Material* mtl, float x, float y, float z, float radius, float width, Light** pLight);
private:
	float		m_WheelRadius;
	Node* m_WheelNode[4];
	Light* m_Lights[2];
	bool		m_IsMoving;
	double	m_MovingTime;
};

extern Jeep* g_Jeep;