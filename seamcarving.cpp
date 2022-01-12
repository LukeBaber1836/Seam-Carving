#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "functions.h"
using namespace std;

int main() {
    string filename;
    int width = 0;
    int height = 0;
    int targetWidth = 0;
    int targetHeight = 0;

    // ###################################### Obtaining Image Data ##############################################
    cout << "Input filename: ";
    cin >> filename;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Invalid filename input." << endl;
        return 1;
    }

    // Get the width and height of the picture which will be edited, and test its validity
    cout << "Original width and height" << endl;
    cout << "Width:";
    cin >> width;
    cout << "Height:";
    cin >> height;
    cout << endl;

    // Test for integer values
    if(cin.fail()) {
        if (width == 0) {
            cout << "Error: width is a non-integer value" << endl;
            return 1;
        } else if (height == 0) {
            cout << "Error: height is a non-integer value" << endl;
            return 1;
        }
    }

    if (width <= 0) {
        cout << "Error: width must be greater than 0. You entered " << width << endl;
        return 2;
    }
    else if (height <= 0) {
        cout << "Error: height must be greater than 0. You entered " << height << endl;
        return 3;
    }

    // Get the target width and  target height of the picture which will be edited, and test its validity
    cout << "Input target width and height: " << endl;
    cout << "Width:";
    cin >> targetWidth;
    cout << "Height:";
    cin >> targetHeight;
    cout << endl;

    // Test for integer inputs
    if(cin.fail()){
        if(targetWidth == 0) {
            cout << "Error: target width is a non-integer value" << endl;
            return 1;
        }
        else if(targetHeight == 0) {
            cout << "Error: target height is a non-integer value" << endl;
            return 1;
        }
    }

    if (targetWidth <= 0) {
        cout << "Error: target width must be greater than 0. You entered " << targetWidth << endl;
        return 2;
    } else if (targetWidth > width) {
        cout << "Error: target width must be less than width, " << targetWidth
             << " is greater than " << width << endl;
        return 21;
    }

    if (targetHeight <= 0) {
        cout << "Error: target height must be greater than 0. You entered " << targetHeight << endl;
        return 3;
    } else if (targetHeight > height) {
        cout << "Error: target height must be less than height, " << targetHeight
             << " is greater than " << height << endl;
        return 31;
    }

// ##################################### Outputting And Editing Image ##############################################

    // Initiate variables
    int originalWidth = width;

    Pixel** image = createImage(width, height); // create array of the size that will be needed
    if (image != nullptr) {
        if (loadImage(filename, image, width, height)) {

            int* verticalSeam = createSeam(height);
            int* horizontalSeam = createSeam(width);

            cout << "Start carving..." << endl;

            while (height > targetHeight) {
                horizontalSeam = findMinHorizontalSeam(image, width, height);
                removeHorizontalSeam(image, width, height, horizontalSeam);
                deleteSeam(horizontalSeam);
                height--;
            }

            while (width > targetWidth) {
                verticalSeam = findMinVerticalSeam(image, width, height);
                removeVerticalSeam(image, width, height, verticalSeam);
                deleteSeam(verticalSeam);
                width--;
            }

            // set up output filename
            cout << "Image has been carved" << endl;
            stringstream ss;
            ss << "Carved" << width << "X" << height << "." << filename;
            outputImage(ss.str().c_str(), image, width, height);
        }
        // Deallocate memory to avoid memory leak
        deleteImage(image, originalWidth);
    }
}