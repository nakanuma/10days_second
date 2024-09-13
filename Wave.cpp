#include "Wave.h"

void Wave::Debug() {
#ifdef _DEBUG

	ImGui::Text("%d", wave_);

#endif
}

int Wave::GetWave() const { return wave_; }

void Wave::SetWave(int wave) { wave_ = wave; }