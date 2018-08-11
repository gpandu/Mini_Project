#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
 
using namespace cv;
using namespace std;
Mat image_array2[10];
Mat image_array[10]; //global declaration of image arrays
int input_image; 


int gradientmax(Mat image_pad,int p,int q){   //Maximum gradient finder
		   int temp1, temp2;

			temp1 = (int)(image_pad.at<uchar>(p,q+1))-(int)(image_pad.at<uchar>(p,q-1));
			temp2 = (int)(image_pad.at<uchar>(p-1,q))-(int)(image_pad.at<uchar>(p+1,q));

			if(abs(temp1)>=abs(temp2))
			{
				return(temp1);
			}
			else
				return(temp2);
		
}
		

void True_positive(Mat ground_truth,Mat output,float tp)  // to calculate TPs and FNs

{
	int white =0;
	tp=0;
	float fn=0;
  for (int y=0;y<ground_truth.rows;++y)
        {
                for (int x=0; x<ground_truth.cols;++x)
                {
					if((int)ground_truth.at<uchar>(y,x)==255){
					             white++;
					             if(output.at<uchar>(y,x)==255)
					                    tp++;
								 else
									   fn++;
					}
				}
  }
  
  //cout<<white<<"\n";
  
  cout<<"true_positives:"<<tp<< " (" <<tp/white*100<< "%)TP rate."<<"\n";
  cout<<"false_negtives:"<<fn<< " (" <<fn/white*100<< "%)TN rate."<<"\n";
}

void True_negetive(Mat ground_truth,Mat output,float tn)  // to calculate TNs and FPs
{
	int black=0;
	tn=0;
	float fp=0;
  for (int y=0;y<ground_truth.rows;++y)
        {
                for (int x=0; x<ground_truth.cols;++x)
                {
					if((int)ground_truth.at<uchar>(y,x)==0){
					             black++;
					             if(output.at<uchar>(y,x)==0)
					                    tn++;
								 else
									   fp++;
					}
				}
  }
 
 // cout<<black<<"\n";
  
  cout<<"true_negetives:"<<tn<< " (" <<tn/black*100<< "%)TN rate."<<"\n";
  cout<<"false_positives:"<<fp<< " (" <<fp/black*100<< "%)FP rate."<<"\n";

  cout<<"\n\n";
}
void compare_disparity_images(Mat ground_truth, Mat output,Mat Diff)  // to obtain diffrence image 
{
        
        static const unsigned char no_data_value = 0;
       
        for (int y=0;y<ground_truth.rows;++y)
        {
                for (int x=0; x<ground_truth.cols;++x)
                {
                        
                               Diff.at<uchar>(y,x) = abs((int)(ground_truth.at<uchar>(y,x))-(int)(output.at<uchar>(y,x)));
                               
                }
        }      
		
}


int count_bad_disparity(Mat Diff, int tolerance)// to obtain pixels that do not matched with pixels of ground truth
{
        int count_bad = 0;
        for (int y=0;y<Diff.rows; ++y)
        {
                for (int x=0; x<Diff.cols; ++x)
                {
                        if ((int)Diff.at<uchar>(y,x)>tolerance)
                        {
                                ++count_bad;
                        }
                }
        }
 
        return count_bad;
}




                               
void seg_image(int, void*){      //Function that includes segmentation and other functions for calculating TN TP FP FN
	long int l=0,p=0;
    
	Mat image = Mat(image_array[input_image]);
	int n=2,k=0;
	  Mat image_pad=image.clone();
	 
	copyMakeBorder(image,image_pad,n-1,n-1,n-1,20,BORDER_REPLICATE);
	//printf("%d\n",image_pad.rows);
	//  printf("%d\n",image_pad.cols);
	
	
	for(int x=n-1;x<image_pad.rows-(n-1);x++){
		for(int y=n-1;y<image_pad.cols-(n-1);y++){
			k=gradientmax(image_pad,x,y);
			l=l+abs(k);
		//gradientXimage.at<uchar>(x,y)=gradientmax(image_pad,x,y);  
		p=p+abs((int)(image_pad.at<uchar>(x,y))*k);
	
	 }
	}
	int  threshold = p/l;
	
	Mat image_out = image.clone();
	for(int x=0;x<image.rows;x++){
		for(int y=0;y<image.cols;y++){
			if((int)(image.at<uchar>(x,y))>=threshold)
				image_out.at<uchar>(x,y)=255;
			else
	image_out.at<uchar>(x,y)=0;
			
	 }
	}
	namedWindow("retinal_image");
	imshow("retinal_image",image);
	namedWindow("seg_output");
	imshow("seg_output",image_out);
	Mat ground_truth= Mat(image_array2[input_image]);
	Mat output=image_out.clone();
	Mat Diff=ground_truth.clone();
	compare_disparity_images(ground_truth,output,Diff);
	int count_bad = count_bad_disparity(Diff,1);
	cout<<"Comparison Measures wrt Ground truth image:"<<"\n\n";        //Comparison measures
	printf("threshold is:%d\n",threshold);
    cout << "The image has matched pixels:" <<(ground_truth.rows*ground_truth.cols-count_bad)<< " (" << (100-100.0f*count_bad /(ground_truth.rows*ground_truth.cols)) << "%) Accuracy." << std::endl;
   float tp=0;
    True_positive(ground_truth,output,tp);
    float tn=0;
    True_negetive(ground_truth,output,tn);
	imwrite("seg21.bmp",image_out);
	
}




int main(int argc, char *argv[])
{

	int img_max;   //read image name
	printf("Number of images you want:");
	scanf("%d",&img_max);
	char name[20];
	char gtruth[20];
for(int i=0;i<img_max;i++){
		
	cout<<"enter the image name:";
	cin>>name;
                      // Load the image into matrix
    image_array[i]=imread(name,CV_LOAD_IMAGE_UNCHANGED);
	cout<<"corresponding ground_truth image name:";
	cin>>gtruth;
	
	image_array2[i]=imread(gtruth,CV_LOAD_IMAGE_UNCHANGED);;
}
cout<<"\n\n";
	char TrackbarName[50];
	input_image=0;
	namedWindow("retinal_image");
	// Trackbar to access segmentation fuction for different images
	createTrackbar("Gaborimage_no", "retinal_image", &input_image,img_max-1,seg_image);
    seg_image( input_image, 0 );
	
    waitKey();
	system("pause");
	return 0;
	}
