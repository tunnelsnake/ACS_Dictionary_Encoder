#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>


int main(int argc, char ** argv) {
    std::vector<bool> s = {true, true, false, false, true, true, false, false, true, true, false};
    auto it = s.begin();
    auto addr = &(*it);

}