///////////////////////////////////////Lire une vid�o ou directement d'une webcam, d'un flux vid�o (UDP, ARDRONE)///////////////////////////////
////////////////////////////choisir des points d'interets///////////////////////////
/////////////////////////////Calculer le Flux Optique pour chaque point/////////////////////////////
//////////////////////////////Sortir une donn�e de type list coordonn�es du point et son FO////////// LISV f�vrier 2012

#include <iostream>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include "opencv2/highgui/highgui.hpp" //hough
#include "opencv2/imgproc/imgproc.hpp" //hough
//
#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
#include <iomanip>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
//
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <string>
#include <windows.h>


#include "fonctions.h"

#define sign(x) ((x) > 0 ? 1 : -1)
using namespace cv; //hough
using namespace std;
static const double pi = 3.14159265358979323846;
inline static double square(int a)

int h, s;

IplImage *img;


inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )

int main()
{
    //Gestion du menu dans la console
 // system("clear");

  int choix_menu=0, flux_video;

  cout << "LISV 2012 -  Lecteur de flux video et calcul de Flux Optique" << endl << "\t - en differe pour les videos sur les disques" << endl << "\t - en temps reel pour la webcam" << endl << endl << "  Menu" << endl;
  cout << "[1] Lire une video" << endl;
  cout << "[2] Webcam" << endl;
  cout << "[3] Camera horizontale du drone" << endl;
  cout << "[4] Camera verticale du drone" << endl;


  cin >> choix_menu;

  flux_video = choix_menu;

  //system("clear");

  cout << "----- Lecteur de Flux Video -----" << endl << endl;

  string adresse_video;
  int webcam;
//IplImage *im;
//CvCapture *avi;
/* Ouverture de la video */
//avi = cvCaptureFromAVI("ma_video.AVI");
//while(cvGrabFrame(avi))
//{
//im = cvRetrieveFrame(avi);
/* Traitement de l�image */

  CvCapture *capture;

  switch (choix_menu)
    {
    case 1:
      cout << "Entrez l'adresse de votre video sinon [0]-Pour demo : ";
      cin >> adresse_video;

      if( adresse_video == "0" )
	  {
	  capture = cvCreateFileCapture("demo.AVI");
	  //capture = cvCaptureFromAVI("demo1.AVI");
	  // cout << "capturer la vid�o demo.avi "<< endl; // juste pour les tests
	  }
      else
	capture = cvCreateFileCapture(adresse_video.c_str());
	//cout << "capturer adresse vid�o "<< endl; // juste pour les tests 2
      break;
    //////////////////////////////////////////////////////////
    case 2:
      cout << "Choisir une webcam - [0] Par defaut : ";
      cin >> webcam;
      capture = cvCreateCameraCapture( webcam );
      //cout << "capturer la vid�o d'une webcam "<< endl; // juste pour les tests
      break;
    ///////////////////////////////////////////////////////////////
      case 3:
      cout << " en cours de realisation ";
      //cin >> webcam;
      //capture = cvCreateCameraCapture( webcam );
      //cout << "capturer la vid�o d'une webcam "<< endl; // juste pour les tests
      return 30;
      break;

      case 4:
      cout << " en cours de realisation ";
      //cin >> webcam;
      //capture = cvCreateCameraCapture( webcam );
      //cout << "capturer la vid�o d'une webcam "<< endl; // juste pour les tests
      return 30;
      break;


      default:
      cerr << "ERREUR 30 : Votre choix est incorrect" << endl << endl;
      return 30;
      break;
    }

     cout << endl;

  int methode_analyse;


  cout << "Travailler par rapport aux :" << endl << "\t [1] Mouvements ,base BGR" << "\t [2] FO, bas� point et algo  LK" << endl<< "\t [3] FO, bas� ligne et algo  LK" << endl;
  cin >> methode_analyse;


           if  (methode_analyse == 1 )
{

        // Initialisation vitesse moyenne
  double vs=1, vm=0, vi=0, i=0, vc=0, qv=0;
  cout << "Coefficient de calibrage ( 0.038 pour la video de demonstration ) :";
  cin >> qv;

  // Initialisation �criture
  CvFont font;
  cvInitFont( &font , CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0.0, 1);
  char buffer[150];

  // Initialisation de la capture
  if( !capture )
    {
      cerr << "ERREUR 10 : Impossible d'ouvrir la source video" << endl << endl;
      return 10;
    };

  img=cvQueryFrame( capture );
  if( !img )
    {
      cerr << "ERREUR 20 : Erreur de lecture de la video" << endl << endl;
      return 20;
    };

  // Inititalisation des fenetres
  cvNamedWindow("analyse", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("parametre", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("video", CV_WINDOW_AUTOSIZE);


  // Initialisation des images pour la recurrence
  IplImage *masquea, *masqueb;
  IplImage *imga, *imgb;
  IplImage *hsv;
  imga = cvCloneImage(img);
  imgb = cvCloneImage(imga);
  hsv = cvCloneImage(img);
  cvCvtColor(img, hsv, CV_BGR2HSV);

  masquea = cvCreateImage(cvGetSize(img), img->depth, 1);
  masqueb = cvCreateImage(cvGetSize(img), img->depth, 1);

  // Initialisation des commandes clavier
  int p=1;
  char key = 'a';
  int enable_vm=0;

  // Initialisation du temps
  double ta, tb;
  double fps;
  tb = (double)getTimeMillis();
  if( flux_video == 1 )// En fonction de si c'est une Webcam ou une Video
    {
      fps = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS );
    };


  // Initialisation souris et des trackbars

  int tol;

   tol=160;
   cvCreateTrackbar("Tolerance ", "parametre", &tol, 255, NULL);

  while(key != 'q')
    {
      if( p==1 )
	{
	  // Calcul et affichage de la vitesse
	  if( flux_video == 2 ) // Si Webcam, on calcule le fps (car annalyse en temps r�el)
	    {
	      ta = (double)getTimeMillis();
	      vi = vitesse(objetPos( masquea ), objetPos( masqueb ), (ta-tb)/1000);
	      sprintf(buffer,"Vitesse instantanee : %.2lf pixels/sec", vi );
	      tb = ta;
	    }
	  else // Si Video, on prend le fps des caract�ristiques de la video
	    {
	      vi = vitesse(objetPos( masquea ), objetPos( masqueb ), 1/fps);
	      sprintf(buffer,"Vitesse instantanee : %.2lf pixels/sec par rapport au fps de la video", vi );
	    }
	  cvPutText( img, buffer, cvPoint(10,20), &font, cvScalar(0,0,255));
	  if(!(isnan(vi)))
	    {
	      if( enable_vm == 1 && ((vm+vm/2>vi && vm-vm/2<vi) || i<5 ))// permet de lisser les valeurs en tol�rant des valeur a max 50% au dessu
		{
		  i++;
		  vs+=vi;
		  vm=vs/i;
		}
	      sprintf(buffer,"Vitesse moyenne : %.2lf pixels/sec",  vm);
	      cvPutText( img, buffer, cvPoint(10,40), &font, cvScalar(0,0,255));
	      vc=vm*qv;
	      sprintf(buffer,"Vitesse moyenne : %.2lf cm/sec",  vc);
	      cvPutText( img, buffer, cvPoint(10,60), &font, cvScalar(0,0,255));

	    }

	  // Affichage du barycentre des points
	  cvDrawCircle(img, objetPos( masquea ), 15, CV_RGB(255, 0, 0), -1);// Dessine un cerlce rouge sur le mobile


	  cvShowImage( "video", img );


	  //Binarisation de l'image, Choix de la methode d'analyse de l'image
	  cvCopyImage( masquea, masqueb );
	   // On observe les differences entre 2 frames succesives
    analyseDifferentierFrames( imga, imgb, masquea, tol );


	  cvShowImage( "analyse", masquea );
	}

      // Gestion du clavier
      key = cvWaitKey(10);
      if( p==0 && key == 'p' )//pause/play
	p=1;
      else if( p==1 && key == 'p' )
	p=0;
      if( key == 'm' && enable_vm == 0)//vitasse moyenne
	enable_vm=1;
      else if( key == 'm' && enable_vm == 1 )
	enable_vm=0;
      if( key == 'r' )//reinitialisation de la vitesse moyenne
	{
	  vs=0;
	  i=0;
	}
      // On capture la frame suivante
      if( p==1 )
	{
	  if((img = cvQueryFrame( capture )))
	    {
	      cvCopyImage(imga, imgb); cvCopyImage(img, imga);
	    }
	  else
	    {
	      key = 'q';
	    }
	}
    }
    // Lib�re la memoire
  cvDestroyAllWindows();
  masquea = NULL; masqueb = NULL;
  img = NULL; imga = NULL; imgb = NULL;
  cvReleaseCapture( &capture );

return 90;
 // else
  //

}

//fin du cas

  else if   (methode_analyse == 2 )
  {

      ///////  LUCAS ....
      /* Create an object that decodes the input video stream. */
	//CvCapture *input_video = cvCaptureFromFile("optical_flow_input1.avi" );
        dt =(tb-ta)/1000;




        double vitesse_inst;
        vitesse_inst =hypotenuse/dt;

        cout << setw(15) << "vitesse_inst" <<"...."<<vitesse_inst<< endl;


			donc pour bien reproduire le flux on va calculer les coordonn�es du point q en appliquant
			un facteur �gale � 3 */


   cvDestroyAllWindows();

  cvReleaseCapture( &capture );

return 90;
 // else
  //


      ////////
  }
  else if (methode_analyse == 3 )
  {
/*
   CvMat src= cvQueryFrame( capture );

      CvMat dst, cdst;
 CvCanny(src, dst, 50, 200, 3);
 cvtColor(dst, cdst, CV_GRAY2BGR);

 #if 0
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }
 #else
  vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
  }
 #endif
 imshow("source", src);
 imshow("detected lines", cdst);

   cvWaitKey(33);

      //
*/
/*
IplImage* frame;

    while(1) {
        frame = cvQueryFrame( capture );

        int depth_img =frame->depth;
        int height_img =frame->height;
        int width_img =frame->width;
        int size_img =frame->imageSize;
        int nchan_img =frame->nChannels;
        int nsize_img =frame->nSize;

        cout << setw(15) << "depth" <<  depth_img << endl;
        cout << setw(15) << "height" <<  height_img << endl;
        cout << setw(15) << "width" <<  width_img << endl;
        cout << setw(15) << "size" <<  size_img << endl;
        cout << setw(15) << "nchan" <<  nchan_img << endl;
        cout << setw(15) << "nsize" <<  nsize_img << endl;


        IplImage* out = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
        IplImage* gray_out = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
        IplImage* canny_out = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
        cvSmooth( frame, out, CV_GAUSSIAN, 11, 11 );
        cvCvtColor(out , gray_out, CV_RGB2GRAY);
        cvCanny( gray_out, canny_out, 10, 10, 3 );

        if( !frame ) break;
    vector<Vec2f> lines;


    CvMat *mat = cvCreateMat(gray_out->height,gray_out->width,CV_32FC3 );
          // cvConvert( gray_out, mat );

   HoughLines(mat, lines, 1, CV_PI/180, 100, 0, 0 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
       cvLine( gray_out, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
    }

         cvShowImage( "Source",frame);
         cvShowImage( "Line", gray_out );
        cvShowImage( "Edge", canny_out );
       cvShowImage( "mat", mat);
        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }
      cvReleaseCapture( &capture );
      cvDestroyWindow( "Edge" );
      cvDestroyWindow( "Line" );
      cvDestroyWindow( "Source" );
    return 90;
*/


/*


//d�finition des images a utiliser
    IplImage *src = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
    IplImage *src_gray = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
    IplImage *canny = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);

    double seuil_bas = 5.0;
    double seuil_haut = 10.0;
    int taille_masque=3;

    //i,j,k sont utilis�s pour r�cuperer les valeurs des trackbars
    int i = (int) seuil_bas;
    int j = (int) seuil_haut;
    int k;

    //boucle pour lire de la webcam
    CvCapture* capture = cvCreateCameraCapture( CV_CAP_ANY );
    if( !capture )
        capture = cvCreateCameraCapture( CV_CAP_ANY );
    if(!cvGrabFrame( capture ))
        return 20;
        */
        /*
    int key=0;

    while(1)
    {
        //si appui sur la touche 'echap' alors terminer
        if(key == 27 ) break;
        //capture les frames dans l'image 'src'
        //src = cvRetrieveFrame( capture );
        src = cvQueryFrame( capture );

        //convertit 'src' en niveaux de gris 'src_gray'
        cvConvertImage(src, src_gray);

        //applique le filtre m�dian 2 fois de suite pour r�duire le bruit
        for (int l=0;l<2;l++){
            cvSmooth(src_gray,src_gray,CV_MEDIAN,3);
        }

        //definition des trackbars
        cvCreateTrackbar( "seuil_bas", "out", &i, 100, NULL );
        cvCreateTrackbar( "seuil_haut", "out", &j, 100, NULL );
        cvCreateTrackbar( "masque", "out", &taille_masque, 7, NULL );

        //les valeurs de la taille du masque autres que 3,5,7 g�n�rant une erreur dans cvCanny():
        k = cvGetTrackbarPos("masque","out");
        switch (k){
        case 0:
        case 1:
        case 2:
        {cvSetTrackbarPos("masque","out",3);}
        break;
        case 4:
        {cvSetTrackbarPos("masque","out",5);}
        break;
        case 6:
        {cvSetTrackbarPos("masque","out",7);}
        break;
        }

        //ex�cute canny
        cvCanny(src_gray, canny, i, j, taille_masque );

        //fenetres pour afficher les images
        cvNamedWindow( "in", CV_WINDOW_AUTOSIZE );
        cvShowImage( "in", src );
        cvNamedWindow( "out", CV_WINDOW_AUTOSIZE );
        cvShowImage( "out", canny);

        // On attend 10 ms
        key = cvWaitKey(10);

        // On essaye de capturer la frame suivante
        if(!cvGrabFrame( capture )) key = 27;

        }

    // On d�truit les fen�tres cr��es
    cvDestroyAllWindows();

    // On d�truit la capture
    cvReleaseCapture( &capture );


return 30;
*/
///////////////////////////////essai mardi 27 f�vrier 2012

 int key=0;

 while( 1) {


     //@@@@@@@@@@@@@@@@@@@@@@
        IplImage* frame;
        frame = cvQueryFrame( capture );

   /*     int depth_img =frame->depth;
        int height_img =frame->height;
        int width_img =frame->width;
        int size_img =frame->imageSize;
        int nchan_img =frame->nChannels;
        int nsize_img =frame->nSize;
   */
        IplImage* out = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
        IplImage* gray_out = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
        IplImage* canny_out = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
        cvSmooth( frame, out, CV_GAUSSIAN, 11, 11 );


        cvCvtColor(out , gray_out, CV_RGB2GRAY);
        cvCanny( gray_out, canny_out, 10, 10, 3 );
     //@@@@@@@@@@@@@@@@@@@@@@@

      // IplImage *img = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
       //IplImage *img_gray = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
       //IplImage *dst = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
         // img = cvRetrieveFrame( capture );
		 //img=cvQueryFrame( capture );
        //Cr�e une image d'un seul canal
		// cvCanny(img, dst, 50, 200, 3);
        //Convertit 'img' en niveaux de gris 'img_gray'.
        // cvCvtColor( dst, img_gray, CV_RGB2GRAY );
        //Cr�e un espace de stockage m�moire qui sera utilis� par cvHoughLines2
         CvMemStorage *line_storage = cvCreateMemStorage(0);
        //Cr�e une s�quence pour stocker (rho et theta) retourn�s par cvHoughLines2
         CvSeq* lines;

        //applique Hough en choisissant le seuil, les dimensions de l'accumulateur :radians,pixels.
    lines = cvHoughLines2(canny_out, line_storage, CV_HOUGH_STANDARD, 1, 1*(CV_PI/180), 10 );

        for( int i = 0; i < 50; i++ )
        {
            //r�cup�re les (rho et theta) retourn�s par cvHoughLines2
            float* line = (float*)cvGetSeqElem(lines,i);
            float rho = line[0];
            float theta = line[1];
            //D�finit les 2 points qui serviront � tracer la droite
            CvPoint pt1, pt2;
            pt1.x = cvRound(cos(theta)*rho + 1000*(-sin(theta)));
            pt1.y = cvRound(sin(theta)*rho + 1000*(cos(theta)));
            pt2.x = cvRound(cos(theta)*rho - 1000*(-sin(theta)));
            pt2.y = cvRound(sin(theta)*rho - 1000*(cos(theta)));

            //Dessine la droite
            cvLine( out, pt1, pt2, CV_RGB(0,255,255), 1, 8 );
        }

        //fenetres pour afficher les images
        cvNamedWindow( "out", CV_WINDOW_AUTOSIZE );
        cvShowImage( "out", out );
        //cvNamedWindow( "out", CV_WINDOW_AUTOSIZE );
       // cvShowImage( "out", canny_out);

        // On attend 10 ms
        key = cvWaitKey(10);

        // On essaye de capturer la frame suivante
        if(!cvGrabFrame( capture )) key = 27;

        }

    // On d�truit les fen�tres cr��es
    cvDestroyAllWindows();

    // On d�truit la capture
    cvReleaseCapture( &capture );


return 30;

//////////////////////////////////////////////////////
  }

  else
  {
      cerr << "erreur" << endl << endl;
  }
}
