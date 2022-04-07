#pragma once

#include "types.hpp"

template <typename T, u64 BUF_SIZE>
class Buffer {
	T buffer[BUF_SIZE];
	u64 head = 0;
	u64 tail = 0;
	u64 count = 0;

public:
	Buffer(){};

	//must check if full before push
	void push(const T& val){
		count++;
		buffer[tail++] = val;
		tail = tail % BUF_SIZE;
	}

	//must check if empty before pop
	void pop(){
		head++;
		head = head % BUF_SIZE;
		count--;
	}

	T front(){
		return buffer[head];	//returns garbage if empty
	}

	const T& front() const {
		return buffer[head];	//returns garbage if empty
	}

	bool isFull(){
		return count == BUF_SIZE;
	}

	bool isEmpty(){
		return count == 0;
	}

	u64 size(){
		return count;
	}
};
