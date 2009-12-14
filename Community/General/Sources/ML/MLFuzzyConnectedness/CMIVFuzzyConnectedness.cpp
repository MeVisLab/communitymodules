/*=========================================================================
Author: Chunliang Wang (chunliang.wang@imv.liu.se)

Copyright (c) 2007-2009
Center for Medical Image Science and Visualization (CMIV),
Linköping University, Sweden, http://www.cmiv.liu.se/
ALL RIGHTS RESERVED

This image processing plugin from CMIV is free software;
you can redistribute it and/or modify it under the terms of the
GNU Lesser General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option)
any later version.

This image processing plugin is distributed in
the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
=========================================================================*/

#include "CMIVFuzzyConnectedness.h"
#include <string.h> // For memset()

CMIVFuzzyConnectedness::CMIVFuzzyConnectedness()
{
  imageWidth=0;
  imageHeight=0;
  imageAmount=0;
  imageSize=0;
  minmumValue=0.0;
  inputData=0L;
  outputData=0L;
  directionOfData=0L;
  colorOfData=0L;
}
void CMIVFuzzyConnectedness::setProperty(long width, long height, long amount,  float minval)
{

  imageWidth=width;
  imageHeight=height;
  imageAmount=amount;
  imageSize=width*height;
  minmumValue=minval;
    
}  

void CMIVFuzzyConnectedness::startShortestPathSearchAsFloat(float* pIn, float* pOut, unsigned char* pMarker, unsigned char* pPointers)
{

  long i,j,k;
  int changed;
  float maxvalue;
  unsigned char maxcolorindex;

  
  long itemp;
  long ilong,iwidth,iheight;
  long position_i1,position_i2,position_j1,position_j2,position_j3;
  
  if(!pMarker || !pIn || !pOut || !pPointers)
    return;

  
  ilong=imageWidth;
  iwidth=imageHeight;
  iheight=imageAmount;
  

  inputData=pIn;
  outputData=pOut;
  directionOfData=pPointers;
  
  unsigned char* marker=pMarker;
  long long* longmarker=(long long*)marker;

  //initialize
  long tsize=imageSize*imageAmount;
  
  for(i=0;i<tsize;i++)
  {
    if(*(marker+i))
    {
      if(*(marker+i)<0x40)
      {
        *(directionOfData+i)=*(marker+i)|0x80;
        *(outputData+i) = *(inputData+i);
      }
      else
      {
        *(directionOfData+i)=0x80;
        *(outputData+i) = minmumValue;
      }
    }
    else
    {
      *(directionOfData+i)=0x00;
      *(outputData+i) = minmumValue;
    }
    
  }

  
  runFirstRoundFasterWith26Neigbhorhood();
  
  //marker will be borrowed for recording which point has been changed
  memset(marker,0xff,imageSize*imageAmount/8+1);
    
  do
  {
    changed=0;
    
//**********************positive direction*****************************
    for(i=1;i<iheight-1;i++)
    {
      position_i1 = (i-1)*imageSize;
      position_i2 = i*imageSize;
      
      for(j=1;j<iwidth-1;j++)
      {
        position_j1 = (j-1)*ilong;
        position_j2 = j*ilong;
        position_j3 = (j+1)*ilong;
        
        for(k=1;k<ilong-1;k++)
        {
          itemp= position_i2+position_j2+k;
          if(!(*(longmarker+(itemp>>6))))
          {
            itemp=itemp>>6;
            do
            {
              itemp++;
            }while(!(*(longmarker+itemp)));
            itemp=itemp<<6;
            k=itemp-position_i2-position_j2;
            
            if(k>=ilong-1)
              continue;
            
          }

          if(!(*(marker+(itemp>>3))))
          {
            itemp=itemp>>3;
            do
            {
              itemp++;
            }while(!(*(marker+itemp)));
            itemp=itemp<<3;
            k=itemp-position_i2-position_j2;
            
            if(k>=ilong-1)
              continue;
  
          }
            
          if((*(marker+(itemp>>3)))&(0x01<<(itemp&0x07)))//if this point need to be check again
          {
            if(*(directionOfData + itemp)&0xc0)//if this is a seed point or saturated point
              *(marker+(itemp>>3)) = *(marker+(itemp>>3))&(~(0x01<<(itemp&0x07)));
            else
            {
    //1
              itemp=position_i1+position_j1+k-1;
              maxvalue=*(outputData+itemp);
              maxcolorindex=1;
    //2
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=2;
              }
    //3
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=3;
              }
    //4
              itemp=position_i1+position_j2+k-1;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=4;
              }
    //5
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=5;
              }

    //6
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=6;
              }
    //7          
              itemp=position_i1+position_j3+k-1;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=7;
              }
    //8  
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=8;
              }

    //9          
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=9;
              }
    //10  
              itemp=position_i2+position_j1+k-1;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=10;
              }
    //11
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=11;
              }
    //12
              itemp++;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=12;
              }
    //13
              itemp=position_i2+position_j2+k-1;
              if(*(outputData+itemp)>maxvalue)
              {
                maxvalue=*(outputData+itemp);
                maxcolorindex=13;
              }
    //update g
  
              itemp=position_i2+position_j2+k;
              if(maxvalue>*(outputData+itemp))
              {
                if(*(inputData+itemp)>*(outputData+itemp))
                {
                  //*(outputData+itemp)=min(maxvalue,*(inputData+itemp));
                  if(maxvalue>*(inputData+itemp))
                  {
                    *(outputData+itemp)=*(inputData+itemp);
                    *(directionOfData+itemp)=maxcolorindex|0x40;
                  }
                  else 
                  {
                    *(outputData+itemp)=maxvalue;
                    *(directionOfData+itemp)=maxcolorindex;
                  }
                  
                  int ii,jj,kk;
                  itemp=position_i1+position_j1+k-1;
                  for(ii=0;ii<3;ii++)
                  {
                    for(jj=0;jj<3;jj++)
                    {
                      for(kk=0;kk<3;kk++)
                      {
                        
                        *(marker+(itemp>>3))|=(0x01<<(itemp&0x07));
                        itemp++;
                      }
                      itemp=itemp-3+ilong;
                      
                    }
                    itemp=itemp-ilong-ilong-ilong+imageSize;
                  }
                  
                  changed++;        
                }
              
                else
                  *(marker+(itemp>>3))&=(~(0x01<<(itemp&0x07)));
                  
              }
              else 
                *(marker+(itemp>>3))&=(~(0x01<<(itemp&0x07)));

            }
          }
        }
      }
    }
        
