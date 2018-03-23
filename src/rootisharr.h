#ifndef ROOTISHARR_H
#define ROOTISHARR_H

#include<cstdlib>
#include<iostream>
#include<stdexcept>
#include<math.h>
using namespace std;
#include "mvector.h"

///////////////////////////////////////////////////////////////////////////////

template<class t>
class rootisharr{
private:
  mvector<t*> block;
  int siz;
  int maxsiz;
  int blocknum;

  void expand();
  void shrink();
public:
  rootisharr();
  rootisharr(int _siz);
  rootisharr(int _siz, t data);
  rootisharr(rootisharr<t> &other);
  ~rootisharr();

  int size();
  int capacity();
  bool empty();

  void push_back(t data);
  void push_front(t data);
  void insert(t data, int index) throw(out_of_range);

  t pop_back() throw(out_of_range);
  t pop_front() throw(out_of_range);
  t erase(int index) throw(out_of_range);
  void clear();

  t& operator[](int index) throw(out_of_range);
  void operator=(rootisharr<t> &other);

  void print(); // experimental
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<class t>
rootisharr<t>::rootisharr()
{
  siz=0;
  blocknum=1;
  maxsiz=1;
  t *tempptr=new t[1];
  block.push_back(tempptr);
  tempptr=NULL;
}

template<class t>
rootisharr<t>::rootisharr(int _siz)
{
  blocknum=1;
  maxsiz=1;
  t *tempptr=new t[1];
  block.push_back(tempptr);
  while(maxsiz<_siz)
  {
    blocknum++;
    tempptr=new t[blocknum];
    maxsiz+=blocknum;
    block.push_back(tempptr);
  }
  siz=_siz;
}

template<class t>
rootisharr<t>::rootisharr(int _siz, t data)
{
  siz=1;
  blocknum=1;
  maxsiz=1;
  t *tempptr=new t[1];
  tempptr[0]=data;
  block.push_back(tempptr);
  while(maxsiz<_siz)
  {
    blocknum++;
    tempptr=new t[blocknum];
    maxsiz+=blocknum;
    block.push_back(tempptr);

    for(int i=0;((i<blocknum)&&(siz<_siz));i++)
    {
      tempptr[i]=data;
      siz++;
    }
  }
}

template<class t>
rootisharr<t>::rootisharr(rootisharr<t> &other)
{
  siz=other.siz;
  maxsiz=other.maxsiz;
  blocknum=other.blocknum;
  t *tempptr;
  for(int i=0;i<blocknum;i++)
  {
    tempptr=new t[i+1];
    block.push_back(tempptr);
  }

  int index1;
  int index2;
  for(int i=0;i<siz;i++)
  {
    index1=ceil((-3.0+(sqrt(9+(8*i))))/2.0);
    index2=i-((index1*(index1+1))/2);
    block[index1][index2]=other.block[index1][index2];
  }
}

template<class t>
rootisharr<t>::~rootisharr()
{
  for(int i=0;i<blocknum;i++)
    delete[] block[i];

  block.clear();
}

///////////////////////////////////////////////////////////////////////////////

template<class t>
void rootisharr<t>::expand()
{
  blocknum++;
  maxsiz+=blocknum;
  t* tempptr=new t[blocknum];
  block.push_back(tempptr);
  tempptr=NULL;
}

template<class t>
void rootisharr<t>::shrink()
{
  maxsiz-=blocknum;
  blocknum--;
  delete[] block[blocknum];
  block.pop_back();
}

///////////////////////////////////////////////////////////////////////////////

template<class t>
int rootisharr<t>::size() { return siz; }

template<class t>
int rootisharr<t>::capacity() { return maxsiz; }

template<class t>
bool rootisharr<t>::empty() { return ((siz==0) ? true : false);}

///////////////////////////////////////////////////////////////////////////////

template<class t>
void rootisharr<t>::push_back(t data)
{
  if(siz==maxsiz)
    expand();

  int index1=ceil((-3.0+(sqrt(9+(8*siz))))/2.0);
  int index2=siz-((index1*(index1+1))/2);
  block[index1][index2]=data;
  siz++;
}

template<class t>
void rootisharr<t>::push_front(t data)
{
  if(siz==maxsiz)
    expand();

  int index1;
  int index2;
  int index3;
  int index4;
  for(int i=siz;i>0;i--)
  {
    index1=ceil((-3.0+(sqrt(9+(8*i))))/2.0);
    index2=i-((index1*(index1+1))/2);
    index3=ceil((-3.0+(sqrt(9+(8*(i-1)))))/2.0);
    index4=i-1-((index3*(index3+1))/2);
    block[index1][index2]=block[index3][index4];
  }

  block[0][0]=data;
  siz++;
}

template<class t>
void rootisharr<t>::insert(t data, int index) throw(out_of_range)
{
  if((index<0)||(index>siz))
    throw out_of_range("rootishArray out_of_range");

  if(siz==maxsiz)
    expand();

  int index1;
  int index2;
  int index3;
  int index4;
  for(int i=siz;i>index;i--)
  {
    index1=ceil((-3.0+(sqrt(9+(8*i))))/2.0);
    index2=i-((index1*(index1+1))/2);
    index3=ceil((-3.0+(sqrt(9+(8*(i-1)))))/2.0);
    index4=i-1-((index3*(index3+1))/2);
    block[index1][index2]=block[index3][index4];
  }

  index1=ceil((-3.0+(sqrt(9+(8*index))))/2.0);
  index2=index-((index1*(index1+1))/2);
  block[index1][index2]=data;
  siz++;
}

///////////////////////////////////////////////////////////////////////////////

template<class t>
t rootisharr<t>::pop_back() throw(out_of_range)
{
  if(siz==0)
    throw out_of_range("rootishArray out_of_range");

  siz--;
  int index1=ceil((-3.0+(sqrt(9+(8*siz))))/2.0);
  int index2=siz-((index1*(index1+1))/2);
  t temp=block[index1][index2];

  if((siz)==(maxsiz-blocknum))
    shrink();

  return temp;
}

template<class t>
t rootisharr<t>::pop_front() throw(out_of_range)
{
  if(siz==0)
    throw out_of_range("rootishArray out_of_range");

  t temp=block[0][0];
  int index1;
  int index2;
  int index3;
  int index4;
  for(int i=0;i<siz-1;i++)
  {
    index1=ceil((-3.0+(sqrt(9+(8*i))))/2.0);
    index2=i-((index1*(index1+1))/2);
    index3=ceil((-3.0+(sqrt(9+(8*(i+1)))))/2.0);
    index4=i+1-((index3*(index3+1))/2);
    block[index1][index2]=block[index3][index4];
  }

  if((siz-1)==(maxsiz-blocknum))
    shrink();

  siz--;
  return temp;
}

template<class t>
t rootisharr<t>::erase(int index) throw(out_of_range)
{
  if((index<0)||(index>=siz))
    throw out_of_range("rootishArray out_of_range");

  siz--;

  int index1=ceil((-3.0+(sqrt(9+(8*index))))/2.0);
  int index2=index-((index1*(index1+1))/2);
  int index3;
  int index4;

  t temp=block[index1][index2];
  for(int i=index;i<siz;i++)
  {
    index1=ceil((-3.0+(sqrt(9+(8*i))))/2.0);
    index2=i-((index1*(index1+1))/2);
    index3=ceil((-3.0+(sqrt(9+(8*(i+1)))))/2.0);
    index4=i+1-((index3*(index3+1))/2);
    block[index1][index2]=block[index3][index4];
  }

  if((siz-1)==(maxsiz-blocknum))
    shrink();

  return temp;
}

template<class t>
void rootisharr<t>::clear()
{
  for(int i=0;i<blocknum;i++)
    delete[] block[i];

  block.clear();
  siz=0;
  blocknum=1;
  maxsiz=1;
  t *tempptr=new t[1];
  block.push_back(tempptr);
  tempptr=NULL;
}

///////////////////////////////////////////////////////////////////////////////

template<class t>
t& rootisharr<t>::operator[](int index) throw(out_of_range)
{
  if((index<0)||(index>=siz))
    throw out_of_range("rootishArray out_of_range");

  int index1=ceil((-3.0+(sqrt(9+(8*index))))/2.0);
  int index2=index-((index1*(index1+1))/2);
  return block[index1][index2];
}

template<class t>
void rootisharr<t>::operator=(rootisharr<t> &other)
{
  clear();
  siz=other.siz;
  maxsiz=other.maxsiz;
  blocknum=other.blocknum;
  t *tempptr;
  for(int i=1;i<blocknum;i++)
  {
    tempptr=new t[i+1];
    block.push_back(tempptr);
  }

  int index1;
  int index2;
  for(int i=0;i<siz;i++)
  {
    index1=ceil((-3.0+(sqrt(9+(8*i))))/2.0);
    index2=i-((index1*(index1+1))/2);
    block[index1][index2]=other.block[index1][index2];
  }
}

template<class t>
void rootisharr<t>::print()
{
  int count=0;
  for(int i=0;i<blocknum;i++)
  {
    cout<<"block "<<i<<endl;
    for(int j=0;((j<i+1)&&(count<siz));j++)
    {
      cout<<"\t"<<block[i][j]<<endl;
      count++;
    }
  }
}

#endif // ROOTISHARR_H
