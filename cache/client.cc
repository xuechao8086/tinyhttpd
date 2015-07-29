#include <iostream>
#include <string>
#include <libmemcached/memcached.h>

using namespace std;

int main(int argc, char *argv[])
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *server;
    time_t expiration; 
    uint32_t flags;
    
    memc = memcached_create(NULL);
    server = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, server);
    memcached_server_list_free(server);

    string key = "name";
    string value = "charliezhao";
    size_t value_length = value.length();
    size_t key_length = key.length();
    
    //save data
    rc = memcached_set(memc, key.c_str(), key.length(), value.c_str(), value.length(), expiration, flags);
    if (rc == MEMCACHED_SUCCESS)
    {
        cout<<"save data:"<<value<<" successful"<<endl;
    }else {
        cout<<"svae data fail, ret="<<rc<<endl;
    }


    //get data
    char *result = memcached_get(memc, key.c_str(), key_length, &value_length, &flags, &rc);
    if(rc == MEMCACHED_SUCCESS)
    {
        cout<<"get value:"<<result<<" successful"<<endl;
    } else {
        cout<<"get value fail, ret="<<result<<endl;
    }

    return 0;
}

