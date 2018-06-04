//
// Created by xie on 18-5-29.
//

#include "PDA2Dot.h"
#include "PDA.h"
struct Indent
{
    int ind;
    Indent(int ind = 1):ind(ind) {}
    friend std::ostream& operator <<(std::ostream& os, const Indent& ind);
};


std::ostream& operator <<(std::ostream& os, const Indent& ind)
{
    for (int i = 0; i < ind.ind; ++i)
        os << "\t";

    return os;
}
PDA2Dot::PDA2Dot(PDA *pda, const char *file) :pda(pda), file(file){
    reopen(file);
}
void PDA2Dot::reopen(const char *new_file)
{
    if (!new_file)
        new_file = "/dev/stdout";

    if (out.is_open())
        out.close();

    out.open(new_file);
    file = new_file;
}
bool PDA2Dot::ensureFile(const char *fl)
{
    if (fl)
        reopen(fl);

    if (!out.is_open()) {
        std::cerr << "File '" << file << "' not opened"
                  << std::endl;
        return false;
    }

    return true;
}
void PDA2Dot::start()
{
    out << "digraph \"PDA\" {\n";
    out << "\tcompound=true label=\"PDA "
        << " has " << pda->size() << " paths\\n\n"
            <<"\t Can Summ: white \\n\n"
            <<"\t Can not Summ: greenyellow \"\n\n";
}

void PDA2Dot::end()
{
    out << "}\n";
}
void PDA2Dot::dump_node(Location *node, int ind)
{


    Indent Ind(ind);

    out << Ind
        << "NODE" << node << " [label=\"";
    if(node->isIterPath())
        out<<"[*]\\n";
    else
        out<<"[1]\\n";
    out <<"PC: "<< node->getPathCondition()<<"\\n";
    expr_map <expr> valChange = node->getValChange();
    for(auto it = valChange.begin(), ed = valChange.end(); it != ed; it++)
        out<<it->first <<" := "<<it->second<<"\\n";




    // end of label
    out << "\" ";
    if(!node->canBeSumm())
        out << "style=filled fillcolor=greenyellow";

    else
        out << "style=filled fillcolor=white";

    out << "]\n";


}



void PDA2Dot::dump_nodes()
{
    out << "\t/* nodes */\n";
    vector<Location*> nodes = pda->getPaths();
    for (auto I = nodes.begin(), E = nodes.end(); I != E; ++I) {
        dump_node(*I);
    }

}
void PDA2Dot::dump_node_edges(Location *node1, Location* node2, int ind)
{
    Indent Ind(ind);

    out << Ind << "/* -- node " << "\n"
        << Ind << " * ------------------------------------------- */\n";


    out << Ind << "NODE" << node1 << " -> NODE" <<  node2
        << " [color=\"black\" rank=max]\n";

}



void PDA2Dot::dump_edges()
{

    map<Location*, Location*> trans = pda->getTransitions();
    for (auto I = trans.begin(), E = trans.end(); I != E; I++)
        dump_node_edges(I->first, I->second);
}


bool PDA2Dot::dump(const char *new_file)
{


    if (!ensureFile(new_file))
        return false;

    start();




    dump_nodes();
    dump_edges();



    end();

    out.close();
    return true;
}


bool PDA2Dot::open(const char *new_file)
{
    if (out.is_open()) {
        std::cerr << "File already opened (" << file << ")"
                  << std::endl;
        return false;
    } else
        reopen(new_file);
}