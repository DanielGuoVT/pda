//
// Created by xie on 18-5-23.
//

#ifndef PROTEUS_UTILS_H
#define PROTEUS_UTILS_H


#include <z3++.h>
#include <map>
#include <set>

#define UNHANDLE "Unsupport Instructions"
struct expr_cmp {
    bool operator()(const z3::expr& a, const z3::expr&  b) const {
        return a.hash()<b.hash();
    }
};
template<class  T>
using expr_map = std::map<z3::expr, T, expr_cmp>;
using expr_set = std::set<z3::expr,expr_cmp>;
extern z3::context z3context;
std::string intTostring(int num);
bool isSat(std::vector<z3::expr> exp);
bool isSat(z3::expr exp);
z3::expr substitute(z3::expr e, expr_map<z3::expr> &emap);


#endif //PROTEUS_UTILS_H
