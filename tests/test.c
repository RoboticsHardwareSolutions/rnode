#include "test_node.h"
#include "runit.h"

int main()
{
    test_node_inter_proc();
    test_node_group_inproc3();
    test_node_group_inproc8();
    test_node_inproc();
    test_node_slot_exhaustion();
    test_node_url_too_long();

    runit_report();
    return runit_at_least_one_fail;
}
