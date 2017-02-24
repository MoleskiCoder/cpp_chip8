#pragma once

#include <vector>

class Memory;

class GraphicsPlane {
public:
	enum {
		ScreenWidthLow = 64,
		ScreenHeightLow = 32,

		ScreenWidthHigh = 128,
		ScreenHeightHigh = 64
	};

	GraphicsPlane() {};
	GraphicsPlane(bool clip, bool countExceededRows);

	const std::vector<int>& getGraphics() const {
		return m_graphics;
	}

	void setHighResolution(bool value) {
		m_highResolution = value;
		allocateMemory();
	}

	size_t draw(const Memory& memory, int address, int drawX, int drawY, int width, int height);

	void scrollUp(int count);
	void scrollDown(int count);
	void scrollLeft();
	void scrollRight();

	void clear();

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(
			m_graphics,
			m_highResolution,
			m_clip,
			m_countExceededRows);
	}

	std::vector<int> m_graphics;
	bool m_highResolution;
	bool m_clip;
	bool m_countExceededRows;

	void allocateMemory();

	bool getHighResolution() const {
		return m_highResolution;
	}

	int getWidth() const {
		return getHighResolution() ? ScreenWidthHigh : ScreenWidthLow;
	}

	int getHeight() const {
		return getHighResolution() ? ScreenHeightHigh : ScreenHeightLow;
	}

	void clearRow(int row);
	void clearColumn(int column);
	void copyRow(int source, int destination);
	void copyColumn(int source, int destination);
};
