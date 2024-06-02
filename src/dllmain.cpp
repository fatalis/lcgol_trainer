#include "Trainer.h"

static std::shared_ptr<Trainer> s_trainer;

/* TODO:

use GetViewport instead of present params

*/
__attribute__((constructor))
void dllmain()
{
    s_trainer = std::make_shared<Trainer>();
    s_trainer->Initialize();
}
