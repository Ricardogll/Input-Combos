#include "ctApp.h"
#include "InputEvent.h"
#include "ctRender.h"
#include "Combo.h"

Combo::Combo()
{
}

Combo::~Combo()
{

	vector<InputEvent*>::const_iterator it_combo_to_check = this->input_events.begin();

	while (it_combo_to_check != this->input_events.end()) {
		delete *it_combo_to_check;
		it_combo_to_check++;
	}

	input_events.clear();

}

void Combo::LoadInputEvent(InputEvent* input_event) {
	this->input_events.push_back(input_event);
}

void Combo::SetType(ComboType type){
	this->type = type;
}

ComboType Combo::GetType()const {
	return this->type;
}

bool Combo::CheckForSolvedCombo(list<InputEvent*> event_chain) const{

	//TODO 4.1: Find if there is a match between the list of events in the combo(this->input_events) and the volatile chain(event_chain).
	///Hints: You must iterate both lists and ask:
	/// * Is this combo input equal to event_chain ?
	/// * Is this combo input valid for a time limit ?
	/// * Is the combo completed ?
	/// If all the answers are yes, the combo has been performed correctly.
	/// Obviously you have to control the denials!

	
	//No combo solved?
	return false;
}