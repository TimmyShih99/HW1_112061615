#include <queue>
#include <cmath>

#include "SobelFilter.h"

SobelFilter::SobelFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// sobel mask
const int mask[MASK_X][MASK_Y] = {{1, 4, 7, 4, 1},
                                   {4, 16, 26, 16, 4},
                                   {7, 26, 41, 26, 7},
                                   {4, 16, 26, 16, 4},
                                   {1, 4, 7, 4, 1}};

void SobelFilter::do_filter() {
  // Define 2D buffer to store input pixels
  std::queue<int> buffer[BUFFER_SIZE];

  while (true) {
    int val = 0;

    // Read new pixel values into buffer
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        int pixel = (i_r.read() + i_g.read() + i_b.read()) / 3;
        buffer[u * MASK_Y + v].push(pixel);
      }
    }

    // Calculate the filtered value using the mask and buffered pixel values
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        val += buffer[u * MASK_Y + v].front() * mask[u][v];
      }
    }

    // Remove the oldest pixel value from the buffer
    for (unsigned int u = 0; u < BUFFER_SIZE; ++u) {
      buffer[u].pop();
    }

    double result = (double)val / 273; // Divide by the sum of the Gaussian mask
    o_result.write((int)result);
    wait(10); // Emulate module delay
  }
}
