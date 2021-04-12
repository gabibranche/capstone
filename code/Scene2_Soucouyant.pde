//Title: Soucouyant Final PDE
//Author: Gabrielle Branche
//Date: 12th APril, 2021
//Description: This code is used to allow a flame graphic to be mapped on to the dancer in Scene 2 of Soucouyant Stories
//Code adapted from Daniel Shiffman's average point tracking script
// https://github.com/shiffman/OpenKinect-for-Processing
// http://shiffman.net/p5/kinect/


//import libraries
import org.openkinect.freenect.*;
import org.openkinect.processing.*;
import codeanticode.syphon.*;


// The Kinect tracking happens in another class
KinectTracker tracker;
Kinect kinect;

// Syphon used to link Processing Script to Madmapper:
SyphonServer server;


// Variable used to generate the flame graphic
float Fx; 
float Fy; 
float Fnx; 
float Fny; 
float Fradian; 
float scale = 0.03;
PVector offset1, offset2;


// Variables used to make flame buffer
color c1, c2, c3, c4, c5;
int index1, index2, index3, index4;

//declaring buffers and graphics
PGraphics buffer1;
PGraphics buffer2;
PGraphics cooling;
PGraphics canvas;

//using pixel count to determine depth
float z;

// when determining the start of the cooling effect
float ystart = 0.0;




void setup() {
  size(640, 520, P3D);
  frameRate(60);
  canvas = createGraphics(width, height, P3D);                      // Graphic used to create a blank Canvas needed for Madmapper when flame is not generated

  buffer1 = createGraphics(width, height, P3D);                
  buffer2 = createGraphics(width, height, P3D);                     // Create Flame Buffer and Cooling Graphics
  cooling = createGraphics(width, height, P3D);

  offset1 = new PVector(random(100000), random(100000));            // Contols the extent to watch the flame varies
  offset2 = new PVector(random(10000), random(10000));

  kinect = new Kinect(this);                                        // Initiates Kinect
  tracker = new KinectTracker();

  server = new SyphonServer(this, "Processing Syphon");             // Initiates Syphon
}


void createBuffer() {
  for (int x = 0; x< width; x++) {
    for (int y = 1; y < height-1; y++) {

      int index0 = (x) + (y) * width;
      int index1 = (x+1) + (y) * width;
      int index2 = (x-1) + (y) * width;                             // takes a given pixel and saves the neighbouring pixels in a respective index
      int index3 = (x) + (y+1) * width;
      int index4 = (x) + (y-1) * width;

      c1 = buffer1.pixels[index1];
      c2 = buffer1.pixels[index2];
      c3 = buffer1.pixels[index3];                                  // Adds the buffer color values to the neighbouring pixels stored in the index variables
      c4 = buffer1.pixels[index4];
      c5 = cooling.pixels[index0];

      // changes the brightenss of the color of the neighbouring pixels to give a fading effect
      float newC = brightness(c1) + brightness(c2) + brightness(c3) + brightness(c4);
      newC = newC - brightness(c5);

      buffer2.pixels[index4] = color(newC*0.25, 0, 0);              // creates a new buffer based on the values stored in newC
    }
  }
}



//flame that follows dancer
void followFire() {

  PVector v1 = tracker.getPos();                                    // Gets the values of the average point tracking indentified by the Kinect

  buffer1. beginDraw();                                             // Begin drawing PGraphics
  buffer1.noStroke();


  push();                                                        
  float yAxis = v1.y+height/8;                                      // shifts y value of the average point down to accomodate for the limitations of the projection on the walls
  buffer1.translate(v1.x, yAxis );                                  // translates the flame grapgic according to the x value of the kinect tracker and yAxis


  float scaleRadious = map(z, 1000, 100000, 20, 5);                 // scales the flame according to how close or far the dancer is from the kinect; gives the effect of the flame growing more powerful closer to the tree


  //draws the flame and randomises it so it looks animated
  for (float radious = scaleRadious; radious > 0; radious -= 1) {   // determines the size of the flame using scaleRadious and moves from the furthest point from the center inward
    buffer1.fill(255, 0, 0);                                        // makes the flame red                          
    buffer1.beginShape();                                           // begin drawing PGraphic
    for (float angle = 0; angle < 360; angle += 1) {                // makes the flame a circular effect
      Fradian = radians(angle);                                     // converts the angle to radians
      Fx = radious * cos(Fradian);                                  // calculates the x position of the circle
      Fy = radious * sin(Fradian);                                  // calculates the y position of the circle
      
      // adds variation to the x and y components of the flame to make it look more animated; x value is smaller so that the flame varies more vertically than horizontally
      Fnx = Fx + map(noise(Fx * scale + offset1.x, Fy * scale + offset1.y, frameCount * 0.050), 0, 1, -250, 250);  
      Fny = Fy + map(noise(Fx * scale + offset2.x, Fy * scale + offset2.y, frameCount * 0.050), 0, 1, -500, 500);
      
      buffer1.vertex(Fnx, Fny);                                     // adds the buffer1 PGraphic to each point in the flame
    }
    buffer1.endShape(CLOSE);
  }
  pop();
  buffer1.endDraw();
}



//cooling effect
void cool() {
  cooling.beginDraw();                                               // Function needed to initate PGraphics
  cooling.loadPixels();                                              // Function needed to initiate drawing pixels for cooling

  float xoff = 0.0; // Start xoff at 0                               // Initiates x position of cooling
  float increment = 0.05;                                            // Determines how quickly cooling will take place

  // For every x,y coordinate in a 2D space, calculate a noise value and produce a brightness value
  for (int x = 0; x < width; x++) {
    xoff += increment;                                               // Increment xoff 
    float yoff = ystart;                                             // For every xoff, start yoff at 0
    for (int y = 0; y < height; y++) {
      yoff += increment;                                             // Increment yoff

      float bright = noise(xoff, yoff) * 80;                         // Calculate noise and scale by value that corresponds with desired rate

      cooling.pixels[x+y*width] = color(bright);                     // Set each pixel onscreen to a grayscale value
    }
  }

  cooling.updatePixels();
  ystart += increment;                                               // Increment y

  cooling.endDraw();                                                 // End PGraphics
}


void draw() {
  background(0);
  //println(frameRate);                                              // used for debugging
  //println(z);            

  tracker.track();                                                   // Run the tracking analysis  
  //tracker.display();                                               //Show the image; used for debugging

  // if dancer is in view of the kinect
  if (z>=500) {

    cool();
    followFire();

    buffer2.beginDraw();                                              
    buffer1.loadPixels();                                            // Load and Draw buffers
    buffer2.loadPixels();

    createBuffer();                                                  // Creating buffer

    buffer2.updatePixels();                                          // End PGraphic
    buffer2.endDraw();

    
    PGraphics temp = buffer1;
    buffer1 = buffer2;                                               // Swap PGraphics
    buffer2 = temp;

    image(buffer2, 0, 0);                                            // Draw Image
    server.sendImage(buffer1);                                       // Send Image to Madmapper via syphon
  } else {

    //send black canvas if not dancer detected
    canvas.beginDraw();
    canvas.fill(0);
    canvas.ellipse(width/2, height/2, 100, 100);                     // Ellipse drawn to have something in canvas
    canvas.endDraw();

    server.sendImage(canvas);                                        // Send Image to Madmapper via syphon
  }
}
