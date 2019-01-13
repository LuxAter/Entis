#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "loop/loop.h"

int xspacing = 16;   // How far apart should each horizontal location be spaced
int w;              // Width of entire wave

float theta = 0.0;  // Start angle at 0
float amplitude = 75.0;  // Height of wave
float period = 500.0;  // How many pixels before the wave repeats
float dx;  // Value for incrementing X, a function of period and xspacing
int n;
float* yvalues;  // Using an array to store height values for the wave

void create() {
  width = 640;
  height = 360;
  w = width+16;
  dx = (TWO_PI / period) * xspacing;
  n = w / xspacing;
  yvalues = (float*)malloc(sizeof(float) * n);
  for(int i = 0; i < n; ++i){
    yvalues[i]=0;
  }
}

void destroy(){
  free(yvalues);
}

void calcWave() {
  // Increment theta (try different values for 'angular velocity' here
  theta += (float)mouseY / (float)height;

  // For every x value, calculate a y value with sine function
  float x = theta;
  for (int i = 0; i < n; i++) {
    yvalues[i] = sin(x)*amplitude;
    x+=dx;
  }
}

void renderWave() {
  /* noStroke(); */
  // A simple way to draw the wave with an ellipse at each location
  for (int x = 0; x < n; x++) {
    circle(x*xspacing, height/2+yvalues[x], 10);
  }
}

void key_press(uint16_t key){
  if(key == 'c'){
    fill(rand() % 0xFFFFFF);
  }
}

void draw() {
  background(0);
  calcWave();
  renderWave();
}

