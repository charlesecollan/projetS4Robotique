#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <iostream>
#include <cstring>
#include <string>


#include <windows.h>


#include "fonctions.h"

#define sign(x) ((x) > 0 ? 1 : -1)

using namespace std;

void analyseDifferentierFrames( IplImage *imga, IplImage *imgb, IplImage *masque, int tol )
{
  // binarisation de l'image en fonction du mouvement
  // ce base sur la difference des images imga et imgb

  CvScalar scalA, scalB, scalM;
  IplConvKernel *kernel;
  for(int x=0; x < imga->width; x++)
    {
      for(int y=0; y < imga->height; y++)
	{
	  scalA = cvGet2D(imga, y, x);
	  scalB = cvGet2D(imgb, y, x);
	  scalM.val[0] = abs(scalA.val[0]-scalB.val[0])+abs(scalA.val[1]-scalB.val[1])+abs(scalA.val[2]-scalB.val[2]);
	  if( scalM.val[0] <=  tol )//la tolerance
	    scalM.val[0] = 0;
	  else
	    scalM.val[0] = 255;
	  cvSet2D(masque, y, x, scalM);
	}
    }

  // netoyage des parasites eventuels restant
  kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);//operateur morphologique
  cvDilate(masque, masque, kernel, 1);
  cvErode(masque, masque, kernel, 1);

  cvReleaseStructuringElement(&kernel);
}

