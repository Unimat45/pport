#pragma once
#include <iostream>
#include <initializer_list>

template <typename T>
class bArray
{
private:
	T* arr;
	size_t capacity;
	size_t current;

	void DoubleCapacity()
	{
		T* temp = new T[capacity * 2];
		for (size_t i = 0; i < capacity; i++)
		{
			temp[i] = arr[i];
		}

		delete[] arr;
		capacity *= 2;
		arr = temp;
	}

public:
	~bArray() { delete[] arr; }
	bArray() { arr = new T[1]; capacity = 1; current = 0; }
	bArray(std::initializer_list<T> list)
	{
		arr = new T[list.size()];
		for (size_t i = 0; i < list.size(); i++)
		{
			arr[i] = *(list.begin() + i);
		}
		capacity = list.size();
		current = list.size();
	}

	size_t size() { return current; }

	void push(T data)
	{
		if (current == capacity)
			DoubleCapacity();
		arr[current] = data;
		current++;
	}

	void push(T data, int index)
	{
		if (index == capacity)
			return push(data);
		if (index < 0)
			index = current - index;
		arr[index] = data;
	}

	T pop()
	{
		current--;
		return arr[current];
	}

	T operator[](int index)
	{
		if (index < 0)
			index = current - index;
		if (index < current)
			return arr[index];
		throw std::invalid_argument("Index Error: inavlid index");
	}

	std::string Join(char* sep)
	{
		std::string data = "";
		for (size_t i = 0; i < current; i++)
		{
			data += std::string(arr[i]) + (i != current - 1 ? sep : "");
		}
		return data;
	}

	std::string Join(const char* sep)
	{
		std::string data = "";
		
		for (size_t i = 0; i < current; i++)
		{
			data += std::string(arr[i]) + (i != current - 1 ? sep : "");
		}
		return data;
	}

	std::string Join(std::string sep)
	{
		std::string data = "";
		for (size_t i = 0; i < current; i++)
		{
			data += std::string(arr[i]) + (i != current - 1 ? sep : "");
		}
		return data;
	}

	bool Has(T arg) {
		for (int i = 0; i < current; i++) {
			if (this->arr[i] == arg) return true;
		}
		return false;
	}
};
