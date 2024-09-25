#ifndef _thread_donation_h
#define _thread_donation_h

#include "thread.h"

void donate_to_thread(struct thread * donor, struct thread * donee);
void remove_donation(struct thread * ex_donor, struct thread * ex_donee);

void
donate_to_thread(struct thread * donor, struct thread * donee)
{
    ASSERT(donor != NULL);
    ASSERT(donee != NULL);

    list_insert_ordered(&donee->donor_list, &donor->donors_elem, thread_priority_cmp, NULL);
    thread_priority_update(donee);
    return;
}

void
remove_donation(struct thread * ex_donor, struct thread * ex_donee)
{
    ASSERT(ex_donor != NULL);
    ASSERT(ex_donee != NULL);

    list_remove(&ex_donor->donors_elem);
    thread_priority_update(ex_donee);
    return;
}

#endif

