//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\core.hpp>
//#include <opencv\cv.hpp>
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;
using namespace std;

// Read input video
VideoCapture cap("video.mp4");

// Get frame count
int n_frames = int(cap.get(CAP_PROP_FRAME_COUNT)); 

// Get width and height of video stream
int w = int(cap.get(CAP_PROP_FRAME_WIDTH)); 
int h = int(cap.get(CAP_PROP_FRAME_HEIGHT));

// Get frames per second (fps)
double fps = cap.get(CV_CAP_PROP_FPS);

// Set up output video
VideoWriter out("video_out.avi", CV_FOURCC('M','J','P','G'), fps, Size(2 * w, h));

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define variable for storing frames
Mat curr, curr_gray;
Mat prev, prev_gray;

// Read first frame
cap &gt;&gt; prev;

// Convert frame to grayscale
cvtColor(prev, prev_gray, COLOR_BGR2GRAY);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Pre-define transformation-store array
  vector <TransformParam> transforms; 

  // 
  Mat last_T;

  for(int i = 1; i < n_frames-1; i++)
  {
    // Vector from previous and current feature points
    vector <Point2f> prev_pts, curr_pts;

    // Detect features in previous frame
    goodFeaturesToTrack(prev_gray, prev_pts, 200, 0.01, 30);

    // Read next frame 
    bool success = cap.read(curr);
    if(!success) break; 
    
    // Convert to grayscale
    cvtColor(curr, curr_gray, COLOR_BGR2GRAY);

    // Calculate optical flow (i.e. track feature points)
    vector <uchar> status;
    vector <float> err;
    calcOpticalFlowPyrLK(prev_gray, curr_gray, prev_pts, curr_pts, status, err);

    // Filter only valid points
    auto prev_it = prev_pts.begin(); 
    auto curr_it = curr_pts.begin(); 
    for(size_t k = 0; k < status.size(); k++) 
    {
        if(status[k]) 
        {
          prev_it++; 
          curr_it++; 
        }
        else 
        {
          prev_it = prev_pts.erase(prev_it);
          curr_it = curr_pts.erase(curr_it);
        }
    }

    
    // Find transformation matrix
    Mat T = estimateRigidTransform(prev_pts, curr_pts, false); 

    // In rare cases no transform is found. 
    // We'll just use the last known good transform.
    if(T.data == NULL) last_T.copyTo(T);
    T.copyTo(last_T);

    // Extract traslation
    double dx = T.at<double>(0,2);
    double dy = T.at<double>(1,2);
    
    // Extract rotation angle
    double da = atan2(T.at<double>(1,0), T.at<double>(0,0));

    // Store transformation 
    transforms.push_back(TransformParam(dx, dy, da));

    // Move to next frame
    curr_gray.copyTo(prev_gray);

    cout << "Frame: " << i << "/" << n_frames << " -  Tracked points : " << prev_pts.size() << endl;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  struct Trajectory
{
    Trajectory() {}
    Trajectory(double _x, double _y, double _a) {
        x = _x;
        y = _y;
        a = _a;
    }

    double x;
    double y;
    double a; // angle
};

vector&lt;Trajectory&gt; cumsum(vector&lt;TransformParam&gt; &amp;transforms)
{
  vector &lt;Trajectory&gt; trajectory; // trajectory at all frames
  // Accumulated frame to frame transform
  double a = 0;
  double x = 0;
  double y = 0;

  for(size_t i=0; i &lt; transforms.size(); i++) 
  {
      x += transforms[i].dx;
      y += transforms[i].dy;
      a += transforms[i].da;

      trajectory.push_back(Trajectory(x,y,a));

  }

  return trajectory; 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Smooth trajectory using moving average filter
  vector &lt;Trajectory&gt; smoothed_trajectory = smooth(trajectory, SMOOTHING_RADIUS); 

  vector &lt;TransformParam&gt; transforms_smooth;

for(size_t i=0; i &lt; transforms.size(); i++)
{
  // Calculate difference in smoothed_trajectory and trajectory
  double diff_x = smoothed_trajectory[i].x - trajectory[i].x;
  double diff_y = smoothed_trajectory[i].y - trajectory[i].y;
  double diff_a = smoothed_trajectory[i].a - trajectory[i].a;

  // Calculate newer transformation array
  double dx = transforms[i].dx + diff_x;
  double dy = transforms[i].dy + diff_y;
  double da = transforms[i].da + diff_a;

  transforms_smooth.push_back(TransformParam(dx, dy, da));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cap.set(CV_CAP_PROP_POS_FRAMES, 1);
Mat T(2,3,CV_64F);
Mat frame, frame_stabilized, frame_out; 


for( int i = 0; i &lt; n_frames-1; i++) { bool success = cap.read(frame); if(!success) break; // Extract transform from translation and rotation angle. transforms_smooth[i].getTransform(T); // Apply affine wrapping to the given frame warpAffine(frame, frame_stabilized, T, frame.size()); // Scale image to remove black border artifact fixBorder(frame_stabilized); // Now draw the original and stabilised side by side for coolness hconcat(frame, frame_stabilized, frame_out); // If the image is too big, resize it. if(frame_out.cols &gt; 1920) 
  {
      resize(frame_out, frame_out, Size(frame_out.cols/2, frame_out.rows/2));
  }

  imshow("Before and After", frame_out);
  out.write(frame_out);
  waitKey(10);
}

