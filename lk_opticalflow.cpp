int lucas_kanade(const string& filename)
{
    // Read the video 
    VideoCapture capture(filename);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }

    // Create random colors
    vector<Scalar> colors;
    RNG rng;
    for(int i = 0; i < 100; i++)
    {
        int r = rng.uniform(0, 256);
        int g = rng.uniform(0, 256);
        int b = rng.uniform(0, 256);
        colors.push_back(Scalar(r,g,b));
    }

    Mat old_frame, old_gray;
    vector<Point2f> p0, p1;

    // Read first frame and find corners in it
    capture >> old_frame;
    cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(old_gray, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);

    // Create a mask image for drawing purposes
    Mat mask = Mat::zeros(old_frame.size(), old_frame.type());
    
while(true){
    // Read new frame
    Mat frame, frame_gray;
    capture >> frame;
    if (frame.empty())
        break;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

    // Calculate optical flow
    vector<uchar> status;
    vector<float> err;
    TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
    calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15,15), 2, criteria);
    vector<Point2f> good_new;

    // Visualization part
    for(uint i = 0; i < p0.size(); i++)
    {
        // Select good points
        if(status[i] == 1) {
            good_new.push_back(p1[i]);
            // Draw the tracks
            line(mask,p1[i], p0[i], colors[i], 2);
            circle(frame, p1[i], 5, colors[i], -1);
        }
    }

    // Display the demo
    Mat img;
    add(frame, mask, img);
    if (save) {
        string save_path = "./optical_flow_frames/frame_" + to_string(counter) + ".jpg";
        imwrite(save_path, img);
    }
    imshow("flow", img);
    int keyboard = waitKey(25);
    if (keyboard == 'q' || keyboard == 27)
        break;

    // Update the previous frame and previous points
    old_gray = frame_gray.clone();
    p0 = good_new;
    counter++;
}
