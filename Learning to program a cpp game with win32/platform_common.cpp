struct ButtonState {
	bool isDown;
	bool changed;
};

enum buttons {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_RIGHT,
	BUTTON_LEFT,

	BUTTON_COUNT
};

struct Input {
	ButtonState buttons[BUTTON_COUNT]; 
};

