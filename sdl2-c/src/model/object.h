
#ifndef OBJECT_H
#define OBJECT_H

typedef struct{

    void* (*update)();
    void* (*render)();
    void* (*clean)();

}Object;



#endif