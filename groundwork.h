#ifndef GROUNDWORK_H
#define GROUNDWORK_H

#include <algorithm>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "resource.h"
#include "vector.h"
#include "vertex.h"

using namespace std;

struct ConstantBuffer {
  XMMATRIX world;
  XMMATRIX view;
  XMMATRIX projection;
  XMFLOAT4 info;
};

//*****************************************
class bitmap {

  public:
    BYTE *image;
    int array_size;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;

    bitmap() {
      image = NULL;
    }

    ~bitmap() {
      if(image) delete[] image;
      array_size = 0;
    }

    bool read_image(char *filename) {
      ifstream bmpfile(filename, ios::in | ios::binary);
      if (!bmpfile.is_open()) return FALSE;	// Error opening file
      bmpfile.read((char*)&bmfh, sizeof(BITMAPFILEHEADER));
      bmpfile.read((char*)&bmih, sizeof(BITMAPINFOHEADER));
      bmpfile.seekg(bmfh.bfOffBits, ios::beg);
      //make the array
      if (image)delete[] image;
      int size = bmih.biWidth*bmih.biHeight * 3;
      image = new BYTE[size];//3 because red, green and blue, each one byte
      bmpfile.read((char*)image,size);
      array_size = size;
      bmpfile.close();
      check_save();
      return TRUE;
    }

    BYTE get_pixel(int x, int y,int color_offset) { //color_offset = 0,1 or 2 for red, green and blue
      int array_position = x*3 + y* bmih.biWidth*3+ color_offset;
      if (array_position >= array_size) return 0;
      if (array_position < 0) return 0;
      return image[array_position];
    }

    void check_save() {
      ofstream nbmpfile("newpic.bmp", ios::out | ios::binary);
      if (!nbmpfile.is_open()) return;
      nbmpfile.write((char*)&bmfh, sizeof(BITMAPFILEHEADER));
      nbmpfile.write((char*)&bmih, sizeof(BITMAPINFOHEADER));
      //offset:
      int rest = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
      if (rest > 0) {
        BYTE *r = new BYTE[rest];
        memset(r, 0, rest);
        nbmpfile.write((char*)&r, rest);
      }

      nbmpfile.write((char*)image, array_size);
      nbmpfile.close();
    }
};
//**********************************
XMFLOAT3 mul(XMFLOAT3 a, XMMATRIX &m) {
    XMVECTOR f = XMLoadFloat3(&a);
    f = XMVector3TransformCoord(f, m);
    XMStoreFloat3(&a, f);


    return a;
}
//****************************************************88
class billboard
{
public:
    billboard()
    {
        position = XMFLOAT3(0, 0, 10);
        scale = 1;
        transparency = 1;
    }
    XMFLOAT3 position; //obvious
    float scale;		//in case it can grow
    float transparency; //for later use
    XMMATRIX get_matrix(XMMATRIX &ViewMatrix)
    {

        XMMATRIX view, R, T, S;
        view = ViewMatrix;
        //eliminate camera translation:
        view._41 = view._42 = view._43 = 0.0;
        XMVECTOR det;
        R = XMMatrixInverse(&det, view);//inverse rotation
        T = XMMatrixTranslation(position.x, position.y, position.z);
        S = XMMatrixScaling(scale, scale, scale);
        return S*R*T;
    }
    
    float distanceZ(XMMATRIX &ViewMatrix) {

        XMMATRIX view = ViewMatrix;
        XMFLOAT3 res = mul(position, view); // Multiplication of position float3 and view matrix

        float cam_dist = res.z * position.z;
    }



    XMMATRIX get_matrix_y(XMMATRIX &ViewMatrix) //enemy-type
    {

    }
};

