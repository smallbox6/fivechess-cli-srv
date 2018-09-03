#include "packetsprocess.h"

#include <iostream>
#include <pthread.h>

extern ServerPack g_serverpack;
extern PacketType g_packtype;

extern pthread_t g_twoplayId;
extern pthread_cond_t  cond;
extern pthread_mutex_t mute;

void* PlayAgreeThread(void *p)
{
    TwoConnect twoconn;
    std::cout << "Play_agree_thread runing!\n\n";
    for(;;)
    {
        pthread_mutex_lock(&mute);
        while(g_serverpack.twoconnqueue.size() == 0)
            pthread_cond_wait(&cond, &mute);
        twoconn = g_serverpack.twoconnqueue.front();
        g_serverpack.twoconnqueue.pop();
        TwoFight(twoconn);
        pthread_mutex_unlock(&mute);
    }
}