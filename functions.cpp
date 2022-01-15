// Spring 2020 Week 7
// Computer Science 121 Seam Carving Project

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "functions.h"
using namespace std;

Pixel** createImage(int width, int height) {
  // Create a one dimensional array on the heap of pointers to Pixels 
  // that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    // that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  //cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;

  cout << "Image deleted" << endl;
}

bool loadImage(string filename, Pixel** image, int width, int height) {
    int row = 0;
    int col = 0;
    int tracker = 0;
    int w = 0, h = 0;

    // declare/define and open input file stream
    ifstream ifs (filename);
    // check if input stream opened successfully
    if (!ifs.is_open()) {
        cout << "Error: failed to open input file - " << filename << endl;
        return false;
    }
    // get type from preamble
    char type[3];
    ifs >> type; // should be P3
    if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
        cout << "Error: type is " << type << " instead of P3" << endl;
        return false;
    }
    // get width (w) and height (h) from preamble, and test for validity
    ifs >> w >> h;

    if(ifs.fail()){
        cout << "Error: read non-integer value" << endl;
        return false;
    }
    if (w != width) { // check that width matches what was passed into the function
        cout << "Error: input width (" << width << ") does not match value in file (" << w << ")" << endl;
        return false;
    }
    if (h != height) { // check that height matches what was passed into the function
        cout << "Error: input height (" << height << ") does not match value in file (" << h << ")" << endl;
        return false;
    }

    // get maximum value from preamble
    int colorMax = 0;
    ifs >> colorMax;
    if (colorMax < 255) {
        cout << "Error: not enough color values" << endl;
        return false;
    }
    if (colorMax > 255) {
        cout << "Error: too many color values" << endl;
        return false;
    }

    for (row = 0; row < height; ++row) {
        tracker = tracker + 3;
        for (col = 0; col < width; ++col) {
            int red, green, blue;
            ifs >>  red >> green >> blue;

            if (ifs.fail() && (red == 0 || green == 0 || blue == 0)) {
                cout << "Error: read non-integer value" << endl;
                return false;
            }

            // Test validity of red values
            if (red)
                if (red < 0 || red > 255) {
                    cout << "Error: invalid color value " << red << endl;
                    return false;
                }
            // Test validity of green values
            if (green < 0 || green > 255) {
                cout << "Error: invalid color value " << green << endl;
                return false;
            }
            // Test validity of blue values
            if (blue < 0 || blue > 255) {
                cout << "Error: invalid color value " << blue << endl;
                return false;
            }
            Pixel temp = Pixel();
            temp.r = red;
            temp.g = green;
            temp.b = blue;
            image[col][row] = temp;
        }
        if (ifs.eof() && ifs.fail()){
            cout << "Error: not enough color values" << endl;
            return false;
        }
    }
    int lastVal = 0;
    ifs >> lastVal;
    if (!ifs.eof() && !ifs.fail()) {
        cout << "Error: too many color values" << endl;
        return false;
    }
  return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
    int row = 0;
    int col = 0;
    // declare/define and open input file stream
    ofstream ofs(filename);
    // check if input stream opened successfully
    if (!ofs.is_open()) {
        cout << "Error: failed to open output file - " << filename << endl;
        return false;
    }

    ofs << "P3\n" << width << " " << height << "\n255\n";

    for (row = 0; row < height; ++row) {
        for (col = 0; col < width; ++col) {
            Pixel temp = image[col][row];
            ofs << temp.r << " " << temp.g << " " << temp.b << " ";
        }
        ofs << "\n";
    }
    if (ofs.is_open()) {
        ofs.close();
    }
    return true;
}