//*****************************************************************************
//lets assume a wall is 10/10 big!
#define FULLWALL 2
#define HALFWALL 1
class wall
  {
  public:
    XMFLOAT3 position;
      int texture_no;
      int rotation; //0,1,2,3,4,5 ... facing to z, x, -z, -x, y, -y
      wall()
        {
        texture_no = 0;
        rotation = 0;
        position = XMFLOAT3(0,0,0);
        }
      XMMATRIX get_matrix()
        {
        XMMATRIX R, T, T_offset;				
        R = XMMatrixIdentity();
        T_offset = XMMatrixTranslation(0, 0, -HALFWALL);
        T = XMMatrixTranslation(position.x, position.y, position.z);
        switch (rotation)//0,1,2,3,4,5 ... facing to z, x, -z, -x, y, -y
          {
            default:
            case 0:	R = XMMatrixRotationY(XM_PI);		T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
            case 1: R = XMMatrixRotationY(XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
            case 2:										T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
            case 3: R = XMMatrixRotationY(-XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
            case 4: R = XMMatrixRotationX(XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, -HALFWALL); break;
            case 5: R = XMMatrixRotationX(-XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, -HALFWALL); break;
          }
        return T_offset * R * T;
        }
  };
//********************************************************************************************
class Billboard {
  const double IMPULSE_CONTROL = 100000.0;

  public:
    float scale;        //in case it can grow
    float transparency; //for later use

    XMFLOAT3 imp;
    XMFLOAT3 pos; //obvious

    Billboard() {
      imp = XMFLOAT3(0, 0, 0);
      pos = XMFLOAT3(0, 0, 5);
      scale = 1;
      transparency = 1;
    }

    XMMATRIX GetMatrix(float elapsed, XMMATRIX &view) {
      pos.x += imp.x * elapsed / IMPULSE_CONTROL;
      pos.y += imp.y * elapsed / IMPULSE_CONTROL;
      pos.z += imp.z * elapsed / IMPULSE_CONTROL;

      XMMATRIX R = view, T, S;
      R._41 = R._42 = R._43 = 0.0; //eliminate camera translation:
      XMVECTOR det;
      R = XMMatrixInverse(&det, R);//inverse rotation
      T = XMMatrixTranslation(pos.x, pos.y, pos.z);
      S = XMMatrixScaling(scale, scale, scale);
      return S * R * T;
    }
    float distanceZ(XMMATRIX &ViewMatrix) {

        XMMATRIX view = ViewMatrix;
        XMFLOAT3 res = mul(pos, view); // Multiplication of position float3 and view matrix

        float cam_dist = res.z * pos.z;

        return cam_dist;
    }

};

class level
  {
  private:
    bitmap leveldata;
    vector<wall*> walls;						//all wall positions
    vector<ID3D11ShaderResourceView*> textures;	//all wall textures
    void process_level()
      {
      //we have to get the level to the middle:
      int x_offset = (leveldata.bmih.biWidth/2)*FULLWALL;

      //lets go over each pixel without the borders!, only the inner ones
      for (int yy = 1; yy < (leveldata.bmih.biHeight - 1);yy++)
        for (int xx = 1; xx < (leveldata.bmih.biWidth - 1); xx++)
          {
          //wall information is the interface between pixels:
          //blue to something not blue: wall. texture number = 255 - blue
          //green only: floor. texture number = 255 - green
          //red only: ceiling. texture number = 255 - red
          //green and red: floor and ceiling ............
          BYTE red, green, blue;

          blue = leveldata.get_pixel(xx, yy, 0);
          green = leveldata.get_pixel(xx, yy, 1);
          red = leveldata.get_pixel(xx, yy, 2);
          
          if (blue > 0)//wall possible
            {
            int texno = 255 - blue;
            BYTE left_red = leveldata.get_pixel(xx - 1, yy, 2);
            BYTE left_green = leveldata.get_pixel(xx - 1, yy, 1);
            BYTE right_red = leveldata.get_pixel(xx + 1, yy, 2);
            BYTE right_green = leveldata.get_pixel(xx + 1, yy, 1);
            BYTE top_red = leveldata.get_pixel(xx, yy+1, 2);
            BYTE top_green = leveldata.get_pixel(xx, yy+1, 1);
            BYTE bottom_red = leveldata.get_pixel(xx, yy-1, 2);
            BYTE bottom_green = leveldata.get_pixel(xx, yy-1, 1);

            if (left_red>0 || left_green > 0)//to the left
              init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 3, texno);
            if (right_red>0 || right_green > 0)//to the right
              init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 1, texno);
            if (top_red>0 || top_green > 0)//to the top
              init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 2, texno);
            if (bottom_red>0 || bottom_green > 0)//to the bottom
              init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 0, texno);
            }
          if (red > 0)//ceiling
            {
            int texno = 255 - red;
            init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0,yy*FULLWALL), 5, texno);
            }
          if (green > 0)//floor
            {
            int texno = 255 - green;
            init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0,yy*FULLWALL), 4, texno);
            }
          }
      }
    void init_wall(XMFLOAT3 pos, int rotation, int texture_no)
      {
      wall *w = new wall;
      walls.push_back(w);
      w->position = pos;
      w->rotation = rotation;
      w->texture_no = texture_no;
      }
  public:
    level()
      {
      }
    bitmap *get_bitmap()//get method *NEW*
    {
        return &leveldata;
    }
    void init(char *level_bitmap)
      {
      if(!leveldata.read_image(level_bitmap))return;
      process_level();
      }
    bool init_texture(ID3D11Device* pd3dDevice,LPCWSTR filename)
      {
      // Load the Texture
      ID3D11ShaderResourceView *texture;
      HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, filename, NULL, NULL, &texture, NULL);
      if (FAILED(hr))
        return FALSE;
      textures.push_back(texture);
      return TRUE;
      }
    ID3D11ShaderResourceView *get_texture(int no)
      {
      if (no < 0 || no >= textures.size()) return NULL;
      return textures[no];
      }
    XMMATRIX get_wall_matrix(int no)
      {
      if (no < 0 || no >= walls.size()) return XMMatrixIdentity();
      return walls[no]->get_matrix();
      }
    int get_wall_count()
      {
      return walls.size();
      }
    void render_level(ID3D11DeviceContext* ImmediateContext,ID3D11Buffer *vertexbuffer_wall,XMMATRIX *view, XMMATRIX *projection, ID3D11Buffer* dx_cbuffer)
      {
      //set up everything for the waqlls/floors/ceilings:
      UINT stride = sizeof(SimpleVertex);
      UINT offset = 0;			
      ImmediateContext->IASetVertexBuffers(0, 1, &vertexbuffer_wall, &stride, &offset);
      ConstantBuffer constantbuffer;			
      constantbuffer.view = XMMatrixTranspose(*view);
      constantbuffer.projection = XMMatrixTranspose(*projection);			
      XMMATRIX wall_matrix,S;
      ID3D11ShaderResourceView* tex;
      //S = XMMatrixScaling(FULLWALL, FULLWALL, FULLWALL);
      S = XMMatrixScaling(1, 1, 1);
      for (int ii = 0; ii < walls.size(); ii++)
        {
        wall_matrix = walls[ii]->get_matrix();
        int texno = walls[ii]->texture_no;
        if (texno >= textures.size())
          texno = 0;
        tex = textures[texno];
        wall_matrix = wall_matrix;// *S;

        constantbuffer.world = XMMatrixTranspose(wall_matrix);
        
        ImmediateContext->UpdateSubresource(dx_cbuffer, 0, NULL, &constantbuffer, 0, 0);
        ImmediateContext->VSSetConstantBuffers(0, 1, &dx_cbuffer);
        ImmediateContext->PSSetConstantBuffers(0, 1, &dx_cbuffer);
        ImmediateContext->PSSetShaderResources(0, 1, &tex);
        ImmediateContext->Draw(6, 0);
        }
      }
  };
  class StopWatchMicro_
  {
  private:
    LARGE_INTEGER last, frequency;
  public:
    StopWatchMicro_()
    {
      QueryPerformanceFrequency(&frequency);
      QueryPerformanceCounter(&last);

    }
    long double elapse_micro()
    {
      LARGE_INTEGER now, dif;
      QueryPerformanceCounter(&now);
      dif.QuadPart = now.QuadPart - last.QuadPart;
      long double fdiff = (long double)dif.QuadPart;
      fdiff /= (long double)frequency.QuadPart;
      return fdiff*1000000.;
    }
    long double elapse_milli()
    {
      elapse_micro() / 1000.;
    }
    void start()
    {
      QueryPerformanceCounter(&last);
    }
  };


  class camera
    {
    private:

    public:
      int w, s, a, d;
      XMFLOAT3 position;
      XMFLOAT3 rotation;
      camera()
        {
        w = s = a = d = 0;
        position = position = XMFLOAT3(0, 0, -10);
        }
      void animation(bitmap *leveldata)
        {
        XMMATRIX R, T;
        R = XMMatrixRotationY(-rotation.y);

        XMFLOAT3 forward = XMFLOAT3(0, 0, 1);
        XMVECTOR f = XMLoadFloat3(&forward);
        f = XMVector3TransformCoord(f, R);
        XMStoreFloat3(&forward, f);
        XMFLOAT3 side = XMFLOAT3(1, 0, 0);
        XMVECTOR si = XMLoadFloat3(&side);
        si = XMVector3TransformCoord(si, R);
        XMStoreFloat3(&side, si);

        XMFLOAT3 temp;
        temp = XMFLOAT3(position.x, position.y, position.z);

        if (w)
        {
            temp.x -= forward.x * 0.1;
            temp.y -= forward.y * 0.1;
            temp.z -= forward.z * 0.1;
        }
        if (s)
        {
            temp.x += forward.x * 0.1;
            temp.y += forward.y * 0.1;
            temp.z += forward.z * 0.1;
        }
        if (d)
        {
            rotation.y -= 0.05;
        }
        if (a)
        {
            rotation.y += 0.05;
        }

        float xoffset = 50.5;
        float zoffset = 0.5;
        //XMFLOAT3 bitPos;
        //bitPos = XMFLOAT3(temp.x, temp.y, temp.z);


        float x = -temp.x / 2;
        float z = -temp.z / 2;

        BYTE blue = leveldata->get_pixel(x + xoffset, z + zoffset, 0);

        if (blue != 0) {
            position.x = position.x;
            position.y = position.y;
            position.z = position.z;

        }
        else {
            position.x = temp.x;
            position.y = temp.y;
            position.z = temp.z;
        }
        }
      XMMATRIX get_matrix(XMMATRIX *view)
        {
        XMMATRIX R, T;
        R = XMMatrixRotationY(rotation.y);
        T = XMMatrixTranslation(position.x, position.y, position.z);
        return T*(*view)*R;
        }
    };

  float Length(XMFLOAT3 in)
  {
      return sqrt(in.x*in.x + in.y*in.y + in.z*in.z);
  }

  //********************************************************************************
  class Target {
  private:
      XMFLOAT3 pos;   //Target should be made into an array or a matrix of area
  public:
      Target() {
          pos = XMFLOAT3(0, 0, 0);
      }
      XMFLOAT3 getPos() {
          return pos;
      }
      void setPos(int x, int y, int z) {
          pos.x = x;
          pos.y = y;
          pos.z = z;
      }
  };
  class Potato
  {
  private:
      enum POTATO_STATE { ATTACHED_TO_PLAYER, THROWN, ATTACHED_TO_TARGET };

      POTATO_STATE state;
      XMFLOAT3 pos;
      XMFLOAT3 dir;
      Target myTarget;

  public:
      Potato()
      {
          state = ATTACHED_TO_PLAYER;
          pos = XMFLOAT3(0, 0, 0);
          dir = XMFLOAT3(0, 0, 0);
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
      static bool Collided(Potato* p, Target t)
      {
          XMFLOAT3 d = p->pos - t.getPos(); /*distance between potato and target*/
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

      void Update(long elapsed, bitmap* leveldata, Target* targetArr, int targetArrSize) //<-- targetArr could also be a vector
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
              }
              else if (Collided(this, leveldata) == true) //<-- didn't hit a target
              {
                  //hit a wall
                  state = ATTACHED_TO_PLAYER;
              }
          }
      }

      XMMATRIX getWorldMatrix(camera* cam, Target* target)
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
              return XMMatrixTranslation(target->getPos().x, target->getPos().y, target->getPos().z); /*again, include scale if necessary. include -rotations if billboard*/
                                                                                                      /*also, could use offsets to put potato relative to target (not have potato on center of target), but much harder. */
                                                                                                      /*Better if that is done during office hours.*/
          }
          return XMMatrixIdentity();
      }
  };

#endif