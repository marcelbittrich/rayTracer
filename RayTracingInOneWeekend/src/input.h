#pragma once

struct InputType 
{
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool leftClick = false;
	bool leftShift = false;
};

class Input
{
public:
	Input() {};
	void HandleInput(bool& isRunning);

	bool Up() const { return type.up; };
	bool Down() const { return type.down; };
	bool Left() const { return type.left; };
	bool Right() const { return type.right; };
	bool leftClick() const { return type.leftClick; };
	bool LeftShift() const { return type.leftShift; };

private:
	InputType type;
	
};

