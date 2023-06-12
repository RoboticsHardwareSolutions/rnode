#include "test_node.h"
#include "../node_sem.h"
#include "../runit/src/runit.h"

int main()
{
    test_node_inter_proc();
    test_node_group_inproc3();
    test_node_group_inproc8();
    test_node_inproc();

    runit_report();
    return runit_at_least_one_fail;
}
