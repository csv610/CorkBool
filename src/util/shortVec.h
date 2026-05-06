// +-------------------------------------------------------------------------
// | shortVec.h
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
// | COPYRIGHT:
// |    Copyright Gilbert Bernstein 2013
// |    See the included COPYRIGHT file for further details.
// |    
// |    This file is part of the Cork library.
//
// |    Cork is free software: you can redistribute it and/or modify
// |    it under the terms of the GNU Lesser General Public License as
// |    published by the Free Software Foundation, either version 3 of
// |    the License, or (at your option) any later version.
//
// |    Cork is distributed in the hope that it will be useful,
// |    but WITHOUT ANY WARRANTY; without even the implied warranty of
// |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// |    GNU Lesser General Public License for more details.
//
// |    You should have received a copy 
// |    of the GNU Lesser General Public License
// |    along with Cork.  If not, see <http://www.gnu.org/licenses/>.
// +-------------------------------------------------------------------------
#pragma once

#include "prelude.h"
#include <vector>

template<class T, uint LEN>
class ShortVec : public std::vector<T> {
public:
    explicit ShortVec() : std::vector<T>() {}
    explicit ShortVec(uint size) : std::vector<T>(size) {}
    ShortVec(uint size, const T& fill_val) : std::vector<T>(size, fill_val) {}
    ShortVec(const ShortVec& cp) : std::vector<T>(cp) {}
    ShortVec(ShortVec&& cp) noexcept : std::vector<T>(std::move(cp)) {}

    using std::vector<T>::operator[];
    using std::vector<T>::at;
    using std::vector<T>::size;
    using std::vector<T>::resize;
    using std::vector<T>::push_back;
    using std::vector<T>::pop_back;
    using std::vector<T>::begin;
    using std::vector<T>::end;
    using std::vector<T>::empty;
    using std::vector<T>::clear;
    using std::vector<T>::front;
    using std::vector<T>::back;
    using std::vector<T>::data;

    void erase(const T& val) {
        for (uint i = 0; i < size(); i++) {
            if ((*this)[i] == val) {
                std::swap((*this)[i], (*this)[size() - 1]);
                pop_back();
                break;
            }
        }
    }
};