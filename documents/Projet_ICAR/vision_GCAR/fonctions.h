//Fonctions necessaire a l' analyse du flux video pour obtenir l'image binaire
void analyseDifferentierFrames( IplImage *imga, IplImage *imgb, IplImage *masque, int tol );

//Fonctions necessaire au calcul de la vitesse
CvPoint objetPos( IplImage* masque );
double vitesse( CvPoint posa, CvPoint posb, double dt );
long long getTimeMillis();


//Variable generale
extern int h, s;
extern IplImage* img;
