#ifndef __COMMONO_H__
#define __COMMONO_H__

#pragma once

#define PI 3.141592

template <typename T>
int Has(vector<T*> &arr, T *element)
{
	for (auto v : arr) {
		if (v == element) {
			return 1;
		}
	}
	return 0;
}

template <typename T>
void Add(vector<T*> &arr, T *element)
{
	if (!Has(arr, element)) {
		arr.push_back(element);
	}
}

#endif