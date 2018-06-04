//
// Created by xie on 18-5-23.
//

#include <stdfix.h>
#include <vector>
#include "Utils.h"


z3::context z3context;
std::string intTostring(int num) {
    std::stringstream ss;
    ss << num;
    std::string str = ss.str();
    return str;
}
bool isSat(z3::expr exp) {
    z3::solver sol(z3context);
    sol.add(exp);
    if (sol.check() == z3::sat)
        return true;
    else
        return false;

}
bool isSat(std::vector<z3::expr> exp) {
    z3::solver sol(z3context);
    for(auto it = exp.begin(), ed = exp.end(); it != ed; it++)
        sol.add(*it);
    if (sol.check() == z3::sat)
        return true;
    else
        return false;

}

z3::expr substitute(z3::expr e, expr_map<z3::expr> &emap) {
    z3::expr_vector ev(z3context), ev_prime(z3context);

    for (auto it = emap.begin(), end = emap.end(); it != end; it++) {
        assert(it->first.get_sort()=it->second.get_sort());
        ev.push_back(it->first);
        ev_prime.push_back(it->second);
    }

    z3::expr re= e.substitute(ev, ev_prime);
    return re;
}