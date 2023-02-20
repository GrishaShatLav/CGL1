#include "Game.h"
#include "TriangleComponent.h"

int main() {
	Game* game = Game::CreateInstance(L"My Game", 800, 800);
	game->components.push_back(new TriangleComponent(-0.5f));
	game->components.push_back(new TriangleComponent(0.5f));
	game->Run();

	delete game;
}
