#ifndef POTATO_H
#define POTATO_H

#include "global.h"

class Potato
{
    private:
        enum POTATO_STATE { ATTACHED_TO_PLAYER, THROWN, ATTACHED_TO_TARGET };

        POTATO_STATE state;
        XMFLOAT3 dir;
        // Target myTarget;
        Billboard *myTarget;

    public:
        XMFLOAT3 pos;
        int owner = 0;

        Potato()
        {
            state = ATTACHED_TO_PLAYER;
            pos = XMFLOAT3(0, 0, 0);
            dir = XMFLOAT3(0, 0, 0);
        }

        string getState() 
        {
            if (state == ATTACHED_TO_TARGET) 
            {
                return "YOU WIN! :)";
            }
            return "YOU LOSE! :(";
        }
        void Throw(camera* cam)
        {
            if (state == ATTACHED_TO_PLAYER)
            {
                state = THROWN;
                pos = -cam->position;
                dir = XMFLOAT3(
                    cos(cam->rotation.x)*sin(-cam->rotation.y),
                    sin(cam->rotation.x),
                    cos(cam->rotation.x)*cos(-cam->rotation.y)
                );
            }
        }

        static bool Collided(Potato* p, Billboard* t)
        {
            XMFLOAT3 d = p->pos - t->pos; /*distance between potato and target*/
            float tolerance = 2;
            if (Length(d) <= tolerance) {
                return true; // If true then a hit
            }
            return false;
        }

        static bool Collided(Potato* p, bitmap* leveldata)
        {
            /* Collision detection similar to camera and bitmap */
            if (p->state == THROWN)
            {
                float xoffset = 50.5;
                float zoffset = -2.5;
                float x = p->pos.x / 2;
                float z = p->pos.z / 2;

                BYTE blue = leveldata->get_pixel(x + xoffset, z + zoffset, 0);

                if (blue > 0) {
                    return true;
                }
            }
            return false;
        }

        void Update(long elapsed, bitmap* leveldata, Billboard *targetArr[], int targetArrSize) //<-- targetArr could also be a vector
        {
            if (state == THROWN)
            {
                float time = elapsed / 100000.0;

                float speed = 0.3; //Was at 1

                dir.y -= 0.004; // Makes it go downwards

                pos.x += speed * time *dir.x;
                pos.y += speed *time *dir.y;
                pos.z += speed *time*dir.z;

                if (pos.y < -0.5) {
                    dir.y *= -0.9;
                }

                int i = 0;
                for (; i < targetArrSize && !Collided(this, targetArr[i]); ++i);

                if (i < targetArrSize) //<-- not at end of array = hit a target
                {
                    state = ATTACHED_TO_TARGET;
                    myTarget = targetArr[i];
                    owner = i;
                }
                else if (Collided(this, leveldata) == true) //<-- didn't hit a target
                {
                    //hit a wall
                    state = ATTACHED_TO_PLAYER;
                }
            }
        }

        XMMATRIX getWorldMatrix(camera* cam)
        {
            switch (state)
            {
            case ATTACHED_TO_PLAYER:
                /*if visible to player (player can see potato when holding it), create desired matrix, otherwise, return a zero matrix*/
                return XMMATRIX(
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0);
            case THROWN:
                return XMMatrixTranslation(pos.x, pos.y, pos.z); /*include scale if necessary. include -cam.rotation.y and -cam.rotation.x if billboard*/
            case ATTACHED_TO_TARGET:
                return XMMatrixTranslation(myTarget->pos.x, myTarget->pos.y, myTarget->pos.z); /*again, include scale if necessary. include -rotations if billboard*/
                                                                                               /*also, could use offsets to put potato relative to target (not have potato on center of target), but much harder. */
                                                                                               /*Better if that is done during office hours.*/
            }
            return XMMatrixIdentity();
        }
};

#endif