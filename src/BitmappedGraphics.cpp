#include "stdafx.h"
#include "BitmappedGraphics.h"

BitmappedGraphics::BitmappedGraphics(int numberOfPlanes, bool clip, bool countExceededRows, bool countRowHits)
: m_numberOfPlanes(numberOfPlanes),
  m_planes(numberOfPlanes, GraphicsPlane(clip, countExceededRows)),
  m_planeMask(DefaultPlane),
  m_highResolution(false),
  m_countRowHits(countRowHits) {
}

void BitmappedGraphics::initialise() {
	setHighResolution(false);
}

int BitmappedGraphics::draw(const Memory& memory, int address, int drawX, int drawY, int width, int height) {
	auto bytesPerRow = width / 8;
	size_t hits = 0;
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		if (isPlaneSelected(plane)) {
			hits += m_planes[plane].draw(memory, address, drawX, drawY, width, height);
			address += height * bytesPerRow;
		}
	}
	setDirty(true);
	if (!m_countRowHits)
		hits = hits > 0 ? 1 : 0;
	return (int)hits;
}

void BitmappedGraphics::scrollDown(int count) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollDown(plane, count);
	}
	setDirty(true);
}

void BitmappedGraphics::scrollUp(int count) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollUp(plane, count);
	}
	setDirty(true);
}

void BitmappedGraphics::scrollLeft() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollLeft(plane);
	}
	setDirty(true);
}

void BitmappedGraphics::scrollRight() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollRight(plane);
	}
	setDirty(true);
}

void BitmappedGraphics::clear() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeClear(plane);
	}
	setDirty(true);
}

bool BitmappedGraphics::isPlaneSelected(int plane) const {
	auto mask = 1 << plane;
	auto selected = (getPlaneMask() & mask) != 0;
	return selected;
}

void BitmappedGraphics::maybeScrollDown(int plane, int count) {
	if (isPlaneSelected(plane)) {
		m_planes[plane].scrollDown(count);
	}
}

void BitmappedGraphics::maybeScrollUp(int plane, int count) {
	if (isPlaneSelected(plane)) {
		m_planes[plane].scrollUp(count);
	}
}

void BitmappedGraphics::maybeScrollLeft(int plane) {
	if (isPlaneSelected(plane)) {
		m_planes[plane].scrollLeft();
	}
}

void BitmappedGraphics::maybeScrollRight(int plane) {
	if (isPlaneSelected(plane)) {
		m_planes[plane].scrollRight();
	}
}

void BitmappedGraphics::maybeClear(int plane) {
	if (isPlaneSelected(plane)) {
		m_planes[plane].clear();
	}
}