//*******************************negitive direction*************************
    for(i=iheight-2;i>0;i--)
    {
      position_i1 = (i+1)*imageSize;
      position_i2 = i*imageSize;
      
      for(j=iwidth-2;j>0;j--)
      {
        position_j1 = (j-1)*ilong;
        position_j2 = j*ilong;
        position_j3 = (j+1)*ilong;
        
        for(k=ilong-2;k>0;k--)
        {  
          itemp= position_i2+position_j2+k;
          
          
          if(!(*(longmarker+(itemp>>6))))
          {
            itemp=itemp>>6;
            do
            {
              itemp--;
            }while(!(*(longmarker+itemp)));
            
            itemp=(itemp<<6)+63;
            k=itemp-position_i2-position_j2;
            
            if(k<1)
              continue;
            
          }
          
          if(!(*(marker+(itemp>>3))))
          {
            itemp=itemp>>3;
            do
            {
              itemp--;
            }while(!(*(marker+itemp)));
            itemp=(itemp<<3)+7;
            k=itemp-position_i2-position_j2;
            
            if(k<1)
              continue;
            
          }
          if((*(marker+(itemp>>3)))&(0x01<<(itemp&0x07)))//if this point need to be check again
          {
            if(*(directionOfData + itemp)&0xc0)//if this is a seed point or saturated point
              *(marker+(itemp>>3))=*(marker+(itemp>>3))&(~(0x01<<(itemp&0x07)));
            else
            {
        //1
                  itemp=position_i1+position_j3+k+1;
                  maxvalue=*(outputData+itemp);
                  maxcolorindex=27;
        //2
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=26;
                  }
        //3
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=25;
                  }
        //4
                  itemp=position_i1+position_j2+k+1;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=24;
                  }
        //5
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=23;
                  }

        //6
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=22;
                  }
        //7          
                  itemp=position_i1+position_j1+k+1;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=21;
                  }
        //8  
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=20;
                  }
        //9          
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=19;
                  }
        //10  
                  itemp=position_i2+position_j3+k+1;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=18;
                  }
        //11
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=17;
                  }
        //12
                  itemp--;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=16;
                  }
        //13
                  itemp=position_i2+position_j2+k+1;
                  if(*(outputData+itemp)>maxvalue)
                  {
                    maxvalue=*(outputData+itemp);
                    maxcolorindex=15;
                  }
        //update g
      
                  itemp=position_i2+position_j2+k;
                  if(maxvalue>*(outputData+itemp))
                  {
                    if(*(inputData+itemp)>*(outputData+itemp))
                    {
                                  //*(outputData+itemp)=min(maxvalue,*(inputData+itemp));
                      if(maxvalue>*(inputData+itemp))
                      {
                        *(outputData+itemp)=*(inputData+itemp);
                        *(directionOfData+itemp)=maxcolorindex|0x40;
                      }
                      else 
                      {
                        *(outputData+itemp)=maxvalue;
                        *(directionOfData+itemp)=maxcolorindex;
                      }
                      
                      int ii,jj,kk;
                      itemp=position_i2-imageSize+position_j1+k-1;
                      for(ii=0;ii<3;ii++)
                      {
                        for(jj=0;jj<3;jj++)
                        {
                          for(kk=0;kk<3;kk++)
                          {
                            
                            *(marker+(itemp>>3))|=(0x01<<(itemp&0x07));
                            itemp++;
                          }
                          itemp=itemp-3+ilong;
                          
                        }
                        itemp=itemp-ilong-ilong-ilong+imageSize;
                      }                            
                      
                      changed++;
                    }
                    else
                      *(marker+(itemp>>3))&=(~(0x01<<(itemp&0x07)));
              

                  }
                  else 
                    *(marker+(itemp>>3))&=(~(0x01<<(itemp&0x07)));

              }
          }
        }
      }
    }

  }while(changed);
  
  checkSaturatedPoints();
  memset(pMarker,0x00,imageSize*imageAmount);
  caculateColorMapFromPointerMap(pMarker, pPointers);
}
void CMIVFuzzyConnectedness::runFirstRoundFasterWith26Neigbhorhood()
{

  long i,j,k;
  float maxvalue;
  unsigned char maxcolorindex;
  long itemp;
  long ilong,iwidth,iheight;
  long position_i1,position_i2,position_j1,position_j2,position_j3;
  int countNum=0;
  
  
  ilong=imageWidth;
  iwidth=imageHeight;
  iheight=imageAmount;


  
  
  //**********************positive direction*****************************


  for(i=1;i<iheight-1;i++)
  {
    position_i1 = (i-1)*imageSize;
    position_i2 = i*imageSize;
    
    for(j=1;j<iwidth-1;j++)
    {
      position_j1 = (j-1)*ilong;
      position_j2 = j*ilong;
      position_j3 = (j+1)*ilong;
      for(k=1;k<ilong-1;k++)
        if(!(*(directionOfData + position_i2+position_j2+k)&0xc0))
        {
          countNum++;
          //1
          itemp=position_i1+position_j1+k-1;
          maxvalue=*(outputData+itemp);
          maxcolorindex=1;
          //2
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=2;
          }
          //3
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=3;
          }
          //4
          itemp=position_i1+position_j2+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=4;
          }
          //5
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=5;
          }
          
          //6
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=6;
          }
          //7          
          itemp=position_i1+position_j3+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=7;
          }
          //8  
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=8;
          }
          
          //9          
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=9;
          }
          //10 
          itemp=position_i2+position_j1+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=10;
          }
          //11
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=11;
          }
          //12
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=12;
          }
          //13
          itemp=position_i2+position_j2+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=13;
          }
          //update g
          itemp=position_i2+position_j2+k;
          if(maxvalue>*(outputData+itemp))
          {
            if(*(inputData+itemp)>*(outputData+itemp))
            {
              //*(outputData+itemp)=min(maxvalue,*(inputData+itemp));
              if(maxvalue>*(inputData+itemp))
              {
                *(outputData+itemp)=*(inputData+itemp);
                *(directionOfData+itemp)=maxcolorindex|0x40;
              }
              else 
              {
                *(outputData+itemp)=maxvalue;
                *(directionOfData+itemp)=maxcolorindex;
              }
              
                  
            }
  
          }
                    
        }
    }
  }
    countNum=0;
    
  //*******************************negitive direction*************************
  for(i=iheight-2;i>0;i--)
  {
    position_i1 = (i+1)*imageSize;
    position_i2 = i*imageSize;
    
    for(j=iwidth-2;j>0;j--)
    {
      position_j1 = (j-1)*ilong;
      position_j2 = j*ilong;
      position_j3 = (j+1)*ilong;
      
      for(k=ilong-2;k>0;k--)
        if(!(*(directionOfData + position_i2+position_j2+k)&0xc0))
        {
          countNum++;
          //1
          itemp=position_i1+position_j3+k+1;
          maxvalue=*(outputData+itemp);
          maxcolorindex=27;
          //2
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=26;
          }
          //3
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=25;
          }
          //4
          itemp=position_i1+position_j2+k+1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=24;
          }
          //5
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=23;
          }
          
          //6
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=22;
          }
          //7          
          itemp=position_i1+position_j1+k+1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=21;
          }
          //8  
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=20;
          }
          //9          
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=19;
          }
          //10 
          itemp=position_i2+position_j3+k+1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=18;
          }
          //11
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=17;
          }
          //12
          itemp--;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=16;
          }
          //13
          itemp=position_i2+position_j2+k+1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=15;
          }
          //update g
          
          itemp=position_i2+position_j2+k;
          if(maxvalue>*(outputData+itemp))
          {
            //*(outputData+itemp)=min(maxvalue,*(inputData+itemp));
            if(maxvalue>*(inputData+itemp))
            {
              *(outputData+itemp)=*(inputData+itemp);
              *(directionOfData+itemp)=maxcolorindex|0x40;
            }
            else 
            {
              *(outputData+itemp)=maxvalue;
              *(directionOfData+itemp)=maxcolorindex;
            }
            
            
          }

          
        }
    }
  }
    countNum++;
      

}
void CMIVFuzzyConnectedness::checkSaturatedPoints()
{
  long i,j,k;
  float maxvalue, oldmaxvalue;
  unsigned char maxcolorindex,oldcolorindex;
  long itemp;
  long ilong,iwidth,iheight;
  long position_i1,position_i2,position_i3,position_j1,position_j2,position_j3;
  ilong=imageWidth;
  iwidth=imageHeight;
  iheight=imageAmount;


  for(i=1;i<iheight-1;i++)
  {
    position_i1 = (i-1)*imageSize;
    position_i2 = i*imageSize;
    position_i3 = (i+1)*imageSize;
    for(j=1;j<iwidth-1;j++)
    {
      position_j1 = (j-1)*ilong;
      position_j2 = j*ilong;
      position_j3 = (j+1)*ilong;
      for(k=1;k<ilong-1;k++)
        if((!(*(directionOfData + position_i2+position_j2+k)&0x80))&&(*(directionOfData + position_i2+position_j2+k)&0x40))
        {
          oldcolorindex=*(directionOfData + position_i2+position_j2+k)&0x3f;
          
          //1
          itemp=position_i1+position_j1+k-1;
          maxvalue=*(outputData+itemp);
          maxcolorindex=1;
          oldmaxvalue=maxvalue;
          //2
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=2;
          }
          if(oldcolorindex==2)
            oldmaxvalue=*(outputData+itemp);
          //3
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=3;
          }
          if(oldcolorindex==3)
            oldmaxvalue=*(outputData+itemp);
          //4
          itemp=position_i1+position_j2+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=4;
          }
          if(oldcolorindex==4)
            oldmaxvalue=*(outputData+itemp);
          //5
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=5;
          }
          if(oldcolorindex==5)
            oldmaxvalue=*(outputData+itemp);
          //6
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=6;
          }
          if(oldcolorindex==6)
            oldmaxvalue=*(outputData+itemp);
          //7          
          itemp=position_i1+position_j3+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=7;
          }
          if(oldcolorindex==7)
            oldmaxvalue=*(outputData+itemp);
          //8  
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=8;
          }
          if(oldcolorindex==8)
            oldmaxvalue=*(outputData+itemp);
          //9          
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=9;
          }
          if(oldcolorindex==9)
            oldmaxvalue=*(outputData+itemp);
          //10 
          itemp=position_i2+position_j1+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=10;
          }
          if(oldcolorindex==10)
            oldmaxvalue=*(outputData+itemp);
          //11
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=11;
          }
          if(oldcolorindex==11)
            oldmaxvalue=*(outputData+itemp);
          //12
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=12;
          }
          if(oldcolorindex==12)
            oldmaxvalue=*(outputData+itemp);
          //13
          itemp=position_i2+position_j2+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=13;
          }
          if(oldcolorindex==13)
            oldmaxvalue=*(outputData+itemp);
          //15
          itemp+=2;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=15;
          }
          if(oldcolorindex==15)
            oldmaxvalue=*(outputData+itemp);
          //16
          itemp=position_i2+position_j3+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=16;
          }
          if(oldcolorindex==16)
            oldmaxvalue=*(outputData+itemp);
          //17
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=17;
          }
          if(oldcolorindex==17)
            oldmaxvalue=*(outputData+itemp);
          //18
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=18;
          }
          if(oldcolorindex==18)
            oldmaxvalue=*(outputData+itemp);
          //19
          itemp=position_i3+position_j1+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=19;
          }
          if(oldcolorindex==19)
            oldmaxvalue=*(outputData+itemp);
          //20
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=20;
          }
          if(oldcolorindex==20)
            oldmaxvalue=*(outputData+itemp);
          //21
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=21;
          }
          if(oldcolorindex==21)
            oldmaxvalue=*(outputData+itemp);
          //22
          itemp=position_i3+position_j2+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=22;
          }
          if(oldcolorindex==22)
            oldmaxvalue=*(outputData+itemp);
          //23
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=23;
          }
          if(oldcolorindex==23)
            oldmaxvalue=*(outputData+itemp);
          //24
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=24;
          }  
          if(oldcolorindex==24)
            oldmaxvalue=*(outputData+itemp);
          //25
          itemp=position_i3+position_j3+k-1;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=25;
          }
          if(oldcolorindex==25)
            oldmaxvalue=*(outputData+itemp);
          //26
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=26;
          }
          if(oldcolorindex==26)
            oldmaxvalue=*(outputData+itemp);
          //27
          itemp++;
          if(*(outputData+itemp)>maxvalue)
          {
            maxvalue=*(outputData+itemp);
            maxcolorindex=27;
          }  
          if(oldcolorindex==27)
            oldmaxvalue=*(outputData+itemp);
          //update direction
          itemp=position_i2+position_j2+k;
          if(maxvalue>oldmaxvalue)
            *(directionOfData+itemp)=maxcolorindex;
          else
            *(directionOfData+itemp)=oldcolorindex;
            
    
            
        }
    }
  }
    
  
}

