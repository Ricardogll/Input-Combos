#include "ctDefs.h"
#include "ctLog.h"
#include "ctApp.h"
#include "ctInput.h"
#include "ctWindow.h"
#include "ctRender.h"
#include "SDL/include/SDL.h"

#define MAX_KEYS 300

ctInput::ctInput() : ctModule()
{
	name ="input";

	keyboard = new ctKeyState[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(ctKeyState) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(ctKeyState) * NUM_MOUSE_BUTTONS);
}

// Destructor
ctInput::~ctInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ctInput::Awake(pugi::xml_node& config)
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool ctInput::Start()
{
	SDL_StopTextInput();
	return true;
}

// Called each loop iteration
bool ctInput::PreUpdate()
{
	static SDL_Event event;

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			is_keyboard_available = true;
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if (mouse_buttons[i] == KEY_DOWN)
			mouse_buttons[i] = KEY_REPEAT;

		if (mouse_buttons[i] == KEY_UP)
			mouse_buttons[i] = KEY_IDLE;
	}

	for (int i = 0; i < NUM_PAD_BUTTONS; ++i)
	{
		if (pad_buttons[i] == KEY_DOWN)
			pad_buttons[i] = KEY_REPEAT;

		if (pad_buttons[i] == KEY_UP)
			pad_buttons[i] = KEY_IDLE;
	}

	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
		case SDL_QUIT:
			windowEvents[WE_QUIT] = true;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
				//case SDL_WINDOWEVENT_LEAVE:
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				windowEvents[WE_HIDE] = true;
				break;

				//case SDL_WINDOWEVENT_ENTER:
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:
				windowEvents[WE_SHOW] = true;
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_buttons[event.button.button - 1] = KEY_DOWN;
			//LOG("Mouse button %d down", event.button.button-1);
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_buttons[event.button.button - 1] = KEY_UP;
			//LOG("Mouse button %d up", event.button.button-1);
			break;

		case SDL_MOUSEMOTION:
			int scale = App->win->GetScale();
			mouse_motion_x = event.motion.xrel / scale;
			mouse_motion_y = event.motion.yrel / scale;
			mouse_x = event.motion.x / scale;
			mouse_y = event.motion.y / scale;
			//LOG("Mouse motion x %d y %d", mouse_motion_x, mouse_motion_y);
			break;

		case SDL_JOYAXISMOTION:
			if (event.jaxis.which == 0)
				is_keyboard_available = false;
			{
				if (event.jaxis.axis == 0)
				{
					if (event.jaxis.value < -PAD_DEAD_ZONE || event.jaxis.value > PAD_DEAD_ZONE)
						x_axis = event.jaxis.value;
					else
						x_axis = 0;
				}
				else if (event.jaxis.axis == 1)
				{
					if (event.jaxis.value < -PAD_DEAD_ZONE || event.jaxis.value > PAD_DEAD_ZONE)
						y_axis = event.jaxis.value;
					else
						y_axis = 0;
				}
			}
			break;

		case SDL_JOYBUTTONDOWN:
			if (event.jbutton.which == 0)
			{
				// RUMBLE TEST
				if (SDL_HapticRumblePlay(pad_controller_haptic, 0.75, 500) != 0)
				{
					printf("Warning: Unable to play rumble! %s\n", SDL_GetError());
				}
				is_keyboard_available = false;
				pad_buttons[event.jbutton.button - 1] = KEY_DOWN;
			}
			break;

		case SDL_JOYBUTTONUP:
			if (event.jbutton.which == 0)
				pad_buttons[event.jbutton.button - 1] = KEY_UP;
		}
	}

	return true;
}

// Called before quitting
bool ctInput::CleanUp()
{
	LOG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// ---------
bool ctInput::GetWindowEvent(ctEventWindow ev)
{
	return windowEvents[ev];
}

void ctInput::GetMousePosition(int& x, int& y)
{
	x = mouse_x;
	y = mouse_y;
}

void ctInput::GetMouseMotion(int& x, int& y)
{
	x = mouse_motion_x;
	y = mouse_motion_y;
}

void ctInput::GetWorldMousePosition(int& x, int& y)
{
	x = App->render->ScreenToWorld(mouse_x, mouse_y).x;
	y = App->render->ScreenToWorld(mouse_x, mouse_y).y;
}