#pragma once
#ifndef COMBINATION_HPP
#define COMBINATION_HPP

#include <iostream>
#include <iomanip>

class Combination {
public:
    Combination(int size) {
        this->size = size;
        this->data = new int[size];
    }

    ~Combination(){
        delete[] data;
    }

    void reset(int init){
        this->init = init;
        for (int i = 0; i < size - 1; i++) {
            data[i] = 0;
        }
        data[size - 1] = init;
    }

    bool next() {
        if (data[0] == init) {
            return false;
        }
        int last = size - 1;
        if (data[last] != 0) {
            data[last]--;
            data[last - 1]++;
            return true;
        }
        for (int i = last - 1; i >= 1; i--) {
            if (data[i] != 0) {
                data[last] = data[i] - 1;
                data[i] = 0;
                data[i - 1]++;
                return true;
            }
        }
        return false;
    }

    // no range check
    int operator[](int p) const {
        return data[p];
    }

    void print(std::ostream& os) {
        os << "Combination:";
        for (int i = 0; i < size; i++) {
            os << std::dec << data[i] << ",";
        }
        os << std::endl;
    }

private:
    int size;
    int init;
    int * data;
};


#endif // COMBINATION_HPP
