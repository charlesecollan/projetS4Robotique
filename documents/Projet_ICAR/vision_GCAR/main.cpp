///////////////////////////////////////Lire une vidéo ou directement d'une webcam, d'un flux vidéo (UDP, ARDRONE)///////////////////////////////
////////////////////////////choisir des points d'interets///////////////////////////
/////////////////////////////Calculer le Flux Optique pour chaque point/////////////////////////////
//////////////////////////////Sortir une donnée de type list coordonnées du point et son FO////////// LISV février 2012

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
inline static double square(int a)        {	       return a * a;         }

int h, s;

IplImage *img;


inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )  {	if ( *img != NULL )	return;	*img = cvCreateImage( size, depth, channels );	if ( *img == NULL )	{		fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");		exit(-1);	} }

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
/* Traitement de l’image */

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
	  // cout << "capturer la vidéo demo.avi "<< endl; // juste pour les tests
	  }
      else
	capture = cvCreateFileCapture(adresse_video.c_str());
	//cout << "capturer adresse vidéo "<< endl; // juste pour les tests 2
      break;
    //////////////////////////////////////////////////////////
    case 2:
      cout << "Choisir une webcam - [0] Par defaut : ";
      cin >> webcam;
      capture = cvCreateCameraCapture( webcam );
      //cout << "capturer la vidéo d'une webcam "<< endl; // juste pour les tests
      break;
    ///////////////////////////////////////////////////////////////
      case 3:
      cout << " en cours de realisation ";
      //cin >> webcam;
      //capture = cvCreateCameraCapture( webcam );
      //cout << "capturer la vidéo d'une webcam "<< endl; // juste pour les tests
      return 30;
      break;

      case 4:
      cout << " en cours de realisation ";
      //cin >> webcam;
      //capture = cvCreateCameraCapture( webcam );
      //cout << "capturer la vidéo d'une webcam "<< endl; // juste pour les tests
      return 30;
      break;


      default:
      cerr << "ERREUR 30 : Votre choix est incorrect" << endl << endl;
      return 30;
      break;
    }

     cout << endl;

  int methode_analyse;


  cout << "Travailler par rapport aux :" << endl << "\t [1] Mouvements ,base BGR" << "\t [2] FO, basé point et algo  LK" << endl<< "\t [3] FO, basé ligne et algo  LK" << endl;
  cin >> methode_analyse;


           if  (methode_analyse == 1 )
{

        // Initialisation vitesse moyenne
  double vs=1, vm=0, vi=0, i=0, vc=0, qv=0;
  cout << "Coefficient de calibrage ( 0.038 pour la video de demonstration ) :";
  cin >> qv;

  // Initialisation écriture
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
	  if( flux_video == 2 ) // Si Webcam, on calcule le fps (car annalyse en temps réel)
	    {
	      ta = (double)getTimeMillis();
	      vi = vitesse(objetPos( masquea ), objetPos( masqueb ), (ta-tb)/1000);
	      sprintf(buffer,"Vitesse instantanee : %.2lf pixels/sec", vi );
	      tb = ta;
	    }
	  else // Si Video, on prend le fps des caractéristiques de la video
	    {
	      vi = vitesse(objetPos( masquea ), objetPos( masqueb ), 1/fps);
	      sprintf(buffer,"Vitesse instantanee : %.2lf pixels/sec par rapport au fps de la video", vi );
	    }
	  cvPutText( img, buffer, cvPoint(10,20), &font, cvScalar(0,0,255));
	  if(!(isnan(vi)))
	    {
	      if( enable_vm == 1 && ((vm+vm/2>vi && vm-vm/2<vi) || i<5 ))// permet de lisser les valeurs en tolérant des valeur a max 50% au dessu
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
    // Libère la memoire
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
	//CvCapture *input_video = cvCaptureFromFile("optical_flow_input1.avi" );	//if (input_video == NULL)	//{		/* Either the video didn't exist OR it uses a codec OpenCV		 * doesn't support.		 */	//	fprintf(stderr, "Error: Can't open video.\n");	//	return -1;	//}	/* This is a hack.  If we don't call this first then getting capture	 * properties (below) won't work right.  This is an OpenCV bug.  We	 * ignore the return value here.  But it's actually a video frame.	 */	cvQueryFrame( capture ); //     //img=cvQueryFrame( capture );	/* Read the video's frame size out of the AVI. */	CvSize frame_size;	frame_size.height =(int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT );	frame_size.width =(int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH );	/* Determine the number of frames in the AVI. */	long number_of_frames;	/* Go to the end of the AVI (ie: the fraction is "1") */	cvSetCaptureProperty( capture, CV_CAP_PROP_POS_AVI_RATIO, 1. );	/* Now that we're at the end, read the AVI position in frames */	number_of_frames = (int) cvGetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES );	/* Return to the beginning */	cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, 0. );	/* Create three windows called "Frame N", "Frame N+1", and "Optical Flow"	 * for visualizing the output.  Have those windows automatically change their	 * size to match the output.	 */	cvNamedWindow("Optical Flow", CV_WINDOW_AUTOSIZE);	long current_frame = 0;    double ta,tb,dt;	while(true)	{		static IplImage *frame = NULL, *frame1 = NULL, *frame1_1C = NULL, *frame2_1C = NULL, *eig_image = NULL, *temp_image = NULL, *pyramid1 = NULL, *pyramid2 = NULL;		/* Go to the frame we want.  Important if multiple frames are queried in		 * the loop which they of course are for optical flow.  Note that the very		 * first call to this is actually not needed. (Because the correct position		 * is set outsite the for() loop.)		 */		cvSetCaptureProperty( capture, CV_CAP_PROP_POS_FRAMES, current_frame );		/* Get the next frame of the video.		 * IMPORTANT!  cvQueryFrame() always returns a pointer to the _same_		 * memory location.  So successive calls:		 * frame1 = cvQueryFrame();		 * frame2 = cvQueryFrame();		 * frame3 = cvQueryFrame();		 * will result in (frame1 == frame2 && frame2 == frame3) being true.		 * The solution is to make a copy of the cvQueryFrame() output.		 */		frame = cvQueryFrame( capture );		if (frame == NULL)		{			/* Why did we get a NULL frame?  We shouldn't be at the end. */			fprintf(stderr, "Error: Hmm. The end came sooner than we thought.\n");			return 200;		}          ta = (double)getTimeMillis();//calcul du temps écoulé entre deux frames dt=(tb-ta)/1000		/* Allocate another image if not already allocated.		 * Image has ONE challenge of color (ie: monochrome) with 8-bit "color" depth.		 * This is the image format OpenCV algorithms actually operate on (mostly).		 */		allocateOnDemand( &frame1_1C, frame_size, IPL_DEPTH_8U, 1 );		/* Convert whatever the AVI image format is into OpenCV's preferred format.		 * AND flip the image vertically.  Flip is a shameless hack.  OpenCV reads		 * in AVIs upside-down by default.  (No comment :-))		 */		cvConvertImage(frame, frame1_1C, CV_CVTIMG_FLIP);		/* We'll make a full color backup of this frame so that we can draw on it.		 * (It's not the best idea to draw on the static memory space of cvQueryFrame().)		 */		allocateOnDemand( &frame1, frame_size, IPL_DEPTH_8U, 3 );		cvConvertImage(frame, frame1, CV_CVTIMG_FLIP);		/* Get the second frame of video.  Sample principles as the first. */		frame = cvQueryFrame( capture );		if (frame == NULL)		{			fprintf(stderr, "Error: Hmm. The end came sooner than we thought.\n");			return 200;		}        tb = (double)getTimeMillis();//calcul du temps écoulé entre deux frames dt=(tb-ta)/1000
        dt =(tb-ta)/1000;
		allocateOnDemand( &frame2_1C, frame_size, IPL_DEPTH_8U, 1 );		cvConvertImage(frame, frame2_1C, CV_CVTIMG_FLIP);		/* Shi and Tomasi Feature Tracking! */		/* Preparation: Allocate the necessary storage. */		allocateOnDemand( &eig_image, frame_size, IPL_DEPTH_32F, 1 );		allocateOnDemand( &temp_image, frame_size, IPL_DEPTH_32F, 1 );		/* Preparation: This array will contain the features found in frame 1. */		CvPoint2D32f frame1_features[400];		/* Preparation: BEFORE the function call this variable is the array size		 * (or the maximum number of features to find).  AFTER the function call		 * this variable is the number of features actually found.		 */		int number_of_features;		/* I'm hardcoding this at 400.  But you should make this a #define so that you can		 * change the number of features you use for an accuracy/speed tradeoff analysis.		 */		number_of_features = 400;		/* Actually run the Shi and Tomasi algorithm!!		 * "frame1_1C" is the input image.		 * "eig_image" and "temp_image" are just workspace for the algorithm.		 * The first ".01" specifies the minimum quality of the features (based on the eigenvalues).		 * The second ".01" specifies the minimum Euclidean distance between features.		 * "NULL" means use the entire input image.  You could point to a part of the image.		 * WHEN THE ALGORITHM RETURNS:		 * "frame1_features" will contain the feature points.		 * "number_of_features" will be set to a value <= 400 indicating the number of feature points found.		 */		cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image, frame1_features, &number_of_features, .01, .01, NULL);		/* Pyramidal Lucas Kanade Optical Flow! */		/* This array will contain the locations of the points from frame 1 in frame 2. */		CvPoint2D32f frame2_features[400];		/* The i-th element of this array will be non-zero if and only if the i-th feature of		 * frame 1 was found in frame 2.		 */		char optical_flow_found_feature[400];		/* The i-th element of this array is the error in the optical flow for the i-th feature		 * of frame1 as found in frame 2.  If the i-th feature was not found (see the array above)		 * I think the i-th entry in this array is undefined.		 */		float optical_flow_feature_error[400];		/* This is the window size to use to avoid the aperture problem (see slide "Optical Flow: Overview"). */		CvSize optical_flow_window = cvSize(3,3);		/* This termination criteria tells the algorithm to stop when it has either done 20 iterations or when		 * epsilon is better than .3.  You can play with these parameters for speed vs. accuracy but these values		 * work pretty well in many situations.		 */		CvTermCriteria optical_flow_termination_criteria			= cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );		/* This is some workspace for the algorithm.		 * (The algorithm actually carves the image into pyramids of different resolutions.)		 */		allocateOnDemand( &pyramid1, frame_size, IPL_DEPTH_8U, 1 );		allocateOnDemand( &pyramid2, frame_size, IPL_DEPTH_8U, 1 );		/* Actually run Pyramidal Lucas Kanade Optical Flow!!		 * "frame1_1C" is the first frame with the known features.		 * "frame2_1C" is the second frame where we want to find the first frame's features.		 * "pyramid1" and "pyramid2" are workspace for the algorithm.		 * "frame1_features" are the features from the first frame.		 * "frame2_features" is the (outputted) locations of those features in the second frame.		 * "number_of_features" is the number of features in the frame1_features array.		 * "optical_flow_window" is the size of the window to use to avoid the aperture problem.		 * "5" is the maximum number of pyramids to use.  0 would be just one level.		 * "optical_flow_found_feature" is as described above (non-zero iff feature found by the flow).		 * "optical_flow_feature_error" is as described above (error in the flow for this feature).		 * "optical_flow_termination_criteria" is as described above (how long the algorithm should look).		 * "0" means disable enhancements.  (For example, the second aray isn't pre-initialized with guesses.)		 */		cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2, frame1_features, frame2_features, number_of_features, optical_flow_window, 5, optical_flow_found_feature, optical_flow_feature_error, optical_flow_termination_criteria, 0 );		/* For fun (and debugging :)), let's draw the flow field. */		for(int i = 0; i < number_of_features; i++)		{			/* If Pyramidal Lucas Kanade didn't really find the feature, skip it. */			if ( optical_flow_found_feature[i] == 0 )	continue;			int line_thickness;				line_thickness = 1;			/* CV_RGB(red, green, blue) is the red, green, and blue components			 * of the color you want, each out of 255.			 */			CvScalar line_color;			line_color = CV_RGB(255,0,0);			/* Let's make the flow field look nice with arrows. */			/* The arrows will be a bit too short for a nice visualization because of the high framerate			 * (ie: there's not much motion between the frames).  So let's lengthen them by a factor of 3.			 */			CvPoint p,q;			p.x = (int) frame1_features[i].x;			p.y = (int) frame1_features[i].y;			q.x = (int) frame2_features[i].x;			q.y = (int) frame2_features[i].y;
			double angle;		angle = atan2( (double) p.y - q.y, (double) p.x - q.x );			double hypotenuse;	hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );


        double vitesse_inst;
        vitesse_inst =hypotenuse/dt;

        cout << setw(15) << "vitesse_inst" <<"...."<<vitesse_inst<< endl;

			/* Le repère de l'image commence en haut à gauche
			donc pour bien reproduire le flux on va calculer les coordonnées du point q en appliquant
			un facteur égale à 3 */			q.x = (int) (p.x - 3 * hypotenuse * cos(angle));			q.y = (int) (p.y - 3* hypotenuse * sin(angle));			/* Now we draw the main line of the arrow. */			/* "frame1" is the frame to draw on.			 * "p" is the point where the line begins.			 * "q" is the point where the line stops.			 * "CV_AA" means antialiased drawing.			 * "0" means no fractional bits in the center cooridinate or radius.			 */			cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );			/* la pointe de la flèche est à l'echelle 5 */			p.x = (int) (q.x + 5 * cos(angle + pi / 4));			p.y = (int) (q.y + 5 * sin(angle + pi / 4));			cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );			p.x = (int) (q.x + 5* cos(angle - pi / 4));			p.y = (int) (q.y + 5 * sin(angle - pi / 4));			cvLine( frame1, p, q, line_color, line_thickness, CV_AA, 0 );		}		/* Now display the image we drew on.  Recall that "Optical Flow" is the name of		 * the window we created above.		 */		cvShowImage("Optical Flow", frame1);

		/* And wait for the user to press a key (so the user has time to look at the image).		 * If the argument is 0 then it waits forever otherwise it waits that number of milliseconds.		 * The return value is the key the user pressed.		 */		int key_pressed;		key_pressed = cvWaitKey(10);		/* If the users pushes "b" or "B" go back one frame.		 * Otherwise go forward one frame.		 */		if (key_pressed == 'b' || key_pressed == 'B')	current_frame--;		else											current_frame++;		/* Don't run past the front/end of the AVI. */		if (current_frame < 0)						current_frame = 0;		if (current_frame >= number_of_frames - 1)	current_frame = number_of_frames - 2;	}
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


