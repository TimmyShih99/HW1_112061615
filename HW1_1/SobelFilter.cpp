#include <cmath>
#include "SobelFilter.h"

SobelFilter::SobelFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

const int mask[MASK_X][MASK_Y] = {{1, 4, 7, 4, 1},      //Gaussian blur filter
                                   {4, 16, 26, 16, 4},  //Divide 273 at the end
                                   {7, 26, 41, 26, 7},
                                   {4, 16, 26, 16, 4},
                                   {1, 4, 7, 4, 1}};

void SobelFilter::do_filter() {
  while (true) {
    int val = 0;
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        val += grey * mask[u][v];
      }
    }
    double result = (double)val / 273; // Divide by the sum of the Gaussian mask
    o_result.write((int)result);
    wait(10); 
  }
}
