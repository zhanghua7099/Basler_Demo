// Grab.cpp
/*
    Note: Before getting started, Basler recommends reading the "Programmer's Guide" topic
    in the pylon C++ API documentation delivered with pylon.
    If you are upgrading to a higher major version of pylon, Basler also
    strongly recommends reading the "Migrating from Previous Versions" topic in the pylon C++ API documentation.

    This sample illustrates how to grab and process images using the CInstantCamera class.
    The images are grabbed and processed asynchronously, i.e.,
    while the application is processing a buffer, the acquisition of the next buffer is done
    in parallel.

    The CInstantCamera class uses a pool of buffers to retrieve image data
    from the camera device. Once a buffer is filled and ready,
    the buffer can be retrieved from the camera object for processing. The buffer
    and additional image data are collected in a grab result. The grab result is
    held by a smart pointer after retrieval. The buffer is automatically reused
    when explicitly released or when the smart pointer object is destroyed.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using cout.
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 10000;

int main( int /*argc*/, char* /*argv*/[] )
{
    // The exit code of the sample application.
    int exitCode = 0;

    // Before using any pylon methods, the pylon runtime must be initialized.
    PylonInitialize();

    try
    {
        // Create an instant camera object with the camera device found first.
        CTlFactory& tlFactory = CTlFactory::GetInstance();
        CInstantCamera Camera_A, Camera_B, Camera_C, Camera_D;

        DeviceInfoList_t devices;
        if (tlFactory.EnumerateDevices( devices ) == 0)
        {
            throw RUNTIME_EXCEPTION( "No camera present." );
        }
        Camera_A.MaxNumBuffer = 4;
        Camera_B.MaxNumBuffer = 4;
        Camera_C.MaxNumBuffer = 4;
        Camera_D.MaxNumBuffer = 4;

        Camera_A.Attach(tlFactory.CreateDevice(devices[0]));
        Camera_A.Open();
        Camera_B.Attach(tlFactory.CreateDevice(devices[1]));
        Camera_B.Open();
        Camera_C.Attach(tlFactory.CreateDevice(devices[2]));
        Camera_C.Open();

        cout << "Using device A: " << Camera_A.GetDeviceInfo().GetSerialNumber() << endl;
        cout << "Using device B: " << Camera_B.GetDeviceInfo().GetSerialNumber() << endl;
        cout << "Using device C: " << Camera_C.GetDeviceInfo().GetSerialNumber() << endl;


        Camera_A.StartGrabbing(GrabStrategy_LatestImages);
        Camera_B.StartGrabbing(GrabStrategy_LatestImages);
        Camera_C.StartGrabbing(GrabStrategy_LatestImages);


        CGrabResultPtr ptrGrabResult_A, ptrGrabResult_B, ptrGrabResult_C, ptrGrabResult_D;
        CImageFormatConverter formatConverter_A, formatConverter_B, formatConverter_C, formatConverter_D;
        formatConverter_A.OutputPixelFormat = PixelType_BGR8packed;
        formatConverter_B.OutputPixelFormat = PixelType_BGR8packed;
        formatConverter_C.OutputPixelFormat = PixelType_BGR8packed;


        CPylonImage pylonImage_A, pylonImage_B, pylonImage_C, pylonImage_D;

        cv::Mat openCvImage_A, openCvImage_B, openCvImage_C, openCvImage_D;

        // basler初始分辨率为1200*1920
	    // cv::Size S = cv::Size(1920/2, 1200/2);
    	// outputVideo.open(outputVideoPath, -1, 30.0, S, true);
        // cv::VideoWriter outputVideo("outcpp-.mp4", CV_FOURCC('M','J','P','G'), 30, cv::Size(1920/2, 1200/2));





        // Create a video writer object.
        CVideoWriter videoWriter_A, videoWriter_B, videoWriter_C;

        // The frame rate used for playing the video (playback frame rate).
        const int cFramesPerSecond = 20;

        // The quality used for compressing the video.
        const uint32_t cQuality = 90; 

        CIntegerParameter width( Camera_A.GetNodeMap(), "Width" );
        CIntegerParameter height( Camera_A.GetNodeMap(), "Height" );
        CEnumParameter pixelFormat( Camera_A.GetNodeMap(), "PixelFormat" );

        
        // Map the pixelType
        CPixelTypeMapper pixelTypeMapper( &pixelFormat );
        EPixelType pixelType = pixelTypeMapper.GetPylonPixelTypeFromNodeValue( pixelFormat.GetIntValue() );

        // Set parameters before opening the video writer.
        videoWriter_A.SetParameter(
        (uint32_t) width.GetValue(),
            (uint32_t) height.GetValue(),
            pixelType,
            cFramesPerSecond,
            cQuality );

        videoWriter_B.SetParameter(
        (uint32_t) width.GetValue(),
            (uint32_t) height.GetValue(),
            pixelType,
            cFramesPerSecond,
            cQuality );

        videoWriter_C.SetParameter(
        (uint32_t) width.GetValue(),
            (uint32_t) height.GetValue(),
            pixelType,
            cFramesPerSecond,
            cQuality );

        // Open the video writer.
        videoWriter_A.Open( "AAAA.mp4" );
        videoWriter_B.Open( "BBBB.mp4" );
        videoWriter_C.Open( "CCCC.mp4" );




        while (Camera_A.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            Camera_A.RetrieveResult( 5000, ptrGrabResult_A, TimeoutHandling_ThrowException );
            Camera_B.RetrieveResult( 5000, ptrGrabResult_B, TimeoutHandling_ThrowException );
            Camera_C.RetrieveResult( 5000, ptrGrabResult_C, TimeoutHandling_ThrowException );


            // Image grabbed successfully?
            if (ptrGrabResult_A->GrabSucceeded() && ptrGrabResult_B->GrabSucceeded() && ptrGrabResult_C->GrabSucceeded())
            {
                
                videoWriter_A.Add( ptrGrabResult_A );
                videoWriter_B.Add( ptrGrabResult_B );
                videoWriter_C.Add( ptrGrabResult_C );

                // Access the image data.

                formatConverter_A.Convert(pylonImage_A, ptrGrabResult_A);
                openCvImage_A = cv::Mat(ptrGrabResult_A->GetHeight(), ptrGrabResult_A->GetWidth(), CV_8UC3, (uint8_t *) pylonImage_A.GetBuffer());
                
                videoWriter_A.Add( ptrGrabResult_A );
                // std::cout<<ptrGrabResult_A->GetHeight()<< ptrGrabResult_A->GetWidth()<<std::endl;

                cv::resize(openCvImage_A, openCvImage_A, cv::Size(), 0.5, 0.5);
                cv::imshow("A", openCvImage_A);

                // outputVideo.write(openCvImage_A);

                formatConverter_B.Convert(pylonImage_B, ptrGrabResult_B);
                openCvImage_B = cv::Mat(ptrGrabResult_B->GetHeight(), ptrGrabResult_B->GetWidth(), CV_8UC3, (uint8_t *) pylonImage_B.GetBuffer());
                cv::resize(openCvImage_B, openCvImage_B, cv::Size(), 0.5, 0.5);
                cv::imshow("B", openCvImage_B);

                formatConverter_C.Convert(pylonImage_C, ptrGrabResult_C);
                openCvImage_C = cv::Mat(ptrGrabResult_C->GetHeight(), ptrGrabResult_C->GetWidth(), CV_8UC3, (uint8_t *) pylonImage_C.GetBuffer());
                cv::resize(openCvImage_C, openCvImage_C, cv::Size(), 0.5, 0.5);
                cv::imshow("C", openCvImage_C);


                if (char(cv::waitKey(1)) == 'q') break;
            }
            else
            {
                cout << "Error: " << std::hex << ptrGrabResult_A->GetErrorCode() << std::dec << " " << ptrGrabResult_A->GetErrorDescription() << endl;
            }
        }
        // outputVideo.release();

    }
    catch (const GenericException& e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
        exitCode = 1;
    }


    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press enter to exit." << endl;
    while (cin.get() != '\n');

    // Releases all pylon resources.
    PylonTerminate();

    return exitCode;
}
