//
// Created by hal9000 on 6/21/24.
//

#ifndef STALKER_STLKR_THREADING_JOBARGUMENTS_H
#define STALKER_STLKR_THREADING_JOBARGUMENTS_H
template <typename threadJob>
struct DistributedJobArgs {
    threadJob job;
    unsigned startIndex;
    unsigned endIndex;
};
template <typename threadJob>
struct JobArgs {
    threadJob job;
    unsigned size;
};


#endif //STALKER_STLKR_THREADING_JOBARGUMENTS_H
