// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef DIM_SERVICES_HH
#define DIM_SERVICES_HH

#include <string>
#include <dis.hxx>

class dim_int : public DimService
{
public:
   dim_int(std::string name);

   void operator=(int x);
   operator int();
 
protected:
   int value;

};



class dim_float : public DimService
{
public:
   dim_float(std::string name, float dead_band=0.01);

   void set_deadband(float db);

   void operator=(float x);
   operator float();

protected:
   float value;
   float deadband;
   
};


class dim_string : public DimService
{
public:
   dim_string(std::string name);

   void operator=(char *str);
   void operator=(std::string str);
   operator std::string();

protected:
   static const int BUF_SIZE=250;
   char buffer[BUF_SIZE];
   
};


#endif
