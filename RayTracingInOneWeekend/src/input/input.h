#pragma once

struct InputType 
{
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool leftClick = false;
	bool rightClick = false;
	bool leftShift = false;
};

class Input
{
public:
	Input() {};
	void HandleInput(bool& isRunning);

	bool Forward() const { return type.forward; };
	bool Backward() const { return type.backward; };
	bool Left() const { return type.left; };
	bool Right() const { return type.right; };
	bool Up() const { return type.up; };
	bool Down() const { return type.down; };
	bool leftClick() const { return type.leftClick; };
	bool rightClick() const { return type.rightClick; };
	bool LeftShift() const { return type.leftShift; };

private:
	InputType type;
	
};

