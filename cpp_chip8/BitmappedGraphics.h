#pragma once

#include <vector>

class Memory;

class BitmappedGraphics
{
public:
	enum {
		DefaultPlane = 0x1,

		ScreenWidthLow = 64,
		ScreenHeightLow = 32,

		ScreenWidthHigh = 128,
		ScreenHeightHigh = 64
	};

	BitmappedGraphics(int numberOfPlanes, bool clip, bool countExceededRows, bool countRowHits);

	int getNumberOfPlanes() const {
		return m_numberOfPlanes;
	}

	int getNumberOfColours() const {
		return 1 << m_numberOfPlanes;
	}

	const std::vector<std::vector<int>>& getGraphics() const {
		return m_graphics;
	}

	bool getHighResolution() const {
		return m_highResolution;
	}

	void setHighResolution(bool value) {
		m_highResolution = value;
	}

	bool getLowResolution() const {
		return !getHighResolution();
	}

	int getWidth() const {
		return getHighResolution() ? ScreenWidthHigh : ScreenWidthLow;
	}

	int getHeight() const {
		return getHighResolution() ? ScreenHeightHigh : ScreenHeightLow;
	}

	int getPlaneMask() const {
		return m_planeMask;
	}

	void setPlaneMask(int value) {
		m_planeMask = value;
	}

	void initialise();

	void allocateMemory();
	
	int draw(const Memory& memory, int address, int drawX, int drawY, int width, int height);

	void scrollUp(int count);
	void scrollDown(int count);
	void scrollLeft();
	void scrollRight();

	void clear();

private:
	int m_numberOfPlanes;
	std::vector<std::vector<int>> m_graphics;
	int m_planeMask;
	bool m_highResolution;
	bool m_clip;
	bool m_countExceededRows;
	bool m_countRowHits;

	bool isPlaneSelected(int plane) const;

	int draw(int plane, const Memory& memory, int address, int drawX, int drawY, int width, int height);

	void allocateMemory(int plane);

	void maybeScrollUp(int plane, int count);
	void scrollUp(int plane, int count);

	void maybeScrollDown(int plane, int count);
	void scrollDown(int plane, int count);

	void maybeScrollLeft(int plane);
	void scrollLeft(int plane);

	void maybeScrollRight(int plane);
	void scrollRight(int plane);

	void maybeClear(int plane);
	void clear(int plane);

	void clearRow(int plane, int row);
	void clearColumn(int plane, int column);
	void copyRow(int plane, int source, int destination);
	void copyColumn(int plane, int source, int destination);
};
