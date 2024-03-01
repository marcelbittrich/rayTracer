#include "iostream"
#include "src/application.h"

int main()
{
	Application app;

	while (app.IsRunning())
	{
		app.Run();
	}

	return 0;
}