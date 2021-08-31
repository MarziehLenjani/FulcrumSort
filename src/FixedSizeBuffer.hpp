
#pragma once

#include "types.hpp"
#include <queue>

template <typename T, u64 BUFFER_SIZE>
class FixedSizeBuffer {
	std::queue<T> arr;

public:
	bool push(const T& val){
		if(size() < BUFFER_SIZE){
			arr.push(val);
			return true;
		}
		return false;
	}

	bool front(T& ret) {
		if(size()){
			ret = arr.front();
			return true;
		}
		return false;
	}

	bool pop(){
		if(size()){
			arr.pop();
			return true;
		}
		return false;
	}

	u64 size() const {
		return arr.size();
	}

};
