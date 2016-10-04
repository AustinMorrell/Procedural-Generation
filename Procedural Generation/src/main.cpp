#include "app.h"

int main()
{
	App* application = new ProceduralGeneration();

	if (application->start() == true)
	{
		while (application->update() == true)
		{
			application->draw();
		}
		application->destroy();
	}
	delete application;

	return 0;
}