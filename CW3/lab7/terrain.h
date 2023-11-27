
#include "node.h"
class Terrain : public Node
{
public:
			Terrain();
virtual	~Terrain();
	void	create(float tileSize, int tileNum, float textureTileSize, float centerRadius);

private:
};
