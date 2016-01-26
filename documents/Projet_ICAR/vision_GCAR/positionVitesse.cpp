#include <opencv/highgui.h>
#include <opencv/cv.h>

#include <iostream>
#include <cstring>
#include <string>


#include <windows.h>


#include "fonctions.h"

#define sign(x) ((x) > 0 ? 1 : -1)

using namespace std;

CvPoint objetPos( IplImage *masque )
{

  int nbPixels = 0;

  // calcul du barycentre de l'objet
  int x, y;
  int sommeX = 0, sommeY = 0;
  CvScalar scal;

  for(x = 0; x < masque->width; x++)
    {
      for(y = 0; y < masque->height; y++)
	{
	  scal = cvGet2D( masque, y, x);
	  if( scal.val[0] == 255 ) {
	    sommeX += x;
	    sommeY += y;
	    (nbPixels)++;
	  }
	}
    }

  // il faudrait lisser le mouvement
  // il faudrait pouvoir differencier plusieurs objets

  // coordonnee du barycentre de l'objet
  if( nbPixels > 20 )
    return cvPoint((int)(sommeX / (nbPixels)), (int)(sommeY / (nbPixels)));
  else
    return cvPoint(-1, -1);
}


long long getTimeMillis()// Retourne l'heure actuel en secondes
{
  // ---------------- Sous Windows ----------------

  // La fonction GetTickCount() de windows.h nous
  // donne directement un timestamp en millisecondes
  long long l= GetTickCount();
  return l;
}


double vitesse(CvPoint posa, CvPoint posb, double dt )// Calcul de la vitesse, c'etait vraiment tres complexe :p
{
  double v, d;
  double echelle=1;

  // Distance parcourue entre 2 frames
  d = sqrt((posa.x-posb.x)*(posa.x-posb.x)+(posa.y-posb.y)*(posa.y-posb.y))*echelle;

  // La vitesse
  v = d/dt;
  return v;
}
