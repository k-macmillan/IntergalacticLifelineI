#ifndef GRUBBY_H
#define GRUBBY_H

#include "sc2api/sc2_api.h"
#include "sc2utils/sc2_manage_process.h"

class Grubby : public sc2::Agent 
{
public:

    Grubby();

    void OnGameStart() override;
    void OnStep() override;
};


#endif