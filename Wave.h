#pragma once

#ifdef DEBUG
#include <ImGui.h>
#endif // DEBUG

// ウェーブ管理
class Wave {

public:
	Wave() = default;
	~Wave() = default;

	// シングルトン
	static Wave& GetInstance() {
		static Wave instance;
		return instance;
	}

	int GetWave() const;
	void SetWave(int wave);

	void Debug();

private:
	// ウェーブ
	int wave_ = 0;
};