//
// Created by xie on 18-5-29.
//

#ifndef PROTEUS_PDA2DOT_H
#define PROTEUS_PDA2DOT_H
#include <iostream>
#include <fstream>

class Location;
class PDA;


class PDA2Dot {
public:
    PDA2Dot(PDA* pda, const char* file = NULL);
    void reopen(const char *new_file);
    bool ensureFile(const char *fl);
    void start();

    void end();
    void dump_node(Location *node, int ind = 1);


    void dump_nodes();
    void dump_node_edges(Location *node1, Location* node2, int ind = 1);


    void dump_edges();


    virtual bool dump(const char *new_file = nullptr);


    bool open(const char *new_file);
    const char *file;
    PDA* pda;

protected:
    std::ofstream out;



};


#endif //PROTEUS_PDA2DOT_H
