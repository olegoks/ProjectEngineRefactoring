#include "LogicEngine.hpp"

const KeystrProcFunc LogicEngine::kDefaultKeystrProcFunc = [](const KeyAction&) {};
const LogicFunc LogicEngine::kDefaultLogicFunc = []() {};
//const NextKeystroke LogicEngine::kDefaultNextKeystroke = []()->Keystroke { return Keystroke{}; };

LogicEngine::LogicEngine()noexcept(true) :
	engine_is_running_{ false },
	camera_{ nullptr },
	device_data_{ nullptr },
	models_{ nullptr },
	logic_func_{ kDefaultLogicFunc },
	keystr_proc_func_{ kDefaultKeystrProcFunc },
	keystrokes_queue_{ nullptr }{}

void LogicEngine::UpdateWorldLogic()noexcept(true) {

	//there is the bag: size returned 2, but real size is 1? and that is why Range out exception in vector.
	//unsigned int number_of_keystrokes = keystroke_buffer_.size();

	unsigned int index = 0;

	//Process world logic
	logic_func_();

	//Process user input
	Keystroke keystroke = keystrokes_queue_->Pop();
	KeyAction useless{ Key::Nothing, Action::Nothing };

	while (keystroke.key_type != useless) {

		keystr_proc_func_(keystroke.key_type);
		keystroke = keystrokes_queue_->Pop();

	}

}

const Vertex3D& LogicEngine::ModelPosition(const ModelId model_id)const noexcept(true) {

	return (*models_)[model_id].model_center_;

}