int energy(Pixel** image, int x, int y, int width, int height) {
    // Energy Values
    int xVal = 0;
    int yVal = 0;
    int totalVal = 0;
    // X trackers
    int xl = 0;
    int xr = 0;
    // Y trackers
    int yd = 0;
    int yu = 0;
    // Test for X-axis edge cases
    // X left
    if (x == 0) {
        xl = width - 1;
    } else {
        xl = x - 1;
    }
    // X right
    if (x == width - 1) {
        xr = 0;
    } else {
        xr = x + 1;
    }

    // Test for Y-axis edge cases
    // Y up
    if (y == 0) {
        yu = height - 1;
    } else {
        yu = y - 1;
    }
    // Y down
    if (y == height - 1) {
        yd = 0;
    } else {
        yd = y + 1;
    }

    // Calculate the energy in the image
    // Calculate for Delta X
    Pixel left = image[xl][y];
    Pixel right = image[xr][y];

    xVal = pow((left.r - right.r), 2) + pow((left.g - right.g), 2)
             + pow((left.b - right.b), 2);

    // Calculate for Delta Y
    Pixel up = image[x][yu];
    Pixel down = image[x][yd];

    yVal = pow((up.r - down.r), 2) + pow((up.g - down.g), 2)
               + pow((up.b - down.b), 2);

    // Sum up to find final result
    totalVal = xVal + yVal;

    // Return final results
    return totalVal;
}

int* createSeam(int length) {
    // Create an array full of zeros
    int* seam = new int[length];
    for (int i = 0; i < length; ++i) {
        seam[i] = 0;
    }
    return seam;
}

