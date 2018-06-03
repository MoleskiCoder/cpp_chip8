#pragma once

#include <functional>
#include <vector>

template<class T> class Signal final {
private:
	typedef std::function<void(const T&)> delegate_t;
	typedef std::vector<delegate_t> delegates_t;

	delegates_t delegates;

public:
	void connect(delegate_t functor) {
		delegates.push_back(functor);
	}

	void fire(const T& e) const {
		if (!delegates.empty())
			for (auto& delegate : delegates)
				delegate(e);
	}
};