void CMIVFuzzyConnectedness::caculateColorMapFromPointerMap(unsigned char* pColor, unsigned char* pPointers)
{
  colorOfData=pColor;
  directionOfData=pPointers;
  int totalvoxel=imageSize*(imageAmount-1);
  int i;
  for(i=imageSize;i<totalvoxel;i++)
  {
      
    if(*(colorOfData+i)==0)
    {
      if(!((*(directionOfData+i))&0x80))
      {
        int direction=*(directionOfData+i)&0x3f;
        if(direction)
        {
          int itemp=0;
          switch(direction)
          {
            case 1: itemp =  (-imageSize-imageWidth-1);
              break;
            case 2: itemp =  (-imageSize-imageWidth);
              break;
            case 3: itemp = (-imageSize-imageWidth+1);
              break;
            case 4: itemp = (-imageSize-1);
              break;
            case 5: itemp = (-imageSize);
              break;
            case 6: itemp = (-imageSize+1);
              break;
            case 7: itemp = (-imageSize+imageWidth-1);
              break;
            case 8: itemp = (-imageSize+imageWidth);
              break;
            case 9: itemp = (-imageSize+imageWidth+1);
              break;
            case 10: itemp = (-imageWidth-1);
              break;
            case 11: itemp = (-imageWidth);
              break;
            case 12: itemp = (-imageWidth+1);
              break;
            case 13: itemp = (-1);
              break;
            case 14: itemp = 0;
              break;
            case 15: itemp = 1;
              break;
            case 16: itemp = imageWidth-1;
              break;
            case 17: itemp = imageWidth;
              break;
            case 18: itemp = imageWidth+1;
              break;
            case 19: itemp = imageSize-imageWidth-1;
              break;
            case 20: itemp = imageSize-imageWidth;
              break;
            case 21: itemp = imageSize-imageWidth+1;
              break;
            case 22: itemp = imageSize-1;
              break;
            case 23: itemp = imageSize;
              break;
            case 24: itemp = imageSize+1;
              break;
            case 25: itemp = imageSize+imageWidth-1;
              break;
            case 26: itemp = imageSize+imageWidth;
              break;
            case 27: itemp = imageSize+imageWidth+1;
              break;
          }
          
          itemp+=i;
          *(colorOfData+i)= colorOfParent(itemp);
        }
      }
      else
        *(colorOfData+i)=(*(directionOfData+i))&0x3f;
    }
      
      
  }
        
}
unsigned char CMIVFuzzyConnectedness::colorOfParent(int pointer)
{
  if(*(colorOfData+pointer)==0)
  {
    if(!((*(directionOfData+pointer))&0x80))
    {

        int direction=*(directionOfData+pointer)&0x3f;
        if(direction)
        {
          int itemp=0;
          switch(direction)
          {
            case 1: itemp =  (-imageSize-imageWidth-1);
              break;
            case 2: itemp =  (-imageSize-imageWidth);
              break;
            case 3: itemp = (-imageSize-imageWidth+1);
              break;
            case 4: itemp = (-imageSize-1);
              break;
            case 5: itemp = (-imageSize);
              break;
            case 6: itemp = (-imageSize+1);
              break;
            case 7: itemp = (-imageSize+imageWidth-1);
              break;
            case 8: itemp = (-imageSize+imageWidth);
              break;
            case 9: itemp = (-imageSize+imageWidth+1);
              break;
            case 10: itemp = (-imageWidth-1);
              break;
            case 11: itemp = (-imageWidth);
              break;
            case 12: itemp = (-imageWidth+1);
              break;
            case 13: itemp = (-1);
              break;
            case 14: itemp = 0;
              break;
            case 15: itemp = 1;
              break;
            case 16: itemp = imageWidth-1;
              break;
            case 17: itemp = imageWidth;
              break;
            case 18: itemp = imageWidth+1;
              break;
            case 19: itemp = imageSize-imageWidth-1;
              break;
            case 20: itemp = imageSize-imageWidth;
              break;
            case 21: itemp = imageSize-imageWidth+1;
              break;
            case 22: itemp = imageSize-1;
              break;
            case 23: itemp = imageSize;
              break;
            case 24: itemp = imageSize+1;
              break;
            case 25: itemp = imageSize+imageWidth-1;
              break;
            case 26: itemp = imageSize+imageWidth;
              break;
            case 27: itemp = imageSize+imageWidth+1;
              break;
          }
          
          itemp += pointer;
          *(colorOfData+pointer) = colorOfParent(itemp);
        }
        else
          return 0;
      
    }
    else if(*(colorOfData+pointer)==0)
      *(colorOfData+pointer)=(*(directionOfData+pointer))&0x3f;
  }

  return(*(colorOfData+pointer));
  
}



