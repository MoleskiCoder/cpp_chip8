#include "stdafx.h"
#include "BitmappedGraphics.h"

BitmappedGraphics::BitmappedGraphics(int numberOfPlanes, bool clip, bool countExceededRows, bool countRowHits)
: m_numberOfPlanes(numberOfPlanes),
  m_planes(numberOfPlanes, GraphicsPlane(clip, countExceededRows)),
  m_planeMask(DefaultPlane),
  m_countRowHits(countRowHits) {}

void BitmappedGraphics::initialise() {
	setHighResolution(false);
}

int BitmappedGraphics::draw(const Memory& memory, int address, const int drawX, const int drawY, const int width, const int height) {
	const auto bytesPerRow = width / 8;
	size_t hits = 0;
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		if (isPlaneSelected(plane)) {
			hits += m_planes[plane].draw(memory, address, drawX, drawY, width, height);
			address += height * bytesPerRow;
		}
	}
	setDirty();
	if (!m_countRowHits)
		hits = hits > 0 ? 1 : 0;
	return (int)hits;
}

void BitmappedGraphics::scrollDown(const int count) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane)
		maybeScrollDown(plane, count);
	setDirty();
}

void BitmappedGraphics::scrollUp(int count) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane)
		maybeScrollUp(plane, count);
	setDirty();
}

void BitmappedGraphics::scrollLeft() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane)
		maybeScrollLeft(plane);
	setDirty();
}

void BitmappedGraphics::scrollRight() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane)
		maybeScrollRight(plane);
	setDirty();
}

void BitmappedGraphics::clear() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane)
		maybeClear(plane);
	setDirty();
}

bool BitmappedGraphics::isPlaneSelected(const int plane) const {
	const auto mask = 1 << plane;
	return (getPlaneMask() & mask) != 0;
}

void BitmappedGraphics::maybeScrollDown(const int plane, const int count) {
	if (isPlaneSelected(plane))
		m_planes[plane].scrollDown(count);
}

void BitmappedGraphics::maybeScrollUp(const int plane, const int count) {
	if (isPlaneSelected(plane))
		m_planes[plane].scrollUp(count);
}

void BitmappedGraphics::maybeScrollLeft(const int plane) {
	if (isPlaneSelected(plane))
		m_planes[plane].scrollLeft();
}

void BitmappedGraphics::maybeScrollRight(const int plane) {
	if (isPlaneSelected(plane))
		m_planes[plane].scrollRight();
}

void BitmappedGraphics::maybeClear(const int plane) {
	if (isPlaneSelected(plane))
		m_planes[plane].clear();
}
