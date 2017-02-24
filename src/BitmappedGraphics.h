#pragma once

#include <vector>

#include "GraphicsPlane.h"

class Memory;

class BitmappedGraphics {
public:
	enum {
		DefaultPlane = 0x1,
	};

	BitmappedGraphics();
	BitmappedGraphics(int numberOfPlanes, bool clip, bool countExceededRows, bool countRowHits);

	int getNumberOfPlanes() const {
		return m_numberOfPlanes;
	}

	int getNumberOfColours() const {
		return 1 << getNumberOfPlanes();
	}

	const std::vector<GraphicsPlane>& getPlanes() const {
		return m_planes;
	}

	bool getHighResolution() const {
		return m_highResolution;
	}

	void setHighResolution(bool value) {
		m_highResolution = value;
		for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
			m_planes[plane].setHighResolution(value);
		}
	}

	bool getLowResolution() const {
		return !getHighResolution();
	}

	int getWidth() const {
		return getHighResolution() ? GraphicsPlane::ScreenWidthHigh : GraphicsPlane::ScreenWidthLow;
	}

	int getHeight() const {
		return getHighResolution() ? GraphicsPlane::ScreenHeightHigh : GraphicsPlane::ScreenHeightLow;
	}

	int getPlaneMask() const {
		return m_planeMask;
	}

	void setPlaneMask(int value) {
		m_planeMask = value;
	}

	bool getDirty() const {
		return m_dirty;
	}

	void setDirty(bool value) {
		m_dirty = value;
	}

	void initialise();

	int draw(const Memory& memory, int address, int drawX, int drawY, int width, int height);

	void scrollUp(int count);
	void scrollDown(int count);
	void scrollLeft();
	void scrollRight();

	void clear();

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(
			m_numberOfPlanes,
			m_planes,
			m_planeMask,
			m_highResolution,
			m_countRowHits,
			m_dirty);
	}

	int m_numberOfPlanes;
	std::vector<GraphicsPlane> m_planes;
	int m_planeMask;
	bool m_highResolution;
	bool m_countRowHits;
	bool m_dirty;

	bool isPlaneSelected(int plane) const;

	void maybeScrollUp(int plane, int count);
	void maybeScrollDown(int plane, int count);
	void maybeScrollLeft(int plane);
	void maybeScrollRight(int plane);

	void maybeClear(int plane);
};