void deleteSeam(int* seam) {
    // Deallocate the seam
    delete [] seam;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
    int totalEnergy = energy(image, start_col, 0, width, height);
    int rightVal = 0;
    int forwardVal = 0;
    int leftVal = 0;
    seam[0] = (start_col);

    for (int x = 1; x < height; ++x) {
        // Load energy values for possible seam calculation.
        if (start_col == 0) {
            forwardVal = energy(image, start_col, x, width, height);
            leftVal = energy(image, start_col + 1, x, width, height);
            rightVal = forwardVal + leftVal;
        } else if (start_col == width - 1) {
            rightVal = energy(image, start_col - 1, x, width, height);
            forwardVal = energy(image, start_col, x, width, height);
            leftVal = rightVal + forwardVal;
        } else {
            rightVal = energy(image, start_col - 1, x, width, height);
            forwardVal = energy(image, start_col, x, width, height);
            leftVal = energy(image, start_col + 1, x, width, height);
        }

        // Test to see which direction should be traversed in the array
        // RIGHT ------------------------------------------------------
        if (rightVal < forwardVal && rightVal < leftVal) {
            // Record the index to the seam array
            start_col = start_col - 1;
            seam[x] = start_col;
            // Add up the energy value
            totalEnergy = totalEnergy + rightVal;
        }

        // LEFT -------------------------------------------------------
        else if (leftVal < rightVal && leftVal < forwardVal) {
            // Record the index to the seam array
            start_col = start_col + 1;
            seam[x] = start_col;
            // Add up the energy value
            totalEnergy = totalEnergy + leftVal;
        }
        else if (leftVal == rightVal && leftVal < forwardVal) {
            // Record the index to the seam array
            start_col = start_col +  1;
            seam[x] = start_col;
            // Add up the energy value
            totalEnergy = totalEnergy + leftVal;
        }

        // FORWARD ----------------------------------------------------
        else if (forwardVal <= rightVal && forwardVal <= leftVal) {
            // adds center energy value to seam[i]
            seam[x] = start_col;
            totalEnergy = totalEnergy + forwardVal;
        }
        else if (rightVal == forwardVal && rightVal < leftVal) {
            // prefer to go directly forward
            seam[x] = start_col;
            totalEnergy = totalEnergy + forwardVal;
        }
        else if (forwardVal == leftVal && forwardVal < rightVal) {
            // prefer to go directly forward
            seam[x] = start_col;
            totalEnergy = totalEnergy + forwardVal;
        }
    }
    return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
    int totalEnergy = energy(image, 0, start_row, width, height);
    int rightVal = 0;
    int forwardVal = 0;
    int leftVal = 0;
    seam[0] = (start_row);

    for (int y = 1; y < width; ++y) {
        // Load energy values for possible seam calculation.
        if (start_row == height - 1) {
            forwardVal = energy(image, y, start_row, width, height);
            leftVal = energy(image, y, start_row - 1, width, height);
            rightVal = forwardVal + leftVal;
        } else if (start_row == 0) {
            rightVal = energy(image, y, start_row + 1, width, height);
            forwardVal = energy(image, y, start_row, width, height);
            leftVal = rightVal + forwardVal;
        } else {
            rightVal = energy(image, y, start_row + 1, width, height);
            forwardVal = energy(image, y, start_row, width, height);
            leftVal = energy(image, y, start_row - 1, width, height);
        }

        // Test to see which direction should be traversed in the array
        // RIGHT ------------------------------------------------------
        if (rightVal < forwardVal && rightVal < leftVal) {
            // Record the index to the seam array
            start_row = start_row + 1;
            seam[y] = start_row;
            // Add up the energy value
            totalEnergy = totalEnergy + rightVal;
        }

        // LEFT -------------------------------------------------------
        else if (leftVal < rightVal && leftVal < forwardVal) {
            // Record the index to the seam array
            start_row = start_row - 1;
            seam[y] = start_row;
            // Add up the energy value
            totalEnergy = totalEnergy + leftVal;
        }
        else if (leftVal == rightVal && leftVal < forwardVal) {
            // Record the index to the seam array
            start_row = start_row - 1;
            seam[y] = start_row;
            // Add up the energy value
            totalEnergy = totalEnergy + leftVal;
        }

        // FORWARD ----------------------------------------------------
        else if (forwardVal <= rightVal && forwardVal <= leftVal) {
            // adds center energy value to seam[i]
            seam[y] = start_row;
            totalEnergy = totalEnergy + forwardVal;
        }
        else if (rightVal == forwardVal && rightVal < leftVal) {
            // prefer to go directly forward
            seam[y] = start_row;
            totalEnergy = totalEnergy + forwardVal;
        }
        else if (forwardVal == leftVal && forwardVal < rightVal) {
            // prefer to go directly forward
            seam[y] = start_row;
            totalEnergy = totalEnergy + forwardVal;
        }
    }
    return totalEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
    // Initialize starting variables
    int* minSeam = new int[height];
    int* seam = new int[height];
    int minEnergy = loadVerticalSeam(image, 0, width, height, seam);
    int currentEnergy = 0;

    // Fill a duplicate seam variable
    for (int i = 0; i < height; i++) {
        minSeam[i] = seam[i];
    }

    // Loop through seam energies to find the minSeam values
    for (int x = 1; x < width; ++x) {
        // Fill in the value for this iteration's current energy
        currentEnergy = loadVerticalSeam(image, x, width, height, seam);

        if (currentEnergy < minEnergy){
            // Replace for the lowest value
            minEnergy = currentEnergy;

            // Loop through this seam to get its index values
            for (int y = 0; y < height; ++y) {
                minSeam[y] = seam[y];
            }
        }
        else {
            continue;
        }
    }
    // Dealocate memory
    delete [] seam;
    seam = nullptr;

    // Return final value
    return minSeam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
    // Initialize starting variables
    int* minSeam = new int[width];
    int* seam = new int[width];
    int minEnergy = loadHorizontalSeam(image, 0, width, height, seam);
    int currentEnergy = 0;

    // Fill a duplicate seam variable
    for (int i = 0; i < width; i++) {
        minSeam[i] = seam[i];
    }

    // Loop through seam energies to find the minSeam values
    for (int y = 1; y < height; ++y) {
        // Fill in the value for this iteration's current energy
        currentEnergy = loadVerticalSeam(image, y, width, height, seam);

        if (currentEnergy < minEnergy){
            // Replace for the lowest value
            minEnergy = currentEnergy;

            // Loop through this seam to get its index values
            for (int x = 0; x < width; ++x) {
                minSeam[x] = seam[x];
            }
        }
        else {
            continue;
        }
    }
    // Dealocate memory
    delete [] seam;
    seam = nullptr;

    // Return final value
    return minSeam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
    Pixel temp;
    for (int y = 0; y < height; y++) {
        temp = image[verticalSeam[y]][y];
        for (int x = verticalSeam[y]; x < width - 1; x++) {
            image[x][y] = image[x + 1][y];
        }
        // Adjust image parameters accordingly
        image[width - 1][y] = temp;
    }
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
    Pixel temp;
    for (int x = 0; x < width; x++) {
        temp = image[x][horizontalSeam[x]];
        for (int y = horizontalSeam[x]; y < height - 1; y++) {
            image[x][y] = image[x][y + 1];
        }
        // Adjust image parameters accordingly
        image[x][height - 1] = temp;
    }
}