//définition des images a utiliser
    IplImage *src = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
    IplImage *src_gray = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
    IplImage *canny = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);

    double seuil_bas = 5.0;
    double seuil_haut = 10.0;
    int taille_masque=3;

    //i,j,k sont utilisés pour récuperer les valeurs des trackbars
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

        //applique le filtre médian 2 fois de suite pour réduire le bruit
        for (int l=0;l<2;l++){
            cvSmooth(src_gray,src_gray,CV_MEDIAN,3);
        }

        //definition des trackbars
        cvCreateTrackbar( "seuil_bas", "out", &i, 100, NULL );
        cvCreateTrackbar( "seuil_haut", "out", &j, 100, NULL );
        cvCreateTrackbar( "masque", "out", &taille_masque, 7, NULL );

        //les valeurs de la taille du masque autres que 3,5,7 générant une erreur dans cvCanny():
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

        //exécute canny
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

    // On détruit les fenêtres créées
    cvDestroyAllWindows();

    // On détruit la capture
    cvReleaseCapture( &capture );


return 30;
*/
///////////////////////////////essai mardi 27 février 2012

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
        //Crée une image d'un seul canal
		// cvCanny(img, dst, 50, 200, 3);
        //Convertit 'img' en niveaux de gris 'img_gray'.
        // cvCvtColor( dst, img_gray, CV_RGB2GRAY );
        //Crée un espace de stockage mémoire qui sera utilisé par cvHoughLines2
         CvMemStorage *line_storage = cvCreateMemStorage(0);
        //Crée une séquence pour stocker (rho et theta) retournés par cvHoughLines2
         CvSeq* lines;

        //applique Hough en choisissant le seuil, les dimensions de l'accumulateur :radians,pixels.
    lines = cvHoughLines2(canny_out, line_storage, CV_HOUGH_STANDARD, 1, 1*(CV_PI/180), 10 );

        for( int i = 0; i < 50; i++ )
        {
            //récupère les (rho et theta) retournés par cvHoughLines2
            float* line = (float*)cvGetSeqElem(lines,i);
            float rho = line[0];
            float theta = line[1];
            //Définit les 2 points qui serviront à tracer la droite
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

    // On détruit les fenêtres créées
    cvDestroyAllWindows();

    // On détruit la capture
    cvReleaseCapture( &capture );


return 30;

//////////////////////////////////////////////////////
  }

  else
  {
      cerr << "erreur" << endl << endl;
  }
}

